#include <iostream>
#include <vector>
#include <string>
#include <utility>

#include "TrieMap.hpp"
#include "Pattern.hpp"


using namespace std;

namespace qhits {
  
  typedef pair<size_t, size_t> hit;

  void buildQIndex(TrieMap<size_t>& index, Pattern& qPattern, string& text) {
    size_t maxPatternStart = text.size() - qPattern.span() + 1;
    for (size_t start = 0; start < maxPatternStart; ++start) {
      index.rewind();
      for (size_t offset : qPattern) index.add(text[start + offset]);
      index.put(start);
    }
    index.rewind();
  }


  vector<hit> findHits(TrieMap<size_t>& index, Pattern& qPattern, string& query) {
    size_t maxPatternStart = query.size() - qPattern.span() + 1;
    vector<hit> hits;
    for (size_t start = 0; start < maxPatternStart; ++start) {
      index.rewind();
      for (size_t offset : qPattern) index.advance(query[start + offset]);
      
      list<size_t> matches = index.get();
      for (size_t match : matches) hits.push_back({start, match});
    }

    index.rewind();
    return hits;
  }
 

}
