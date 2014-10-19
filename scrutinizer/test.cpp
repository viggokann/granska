/* Program to test the correctness of the granska library.
 * First argument is the file name to be scrutinized.
 * The second argument is the number of times it is
 * to be scrutnized.
 */

#include "granskaapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <iostream> 
#include "XMLHandler.hpp"

using namespace xercesc;
using namespace std;

int main(int argc, char** argv){
	//load granska
	loadGranska();	
	//testfile to read from
	FILE* fp;
	char str [1000000];
	fp = fopen(argv[1], "r");
	char c;
	int i = 0;
	while((c = (char)getc(fp)) !=EOF){ 
		if(i < 1000000){
			str[i] = c; 
			i++;
		}
		else{
			printf("File too big (For this test), change the test.cpp file for bigger files\n");
			return -1;
		} 
	}   
	fclose(fp);
	
	int size = 10;
	char* first, *last, *ret;
	for(int i = 0; i < size; i++){
		//call granska and store XML in ret
		ret = granska(str);
		if(i==0) first = ret; 
		else if(i==(size-1)) last = ret;
	}
	printf("\n--------------------------------\n");
	printf("\nRunning tests on ../rulesets/wille/regelsamling.ver8.testfil:\n\n");
	if(strcmp(first,last)==0) {
			printf("\nReproducing test:\t\t\t OK \n");
	}
	else{
		printf("\nReproducing test: FAILED \n");
		return 0;
	}
	if(argc==3 && strcmp(argv[2],"-p")==0) printf("%s", ret);
	
	//Write an xml file
	FILE* fp2;
	fp2 = fopen("x1.xml", "w");
	fputs(first, fp2);
	fclose(fp2);
	
	//Assert that granska output is OK.

	try {
		XMLPlatformUtils::Initialize();
	}
		catch (const XMLException& toCatch) {
		// Do your failure processing here
		return 1;
	}
	char* xmlFile = "x1.xml";
	SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
	parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
	
	XMLHandler defaultHandler;
	parser->setContentHandler(&defaultHandler);
	parser->setErrorHandler(&defaultHandler);
	
	try {
            parser->parse(xmlFile);
        }
        catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            cout << "XMLException message is: \n"
                 << message << "\n";
            XMLString::release(&message);
            return -1;
        }
        catch (...) {
            cout << "Unexpected Exception \n" ;
            return -1;
        }
	XMLPlatformUtils::Terminate();
	printf("\n--------------------------------\n");
	return 0;
}
