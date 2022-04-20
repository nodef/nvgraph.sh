#pragma once
#include <string>
#include <ostream>
#include "_main.hxx"

using std::string;
using std::ostream;




// WRITE-BEGIN
// -----------

void writeBeginYaml(ostream& a) {}

void writeBeginJson(ostream& a) {
  a << "{\n";
}

void writeBegin(ostream& a, string fmt) {
  if (fmt=="yaml") writeBeginYaml(a);
  else writeBeginJson(a);
}




// WRITE-END
// ---------

void writeEndYaml(ostream& a) {}

void writeEndJson(ostream& a) {
  a.seekp(-2, a.cur);
  a << "\n}\n";
}

void writeEnd(ostream& a, string fmt) {
  if (fmt=="yaml") writeEndYaml(a);
  else writeEndJson(a);
}




// WRITE-VALUE
// -----------

template <class T>
void writeValueYaml(ostream& a, string key, T v) {
  if (!key.empty()) a << key << ": ";
  a << stringify(v) << (key.empty()? "\n" : ",\n");
}

template <class T>
void writeValueJson(ostream& a, string key, T v) {
  if (!key.empty()) a << "  \"" << key << "\": ";
  a << stringify(v) << (key.empty()? "\n" : ",\n");
}

template <class T>
void writeValue(ostream& a, string key, T v, string fmt) {
  if (fmt=="yaml") writeValueYaml(a, key, v);
  else writeValueJson(a, key, v);
}




// WRITE-VALUES
// ------------

template <class I>
void writeValuesYaml(ostream& a, string key, I&& vs) {
  if (!key.empty()) a << key << ":\n";
  for (auto v : vs)
    a << "- " << stringify(v) << '\n';
}

template <class I>
void writeValuesJson(ostream& a, string key, I&& vs) {
  if (!key.empty()) a << "  \"" << key << "\": ";
  string pre = key.empty()? "  " : "    ";
  a << "[\n";
  auto b = a.tellp();
  for (auto v : vs)
    a << pre << stringify(v) << ",\n";
  if (a.tellp()!=b) a.seekp(-2, a.cur);
  a << '\n' << pre.substr(2) << (key.empty()? "]\n" : "],\n");
}

template <class I>
void writeValues(ostream& a, string key, I&& vs, string fmt) {
  if (fmt=="yaml") writeValuesYaml(a, key, vs);
  else writeValuesJson(a, key, vs);
}
