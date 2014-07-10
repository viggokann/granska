/* Program to test the correctness of the granska library.
 * First argument is the file name to be scrutinized.
 * The second argument is the number of times it is
 * to be scrutnized.
 */

#include "granskaapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
	loadGranska();
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
	int size = atoi(argv[2]);
	char* first, *last, *ret;
	for(int i = 0; i < size; i++){
		ret = granska(str);
		if(i==0) first = ret; 
		else if(i==(size-1)) last = ret; 
	}
	//printf("%s", last);
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
	delete[] ret;
		
	return 0;
}
