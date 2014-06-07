#include "granskaapi.h"
#include "stdio.h"

using namespace std;
int main(int argc, char** argv){
	loadGranska();
	const char* o = granska((const char*) argv[1]	);
	printf("%s", o);
	return 0;
}
