#ifndef INTERVAL_CARTESIAN_TREE_H
#define INTERVAL_CARTESIAN_TREE_H

#include <cstdint>
#include <random>
#include <list>
#include <queue>
#include <set>

#include <boost/random/linear_congruential.hpp>

template<class Interval_>
class ICTnode;

template<class Interval_>
class Interval_cartesian_tree;

template<class Interval_>
class ICTnode {
  typedef ICTnode<Interval_> Self_;
  typedef Self_* Self_ptr_;
  typedef typename Interval_::Value Value_;
  typedef typename Interval_cartesian_tree<Interval_>::Priority_ Priority_;
  typedef typename Interval_cartesian_tree<Interval_>::Interval_handle_ Interval_handle_;

  struct inf_cmp {
    bool operator()(Interval_handle_ const& a, Interval_handle_ const& b) const;
  };

  struct sup_cmp {
    bool operator()(Interval_handle_ const& a, Interval_handle_ const& b) const;
  };

  typedef std::multiset<Interval_handle_, inf_cmp> lbound_index_t;
  typedef std::multiset<Interval_handle_, sup_cmp> rbound_index_t;

  Value_ key;
  Priority_ priority;
  int ownerCount;
  ICTnode* left;
  ICTnode* right;
  lbound_index_t lbound_idx;
  rbound_index_t rbound_idx;

  template<class Idx1_t, class Idx2_t>
  void move_idx_to(Idx1_t& idx1, Idx2_t& idx2, Self_ptr_ node);

  template<class OutputIterator, class IdxType>
  void collect_from_idx(const IdxType& idx, const Value_& value, OutputIterator out) const;

  friend class Interval_cartesian_tree<Interval_>;
  
public:
  ICTnode(const Value_& key, const Priority_& priority);
  ~ICTnode() = default;

  void place_to_index(const Interval_handle_& ih);
  bool place_if_matches(const Interval_handle_& ih);
  bool delete_from_index(Interval_handle_& ih); // saves deleted value to argument
  template<class OutputIterator>
  void collect_by_lbound(const Value_& value, OutputIterator out) const;
  template<class OutputIterator>
  void collect_by_rbound(const Value_& value, OutputIterator out) const;
  void move_lbound_idx_to(Self_ptr_ node);
  void move_rbound_idx_to(Self_ptr_ node);
};

template <class Interval_>
class Interval_cartesian_tree {
  typedef uint64_t Priority_;
  typedef typename Interval_::Value Value_;
  typedef typename std::list<Interval_>::iterator Interval_handle_;
  typedef typename std::list<Interval_>::const_iterator const_iterator;
  typedef ICTnode<Interval_> Node_;
  typedef Node_* Node_ptr_;

  Node_ptr_ root;
  std::list<Interval_> container;
  std::mt19937 gen;
  std::uniform_int_distribution<Priority_> priority_gen;

  static std::pair<Node_ptr_, Node_ptr_> split(Node_ptr_ node, const Value_& x);
  static Node_ptr_ merge(Node_ptr_ node1, Node_ptr_ node2);

  // first - parent, second - node
  std::pair<Node_ptr_, Node_ptr_> find_node(const Value_& x);
  void place_to_matching(const Interval_handle_& ih);
  bool delete_from_matching(const Interval_& i);
  void delete_tree();

  friend class ICTnode<Interval_>;

public:
  Interval_cartesian_tree();
  template <class InputIterator>
  Interval_cartesian_tree(InputIterator b, InputIterator e);
  ~Interval_cartesian_tree();

  void seed(uint_fast64_t x0);

  void insert(const Interval_& i);
  template <class InputIterator>
  int insert(InputIterator b, InputIterator e);

  bool remove(const Interval_& I);

  bool is_contained(const Value_& value) const;
  template <class OutputIterator>
  OutputIterator find_intervals(const Value_& value, OutputIterator out) const;

  void clear();

  int size() const;

  const_iterator begin() const {
    return container.begin();
  }

  const_iterator end() const {
    return container.end();
  }
};

template<class Interval_>
bool
ICTnode<Interval_>::inf_cmp::operator()(const ICTnode::Interval_handle_& a, const ICTnode::Interval_handle_& b) const {
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

template<class Interval_>
bool
ICTnode<Interval_>::sup_cmp::operator()(const ICTnode::Interval_handle_& a, const ICTnode::Interval_handle_& b) const {
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


template<class Interval_>
ICTnode<Interval_>::ICTnode(const Value_& key, const ICTnode::Priority_& priority)
: key(key)
, priority(priority)
, ownerCount(1)
, left(nullptr)
, right(nullptr)
{}

template<class Interval_>
template<class Idx1_t, class Idx2_t>
void ICTnode<Interval_>::move_idx_to(Idx1_t& idx1, Idx2_t& idx2, ICTnode::Self_ptr_ node) {
  while (!idx1.empty() && (*idx1.begin())->contains_or_inf(node->key)) {
    node->place_to_index(*idx1.begin());
    auto it = idx2.find(*idx1.begin());
    assert(it != idx2.end());
    assert(*it == *idx1.begin());
    idx2.erase(it);
    idx1.erase(idx1.begin());
  }
}

template<class Interval_>
template<class OutputIterator, class IdxType>
void ICTnode<Interval_>::collect_from_idx(const IdxType& idx, const Value_& value, OutputIterator out) const {
  auto it = idx.begin();
  auto const& end = idx.end();
  while (it != end && (*it)->contains(value)) {
    out = **it;
    ++out;
    ++it;
  }
}

template<class Interval_>
void ICTnode<Interval_>::place_to_index(const ICTnode::Interval_handle_& ih) {
  lbound_idx.insert(ih);
  rbound_idx.insert(ih);
}

template<class Interval_>
bool ICTnode<Interval_>::place_if_matches(const ICTnode::Interval_handle_& ih) {
  if (ih->contains_or_inf(key)) {
    place_to_index(ih);
    return true;
  }
  return false;
}

// saves deleted value to argument
template<class Interval_>
bool ICTnode<Interval_>::delete_from_index(ICTnode::Interval_handle_& ih) {
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

template<class Interval_>
template<class OutputIterator>
void ICTnode<Interval_>::collect_by_lbound(const Value_& value, OutputIterator out) const {
  collect_from_idx(lbound_idx, value, out);
}

template<class Interval_>
template<class OutputIterator>
void ICTnode<Interval_>::collect_by_rbound(const Value_& value, OutputIterator out) const {
  collect_from_idx(rbound_idx, value, out);
}

template<class Interval_>
void ICTnode<Interval_>::move_lbound_idx_to(ICTnode::Self_ptr_ node) {
  move_idx_to(lbound_idx, rbound_idx, node);
}

template<class Interval_>
void ICTnode<Interval_>::move_rbound_idx_to(ICTnode::Self_ptr_ node) {
  move_idx_to(rbound_idx, lbound_idx, node);
}

template<class Interval_>
std::pair<typename Interval_cartesian_tree<Interval_>::Node_ptr_, typename Interval_cartesian_tree<Interval_>::Node_ptr_>
Interval_cartesian_tree<Interval_>::split(Node_ptr_ node, Value_ const& x) {
  if (!node) {
    return std::make_pair(nullptr, nullptr);
  }
  if (node->key < x) {
    auto spl = split(node->right, x);
    node->right = spl.first;
    return std::make_pair(node, spl.second);
  } else {
    auto spl = split(node->left, x);
    node->left = spl.second;
    return std::make_pair(spl.first, node);
  }
}

template<class Interval_>
typename Interval_cartesian_tree<Interval_>::Node_ptr_
Interval_cartesian_tree<Interval_>::merge(Node_ptr_ node1, Node_ptr_ node2) {
  if (!node1) {
    return node2;
  }
  if (!node2) {
    return node1;
  }
  if (node1->priority < node2->priority) {
    node2->left = merge(node1, node2->left);
    return node2;
  } else {
    node1->right = merge(node1->right, node2);
    return node1;
  }
}

template<class Interval_>
void Interval_cartesian_tree<Interval_>::delete_tree() {
  if (!root)
    return;
  std::queue<Node_ptr_> q;
  q.push(root);
  while (!q.empty()) {
    Node_ptr_ node = q.front();
    q.pop();
    if (node->left)
      q.push(node->left);
    if (node->right)
      q.push(node->right);
    delete node;
  }
  root = nullptr;
}

// first - parent, second - node
template<class Interval_>
std::pair<typename Interval_cartesian_tree<Interval_>::Node_ptr_, typename Interval_cartesian_tree<Interval_>::Node_ptr_>
Interval_cartesian_tree<Interval_>::find_node(const Value_& x) {
  Node_ptr_ v = root;
  Node_ptr_ p = nullptr;
  while (v) {
    if (v->key == x) {
      break;
    }
    p = v;
    v = v->key < x ? v->left : v->right;
  }
  return std::make_pair(p, v);
}


template<class Interval_>
void Interval_cartesian_tree<Interval_>::place_to_matching(const Interval_cartesian_tree::Interval_handle_& ih) {
  Node_ptr_ v = root;
  const Value_& inf = ih->inf();
  while (true) {
    if (v->place_if_matches(ih)) {
      return;
    }
    v = v->key < inf ? v->right : v->left;
  }
}

template<class Interval_>
bool Interval_cartesian_tree<Interval_>::delete_from_matching(const Interval_& i) {
  std::list<Interval_> tmp;
  tmp.push_front(i);
  Interval_handle_ ih = tmp.begin();
  Node_ptr_ v = root;
  while (v) {
    if (v->delete_from_index(ih)) {
      container.erase(ih);
      return true;
    }
    v = v->key < i.inf() ? v->right : v->left;
  }
  return false;
}

template<class Interval_>
Interval_cartesian_tree<Interval_>::Interval_cartesian_tree()
: root(nullptr)
, container()
, gen(std::random_device()())
, priority_gen()
{}

template<class Interval_>
template<class InputIterator>
Interval_cartesian_tree<Interval_>::Interval_cartesian_tree(InputIterator b, InputIterator e)
: Interval_cartesian_tree()
{
  while (b != e) {
    insert(*b);
    ++b;
  }
}

template<class Interval_>
Interval_cartesian_tree<Interval_>::~Interval_cartesian_tree() {
  delete_tree();
}

template<class Interval_>
void Interval_cartesian_tree<Interval_>::seed(uint_fast64_t x0) {
  gen.seed(x0);
}

template<class Interval_>
int Interval_cartesian_tree<Interval_>::size() const {
  return container.size();
}

template<class Interval_>
void Interval_cartesian_tree<Interval_>::clear() {
  delete_tree();
  container.clear();
}

template<class Interval_>
template<class InputIterator>
int Interval_cartesian_tree<Interval_>::insert(InputIterator b, InputIterator e) {
  int n = 0;
  for(; b != e; ++b) {
    insert(*b);
    ++n;
  }
  return n;
}

template<class Interval_>
void Interval_cartesian_tree<Interval_>::insert(const Interval_& i) {
  container.push_front(i);
  Interval_handle_ ih = container.begin();
  std::pair<Node_ptr_, Node_ptr_> found = find_node(i.inf());
  if (found.second) {
    found.second->ownerCount++;
    place_to_matching(ih);
    return;
  }
  auto* node = new Node_(i.inf(), priority_gen(gen));
  Node_ptr_ v = root;
  Node_ptr_* child_ptr = &root;
  while (v && v->priority > node->priority) {
    child_ptr = node->key < v->key ? &v->left : &v->right;
    v = *child_ptr;
  }
  *child_ptr = node;
  std::pair<Node_ptr_, Node_ptr_> spl = split(v, node->key);
  node->left = spl.first;
  node->right = spl.second;
  for (Node_ptr_ u = node->left; u; u = u->right) {
    u->move_rbound_idx_to(node);
  }
  for (Node_ptr_ u = node->right; u; u = u->left) {
    u->move_lbound_idx_to(node);
  }
  place_to_matching(ih);
}

template<class Interval_>
bool Interval_cartesian_tree<Interval_>::remove(const Interval_& I) {
  if (!delete_from_matching(I)) {
    return false;
  }
  Node_ptr_ v = root;
  Node_ptr_* child_ptr = &root;
  while (v && v->key != I.inf()) {
    child_ptr = I.inf() < v->key ? &v->left : &v->right;
    v = *child_ptr;
  }
  assert(v);
  if (--v->ownerCount) {
    return true;
  }
  Node_ptr_ u = v->left;
  Node_ptr_ w = v->right;
  while (u || w) {
    if (!u || w && w->priority > u->priority) {
      v->move_rbound_idx_to(w);
      w = w->left;
    } else {
      v->move_lbound_idx_to(u);
      u = u->right;
    }
  }
  *child_ptr = merge(v->left, v->right);
  delete v;
  return true;
}

template<class Interval_>
bool Interval_cartesian_tree<Interval_>::is_contained(const Value_& value) const {
  Node_ptr_ v = root;
  while (v) {
    if (value > v->key) {
      if (!v->rbound_idx.empty() && (*v->rbound_idx.begin())->contains(value)) {
        return true;
      }
      v = v->right;
    } else {
      if (!v->lbound_idx.empty() && (*v->lbound_idx.begin())->contains(value)) {
        return true;
      }
      if (v->key == value) {
        return false;
      }
      v = v->left;
    }
  }
  return false;
}

template<class Interval_>
template<class OutputIterator>
OutputIterator Interval_cartesian_tree<Interval_>::find_intervals(const Value_& value, OutputIterator out) const {
  Node_ptr_ v = root;
  while (v) {
    if (value > v->key) {
      v->collect_by_rbound(value, out);
      v = v->right;
    } else {
      v->collect_by_lbound(value, out);
      if (v->key == value) {
        break;
      }
      v = v->left;
    }
  }
  return out;
}


#endif //INTERVAL_CARTESIAN_TREE_H
