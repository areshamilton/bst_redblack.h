#ifndef bst_redblack_h
#define bst_redblack_h

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <stdio.h>
#include "queue.h"

template <typename Key, typename Value>

class bst_red_black {
private:

  struct node {
    Key key;
    Value val;
    node* left;
    node* right;
    int size;

    node(Key key_, Value value, int size_)
    : key(key_), val(value), size(size_), left(nullptr), right(nullptr) { }

    friend std::ostream& operator<<(std::ostream& os, const node& no) {
      return os << no->left << " <-- "
                << "(" << no->key << "," << no->val << " (" << &no << ")) --> "
                << no->right << "\n";
    }
  };

  node* root;

  bool RED   = true;
  bool BLACK = false;

public:
  bst_red_black() { }

  // node helper methods
private:
  bool isRed(node* x) {
    if (x == nullptr) return false;
    return x->color == RED;
  }

    // number of node in subtree rooted at x; 0 if x is null
  int size(node* x) {
    if (x == nullptr) return 0;
    return x->size;
    }

public:
  int size() {
    return size(root);
  }

  bool isEmpty() {
    return root == nullptr;
  }

  // Standard BST search

  // Returns the value associated with the given key.
  Value get(Key& key) {
    if (key ==nullptr) throw new std::invalid_argument("argument to get() is null");
    return get(root, key);
  }

private:
  Value get(node* x, Key& key) {
    if (key == Key()) { throw new std::invalid_argument("calls get() with a null key"); }
    if (x->key == Key()) { return Value(); }
    if      (less(key,    x->key)) { return get(x->left,  key); }
    else if (less(x->key, key))    { return get(x->right, key); }
    else                           { return x->val; }
  }

public:
  bool contains(Key& key) {
    return get(key) != nullptr;
    }

  // red-black tree insertion

  void put(Key key, Value val) {
    if (key == nullptr) throw new std::invalid_argument("first argument to put() is null");
    if (val == nullptr) {
      delete_key(key);
      return;
    }

    root = put(root, key, val);
    root->color = BLACK;
  }

  private:
    // insert the key-value pair in the subtree rooted at h
    node* put(node* h, Key key, Value val) {
      if (h == nullptr) return new node(key, val, 1);

      int cmp = key.compareTo(h->key);
        if      (less(key,    h->key)) { h->left  = put(h->left,  key, val); }
        else if (less(h->key, key))    { h->right = put(h->right, key, val); }
        else                           { h->val   = val; }

        // fix-up any right-leaning links
        if (isRed(h->right) && !isRed(h->left))      { h = rotateLeft(h); }
        if (isRed(h->left)  &&  isRed(h->left->left)) { h = rotateRight(h); }
        if (isRed(h->left)  &&  isRed(h->right))     { flipColors(h); }
        h->size = size(h->left) + size(h->right) + 1;

        return h;
    }

    // red-black tree deletion

public:
   void deleteMin() {
     if (isEmpty()) throw new std::logic_error("BST underflow");

      // if both children of root are black, set root to red
      if (!isRed(root->left) && !isRed(root->right)) { root->color = RED; }

      root = deleteMin(root);
      if (!isEmpty()) { root->color = BLACK; }
    }

  private:
    // delete the key-value pair with the minimum key rooted at h
    node* deleteMin(node* h) {
      if (h->left == nullptr) { return nullptr; }
      if (!isRed(h->left) && !isRed(h->left->left)) { h = moveRedLeft(h); }
      h->left = deleteMin(h->left);
      return balance(h);
    }


public:
  void deleteMax() {
    if (isEmpty()) throw new std::logic_error("BST underflow");

    // if both children of root are black, set root to red
    if (!isRed(root->left) && !isRed(root->right)) { root->color = RED; }

    root = deleteMax(root);
    if (!isEmpty()) { root->color = BLACK; }
    }

private:
  // delete the key-value pair with the maximum key rooted at h
  node* deleteMax(node* h) {
    if (isRed(h->left)) { h = rotateRight(h); }

    if (h->right == nullptr) { return nullptr; }

    if (!isRed(h->right) && !isRed(h->right->left)) { h = moveRedRight(h); }

    h->right = deleteMax(h->right);

    return balance(h);
    }

public:
  void delete_key(Key& key) {
    if (key == nullptr) throw new std::invalid_argument("argument to delete_key() is null");
    if (!contains(key)) return;

    // if both children of root are black, set root to red
    if (!isRed(root->left) && !isRed(root->right)) { root->color = RED; }

    root = delete_key(root, key);
    if (!isEmpty()) { root->color = BLACK; }
  }

private:
  // delete the key-value pair with the given key rooted at h
  node* delete_key(node* h, Key& key) {
    if (key.compareTo(h->key) < 0)  {
      if (!isRed(h->left) && !isRed(h->left->left)) { h = moveRedLeft(h); }
      h->left = delete_key(h->left, key);
    }
    else {
      if (isRed(h->left)) { h = rotateRight(h); }
      if (key.compareTo(h->key) == 0 && (h->right == nullptr)) { return nullptr; }
      if (!isRed(h->right) && !isRed(h->right->left)) { h = moveRedRight(h); }
      if (key.compareTo(h->key) == 0) {
        node* x = min(h->right);
        h->key = x->key;
        h->val = x->val;
        h->right = deleteMin(h->right);
      }
      else  { h->right = delete_key(h->right, key); }
    }
    return balance(h);
    }

// red-black tree helper functions

    // make a left-leaning link lean to the right
  node* rotateRight(node* h) {
    node* x = h->left;
    h->left = x->right;
    x->right = h;
    x->color = x->right->color;
    x->right->color = RED;
    x->size = h->size;
    h->size = size(h->left) + size(h->right) + 1;
    return x;
  }

    // make a right-leaning link lean to the left
  node* rotateLeft(node* h) {
    node* x = h->right;
    h->right = x->left;
    x->left = h;
    x->color = x->left->color;
    x->left->color = RED;
    x->size = h->size;
    h->size = size(h->left) + size(h->right) + 1;
    return x;
  }

  // flip the colors of a node and its two children
  void flipColors(node* h) {
    h->color = !h->color;
    h->left->color = !h->left->color;
    h->right->color = !h->right->color;
  }

  node* moveRedLeft(node* h) {
    flipColors(h);
    if (isRed(h->right->left)) {
      h->right = rotateRight(h->right);
      h = rotateLeft(h);
      flipColors(h);
    }
    return h;
  }

  node* moveRedRight(node* h) {
    flipColors(h);
    if (isRed(h->left->left)) {
      h = rotateRight(h);
      flipColors(h);
    }
    return h;
  }

  // restore red-black tree invariant
  node* balance(node* h) {
    if (isRed(h->right))                      h = rotateLeft(h);
    if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
    if (isRed(h->left) && isRed(h->right))     flipColors(h);

    h->size = size(h->left) + size(h->right) + 1;
    return h;
  }

// utility functions

public:
  int height() {
    return height(root);
  }

private:
  int height(node* x) {
        if (x == nullptr) return -1;
        return 1 + std::max(height(x->left), height(x->right));
  }

public:
   Key min() {
     if (isEmpty()) throw new std::logic_error("calls min() with empty symbol table");
    return min(root)->key;
    }

private:
  node* min(node* x) {
        // assert x != null;
        if (x->left == nullptr) return x;
        else                return min(x->left);
    }

public:
  Key max() {
    if (isEmpty()) throw new std::logic_error("calls max() with empty symbol table");
    return max(root)->key;
  }

private:
   node* max(node* x) {
    if (x->right == nullptr) return x;
    else                    return max(x->right);
    }

public:
  Key floor(Key& key) {
    if (key == nullptr) throw new std::invalid_argument("argument to floor() is null");
    if (isEmpty()) throw new std::logic_error("calls floor() with empty symbol table");
    node* x = floor(root, key);
    if (x == nullptr) throw new std::logic_error("argument to floor() is too small");
    else              return x->key;
    }

private:
  node floor(node* x, Key key) {
    if (x == nullptr) { return nullptr; }

    bool key_less = less(key,    x->key);
    bool key_more = less(x->key, key);

    if (key_less && key_more) { return x; }
    if (key_less)             { return floor(x->left, key); }

    node* t = floor(x->right, key);
    if (t != nullptr) { return t; }
  }

public:
  Key ceiling(Key& key) {
        if (key == nullptr) throw new std::invalid_argument("argument to ceiling() is null");
        if (isEmpty()) throw new std::logic_error("calls ceiling() with empty symbol table");
        node x = ceiling(root, key);
        if (x == nullptr) throw new std::logic_error("argument to ceiling() is too small");
        else              return x->key;
    }

private:
  node* ceiling(node* x, Key key) {
    if (x == Key()) { return nullptr; }

    bool key_less = less(key,    x->key);
    bool key_more = less(x->key, key);

    if (key_less && key_more) { return x; }  // equal
    if (key_less) {
      node* t = ceiling(x->left, key);
      return t != nullptr ? t : x;
    }
    return ceiling(x->right, key);
  }

public:
  Key select(int rank) {
    if (rank < 0 || rank >= size()) {
      std::cerr << "argument to select() is invalid: " << rank;
    }
    return select(root, rank);
  }

private:
  // Return key in BST rooted at x of given rank.
  // Precondition: rank is in legal range.
  Key select(node* x, int rank) {
    if (x == nullptr) return nullptr;
    int leftSize = size(x->left);
    if      (leftSize > rank) return select(x->left,  rank);
    else if (leftSize < rank) return select(x->right, rank - leftSize - 1);
    else                      return x->key;
  }

public:
  int rank(Key& key) {
    if (key == nullptr) throw new std::invalid_argument("argument to rank() is null");
    return rank(key, root);
  }

private:
  // number of keys less than key in the subtree rooted at x
  int rank(Key key, node* x) {
    if (x == nullptr) { return 0; }

    if      (less(key,    x->key)) { return rank(key, x->left); }
    else if (less(x->key, key))    { return 1 + size(x->left) + rank(key, x->right); }
    else                           { return size(x->left); }
  }

public:
  void keys(node* x, queue_<Key>& q, Key low, Key high) {
    if (x == nullptr) { return; }

    bool low_le  = less(low, x->key)  || low  == x->key;
    bool high_ge = less(x->key, high) || high == x->key;

    if (low_le)             { keys(x->left, q, low, high); }   // le == less than or equal to (a la Python)
    if (low_le && high_ge)  { q.enqueue(x->key); }             // ge == greater than or equal to
    if (high_ge)            { keys(x->right, q, low, high); }
  }

  array_queue<Key> keys() {
    if (isEmpty()) { return array_queue<Key>(); }
    Key min_key = min(), max_key = max();
    return keys(min_key, max_key);
  }

  array_queue<Key> keys(Key& low, Key& high) {
    if (low == Key())  { throw new std::invalid_argument("first argument to keys() is null"); }
    if (high == Key()) { throw new std::invalid_argument("second argument to keys() is null"); }

    array_queue<Key> q;
    keys(root, q, low, high);
    return q;
  }

  int size(Key& low, Key& high) {
    if (low == Key())  { throw new std::invalid_argument("first argument to size() is null"); }
    if (high == Key()) { throw new std::invalid_argument("second argument to size() is null"); }

    if (low.compareTo(high) > 0) { return 0; }
    if (contains(high)) { return rank(high) - rank(low) + 1; }
    else                { return rank(high) - rank(low); }
  }


// check integrity of red-black tree

private:
  bool check() {
    if (!isBST())            std::cerr << "Not in symmetric order";
    if (!isSizeConsistent()) std::cerr << "Subtree counts not consistent";
    if (!isRankConsistent()) std::cerr << "Ranks not consistent";
    if (!is23())             std::cerr << "Not a 2-3 tree";
    if (!isBalanced())       std::cerr << "Not balanced";
    return isBST() && isSizeConsistent() && isRankConsistent() && is23() && isBalanced();
  }

  bool isBST() {
    static Key default_key = Key();

    return is_bst(root, default_key, default_key);
  }

  bool isBST(node* x, Key& min, Key& max) {
    if (x == nullptr) { return true; }
    Key default_key = Key();

    if (min != default_key && less(x->key, min) <= 0) { return false; }
    if (max != default_key && less(max,    x->key))   { return false; }
    bool left_bst = is_bst(x->left, min, x->key);
    bool right_bst = is_bst(x->left, min, x->key);
    return left_bst && right_bst;
  }

  bool isSizeConsistent() { return isSizeConsistent(root); }
  bool isSizeConsistent(node* x) {
    if (x == nullptr) return true;
    if (x->size != size(x->left) + size(x->right) + 1) return false;
    return isSizeConsistent(x->left) && isSizeConsistent(x->right);
    }

  // check that ranks are consistent
  bool isRankConsistent() {
    for (int i = 0; i < size(); i++) {
      Key key_selected = select(i);
      if (i != rank(key_selected)) { return false; }
    }
    for (Key& key : keys()) {
      Key key_at_rank = select(rank(key));
      if (less(key, key_at_rank) || less(key_at_rank, key)) {  // if !equal
        return false;
      }
    }
    return true;
  }

  bool is23() { return is23(root); }
  bool is23(node* x) {
    if (x == nullptr) return true;
    if (isRed(x->right)) return false;
    if (x != root && isRed(x) && isRed(x->left)) { return false; }
    return is23(x->left) && is23(x->right);
  }

  bool isBalanced() {
    int black = 0;     // number of black links on path from root to min
    node* x = root;
    while (x != nullptr) {
      if (!isRed(x)) black++;
      x = x->left;
    }
    return isBalanced(root, black);
  }

  bool isBalanced(node* x, int black) {
    if (x == nullptr) return black == 0;
    if (!isRed(x)) black--;
    return isBalanced(x->left, black) && isBalanced(x->right, black);
  }

  array_queue<Key> level_order() {
    array_queue<Key> keys;
    array_queue<node*> q;

    q.enqueue(root);
    while (!q.empty()) {
      node* x = q.dequeue();
      if (x == nullptr) { continue; }

      keys.enqueue(x->key);
      q.enqueue(x->left);
      q.enqueue(x->right);
    }
    return keys;
  }

public:
  static void test_bst_redblack(const std::string& filename) {
    char buf[BUFSIZ];
    bst_red_black<std::string, int> st;

    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
      std::cerr << "Could not open file: '" << filename << "'\n";  exit(2);
    }

    std::cout << "Building symbol table for file: '" << filename << "'. \n";
    std::cout << "(punctuation automatically stripped)... \n";
    int i = 0;
    std::string s;
    while (ifs >> s) {
      strcpy(buf, s.c_str());
      std::string key = std::string(buf);
      if (key != "") { st.put(key, i++); }
    }
    std::cout << "\n";

    std::cout << "\nin level order (root to leaves)...\n";
    array_queue<Key> keys = st.level_order();
    for (std::string& key : keys) {
      std::cout << std::setw(12) << key << "  " << std::setw(2) << st.get(key) << "\n";
    }

    std::cout << "\nin alphabetical order...\n";
    keys = st.keys();
    for (std::string& key : keys) {
      std::cout << std::setw(14) << key << "  " << std::setw(2) << st.get(key) << "\n";
    }
  }
};

#endif
