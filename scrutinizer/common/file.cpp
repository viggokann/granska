/* file.cpp
 * author: Johan Carlberger
 * last change: 990820
 * comments:
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "file.h"
#include "message.h"

const char *AddFileName(char *newFile, const char *dir, const char *file) {
  if (dir)
    strcpy(newFile, dir);
  else {
    newFile[0] = '\0';
    if (!file)
      Message(MSG_WARNING, "trying to create empty file name");
  }
  if (file) {
    // jonas, 13 rows of new code to always have exactly 1 slash between directory levels
    int l = strlen(newFile);      
#ifdef WIN32
    if(l > 0 && newFile[l-1] != '\\')
      strcat(newFile, "\\");
    if(file && file[0] == '\\')
#else
    if(l > 0 && newFile[l-1] != '/')
      strcat(newFile, "/");
    if(file && file[0] == '/')
#endif
      strcat(newFile, file+1); // statement of if-statement in #ifdef
    else
      strcat(newFile, file);
  }
  return newFile;
}

const char *Extension(const char *fileName) {
  for (int i = strlen(fileName); i>0; i--)
    if (fileName[i] == '.')
      return fileName + i + 1;
    else if (fileName[i] == '/')
      return "";
  return "";
}

bool FixIfstream(std::ifstream &s, const char *dir, const char *file, bool warn) {
  s.close();
  s.clear();
  char newFile[MAX_FILE_NAME_LENGTH];	// jb: nocreate is not in the standard
  s.open(AddFileName(newFile, dir, file), std::ios::in | /*std::ios::nocreate |*/ std::ios::binary );
  if (warn && s.fail())
    Message(MSG_WARNING, "cannot open file", newFile);
  return !s.fail();
}

bool FixOfstream(std::ofstream &s, const char *dir, const char *file) {
  s.close();
  s.clear();
  char newFile[MAX_FILE_NAME_LENGTH];
  s.open(AddFileName(newFile, dir, file), std::ios::out | std::ios::binary );
  if (s.fail())
    Message(MSG_WARNING, "cannot open file", newFile);
  return !s.fail();
}

bool CompareLabels(const char *labelRead, const char *labelWanted, bool warn) {
  if (strcmp(labelWanted, labelRead)) {
    if (warn)
      Message(MSG_WARNING, "label wanted:", labelWanted, "; label read:", labelRead);
    return false;
  }
  return true;
}

void SetVersion(std::ofstream &out, const char* label) {
  out << label << std::endl;
}

bool CheckVersion(std::ifstream& in, const char* label) {
  return CheckLabel(in, label, false);
}

bool CheckLabel(std::ifstream& in, const char *labelWanted, bool warn) {
  char labelRead[MAX_FILE_NAME_LENGTH], c;
  while (isspace(in.peek()))
    in.get(c);
  in.getline(labelRead,  MAX_FILE_NAME_LENGTH);
  while (isspace(in.peek()))
    in.get(c);
  return CompareLabels(labelRead, labelWanted, warn);
}

void WriteData(std::ofstream &out, const char *p, int size, const char *varName) {
  if (varName)
    out << varName << ' ';
  //  Message(MSG_STATUS, "writing data", varName, int2str(size));
  out.write(p, size) << ' ';
}

void ReadData(std::ifstream &in, char *p, int size, const char *varName) {
  if (varName) {
    //    Message(MSG_STATUS, "reading data", varName, int2str(size));
    char varNameRead[100];
    in >> varNameRead;
    if (in.get() != ' ')
      Message(MSG_ERROR, "cannot read data from file (error 1)");
    if (!CompareLabels(varNameRead, varName))
      Message(MSG_ERROR, "cannot read data from file (error 2)");
  }
  in.read(p, size);
  if (in.get() != ' ')
    Message(MSG_ERROR, "cannot read data from file (error 3)");
}


