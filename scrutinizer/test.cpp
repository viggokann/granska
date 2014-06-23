/* Program to test the correctness of the granska library.
 * First argument is the file name to be scrutinized.
 * The second argument is the number of times it is
 * to be scrutnized.
 */

#include "granskaapi.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>

using namespace std;

int main(int argc, char** argv){
	loadGranska();
	ifstream infile(argv[1]);
	string line;
	string text;
	while (getline(infile, line))
	{
		text.append(line);
		
	}
	infile.close();
	char *input = new char[text.length()+1];
	char* first = new char[text.length()+1];
	char* last = new char[text.length()+1];
	int size = atoi(argv[2]);
	strcpy(input, text.c_str());
	for(int i = 0; i < size; i++){
		char* o1 = granska(input);
		char* o2 = granska(input);
		if(i==0){ first = o1; delete[] o2;}
		else if(i==(size-1)){last = o2; delete[] o1;}
		else{
			delete[] o1;
			delete[] o2;
		}
	}
	printf("%s", last);
	if(strcmp(first,last)==0) {
			printf("\n--------------------------------------\n");
			printf("\n           SUCCESS                    \n");
			printf("\n--------------------------------------\n");
	}
	else{
		printf("\n--------------------------------------\n");
		printf("\n           FAIL                       \n");
		printf("\n--------------------------------------\n");
		return 0;
	}
	delete[] first;
	delete[] last;
	delete[] input;
		
	return 0;
}
