#pragma once
#include <utility>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "_main.hxx"

using std::pair;
using std::vector;
using std::out_of_range;
using std::iter_swap;
using std::find_if;
using std::lower_bound;
using std::sort;
using std::inplace_merge;




// BITSET-*
// --------
// Helps create bitsets.

#ifndef BITSET_TYPES
#define BITSET_TYPES(K, V, data) \
  using key_type   = K; \
  using value_type = V; \
  using pair_type  = pair<K, V>; \
  using iterator       = decltype(data.begin()); \
  using const_iterator = decltype(data.cbegin());
#endif


#ifndef BITSET_ITERATOR
#define BITSET_ITERATOR(K, V, data) \
  ITERABLE_ITERATOR(inline, noexcept, data.begin(), data.end())

#define BITSET_CITERATOR(K, V, data) \
  ITERABLE_CITERATOR(inline, noexcept, data.begin(), data.end())
#endif


#ifndef BITSET_SIZE
#define BITSET_SIZE(K, V, data)  \
  inline size_t size() const noexcept { return data.size(); }

#define BITSET_EMPTY(K, V) \
  inline bool empty()  const noexcept { return size() == 0; }
#endif


#ifndef BITSET_FCOMPARE
#define BITSET_FCOMPARE(K, V, fn, op) \
  auto fn = [](const pair<K, V>& p, const pair<K, V>& q) { return p.first op q.first; };
#define BITSET_FCOMPARES(K, V) \
  BITSET_FCOMPARE(K, V, fl, <) \
  BITSET_FCOMPARE(K, V, fe, ==)
#endif


#ifndef BITSET_FIND
#define BITSET_FIND(K, V, data) \
  inline auto find(const K& k)        noexcept { return locate_match(k); } \
  inline auto cfind(const K& k) const noexcept { return locate_match(k); } \
  inline auto find(const K& k)  const noexcept { return cfind(k); }

#define BITSET_ENTRIES(K, V, data) \
  /* dont change the keys! */ \
  inline auto values() noexcept { \
    return staticTransformIterable(data, PairSecond<K, V>()); \
  } \
  inline auto pairs() noexcept { \
    return iterable(data); \
  } \
  inline auto ckeys() const noexcept { \
    return staticTransformIterable(data, ConstPairFirst<K, V>()); \
  } \
  inline auto cvalues() const noexcept { \
    return staticTransformIterable(data, ConstPairSecond<K, V>()); \
  } \
  inline auto cpairs() const noexcept { \
    return iterable(data); \
  } \
  inline auto keys()   const noexcept { return ckeys(); } \
  inline auto values() const noexcept { return cvalues(); } \
  inline auto pairs()  const noexcept { return cpairs(); }

#define BITSET_FOREACH(K, V, data) \
  /* dont change the keys! */ \
  template <class F> \
  inline void forEachValue(F fn) { \
    for (pair<K, V>& p : data) fn(p.second); \
  } \
  template <class F> \
  inline void forEachPair(F fn) { \
    for (pair<K, V>& p : data) fn(p); \
  } \
  template <class F> \
  inline void forEach(F fn) { \
    for (pair<K, V>& p : data) fn(p.first, p.second); \
  } \
  template <class F> \
  inline void cforEachKey(F fn) const { \
    for (const pair<K, V>& p : data) fn(p.first); \
  } \
  template <class F> \
  inline void cforEachValue(F fn) const { \
    for (const pair<K, V>& p : data) fn(p.second); \
  } \
  template <class F> \
  inline void cforEachPair(F fn) const { \
    for (const pair<K, V>& p : data) fn(p); \
  } \
  template <class F> \
  inline void cforEach(F fn) const { \
    for (const pair<K, V>& p : data) fn(p.first, p.second); \
  } \
  template <class F> \
  inline void forEachKey(F fn)   const { cforEachKey(fn); } \
  template <class F> \
  inline void forEachValue(F fn) const { cforEachValue(fn); } \
  template <class F> \
  inline void forEachPair(F fn)  const { cforEachPair(fn); } \
  template <class F> \
  inline void forEach(F fn)      const { cforEach(fn); }
#endif


#ifndef BITSET_HAS
#define BITSET_HAS(K, V) \
  inline bool has(const K& k) const noexcept { \
    return locate_match(k) != end(); \
  }

#define BITSET_GET(K, V) \
  inline V get(const K& k) const noexcept { \
    auto it = locate_match(k); \
    return it == end()? V() : (*it).second; \
  }

#define BITSET_SET(K, V) \
  inline bool set(const K& k, const V& v) noexcept { \
    auto it = locate_match(k); \
    if (it == end()) return false; \
    (*it).second = v; \
    return true; \
  }

#define BITSET_SUBSCRIPT(K, V) \
  inline V& operator[](const K& k) noexcept { \
    auto it = locate_match(k); \
    return (*it).second; \
  } \
  inline const V& operator[](const K& k) const noexcept { \
    auto it = locate_match(k); \
    return (*it).second; \
  }

#define BITSET_AT(K, V) \
  inline V& at(const K& k) { \
    auto it = locate_match(k); \
    if (it == end()) throw out_of_range("bitset key not present"); \
    return (*it).second; \
  } \
  inline const V& at(const K& k) const { \
    auto it = locate_match(k); \
    if (it == end()) throw out_of_range("bitset key not present"); \
    return (*it).second; \
  }
#endif


#define BITSET_CORRECT_NONE(K, V) \
  inline bool correct(bool unq, vector<pair<K, V>>& buf) { \
    return false; \
  } \
  inline bool correct(bool unq=false) { \
    return false; \
  }




// UNOREDERD-BITSET
// ----------------
// An integer set that constantly checks duplicates.
// It maintains integers in insertion order.

#define UNORDERED_BITSET_LOCATE(K, V, f0, f1) \
  f0 auto locate_match(const K& k) f1 { \
    auto fe = [&](const pair<K, V>& p) { return p.first == k; }; \
    return find_if(begin(), end(), fe); \
  }


template <class K=int, class V=NONE>
class UnorderedBitset {
  // Data.
  protected:
  vector<pair<K, V>> data;

  // Types.
  public:
  BITSET_TYPES(K, V, data)


  // Iterator operations.
  public:
  BITSET_ITERATOR(K, V, data)
  BITSET_CITERATOR(K, V, data)


  // Size operations.
  public:
  BITSET_SIZE(K, V, data)
  BITSET_EMPTY(K, V)


  // Search operations.
  protected:
  UNORDERED_BITSET_LOCATE(K, V, inline, noexcept)
  UNORDERED_BITSET_LOCATE(K, V, inline, const noexcept)
  public:
  BITSET_FIND(K, V, data)


  // Access operations.
  public:
  BITSET_ENTRIES(K, V, data)
  BITSET_FOREACH(K, V, data)
  BITSET_HAS(K, V)
  BITSET_GET(K, V)
  BITSET_SET(K, V)
  BITSET_SUBSCRIPT(K, V)
  BITSET_AT(K, V)


  // Update operations.
  public:
  BITSET_CORRECT_NONE(K, V)
  inline bool clear() noexcept {
    if (empty()) return false;
    data.clear();
    return true;
  }

  inline bool add(const K& k, const V& v=V()) {
    if (has(k)) return false;
    data.push_back({k, v});
    return true;
  }

  inline bool remove(const K& k) {
    auto it = locate_match(k);
    if (it == end()) return false;
    iter_swap(it, end()-1);
    data.pop_back();
    return true;
  }
};

template <class K=int, class V=NONE>
inline auto unorderedBitset(K _k=K(), V _v=V()) {
  return UnorderedBitset<K, V>();
}




// BITSET (SORTED)
// ---------------
// An integer set that constantly checks duplicates.
// It maintains integers in ascending value order.

#define ORDERED_BITSET_LOCATE(K, V, f0, f1) \
  f0 auto locate_spot(const K& k) f1 { \
    auto fl = [](const pair<K, V>& p, const K& k) { return p.first < k; }; \
    return lower_bound(begin(), end(), k, fl); \
  } \
  f0 auto locate_match(const K& k) f1 { \
    auto it = locate_spot(k); \
    return it == end() || (*it).first != k? end() : it; \
  }


template <class K=int, class V=NONE>
class OrderedBitset {
  // Data.
  protected:
  vector<pair<K, V>> data;

  // Types.
  public:
  BITSET_TYPES(K, V, data)


  // Iterator operations.
  public:
  BITSET_ITERATOR(K, V, data)
  BITSET_CITERATOR(K, V, data)


  // Size operations.
  public:
  BITSET_SIZE(K, V, data)
  BITSET_EMPTY(K, V)


  // Search operations.
  protected:
  ORDERED_BITSET_LOCATE(K, V, inline, noexcept)
  ORDERED_BITSET_LOCATE(K, V, inline, const noexcept)
  public:
  BITSET_FIND(K, V, data)


  // Access operations.
  public:
  BITSET_ENTRIES(K, V, data)
  BITSET_FOREACH(K, V, data)
  BITSET_HAS(K, V)
  BITSET_GET(K, V)
  BITSET_SET(K, V)
  BITSET_SUBSCRIPT(K, V)
  BITSET_AT(K, V)


  // Update operations.
  public:
  BITSET_CORRECT_NONE(K, V)
  inline bool clear() noexcept {
    if (empty()) return false;
    data.clear();
    return true;
  }

  inline bool add(const K& k, const V& v=V()) {
    auto it = locate_spot(k);
    if (it != end() && (*it).first == k) return false;
    data.insert(it, {k, v});
    return true;
  }

  inline bool remove(const K& k) {
    auto it = locate_match(k);
    if (it == end()) return false;
    data.erase(it);
    return true;
  }
};

template <class K=int, class V=NONE>
inline auto orderedBitset(K _k=K(), V _v=V()) {
  return OrderedBitset<K, V>();
}




// PORDERED-BITSET (PARTIALLY-SORTED)
// ----------------------------------
// An integer set that constantly checks duplicates.
// It maintains a portion of integers in ascending value order.

#define PORDERED_BITSET_LOCATE(K, V, f0, f1) \
  f0 auto locate_match_ordered(const K& k) f1 { \
    auto fl = [](const pair<K, V>& p, const K& k) { return p.first < k; }; \
    auto it = lower_bound(begin(), middle(), k, fl); \
    return it == middle() || (*it).first != k? end() : it; \
  } \
  f0 auto locate_match_unordered(const K& k) f1 { \
    auto fe = [&](const pair<K, V>& p) { return p.first == k; }; \
    return find_if(middle(), end(), fe); \
  } \
  f0 auto locate_match(const K& k) f1 { \
    auto it = locate_match_ordered(k); \
    return it != end()? it : locate_match_unordered(k); \
  }


template <class K=int, class V=NONE, size_t LIMIT=64>
class POrderedBitset {
  // Data.
  protected:
  vector<pair<K, V>> data;
  size_t ordered = 0;

  // Types.
  public:
  BITSET_TYPES(K, V, data)


  // Iterator operations.
  public:
  BITSET_ITERATOR(K, V, data)
  BITSET_CITERATOR(K, V, data)
  protected:
  ITERABLE_NAMES(inline, noexcept, middle, begin() + ordered)


  // Size operations.
  public:
  BITSET_SIZE(K, V, data)
  BITSET_EMPTY(K, V)
  protected:
  inline size_t unordered() const noexcept { return size() - ordered; }


  // Search operations.
  protected:
  PORDERED_BITSET_LOCATE(K, V, inline, noexcept)
  PORDERED_BITSET_LOCATE(K, V, inline, const noexcept)
  public:
  BITSET_FIND(K, V, data)


  // Ordering opertions.
  protected:
  inline void mergePartitions() {
    BITSET_FCOMPARE(K, V, fl, <)
    sort(middle(), end(), fl);
    inplace_merge(begin(), middle(), end(), fl);
    ordered = size();
  }


  // Access operations.
  public:
  BITSET_ENTRIES(K, V, data)
  BITSET_FOREACH(K, V, data)
  BITSET_HAS(K, V)
  BITSET_GET(K, V)
  BITSET_SET(K, V)
  BITSET_SUBSCRIPT(K, V)
  BITSET_AT(K, V)


  // Update operations.
  public:
  inline bool correct(bool unq=false) {
    if (unordered() == 0) return false;
    mergePartitions();
    return true;
  }
  inline bool correct(bool unq, vector<pair<K, V>>& buf) { return correct(); }

  inline bool clear() noexcept {
    if (empty()) return false;
    data.clear();
    ordered = 0;
    return true;
  }

  inline bool add(const K& k, const V& v=V()) {
    auto it = locate_match(k);
    if (it != end()) return false;
    data.push_back({k, v});
    if (unordered() <= LIMIT) mergePartitions();
    return true;
  }

  inline bool remove(const K& k) {
    auto it = locate_match(k);
    if (it == end()) return false;
    if (it < middle()) --ordered;
    data.erase(it);
    return true;
  }
};

template <class K=int, class V=NONE>
inline auto porderedBitset(K _k=K(), V _v=V()) {
  return POrderedBitset<K, V, 64>();
}




// RORDERED-BITSET (RISKY!)
// ------------------------
// An integer set that does not check duplicates.
// Removing duplicates can be done manually, with correct().
// It maintains integers in ascending value order (after correct()).

#define RORDERED_BITSET_LOCATE(K, V, f0, f1) \
  PORDERED_BITSET_LOCATE(K, V, f0, f1)


template <class K=int, class V=NONE>
class ROrderedBitset {
  // Data.
  protected:
  vector<pair<K, V>> data;
  size_t ordered = 0;

  // Types.
  public:
  BITSET_TYPES(K, V, data)


  // Iterator operations.
  public:
  BITSET_ITERATOR(K, V, data)
  BITSET_CITERATOR(K, V, data)
  protected:
  ITERABLE_NAMES(inline, noexcept, middle, begin() + ordered)


  // Size operations.
  public:
  BITSET_SIZE(K, V, data)
  BITSET_EMPTY(K, V)


  // Search operations.
  protected:
  RORDERED_BITSET_LOCATE(K, V, inline, noexcept)
  RORDERED_BITSET_LOCATE(K, V, inline, const noexcept)
  public:
  BITSET_FIND(K, V, data)


  // Access operations.
  public:
  BITSET_ENTRIES(K, V, data)
  BITSET_FOREACH(K, V, data)
  BITSET_HAS(K, V)
  BITSET_GET(K, V)
  BITSET_SET(K, V)
  BITSET_SUBSCRIPT(K, V)
  BITSET_AT(K, V)


  // Update operations.
  public:
  inline bool correct(bool unq, vector<pair<K, V>>& buf) {
    BITSET_FCOMPARES(K, V)
    size_t e = size();
    if (ordered == size()) return false;
    sort(middle(), end(), fl);
    if (unq) inplaceMerge(data, ordered, fl);
    else e = inplaceMergeUnique(data, ordered, buf, fl, fe);
    data.resize(e);
    ordered = size();
    return true;
  }
  inline bool correct(bool unq=false) {
    BITSET_FCOMPARES(K, V)
    size_t e = size();
    if (ordered == size()) return false;
    sort(middle(), end(), fl);
    if (!unq) e = unique(middle(), end(), fe) - begin();
    inplaceMerge(data, ordered, fl);
    data.resize(e);
    ordered = size();
    return true;
  }

  inline bool clear() noexcept {
    if (empty()) return false;
    data.clear();
    ordered = 0;
    return true;
  }

  inline bool add(const K& k, const V& v=V()) {
    data.push_back({k, v});
    return true;
  }

  inline bool remove(const K& k) {
    auto it = locate_match(k);
    if (it == end()) return false;
    if (it < middle()) --ordered;
    data.erase(it);
    return true;
  }
};

template <class K=int, class V=NONE>
inline auto rorderedBitset(K _k=K(), V _v=V()) {
  return ROrderedBitset<K, V>();
}




// RETYPE
// ------

template <class K, class V, class KA=K, class VA=V>
constexpr auto retype(const UnorderedBitset<K, V>& x, KA _k=KA(), VA _v=VA()) {
  return UnorderedBitset<KA, VA>();
}
template <class K, class V, class KA=K, class VA=V>
constexpr auto retype(const OrderedBitset<K, V>& x, KA _k=KA(), VA _v=VA()) {
  return OrderedBitset<KA, VA>();
}
template <class K, class V, size_t N, class KA=K, class VA=V, size_t NA=N>
constexpr auto retype(const POrderedBitset<K, V, N>& x, KA _k=KA(), VA _v=VA()) {
  return POrderedBitset<KA, VA, NA>();
}
template <class K, class V, class KA=K, class VA=V>
constexpr auto retype(const ROrderedBitset<K, V>& x, KA _k=KA(), VA _v=VA()) {
  return ROrderedBitset<KA, VA>();
}




// WRITE
// -----

template <class K, class V>
inline void write(ostream& a, const UnorderedBitset<K, V>& x)   { writeValues(a, x); }
template <class K, class V>
inline void write(ostream& a, const OrderedBitset<K, V>& x)     { writeValues(a, x); }
template <class K, class V, size_t N>
inline void write(ostream& a, const POrderedBitset<K, V, N>& x) { writeValues(a, x); }
template <class K, class V>
inline void write(ostream& a, const ROrderedBitset<K, V>& x)    { writeValues(a, x); }

template <class K, class V>
inline ostream& operator<<(ostream& a, const UnorderedBitset<K, V>& x)   { write(a, x); return a; }
template <class K, class V>
inline ostream& operator<<(ostream& a, const OrderedBitset<K, V>& x)     { write(a, x); return a; }
template <class K, class V, size_t N>
inline ostream& operator<<(ostream& a, const POrderedBitset<K, V, N>& x) { write(a, x); return a; }
template <class K, class V>
inline ostream& operator<<(ostream& a, const ROrderedBitset<K, V>& x)    { write(a, x); return a; }
