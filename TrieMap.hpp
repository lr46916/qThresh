#ifndef _TRIEMAP_H_
#define _TRIEMAP_H_

#define TERMINAL '\0'
#define MAX_ALPHABET_SIZE 'Z' - 'A'

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

/**
 * A simple Trie implementation.
 * Supports adding strings and querying, both in O(|s|) time, where s is the given string. 
 * Also supports stateful incremental entries and rewinding to the initial state. 
 *
 * @author Dino Radakovic
 */


template<typename T>
class TrieMap {

private:

  struct Node {
    char symbol;
    size_t childCount;
    Node** children = NULL;
    list<T> values;

    Node(char s, size_t alphaSize) : symbol(s), childCount(alphaSize) {
      children = new Node*[childCount];
    }

    // BEWARE: this doesn't work with cycles, extending should be done with care
    ~Node() {
      for (size_t i = 0; i < childCount; ++i) {
	if (children[i] != NULL) delete children[i];	  
      }
      delete[] children;
    }
    
  };

 
  size_t indexOf(char symbol) {
    return symbolIndex[symbol - 'A'];
  } 

  size_t alphabetSize;
  size_t* symbolIndex;

  Node** roots;
  Node* current;

public:


  TrieMap(size_t alphaSize, size_t index[]) : alphabetSize(alphaSize) {
    roots = new Node*[alphaSize]();
    current = NULL;
    symbolIndex = new size_t[MAX_ALPHABET_SIZE];
    for (size_t i = 0; i < MAX_ALPHABET_SIZE; ++i) symbolIndex[i] = index[i];
  }

  TrieMap() : TrieMap<T>(nucleotideTrie()) {}

  ~TrieMap() {
    for (size_t i = 0; i < alphabetSize; ++i) {
      if (roots[i] != NULL) delete roots[i];
    }
    delete[] roots;
    delete[] symbolIndex;
  }

  void add(char symbol) {
    size_t index = indexOf(symbol);
    Node** newSiblings = this->current == NULL ? roots : this->current->children;
    if (newSiblings[index] == NULL) newSiblings[index] = new Node(symbol, alphabetSize);
    this->current = newSiblings[index];
  }

  void advance(char symbol) {
    size_t index = indexOf(symbol);
    this->current = this->current == NULL ? roots[index] : this->current->children[index]; 
     
  }

  void put(string key, T value) {
    Node* previous = this->current;
    this->rewind();
    for (auto k : key) add(k);
    put(value);
    this->current = previous; 
  }

  void put(T value) {
    if (this->current != NULL) this->current->values.push_back(value);
  }

  void putRewind(T value) {
    this->put(value);
    this->rewind();
  }


  list<T> get() {
    if (current == NULL) return list<T>();
    else return current->values;
  }

  list<T> get(string key) {
    Node* previous = this->current;

    this->rewind();
    this->current = roots[indexOf(key[0])];

    for (size_t i = 1; i < key.length(); ++i) {
      size_t index = indexOf(key[i]);
      if (this->current == NULL) break;
      else this->current = this->current->children[index];
    }

    list<T> result = current == NULL ? list<T>() : current->values;
    this->current = previous;
    return result;
  }



  void rewind() {
    current = NULL;
  }

  /**
   * Returns `true` if the trie contains the query string.
   */
  bool contains(string query) {
    Node** lvl = roots;
    for (auto q : query) {
      size_t index = indexOf(q);
      if (lvl == NULL || lvl[index] == NULL) return false;
      lvl = lvl[index]->children;
    }
    return true;
  }


  static TrieMap<T> nucleotideTrie() {  
    size_t nucleotideIndex['Z' - 'A'];
    nucleotideIndex['A' - 'A'] = 0;
    nucleotideIndex['G' - 'A'] = 1;
    nucleotideIndex['C' - 'A'] = 2;
    nucleotideIndex['T' - 'A'] = nucleotideIndex['U' - 'A'] = 3;
    return TrieMap<T>(4, nucleotideIndex);
  }
  


};

#endif
