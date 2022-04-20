#pragma once
#include <string>

using std::string;
using std::to_string;




// STRINGIFY
// ---------

template <class T>
string stringify(T x) {
  return to_string(x);
}

string stringify(string x) {
  string a("\"");
  a += x;
  a += "\"";
  return a;
}




// COUNT-LINES
// -----------
// For counting temporal edges.

size_t countLines(const char* x) {
  size_t a = 1;
  for (; *x; x++) {
    if (*x == '\r' || *x == '\n') ++a;
    else if (*x == '\r' && *(x+1) == '\n') ++x;
  }
  return a;
}
size_t countLines(const string& x) {
  return countLines(x.c_str());
}
