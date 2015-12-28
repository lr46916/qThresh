#include "Pattern.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

Pattern::Pattern(container matchIndices) : matches(matchIndices) {
  sort(matches.begin(), matches.end());
  size_t min = matches.front();
  for (size_t i = 0; i < matches.size(); ++i) matches[i] -= min;
  this->max = matches.back();
}

size_t Pattern::span() {
  return this->max;
}


container::iterator Pattern::begin() {
  return matches.begin();
}

container::iterator Pattern::end() {
  return matches.end();
}


// int main() {
//   container indices = {4, 5, 8, 33};
//   Pattern p(indices);

//   for (auto m : p) {
//     cout << m << endl;
//   }

// }
