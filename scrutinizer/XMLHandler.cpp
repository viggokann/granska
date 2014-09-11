#include "XMLHandler.hpp"
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <iostream>
#include <string.h>

using namespace xercesc;
using namespace std;

XMLHandler::XMLHandler() : found(false), ordfoljd(0), skiljetecken(0), storbokstav(0)
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
    if(found==false && strcmp(message,"skiljetecken")==0) found = true;
    if(found==false && strcmp(message,"storbokstav")==0) found = true;

    XMLString::release(&message);
}

void XMLHandler::characters(const   XMLCh* const		chars, const XMLSize_t length)
{
    char* message = XMLString::transcode(chars);
    if(found==true && strcmp(message,"ordföljd")==0) ordfoljd++;
    if(found==true && strcmp(message,"skiljetecken")==0) skiljetecken++;
    if(found==true && strcmp(message,"storbokstav")==0) storbokstav++;
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
	if(ordfoljd==3) cout << "Category Ordföljd: OK" << endl;
	else cout << "Category Ordföljd: FAILED" << endl;
	if(skiljetecken==12) cout << "Category Skiljetecken: OK" << endl;
	else cout << "Category Skiljetecken: FAILED" << endl;
	if(storbokstav==24) cout << "Category Storbokstav: OK" << endl;
	else cout << "Category Storbokstav: FAILED" << endl;
}

void XMLHandler::fatalError(const SAXParseException& exception)
{
    char* message = XMLString::transcode(exception.getMessage());
    cout << "Fatal Error: " << message
         << " at line: " << exception.getLineNumber()
         << endl;
    XMLString::release(&message);
}
