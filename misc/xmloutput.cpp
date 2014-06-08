#pragma warning(disable: 4786)
#include "xmloutput.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <stack>
#include <stdlib.h>



namespace Misc
{
    class Output_impl
    {
    public:
        virtual ~Output_impl() {}

        virtual void flush() = 0;
        virtual void push(std::string name) = 0;
        virtual void pop() = 0;
        virtual void add(std::string name, std::string value) = 0;
        virtual void attr(std::string name, std::string value) = 0;
        virtual void file(const char *f) = 0;
        virtual void stream(std::ostream &) = 0;
        virtual void init() = 0;
        virtual void exit() = 0;
        virtual const char* getCharP() = 0;
    };

    class Output_impl_normal : public Output_impl
    {
        struct Attr
        {
            Attr(std::string n, std::string v) : name(n), value(v) {}
            std::string name;
            std::string value;
        };
        std::string node;
        std::string value;
        std::vector<Attr> attrib;
        bool pushed;
        bool inited;
        bool exited;

        std::stack<std::string> elem;
        std::ofstream out;
        std::ostream *s;
        std::ostringstream os;
    public:
        Output_impl_normal()
            : pushed(false), inited(false), exited(false), s(0)
        {}
        ~Output_impl_normal() { exit(); }
		const char* getCharP();
        void flush();
        void push(std::string name);
        void pop();
        void add(std::string name, std::string value);
        void attr(std::string name, std::string value);
        std::ostringstream &ostr()
        {
			return os;
        }
        std::ostream &str()
        {
            if(s)
                return *s;
            else
                return std::cout;
        }
        void file(const char *f)
        {
            exit();

            if(f)
                out.open(f);

            if(out.fail())
                {
                    std::cerr << "could not open output file '" 
                              << f << "'" << std::endl;
                    ::exit(1);
                }
            s = &out;
            reset();
        }
        void stream(std::ostream &o)
        {
            exit();
            s = &o;
            reset();
        }
        void reset()
        {
            node = "";
            value = "";
            attrib.clear();
            pushed = false;
            inited = false;
            exited = false;
        }
        void init()
        {
            if(inited || exited)
                return;
			#ifdef LIB_GRANSKA
            ostr() << "<?xml version=\"1.0\" "
                  << "encoding=\"ISO-8859-1\" "
                  << "standalone=\"yes\"?>"
                  << std::endl
                  << "<Root>";
            #endif
            #ifndef LIB_GRANSKA
            str() << "<?xml version=\"1.0\" "
                  << "encoding=\"ISO-8859-1\" "
                  << "standalone=\"yes\"?>"
                  << std::endl
                  << "<Root>";
            #endif
            reset();
            inited = true;
        }
        void exit()
        {
			#ifndef LIB_GRANSKA
            if(inited && !exited)
                str() << "</Root>";
            #endif
            if(!elem.empty())
                std::cerr << "WARNING: XML will not be well-formed, "
                          << "XML stack was not empty on Output::exit()"
                          << std::endl;
            exited = true;
            inited = false;
        }
    };

    class Output_impl_silent : public Output_impl
    {
		std::ostringstream ostring;
    public:
        Output_impl_silent()  {}
        ~Output_impl_silent() {}

        void flush() {}
        void clear() {}
        void push(std::string name) {}
        void pop() {}
        void add(std::string name, std::string value) {}
        void attr(std::string name, std::string value) {}
        void file(const char *f) {}
        void stream(std::ostream &) {};
        void init() {}
        void exit() {}
        const char* getCharP() {return ostring.str().c_str();}
    };

}

const char* Misc::Output_impl_normal::getCharP(){
	ostr() << "</Root>\n";
	std::string ch1 = ostr().str();
	const char* ch = ostr().str().c_str();
	ostr().str(std::string());
	inited = false;
	return ch;
}

void Misc::Output_impl_normal::flush()
{
    if(exited)
        return;
	#ifdef LIB_GRANSKA
	std::ostringstream &o = ostr();
	#endif
	#ifndef LIB_GRANSKA
    std::ostream &o = str();
	#endif
    if(node != "")
        {
            o << "<" << node;
            for(unsigned int i = 0; i < attrib.size(); i++)
                o << " " << attrib[i].name << "=\""
                  << attrib[i].value << "\"";
            if(!pushed)
                {
                    if(value == "")
                        o << "/>" << std::endl;
                    else
                        o << ">" << value << "</" << node << ">" << std::endl;
                }
            else
                o << ">";
        }
    else if(!inited)	// first node
        init();

    node = "";
    value = "";
    attrib.clear();
    pushed = false;
}

void Misc::Output_impl_normal::push(std::string n)
{
    flush();
    node = n;
    pushed = true;
    elem.push(n);
    //std::cout << std::endl << "push: " << n << " -- " << elem.top() << std::endl;
}

void Misc::Output_impl_normal::pop()
{
    flush();
    #ifdef LIB_GRANSKA
    ostr() << "</" << elem.top() << ">" << std::endl;
	#endif	
    #ifndef LIB_GRANSKA
    str() << "</" << elem.top() << ">" << std::endl;
    #endif
    //std::cout << std::endl << "pop: " << elem.top() << std::endl;
    elem.pop();
}

void Misc::Output_impl_normal::add(std::string n, std::string val)
{
    flush();
    node  = n;
    value = val;
}

void Misc::Output_impl_normal::attr(std::string n, std::string val)
{
    attrib.push_back(Attr(n, val));
} 


Misc::XMLoutput::XMLoutput(bool silent)
{
    if(silent)
        impl = new Output_impl_silent;
    else
        impl = new Output_impl_normal;
}

Misc::XMLoutput::~XMLoutput() { delete impl; }

void Misc::XMLoutput::push(const char *node_name)       { impl->push(node_name); }
void Misc::XMLoutput::pop()			        { impl->pop(); }

const char* Misc::XMLoutput::getCharP(){ return impl->getCharP(); }
void Misc::XMLoutput::add(std::string name, std::string text) { impl->add(name,text);}
void Misc::XMLoutput::add(const char *n, const char *t) { impl->add(n, (t ? t : "")); }
void Misc::XMLoutput::add(const char *n, int v)
{
    std::ostringstream s;
    s << v;
    add(n, s.str().c_str());
}

void Misc::XMLoutput::add(const char *n, double v)
{
    std::ostringstream s;
    s << v;
    add(n, s.str().c_str());
}

void Misc::XMLoutput::attr(std::string name, std::string text) { impl->attr(name,text); }
void Misc::XMLoutput::attr(const char *n, const char *v) { impl->attr(n, v); }
void Misc::XMLoutput::attr(const char *n, int v)
{
    std::ostringstream s;
    s << v;
    attr(n, s.str().c_str());
}

void Misc::XMLoutput::attr(const char *n, double v)
{
    std::ostringstream s;
    s << v;
    attr(n, s.str().c_str());
}
void Misc::XMLoutput::file(const char *file_name) { impl->file(file_name); }
void Misc::XMLoutput::stream(std::ostream &o) { impl->stream(o); }
void Misc::XMLoutput::init() { impl->init(); }
void Misc::XMLoutput::exit() { impl->exit(); }

std::string Misc::fixXML(std::string text) {
    //Oscar, fix of bad XML output
    std::ostringstream xmlOkText;
    int len = text.length();
    int prevOffset = -1;
    int offset;
    char c;    
    for(offset = 0; offset < len; offset++) {        
        c = text.at(offset);
        if(c=='&'||c=='\''||c=='\"'||c=='<'||c=='>') {
            if(prevOffset < offset-1)
                xmlOkText << text.substr(prevOffset+1,offset-prevOffset-1);
            switch(c) {
            case '&': xmlOkText << "&amp;"; break;
            case '\'': xmlOkText << "&apos;"; break;
            case '\"': xmlOkText << "&quot;"; break;
            case '<': xmlOkText << "&lt;"; break;
            case '>': xmlOkText << "&gt;"; break;            
            }
            prevOffset=offset;
        }
    }
    if(prevOffset == -1)
        return text;
    if(prevOffset < --offset) {        
        xmlOkText << text.substr(prevOffset+1, offset-prevOffset);
    }
    return xmlOkText.str();    
}

std::string Misc::fixXML(const char* text) {
    //Oscar, fix of bad XML output   
    std::ostringstream xmlOkText;
    int prevOffset = -1;
    int offset;
    char c;
    for(offset = 0; text[offset]; offset++) {
        c=text[offset];
        if(c=='&'||c=='\''||c=='\"'||c=='<'||c=='>') {
            if(prevOffset < offset-1)
                xmlOkText.write(text+prevOffset+1,offset-prevOffset-1);
            switch(c) {
            case '&': xmlOkText << "&amp;"; break;
            case '\'': xmlOkText << "&apos;"; break;
            case '\"': xmlOkText << "&quot;"; break;
            case '<': xmlOkText << "&lt;"; break;
            case '>': xmlOkText << "&gt;"; break;
            }
            prevOffset=offset;
        }
    }
    if(prevOffset == -1)
        return std::string(text);
    if(prevOffset < --offset) {        
        xmlOkText.write(text+prevOffset+1,offset-prevOffset);
    }
    return xmlOkText.str();  
}
