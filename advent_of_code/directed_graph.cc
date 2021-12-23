#include "advent_of_code/directed_graph.h"

namespace advent_of_code {

std::vector<std::vector<absl::string_view>> DirectedGraphBase::Forest() const {
  std::vector<std::vector<absl::string_view>> forest;

  absl::flat_hash_set<absl::string_view> to_assign = nodes();
  while (!to_assign.empty()) {
    forest.push_back({});
    std::deque<absl::string_view> frontier = {*to_assign.begin()};
    while (!frontier.empty()) {
      absl::string_view name = frontier.front();
      forest.back().push_back(name);
      frontier.pop_front();
      to_assign.erase(name);
      const std::vector<absl::string_view>* incoming = Incoming(name);
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