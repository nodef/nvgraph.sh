#pragma once
#include <string>
#include <ostream>
#include "_debug.hxx"

using std::string;
using std::ostream;
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

template <class T>
void writeString(ostream& a, T x) {
  a << x;
}

void writeString(ostream& a, const string& x) {
  a << '\"' << x << '\"';
}



// COUNT LINES
// -----------
// For counting temporal edges.

size_t countLines(const char* x) {
  ASSERT(x);
  size_t a = 1;
  for (; *x; x++) {
    if (*x == '\r' || *x == '\n') ++a;
    else if (*x == '\r' && *(x+1) == '\n') ++x;
  }
  return a;
}
inline size_t countLines(const string& x) {
  return countLines(x.c_str());
}
