#include <granskaapi.h>
#include <fstream>
#include <cstring>

using namespace std;

int main(int argc, char** argv){
	loadGranska();
	for(int i = 1; i < 3 ; i++){
		ifstream infile(argv[i]);
		string line;
		string text;
		while (getline(infile, line))
		{
			text.append(line);
			
		}
		infile.close();
		char *input = new char[text.length()+1];
		strcpy(input, text.c_str());
		const char* o = granska(input);
		printf("%s", o);
		delete [] input;
	}
	return 0;
}
