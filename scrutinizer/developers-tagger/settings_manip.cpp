#include "settings_manip.h"
#include <fstream>
#include "message.h"
#include "file.h"
#include "settings.h"

/* copy-pasted readsettings from obsolete settings.cc */
bool ReadSettings(const char *dir, const char *file) {
  Message(MSG_STATUS, "loading settings...");
  std::ifstream in;
  if (!FixIfstream(in, dir, file, false))
    return false;
  int result;
  ReadVar(in, result);
  ReadVar(in, xTagTrigramsUsed);
  ReadVar(in, xMorfCommonSuffix);
  ReadVar(in, xMorfCapital);
  ReadVar(in, xMorfNonCapital);
  ReadVar(in, xAmbiguousNewWords);
  ReadVar(in, xNewWordsMemberTaggingOnly);
  ReadVar(in, xMaxLastChars);
  ReadVar(in, xMinLastChars);
  ReadVar(in, xTaggingEquation);
  ReadVar(in, xLambda19);
  ReadVar(in, xLambdaUni);
  ReadVar(in, xLambdaBi);
  ReadVar(in, xLambdaTri);
  ReadVar(in, xLambdaTriExp);
  ReadVar(in, xEpsilonTri);
  ReadVar(in, xLambdaExtra);
  ReadVar(in, xAlphaExtra);
  ReadVar(in, xEpsilonExtra);
  ReadVar(in, xNWordVersions);
  ReadVar(in, xNNewWordVersions);
  ReadVar(in, xAlphaLastChar[1]);
  ReadVar(in, xAlphaLastChar[2]);
  ReadVar(in, xAlphaLastChar[3]);
  ReadVar(in, xAlphaLastChar[4]);
  ReadVar(in, xAlphaLastChar[5]);
  ReadVar(in, xAlphaLastChar[6]);
  ReadVar(in, xAlphaSuffix);
  ReadVar(in, xAlphaMember);
  ReadVar(in, xAlphaCapital);
  ReadVar(in, xAlphaNonCapital);
  ReadVar(in, xAlphaUnknownCapital);
  ReadVar(in, xAlphaUnknownNonCapital);
  ReadVar(in, xNewParameter);
  return true;
}
/* copy-pasted readsettings ends here */

/* copy-pasted writesettings from obsolete settings.cc */
bool WriteSettings(const char *dir, const char *file, int result) {
  Message(MSG_STATUS, "saving settings...");
  std::ofstream out;
  if (!FixOfstream(out, dir, file))
    return false;
  WriteVar(out, result);
  WriteVar(out, xTagTrigramsUsed);
  WriteVar(out, xMorfCommonSuffix);
  WriteVar(out, xMorfCapital);
  WriteVar(out, xMorfNonCapital);
  WriteVar(out, xAmbiguousNewWords);
  WriteVar(out, xNewWordsMemberTaggingOnly);
  WriteVar(out, xMaxLastChars);
  WriteVar(out, xMinLastChars);
  WriteVar(out, xTaggingEquation);
  WriteVar(out, xLambda19);
  WriteVar(out, xLambdaUni);
  WriteVar(out, xLambdaBi);
  WriteVar(out, xLambdaTri);
  WriteVar(out, xLambdaTriExp);
  WriteVar(out, xEpsilonTri);
  WriteVar(out, xLambdaExtra);
  WriteVar(out, xAlphaExtra);
  WriteVar(out, xEpsilonExtra);
  WriteVar(out, xNWordVersions);
  WriteVar(out, xNNewWordVersions);
  WriteVar(out, xAlphaLastChar[1]);
  WriteVar(out, xAlphaLastChar[2]);
  WriteVar(out, xAlphaLastChar[3]);
  WriteVar(out, xAlphaLastChar[4]);
  WriteVar(out, xAlphaLastChar[5]);
  WriteVar(out, xAlphaLastChar[6]);
  WriteVar(out, xAlphaSuffix);
  WriteVar(out, xAlphaMember);
  WriteVar(out, xAlphaCapital);
  WriteVar(out, xAlphaNonCapital);
  WriteVar(out, xAlphaUnknownCapital);
  WriteVar(out, xAlphaUnknownNonCapital);
  WriteVar(out, xNewParameter);
  return true;
}
/* copy-pasted writesettings ends here*/
