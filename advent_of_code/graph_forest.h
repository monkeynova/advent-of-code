#ifndef ADVENT_OF_CODE_GRAPH_FOREST_H
#define ADVENT_OF_CODE_GRAPH_FOREST_H

#include <deque>

#include "advent_of_code/directed_graph.h"

namespace advent_of_code {

template <typename Storage>
std::vector<std::vector<absl::string_view>> GraphForest(
    const DirectedGraph<Storage>& g) {
  std::vector<std::vector<absl::string_view>> forest;

  absl::flat_hash_set<absl::string_view> to_assign = g.nodes();
  while (!to_assign.empty()) {
    forest.push_back({});
    std::deque<absl::string_view> frontier = {*to_assign.begin()};
    while (!frontier.empty()) {
      absl::string_view name = frontier.front();
      forest.back().push_back(name);
      frontier.pop_front();
      to_assign.erase(name);
      const std::vector<absl::string_view>* incoming = g.Incoming(name);
      if (incoming != nullptr) {
        for (absl::string_view tmp : *incoming) {
          if (to_assign.contains(tmp)) frontier.push_back(tmp);
        }
      }
    }
  }

  return forest;
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_GRAPH_FOREST_H