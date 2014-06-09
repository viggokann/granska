/* Program to test the correctness of the granska library.
 * First argument is the file name to be scrutinized.
 * The second argument is the number of times it is
 * to be scrutnized.
 */

#include <granskaapi.h>
#include <fstream>
#include <cstring>
#include <cstdlib>

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
	strcpy(input, text.c_str());
	for(int i = 0; i < atoi(argv[2]); i++){
		char* o = granska(input);
		printf("%s", o);
		delete[] o;
	}
	delete[] input;
		
	return 0;
}
