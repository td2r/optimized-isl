#ifndef INTERVAL_SKIP_LIST_H
#define INTERVAL_SKIP_LIST_H

#include <CGAL/license/Interval_skip_list.h>

#include <CGAL/basic.h>
#include <list>
#include <iostream>
#include <set>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/geometric_distribution.hpp>
#include <boost/random/variate_generator.hpp>


//#define CGAL_ISL_USE_CCC
#define CGAL_ISL_USE_LIST

#if defined(CGAL_ISL_USE_CCC) || ! defined(CGAL_ISL_USE_LIST)
#include <CGAL/Compact_container.h>
#endif

template <class Interval_>
class Interval_skip_list;

template <class Interval_>
class IntervalSLnode;

const int MAX_FORWARD = 48;         // Maximum number of forward pointers

template <class Interval_>
class IntervalSLnode  // interval skip list node
{
private:
  typedef IntervalSLnode<Interval_> Self;
  typedef Self* Self_ptr;
  typedef Interval_ Interval;
  typedef typename Interval::Value Value;
  typedef typename Interval_skip_list<Interval_>::Interval_handle Interval_handle;

  struct inf_cmp {
    bool operator()(Interval_handle const& a, Interval_handle const& b) const;
  };

  struct sup_cmp {
    bool operator()(Interval_handle const& a, Interval_handle const& b) const;
  };

  typedef std::multiset<Interval_handle, inf_cmp> lbound_index_t;
  typedef std::multiset<Interval_handle, sup_cmp> rbound_index_t;

  bool header_node;
  Value key;
  IntervalSLnode** forward;  // array of forward pointers
  lbound_index_t lbound_idx;
  rbound_index_t rbound_idx;
  int ownerCount;  // number of intervals with inf value equal to key
  int topLevel;  // index of top level of forward pointers in this node.
                 // Levels are numbered 0..topLevel.

  // iterates over idx1, deletes from both
  template<class Idx1_t, class Idx2_t>
  void move_idx_to(Idx1_t& idx1, Idx2_t& idx2, Self_ptr node);

  template<class OutputIterator, class IdxType>
  void collect_from_idx(const IdxType& idx, const Value& value, OutputIterator out) const;

public:
  friend class Interval_skip_list<Interval>;

  explicit IntervalSLnode(int top_level);  // constructor for the header
  IntervalSLnode(const Value& key, int top_level);  // constructor
  ~IntervalSLnode();  // destructor

  bool is_header() const;
  int get_height() const; // number of levels of this node
  const Value& get_value() const;
  IntervalSLnode* get_next() const;

  void place_to_index(const Interval_handle& ih);
  bool place_if_matches(const Interval_handle& ih);
  bool delete_from_index(Interval_handle& ih); // saves deleted value to argument
  template<class OutputIterator>
  void collect_by_lbound(const Value& value, OutputIterator out) const;
  template<class OutputIterator>
  void collect_by_rbound(const Value& value, OutputIterator out) const;
  void move_lbound_idx_to(Self_ptr node);
  void move_rbound_idx_to(Self_ptr node);
  void print(std::ostream& os) const;
};

#ifndef CGAL_ISL_USE_LIST
template <class Interval_>
class Interval_for_container : public Interval_
{
private:
  void* p;
public:
  explicit Interval_for_container(const Interval_& i) : Interval_(i), p(nullptr) {}
  void* for_compact_container() const { return p; }
  void for_compact_container(void *ptr) { p = ptr; }
};
#endif

template <class Interval_>
class Interval_skip_list
{
private:
  typedef Interval_ Interval;
  typedef typename Interval::Value Value;

#ifdef CGAL_ISL_USE_LIST
  std::list<Interval> container;
  typedef typename std::list<Interval>::iterator Interval_handle;
#else
  Compact_container<Interval_for_container<Interval_t>> container;
  typedef typename Compact_container<Interval_for_container<Interval_t>>::iterator
    Interval_handle;
#endif

  int maxLevel;
  boost::rand48 random;
  IntervalSLnode<Interval>* header;

  int random_level();  // choose a new node level at random
  void insert_impl(const Interval_handle& ih);

  friend class IntervalSLnode<Interval>;

public:
  Interval_skip_list();
  template <class InputIterator>
  Interval_skip_list(InputIterator b, InputIterator e);
  ~Interval_skip_list();

  void seed(boost::rand48::result_type x0);

  void insert(const Interval& i);
  template <class InputIterator>
  int insert(InputIterator b, InputIterator e);

  bool remove(const Interval& I);

  bool is_contained(const Value& value) const;
  template <class OutputIterator>
  OutputIterator find_intervals(const Value& value, OutputIterator out) const;

  void clear();

  int size() const;

#ifdef CGAL_ISL_USE_LIST
  typedef typename std::list<Interval>::const_iterator const_iterator;
#else
  typedef typename
  Compact_container<Interval_for_container<Interval_t> >::const_iterator
                                                           const_iterator;
#endif

  const_iterator begin() const {
    return container.begin();
  }

  const_iterator end() const {
    return container.end();
  }

  // return node containing
  // Value if found, otherwise null
  IntervalSLnode<Interval>* search(const Value& searchKey);

  void print(std::ostream& os) const;
  void printOrdered(std::ostream& os) const;
};

template <class Interval>
bool IntervalSLnode<Interval>::inf_cmp::operator()(Interval_handle const& a, Interval_handle const& b) const
{
  if (a->inf() != b->inf())
    return a->inf() < b->inf();
  if (a->inf_closed() != b->inf_closed())
    return a->inf_closed();
  // sup comparison need only for deterministic search in std::set
  // signs not actually matter
  if (a->sup() != b->sup())
    return a->sup() < b->sup();
  if (a->sup_closed() != b->sup_closed())
    return b->sup_closed();
  return false;
}

template<class Interval>
bool IntervalSLnode<Interval>::sup_cmp::operator()(const Interval_handle& a, const Interval_handle& b) const {
  if (a->sup() != b->sup())
    return a->sup() > b->sup();
  if (a->sup_closed() != b->sup_closed())
    return a->sup_closed();
  // inf comparison need only for deterministic search in std::set
  // signs not actually matter
  if (a->inf() != b->inf())
    return a->inf() > b->inf();
  if (a->inf_closed() != b->inf_closed())
    return b->inf_closed();
  return false;
}

template <class Interval>
IntervalSLnode<Interval>::IntervalSLnode(int top_level)
  : header_node(true)
  , topLevel(top_level)
  , ownerCount(0)
{
  // top_level is actually one less than the real number of levels
  forward = new IntervalSLnode*[top_level + 1];
  for(int i = 0; i <= top_level; i++) {
    forward[i] = 0;
  }
}

template <class Interval>
IntervalSLnode<Interval>::IntervalSLnode(const Value& key, int top_level)
  : header_node(false)
  , key(key)
  , topLevel(top_level)
  , ownerCount(0)
{
  // top_level is actually one less than the real number of levels
  forward = new IntervalSLnode*[top_level + 1];
  for(int i=0; i <= top_level; i++) {
    forward[i] = 0;
  }
}

template <class Interval>
bool IntervalSLnode<Interval>::is_header() const {
  return header_node;
}

template <class Interval>
int IntervalSLnode<Interval>::get_height() const {
  return topLevel + 1;
}

template <class Interval>
IntervalSLnode<Interval>* IntervalSLnode<Interval>::get_next() const {
  return forward[0];
}

template <class Interval>
const typename IntervalSLnode<Interval>::Value&
IntervalSLnode<Interval>::get_value() const
{
  return key;
}

template<class Interval>
void IntervalSLnode<Interval>::place_to_index(const IntervalSLnode::Interval_handle& ih) {
  lbound_idx.insert(ih);
  rbound_idx.insert(ih);
}

template<class Interval>
bool IntervalSLnode<Interval>::place_if_matches(const IntervalSLnode::Interval_handle& ih) {
  if (ih->contains_or_inf(key)) {
    place_to_index(ih);
    return true;
  }
  return false;
}

// saves deleted value to argument
template<class Interval>
bool IntervalSLnode<Interval>::delete_from_index(IntervalSLnode::Interval_handle& ih)
{
  auto it = lbound_idx.find(ih);
  if (it != lbound_idx.end()) {
    auto it2 = rbound_idx.find(ih);
    assert(it2 != rbound_idx.end());
    assert(*it == *it2);
    ih = *it;
    lbound_idx.erase(it);
    rbound_idx.erase(it2);
    return true;
  }
  return false;
}

template<class Interval>
template<class OutputIterator, class IdxType>
void IntervalSLnode<Interval>::collect_from_idx(const IdxType& idx, const Value& value, OutputIterator out) const {
  auto it = idx.begin();
  auto const& end = idx.end();
  while (it != end && (*it)->contains(value)) {
    out = **it;
    ++out;
    ++it;
  }
}

template<class Interval>
template<class OutputIterator>
void IntervalSLnode<Interval>::collect_by_lbound(const Value& value, OutputIterator out) const {
  collect_from_idx(lbound_idx, value, out);
}

template<class Interval>
template<class OutputIterator>
void IntervalSLnode<Interval>::collect_by_rbound(const Value& value, OutputIterator out) const {
  collect_from_idx(rbound_idx, value, out);
}

// iterates over idx1, deletes from both
template<class Interval>
template<class Idx1_t, class Idx2_t>
void IntervalSLnode<Interval>::move_idx_to(Idx1_t& idx1, Idx2_t& idx2, Self_ptr node) {
  while (!idx1.empty() && (*idx1.begin())->contains_or_inf(node->key)) {
    node->place_to_index(*idx1.begin());
    auto it = idx2.find(*idx1.begin());
    assert(it != idx2.end());
    assert(*it == *idx1.begin());
    idx2.erase(it);
    idx1.erase(idx1.begin());
  }
}

template<class Interval>
void IntervalSLnode<Interval>::move_lbound_idx_to(Self_ptr node) {
  move_idx_to(lbound_idx, rbound_idx, node);
}

template<class Interval>
void IntervalSLnode<Interval>::move_rbound_idx_to(Self_ptr node) {
  move_idx_to(rbound_idx, lbound_idx, node);
}

template <class Interval>
void IntervalSLnode<Interval>::print(std::ostream& os) const
{
  int i;
  os << "IntervalSLnode key:  ";
  if (! header_node) {
    os << key;
  }else {
    os << "HEADER";
  }
  os << "\n";
  os << "number of levels: " << get_height() << std::endl;
  os << "ownerCount = " << ownerCount << std::endl;
  os << "lbound_index: {";
  std::string delim;
  for (auto const& ih : lbound_idx) {
    os << delim << *ih;
    delim = ", ";
  }
  os << "}" << std::endl;
  os << "rbound_index: {";
  delim = "";
  for (auto const& ih : rbound_idx) {
    os << delim << *ih;
    delim = ", ";
  }
  os << "}" << std::endl;
  os << "forward pointers:\n";
  for(i=0; i<=topLevel; i++)
  {
    os << "forward[" << i << "] = ";
    if(forward[i] != nullptr) {
      os << forward[i]->get_value();
    } else {
      os << "nullptr";
    }
    os << std::endl;
  }
  os << std::endl << std::endl;
}

template <class Interval>
IntervalSLnode<Interval>::~IntervalSLnode() {
  delete [] forward;
}

template <class Interval>
Interval_skip_list<Interval>::Interval_skip_list()
  : maxLevel(0)
{
  header = new IntervalSLnode<Interval>(MAX_FORWARD);
  for (int i = 0; i < MAX_FORWARD; i++) {
    header->forward[i] = 0;
  }
}

template <class Interval>
template <class InputIterator>
Interval_skip_list<Interval>::Interval_skip_list(InputIterator b, InputIterator e) {
  maxLevel = 0;
  header = new IntervalSLnode<Interval>(MAX_FORWARD);
  for (int i = 0; i< MAX_FORWARD; i++) {
    header->forward[i] = 0;
  }
  for(; b!= e; ++b){
    insert(*b);
  }
}

template<class Interval_>
void Interval_skip_list<Interval_>::seed(boost::rand48::result_type x0) {
  random.seed(x0);
}

template<class Interval>
void Interval_skip_list<Interval>::insert_impl(const Interval_handle& ih) {
  auto lbound = ih->inf();
  IntervalSLnode<Interval>* node = search(lbound);
  if (node) {
    // node with lbound already persists in list
    // just increase ownerCount and place interval to index of some node
    node->ownerCount++;
    IntervalSLnode<Interval>* v = header;
    for (int i = maxLevel; i >= 0; --i) {
      while (v->forward[i] && v->forward[i]->key < lbound) {
        v = v->forward[i];
        if (v->place_if_matches(ih)) {
          return;
        }
      }
      if (v->forward[i] && v->forward[i]->place_if_matches(ih)) {
        return;
      }
    }
    assert(false); // no node for lbound
  } else {
    // insert node with key equals to lbound
    // at the same time place interval to index of some node
    int lvl = random_level();
    auto* new_node = new IntervalSLnode<Interval>(lbound, lvl);
    new_node->ownerCount = 1;

    // phase 1: search for nearest node from the left at height = lvl
    //          placing interval for index if some node contained by it
    bool placed = false;
    IntervalSLnode<Interval>* v = header;
    for (int i = std::max(maxLevel, lvl); i >= lvl; --i) {
      while (v->forward[i] && v->forward[i]->key < lbound) {
        v = v->forward[i];
        if (!placed) {
          placed = v->place_if_matches(ih);
        }
      }
      // if i == lvl then v->forward[i] located to the right of new node, which is a better fit for interval
      if (!placed && i != lvl && v->forward[i]) {
        placed = v->forward[i]->place_if_matches(ih);
      }
    }
    if (!placed) {
      new_node->place_to_index(ih);
    }

    if (v->forward[lvl] && v->forward[lvl]->get_height() == lvl + 1) {
      // v->forward[lvl] is node with same height right to the new,
      // so some intervals can be moved to the new leftmost node
      v->forward[lvl]->move_lbound_idx_to(new_node);
    }
    // adjust forward pointers at level lvl
    new_node->forward[lvl] = v->forward[lvl];
    v->forward[lvl] = new_node;

    // phase 2: iterate over nodes below the inserted and steal intervals which overlap it
    IntervalSLnode<Interval>* prev_right = new_node->forward[lvl]; // last processed node on the right
    for (int i = lvl - 1; i >= 0; --i) {
      while (v->forward[i] && v->forward[i]->key < lbound) {
        v = v->forward[i];
        v->move_rbound_idx_to(new_node);
      }
      if (v->forward[i] && v->forward[i] != prev_right) {
        v->forward[i]->move_lbound_idx_to(new_node);
        prev_right = v->forward[i];
      }
      // adjust forward pointers at level i
      new_node->forward[i] = v->forward[i];
      v->forward[i] = new_node;
    }
    if (lvl > maxLevel) {
      for (int i = maxLevel + 1; i <= lvl; ++i) {
        header->forward[i] = new_node;
      }
      maxLevel = lvl;
    }
  }
}

template <class Interval>
void
Interval_skip_list<Interval>::insert(const Interval& i)
{
#ifdef CGAL_ISL_USE_LIST
  container.push_front(i);
  Interval_handle ih = container.begin();
#else
  Interval_for_container<Interval_t> ifc(i);
  Interval_handle ih = container.insert(ifc);
#endif
  insert_impl(ih);
}


template<class Interval>
template<class InputIterator>
int Interval_skip_list<Interval>::insert(InputIterator b, InputIterator e) {
  int i = 0;
  for(; b != e; ++b) {
    insert(*b);
    ++i;
  }
  return i;
}

template <class Interval>
bool Interval_skip_list<Interval>::remove(const Interval& I)
{
  // need to obtain Interval_handle for search in index
#ifdef CGAL_ISL_USE_LIST
  std::list<Interval> tmp;
  tmp.push_front(I);
  Interval_handle ih = tmp.begin();
#else
  Compact_container<Interval_for_container<Interval_t>> tmp;
  Interval_for_container<Interval_t> ifc(i);
  Interval_handle ih = container.insert(ifc);
#endif
  auto const& lbound = I.inf();
  bool removed = false;
  IntervalSLnode<Interval>* v = header;
  int i;
  // phase 1: iterate over skip list and try to delete interval if it is in some node
  for (i = maxLevel; i >= 0; --i) {
    while (v->forward[i] && v->forward[i]->key < lbound) {
      v = v->forward[i];
      if (!removed) {
        removed = v->delete_from_index(ih);
      }
    }
    if (!removed && v->forward[i]) {
      removed = v->forward[i]->delete_from_index(ih);
    }
    if (v->forward[i] && v->forward[i]->key == lbound) {
      break;
    }
  }
  if (!removed) {
    assert(i < 0);
    return false;
  }
  assert(v && v->forward[i] && v->forward[i]->key == lbound);
  if (--(v->forward[i]->ownerCount) == 0) {
    // phase 2: remove node from skip list and place intervals from its index to other nodes
    IntervalSLnode<Interval>* rm_node = v->forward[i];
    if (rm_node->forward[i]) {
      rm_node->move_rbound_idx_to(rm_node->forward[i]);
    }
    v->forward[i] = rm_node->forward[i];
    for (--i; i >= 0; --i) {
      while (v->forward[i] != rm_node) {
        v = v->forward[i];
        rm_node->move_lbound_idx_to(v);
      }
      assert(v->forward[i] == rm_node);
      // check that rm_node->forward[i] not null and wasn't processed for index change at previous iteration
      if (rm_node->forward[i] != rm_node->forward[i + 1]) {
        rm_node->move_rbound_idx_to(rm_node->forward[i]);
      }
      v->forward[i] = rm_node->forward[i];
    }
    delete rm_node;
  }
  // ih is valid iterator since IntervalSLnode<Interval_t>::delete_from_index completed successfully
  container.erase(ih);
  return true;
}

template<class Interval>
bool Interval_skip_list<Interval>::is_contained(const Value& value) const {
  IntervalSLnode<Interval>* v = header;
  for (int i = maxLevel; i >= 0; --i) {
    while (v->forward[i] && v->forward[i]->key < value) {
      v = v->forward[i];
      if (!v->rbound_idx.empty() && (*(v->rbound_idx.begin()))->contains(value))
        return true;
    }
    if (v->forward[i]) {
      if (!v->forward[i]->lbound_idx.empty() && (*(v->forward[i]->lbound_idx.begin()))->contains(value))
        return true;
      if (v->forward[i]->key == value)
        break;
    }
  }
  return false;
}

template<class Interval>
template<class OutputIterator>
OutputIterator Interval_skip_list<Interval>::find_intervals(const Value& value, OutputIterator out) const {
  IntervalSLnode<Interval>* v = header;
  IntervalSLnode<Interval>* prev_right = nullptr;
  for (int i = maxLevel; i >= 0; --i) {
    while (v->forward[i] && v->forward[i]->key < value) {
      v = v->forward[i];
      v->collect_by_rbound(value, out);
    }
    if (v->forward[i] && v->forward[i] != prev_right) {
      // for node with key == value intervals with inf() == value and inf_open don't overlap value
      // therefore we collect them by lbound
      v->forward[i]->collect_by_lbound(value, out);
      if (v->forward[i]->key == value) {
        break;
      }
      prev_right = v->forward[i];
    }
  }
  return out;
}

template <class Interval>
void Interval_skip_list<Interval>::clear() {
  IntervalSLnode<Interval>* v = header->get_next();
  IntervalSLnode<Interval>* next;
  while (v != 0) {
    next = v->get_next();
    delete v;
    v = next;
  }
  for (int i = 0; i < MAX_FORWARD; ++i) {
    header->forward[i] = 0;
  }
  maxLevel = 0;
}

template<class Interval_>
int Interval_skip_list<Interval_>::size() const {
  return container.size();
}


template <class Interval>
void Interval_skip_list<Interval>::print(std::ostream& os) const
{
  os << "\nAn Interval_skip_list:  \n";
  os << "|container| == " << container.size() << std::endl;
  IntervalSLnode<Interval>* n = header->get_next();

  while( n != 0 ) {
    n->print(os);
    n = n->get_next();
  }
}

template <class Interval>
std::ostream& operator<<(std::ostream& os,
                         const Interval_skip_list<Interval>& isl)
{
  isl.print(os);
  return os;
}


template <class Interval>
void Interval_skip_list<Interval>::printOrdered(std::ostream& os) const
{
  IntervalSLnode<Interval>* n = header->get_next();
  os << "values in list:  ";
  while( n != 0 ) {
    os << n->key << " ";
    n = n->get_next();
  }
  os << std::endl;
}

template <class Interval>
IntervalSLnode<Interval>*
Interval_skip_list<Interval>::search(const Value& searchKey)
{
  IntervalSLnode<Interval>* v = header;
  for (int i = maxLevel; i >= 0; --i) {
    while (v->forward[i] != 0 && (v->forward[i]->key < searchKey)) {
      v = v->forward[i];
    }
    if (v->forward[i] && v->forward[i]->key == searchKey)
      return v->forward[i];
  }
  return nullptr;
}

template <class Interval>
int Interval_skip_list<Interval>::random_level()
{
  boost::geometric_distribution<> prob(0.5);
  boost::variate_generator<boost::rand48&, boost::geometric_distribution<>> die(random, prob);

  return std::min(die(), MAX_FORWARD - 1);
}

template <class Interval>
Interval_skip_list<Interval>::~Interval_skip_list()
{
  IntervalSLnode<Interval>* next;
  while (header != 0) {
    next = header->get_next();
    delete header;
    header = next;
  }
}

#endif // INTERVAL_SKIP_LIST_H
