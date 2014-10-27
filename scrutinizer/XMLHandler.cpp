#include "XMLHandler.hpp"
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <iostream>
#include <string.h>

using namespace xercesc;
using namespace std;

XMLHandler::XMLHandler() : found(false), sar(0), stavning(0), dom(0), pronomen(0), kong(0), verb(0), 
		tempus(0), fragetecken(0), utropstecken(0), egennamn(0),
		nymening(0), gemen(0) , ordbi(0), ordind(0), kompsuffix(0), kompperi(0), ljud(0)
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
    if(found==true && strcmp(message,"sär")==0) sar++;
    if(found==true && strcmp(message,"stavning")==0) stavning++;
    if(found==true && strcmp(message,"ljud")==0) ljud++;
    if(found==true && strcmp(message,"dom")==0) dom++;
    if(found==true && strcmp(message,"pronomen")==0) pronomen++;
    if(found==true && strcmp(message,"kong")==0) kong++;
    if(found==true && strcmp(message,"verb")==0) verb++;
    if(found==true && strcmp(message,"tempus")==0) tempus++;
    if(found==true && strcmp(message,"frågetecken")==0) fragetecken++;
    if(found==true && strcmp(message,"utropstecken")==0) utropstecken++;
    if(found==true && strcmp(message,"egennamn")==0) egennamn++;
    if(found==true && strcmp(message,"nymening")==0) nymening++;
    if(found==true && strcmp(message,"gemen")==0) gemen++;
    if(found==true && strcmp(message,"ordbi")==0) ordbi++;
    if(found==true && strcmp(message,"ordind")==0) ordind++;
    if(found==true && strcmp(message,"kompsuffix")==0) kompsuffix++;
    if(found==true && strcmp(message,"kompperi")==0) kompperi++;
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
	if(sar==55) cout << "Category Särskrivning:\t\t\t OK" << endl;
	else cout << "Category Särskrivning:\t\t\t FAILED: "<<sar<<"/55"<< endl;
	if(stavning==21) cout << "Category Stavning:\t\t\t OK" << endl;
	else cout << "Category Stavning:\t\t\t FAILED: "<<stavning<<"/21" << endl;
	if(ljud==10) cout << "Category ljudposter:\t\t\t OK" << endl;
	else cout << "Category ljudposter:\t\t\t FAILED: "<<ljud<<"/10" << endl;
	if(dom==7) cout << "Category De/Dem:\t\t\t OK" << endl;
	else cout << "Category De/Dem:\t\t\t FAILED: "<<dom<<"/7" << endl;
	if(pronomen==1) cout << "Category Pronomen:\t\t\t OK" << endl;
	else cout << "Category Pronomen:\t\t\t FAILED: "<<pronomen<<"/1" << endl;
	if(kong==35) cout << "Category Kongruens:\t\t\t OK" << endl;
	else cout << "Category Kongruens:\t\t\t FAILED: "<<kong<<"/35" << endl;
	if(verb==17) cout << "Category Verb:\t\t\t\t OK" << endl;
	else cout << "Category Verb:\t\t\t\t FAILED: "<<verb<<"/17" << endl;
	if(tempus==11) cout << "Category Tempus:\t\t\t OK" << endl;
	else cout << "Category Tempus:\t\t\t FAILED: "<<tempus<<"/11" << endl;
	if(fragetecken==5) cout << "Category Frågetecken:\t\t\t OK" << endl;
	else cout << "Category Frågetecken:\t\t\t FAILED: "<<fragetecken<<"/5" << endl;
	if(utropstecken==2) cout << "Category Utropstecken:\t\t\t OK" << endl;
	else cout << "Category Utropstecken:\t\t\t FAILED: "<<utropstecken<<"/2" << endl;
	if(egennamn==16) cout << "Category Egennamn:\t\t\t OK" << endl;
	else cout << "Category Egennamn:\t\t\t FAILED: "<<egennamn<<"/16" << endl;
	if(nymening==5) cout << "Category Nymening:\t\t\t OK" << endl;
	else cout << "Category Nymening:\t\t\t FAILED: "<<nymening<<"/5" << endl;
	if(gemen==3) cout << "Category Gemener:\t\t\t OK" << endl;
	else cout << "Category Gemener:\t\t\t FAILED: "<<gemen<<"/3" << endl;
	if(ordbi==3) cout << "Category Ordföljd i bisats:\t\t OK" << endl;
	else cout << "Category Ordföljd i bisats:\t\t FAILED: "<<ordbi<<"/3" << endl;
	if(ordind==2) cout << "Category Ordföljd i indirekt frågesats:\t OK" << endl;
	else cout << "Category Ordföljd i indirekt frågesats:\t FAILED: "<<ordind<<"/2" << endl;
	if(kompsuffix==3) cout << "Category Suffixkomparation:\t\t OK" << endl;
	else cout << "Category Suffixkomparation:\t\t FAILED: "<<kompsuffix<<"/2" << endl;
	if(kompperi==7) cout << "Category Perifrasktisk komparation:\t OK" << endl;
	else cout << "Category Perifrasktisk komparation:\t FAILED: "<<kompperi<<"/7" << endl;
}

void XMLHandler::fatalError(const SAXParseException& exception)
{
    char* message = XMLString::transcode(exception.getMessage());
    cout << "Fatal Error: " << message
         << " at line: " << exception.getLineNumber()
         << endl;
    XMLString::release(&message);
}
