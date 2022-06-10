#pragma once
#include <vector>
#include "_main.hxx"

using std::vector;




template <class T>
vector<T> lorenzCurve(const vector<T>& x) {
  // ignore rank of 0th vertex
  vector<T> y(x.begin()+1, x.end()); sortValues(y);
  vector<T> a(100); T total = sumValues(y);
  for (size_t i=0; i<a.size(); i++) {
    size_t n = (i+1) * y.size() / a.size();
    a[i] = sumValues(y, 0, n) / total;
  }
  return a;
}


template <class T>
T giniCoefficient(const vector<T>& lc) {
  T total = T(), ineq = T();
  for (size_t i=0; i<lc.size(); i++) {
    T ideal = T(i+1)/lc.size();
    total += ideal;
    ineq  += ideal - lc[i];
  }
  return ineq / total;
}
