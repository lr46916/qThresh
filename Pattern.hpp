#ifndef _PATTERN_H_
#define _PATTERN_H_

#include <vector>

using namespace std;

typedef vector<size_t> container;

/*
 * Represents a general gapped q-gram shape pattern. 
 * 
 * @author Dino Radakovic
 */
class Pattern {

public:

  Pattern(container matchIndices);

  size_t span();
  
  container::iterator begin();

  container::iterator end();

private:
  
  size_t max;

  container& matches;

};


#endif
