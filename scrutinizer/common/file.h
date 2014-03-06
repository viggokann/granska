/* file.hh
 * author: Johan Carlberger
 * last change: 990818
 * comments: some useful procedures for file input and output
 */

#ifndef _file_hh
#define _file_hh

#include <iostream>
#include <ctype.h>
#include <fstream>
//#include "basics.h"
#include "ensure.h"
#include "message.h"

const int MAX_FILE_NAME_LENGTH = 200;

inline void SkipSpaceButNotNewLine(std::ifstream &in) {
  while (isspace(in.peek()) && in.peek() != '\n')
    in.get();
}

const char *Extension(const char *fileName);
const char *AddFileName(char *newFile, const char *dir, const char *file);
bool CompareLabels(const char *labelRead, const char *labelWanted, bool warn = true);
bool FixIfstream(std::ifstream&, const char *dir, const char *file = NULL, bool warn = true);
bool FixOfstream(std::ofstream&, const char *dir, const char *file = NULL);
// opens the stream on "dir/file"

#define WriteVar(a, b) WriteVariable(a, #b, b)
template <class T>
void WriteVariable(std::ostream &out, const char *varName, T val) {
  out << varName << ' ' << val << '\n';
}

#define ReadVar(a, b) ReadVariable(a, #b, &b)
template <class T>
T ReadVariable(std::istream& in, const char *varNameWanted, T* val) {
  char varNameRead[MAX_FILE_NAME_LENGTH];
  varNameRead[0] = '\0';
  in >> varNameRead >> *val;
  if (!CompareLabels(varNameRead, varNameWanted))
    Message(MSG_ERROR, "variable name wanted:", varNameWanted,
	    "; variable name read:", varNameRead);
  if (in.get() != '\n')
    Message(MSG_ERROR, "while reading varibale", varNameWanted);
  return *val;
}

bool CheckLabel(std::ifstream&, const char *labelWanted, bool warn = true);
// reads and checks a label from the stream

bool CheckVersion(std::ifstream&, const char *label);
// reads and checks a version label from the stream

void SetVersion(std::ofstream&, const char *label);
// writes a version label to the stream

void WriteData(std::ofstream&, const char *p, int size, const char *varName=NULL);
// writes labelled data to the stream

void ReadData(std::ifstream&, char *p, int size, const char *varName=NULL);
// reads labelled data from the stream

#endif



