#ifndef _TRIE_H
#define _TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <algorithm>

template<typename T>
void free_func(T* p){
    free(p);
}

template<typename T>
class Trie
{
public:
  Trie()
    {
      m_root.Clear();
    }
  bool Insert(const char *key, T *value, int rank, int key_len = -1);
  T* Search(const char *key, int key_len = -1) const;
  void Clear();
  bool Empty() const { return m_value_list.empty(); }
  virtual ~Trie() { Clear(); }
private:
  static const int N_NODE_CHILDREN = 256;
  class Node
  {
  public:
    T* value;
    bool occupied;
    int rank;

    void Clear() {
      value = NULL;
      occupied = false;
      rank = 0;
      for(int i=0;i<N_NODE_CHILDREN;i++){
          children[i] = NULL;
      }
    }

    inline Node* GetChild(char index) const { return children[static_cast<unsigned char>(index)]; }
    inline Node* AllocateChild(char index) {
      Node * child = children[static_cast<unsigned char>(index)];
      child = static_cast<Node*>(malloc(sizeof(Node)));
      child->Clear();
      return child;
    }

  private:
    Node *children[N_NODE_CHILDREN];
  };

  Node m_root;
  std::deque<T*> m_value_list;

  void _CheckArgs(const char *key, int &key_len) const;
  void _Clear(Node *node);

  Trie(const Trie<T> &rhs) { };
  Trie &operator =(const Trie<T> &rhs) { return *this; }
};

template<typename T>
void
Trie<T>::_CheckArgs(const char *key, int &key_len) const
{
  if (!key) {
    key_len = 0;
  }
  else if (key_len == -1) {
    key_len = strlen(key);
  }
}

template<typename T>
bool
Trie<T>::Insert(const char *key, T* value, int rank, int key_len)
{
  _CheckArgs(key, key_len);

  Node *next_node;
  Node *curr_node = &m_root;
  int i = 0;

  while (true) {

    if (i == key_len) {
      break;
    }

    next_node = curr_node->GetChild(key[i]);
    if (!next_node) {
      while (i < key_len) {
        curr_node = curr_node->AllocateChild(key[i]);
        ++i;
      }
      break;
    }
    curr_node = next_node;
    ++i;
  }

  if (curr_node->occupied) {
    return false;
  }

  curr_node->occupied = true;
  curr_node->value = value;
  curr_node->rank = rank;
  m_value_list.push_back(curr_node->value);
  return true;
}

template<typename T>
T*
Trie<T>::Search(const char *key, int key_len) const
{
  _CheckArgs(key, key_len);

  const Node *found_node = NULL;
  const Node *curr_node = &m_root;
  int i = 0;

  while (curr_node) {
    if (curr_node->occupied) {
      if (!found_node || curr_node->rank <= found_node->rank) {
        found_node = curr_node;
      }
    }
    if (i == key_len) {
      break;
    }
    curr_node = curr_node->GetChild(key[i]);
    ++i;
  }

  if (found_node) {
    return found_node->value;
  }

  return NULL;
}


template<typename T>
void
Trie<T>::_Clear(Node *node)
{
  Node *child;

  for (int i = 0; i < N_NODE_CHILDREN; ++i) {
    child = node->GetChild(i);
    if (child) {
      _Clear(child);
      free(child);
    }
  }
}

template<typename T>
void
Trie<T>::Clear()
{
  for_each(m_value_list.begin(),m_value_list.end(),free_func);
  m_value_list.clear();

  _Clear(&m_root);
  m_root.Clear();
}


#endif // _TRIE_H
