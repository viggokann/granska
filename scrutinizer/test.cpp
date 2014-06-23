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
	char str [100000];
	fp = fopen(argv[1], "r");
	char c;
	int i = 0;
	while((c = (char)getc(fp)) !=EOF){ 
		if(i < 100000){
			str[i] = c; 
			i++;
		}
		else{
			printf("File too big\n");
			return -1;
		} 
	}   
	fclose(fp);
	char* first, *last, *ret;
	for(int i = 0; i < atoi(argv[2]); i++){
		ret = granska(str);
		if(i==0) first = ret; 
		else if(i==(atoi(argv[2])-1)) last = ret; 
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
