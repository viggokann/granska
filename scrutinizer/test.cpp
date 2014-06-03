#include "granskaapi.h"
#include "stdio.h"

using namespace std;
int main(int argc, char** argv){
	loadGranska();
	granska(argv[1]);
	return 0;
}
