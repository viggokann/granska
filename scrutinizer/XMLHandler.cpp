#include "XMLHandler.hpp"
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <iostream>
#include <string.h>

using namespace xercesc;
using namespace std;

XMLHandler::XMLHandler() : found(false), ordfoljd(0)
{
}

XMLHandler::~XMLHandler()
{
}

void XMLHandler::startElement(const   XMLCh* const    uri,
                            const   XMLCh* const    localname,
                            const   XMLCh* const    qname,
                            const   Attributes&     attrs)
{
    char* message = XMLString::transcode(localname);    
    if(found==false && strcmp(message,"category")==0) found = true;
    XMLString::release(&message);
}

void XMLHandler::characters(const   XMLCh* const		chars, const XMLSize_t length)
{
    char* message = XMLString::transcode(chars);
    if(found==true && strcmp(message,"ordföljd")==0) ordfoljd++;
    XMLString::release(&message);
}

void XMLHandler::endElement(const   XMLCh* const    uri,
                            const   XMLCh* const    localname,
                            const   XMLCh* const    qname)
{
    char* message = XMLString::transcode(localname);
    if(found==true && strcmp(message,"text")==0) found = false;
    XMLString::release(&message);
}

void XMLHandler::endDocument()
{
	if(ordfoljd==3) cout << "Category 'ordföljd': OK" << endl;
}

void XMLHandler::fatalError(const SAXParseException& exception)
{
    char* message = XMLString::transcode(exception.getMessage());
    cout << "Fatal Error: " << message
         << " at line: " << exception.getLineNumber()
         << endl;
    XMLString::release(&message);
}
