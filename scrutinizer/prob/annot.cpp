#pragma warning(disable: 4786)

#include "annot.h"
#include <string>
#include <map>
#include "sentence.h"

//#define NO_ANNOT_FROM_FILE	// don't read from file, use hard-coded annotations


static std::map<std::string, Prob::annot_t> type_map;
static void init_types();

#ifndef NO_ANNOT_FROM_FILE

#include <util/PlatformUtils.hpp>
//#include <parsers/DOMParser.hpp>
#include <parsers/XercesDOMParser.hpp>
//#include <dom/DOM_NamedNodeMap.hpp>
#include <dom/DOMNamedNodeMap.hpp>
#include <dom/DOMNodeList.hpp>
#include "domerror.h"
#include <iostream>

Prob::annot_map Prob::annots;
std::vector<Prob::New_annot> Prob::not_annot;

static std::string to_string(const XMLCh *s)
{
  //char *p = s.transcode();
  char *p = xercesc::XMLString::transcode(s);
  std::string t(p);
  xercesc::XMLString::release(&p);
  
  //delete [] p;	// jb: why doesn't this work?
  return t;
}

static int get_int(const XMLCh *s)
{
  std::string str = to_string(s);
  if(str == "FALSE_ALARM")
    return Prob::FALSE_ALARM;
  else
    return atoi(str.c_str());
}

static int get_enum(const XMLCh *s)
{
  typedef std::map<std::string, Prob::annot_t> map;
  map::const_iterator it = type_map.find(to_string(s));

  if(it == type_map.end())
    {
      //std::cerr << "did not find type " << to_string(s) << std::endl;
      return -1;
    }
  else
    return it->second;
}

// Not needed any more, DOMStrings are gone from xercesc
// std::ostream &operator<< (std::ostream &o, const DOMString &s)
// {
//     return o << to_string(s);
// }

void Prob::init_annot(Prob::annot_map &annots, std::string file)
{
  init_types();

  if(file == "")
    return;

  const char		    *xmlFile = file.c_str();
  xercesc::AbstractDOMParser::ValSchemes    valScheme = xercesc::AbstractDOMParser::Val_Auto;
  bool                     doNamespaces    = false;
  bool                     doSchema        = false;

  // Instantiate the DOM parser
  xercesc::XercesDOMParser parser;
  parser.setValidationScheme(valScheme);
  parser.setDoNamespaces(doNamespaces);
  parser.setDoSchema(doSchema);

  // Create our error handler and install it
  DOMCountErrorHandler errorHandler;
  parser.setErrorHandler(&errorHandler);

  try
    {
      // const unsigned long startMillis = xercesc::XMLPlatformUtils::getCurrentMillis();
      parser.parse(xmlFile);
      // const unsigned long endMillis = xercesc::XMLPlatformUtils::getCurrentMillis();
    }
  catch (...)
    {
      std::cerr << "\nERROR: Unexpected exception during parsing: '" << xmlFile << "'\n";
      throw "error during XML parsing";
    }

  //
  //  Extract the DOM tree, get the list of all the elements and report the
  //  length as the count of elements.
  //

  xercesc::DOMDocument *doc = parser.getDocument(); // doc object owned by parser, we do not need to delete

  XMLCh tmpStringS[10];
  xercesc::XMLString::transcode("s", tmpStringS, 9);
  XMLCh tmpStringFrom[10];
  xercesc::XMLString::transcode("from", tmpStringFrom, 9);
  XMLCh tmpStringPos[10];
  xercesc::XMLString::transcode("pos", tmpStringPos, 9);
  XMLCh tmpStringType[10];
  xercesc::XMLString::transcode("type", tmpStringType, 9);
  XMLCh tmpStringRef[10];
  xercesc::XMLString::transcode("ref", tmpStringRef, 9);
    
  xercesc::DOMNodeList *nl  = doc->getElementsByTagName(tmpStringS);

  unsigned int sz = nl->getLength();

  if(errorHandler.getSawErrors())
    {
      std::cerr << "\nERROR: Unexpected exception during parsing: '" << xmlFile << "'\n";
      throw "error during XML parsing";
    }

  // Print out the stats that we collected and time taken.
  //std::cout << xmlFile << ": " << duration << " ms" << std::endl;

  try
    {
      for(unsigned int i = 0; i < sz; i++)
	{
	  Annot r;

	  r.value[0] = PROBCHECK_OK;
	  r.count    = 0;
	  r.type[0]  = NO_TYPE;
	  r.type[1]  = NO_TYPE;
	  r.type[2]  = NO_TYPE;

	  // sentence
	  xercesc::DOMNode		*n = nl->item(i);
	  xercesc::DOMNamedNodeMap	*nn = n->getAttributes();
	  int offset = get_int(nn->getNamedItem(tmpStringRef)->getNodeValue());
	  //std::cout << "offset = " << r.offset << std::endl;

	  xercesc::DOMNodeList *cl = n->getChildNodes();
	  for(unsigned int j = 0; j < cl->getLength(); j++)
	    {
	      // annotation
	      xercesc::DOMNode *attr = cl->item(j);
	      if(attr->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
		continue; 

	      r.type[r.count] = NO_TYPE;

	      xercesc::DOMNodeList *tl = attr->getChildNodes();
	      for(unsigned int k = 0; k < tl->getLength(); k++)
		{
		  // annotation type
		  xercesc::DOMNode *data = tl->item(k);
		  if(data->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
		    continue; 

		  if(to_string(data->getNodeName()) == "type")
		    {
		      if(r.type[r.count] == NO_TYPE)
			r.type[r.count] = 0;

		      int anntype = get_enum(data->getFirstChild()->getNodeValue());
		      if(anntype == -1)
			{
#if 0
			  // use this if you want to ignore annotations containing
			  // erroneous error types
			  r.type[r.count] = ERR_TYPE;
			  break;
#endif
			}
		      else
			r.type[r.count] |= anntype;

		      //std::cout << "        " << data.getNodeName() 
		      //	  << " = " << std::hex << r.type[r.count]
		      //	  << std::dec << std::endl;
		    }
		  else if(to_string(data->getNodeName()) == "range")
		    {
		      xercesc::DOMNamedNodeMap *na = data->getAttributes();

		      r.value[r.count] = get_int(na->getNamedItem(tmpStringFrom)->getNodeValue());
		      //r.value[r.count] = get_int(na.getNamedItem("to").getNodeValue());
		    }
		  else if(to_string(data->getNodeName()) == "position")
		    {
		      xercesc::DOMNamedNodeMap *na = data->getAttributes();

		      r.value[r.count] = get_int(na->getNamedItem(tmpStringPos)->getNodeValue());
		    }
		}
	      r.count++;
	    }
	  annots[offset] = r;
	}
    }
  catch(...)
    {
      throw "error in the annotation tree XML file";
    }
}

#else  // if !NO_ANNOT_FROM_FILE
void Prob::init_annot(Prob::annot_map &annots, std::string)
{
  init_types();

  // init annots here
}
#endif


void init_types()
{
  using namespace Prob;

  type_map["NO_TYPE"]		= NO_TYPE;
  type_map["PH_INTERPOSED"]	= PH_INTERPOSED;
  type_map["HARD_TTT"]	= HARD_TTT;
  type_map["ERR_TOK_SENT"]	= ERR_TOK_SENT;
  type_map["BAD_TAG"]		= BAD_TAG;
  type_map["FOREIGN"]		= FOREIGN;
  type_map["STYLE"]		= STYLE;
  type_map["BAD_PN"]		= BAD_PN;
  type_map["VERB"]		= VERB;
  type_map["SPELL"]		= SPELL;
  type_map["SEM_GRAM"]	= SEM_GRAM;
  type_map["COMPOUND"]	= COMPOUND;
  type_map["MISSING_WORD"]	= MISSING_WORD;
  type_map["MISSING_COMMA"]	= MISSING_COMMA;
  type_map["WORD_ORDER"]	= WORD_ORDER;
  type_map["LOOK_AT"]		= LOOK_AT;
}


namespace Prob
{
  extern const AbstractSentence *current_sentence;
}

void Prob::not_annotated(int from,
			 int to,
			 std::string comment,
			 const AbstractSentence *s)
{
  New_annot a;
  a.from = from;
  a.to = to;
  a.comment = comment;
		
  if(!s)
    s = current_sentence;
  a.offset = s->GetWordToken(0 + 2)->Offset();

  const int context = 100;
  int begin = from - context;
  int end   = to + context;
  if(begin < 0)
    begin = 0;
  if(end > s->NWords())
    end = s->NWords();

  for(int i = begin; i < end; i++)
    {
      if(i == from)
	a.text += "[";
      a.text += s->GetWordToken(i + 2)->RealString();
      if(i == to)
	a.text += "]";
      a.text += " ";
    }
  not_annot.push_back(a);
}

void Prob::output_new_annotations()
{
  for(unsigned int i = 0; i < not_annot.size(); i++)
    {
#if 0	// mark all as potential grammatical errors
      std::cout << "\t<s ref=\"" << not_annot[i].offset
		<< "\">" << std::endl;
      std::cout << "\t\t<annot>" << std::endl;
      if(not_annot[i].from == not_annot[i].to)
	std::cout << "\t\t\t<position pos=\"" << not_annot[i].from
		  << "\"/>" << std::endl;
      else
	std::cout << "\t\t\t<range from=\"" << not_annot[i].from
		  << "\" to=\"" << not_annot[i].to << "\"/>"
		  << std::endl;
      std::cout << "\t\t\t<text>" << not_annot[i].text 
		<< "</text>" << std::endl;
      std::cout << "\t\t\t<type></type>" << std::endl;
      std::cout << "\t\t\t<comment>" << not_annot[i].comment 
		<< "</comment>" << std::endl;
      std::cout << "\t\t</annot>" << std::endl;
      std::cout << "\t</s>" << std::endl;
#else	// mark all as false
      std::cout << "\t<s ref=\"" << not_annot[i].offset
		<< "\">" << std::endl;
      std::cout << "\t\t<annot>" << std::endl;
      std::cout << "\t\t\t<position pos=\"FALSE_ALARM\"/>" << std::endl;
      std::cout << "\t\t\t<text>" << not_annot[i].text 
		<< "</text>" << std::endl;
      std::cout << "\t\t\t<comment>pos = " << not_annot[i].from
		<< "</comment>" << std::endl;
      std::cout << "\t\t</annot>" << std::endl;
      std::cout << "\t</s>" << std::endl;
#endif
    }
}
