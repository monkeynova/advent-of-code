// http://adventofcode.com/2024/day/24

#include "advent_of_code/2024/day24/day24.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class EvalTree {
 public:
  EvalTree() = default;

  absl::StatusOr<std::vector<std::string_view>>  FixAdder();

  void AddLiteral(std::string_view name, bool value) {
    Node* literal = Ensure(name);
    literal->value = value;
  }

  void AddRule(std::string_view name, std::string_view left, std::string_view right, std::string_view op_str) {
    Node::OpType op = Node::kLiteral;
    if (op_str == "AND") op = Node::kAnd;
    else if (op_str == "OR") op = Node::kOr;
    else if (op_str == "XOR") op = Node::kXor;
    else LOG(FATAL) << "Bad op_str " << op_str;

    Node* rule_node = Ensure(name);
    rule_node->op = op;
    rule_node->left = Ensure(left);
    rule_node->right = Ensure(right);
  }

  std::optional<bool> Evaluate(std::string_view node) {
    auto it = owned_.find(node);
    if (it == owned_.end()) return std::nullopt;
    return it->second->Evaluate();
  }

  std::string DebugString(std::string_view node) {
    auto it = owned_.find(node);
    if (it == owned_.end()) return "";
    return it->second->DebugString();
  }


 private:
  struct Node {
    enum OpType {
      kLiteral = 0,
      kAnd = 1,
      kOr = 2,
      kXor = 3,
    } op;
    std::string_view name;
    bool value = false;
    Node* left = nullptr;
    Node* right = nullptr;

    bool IsXOpY(Node::OpType op_in, int i) {
      if (op != op_in) return false;
      if (left->op != kLiteral) return false;
      if (right->op != kLiteral) return false;
      std::string idx;
      idx.append(1, i / 10 + '0');
      idx.append(1, i % 10 + '0');
      if (left->name.substr(1) != idx) return false;
      if (right->name.substr(1) != idx) return false;
      return true;
    }

    bool Evaluate() {
      if (op == kLiteral) return value;
      bool lval = left->Evaluate();
      bool rval = right->Evaluate();
      switch (op) {
        case kLiteral: break;
        case kAnd: value = lval & rval; break;
        case kOr: value = lval | rval; break;
        case kXor: value = lval ^ rval; break;                
      }
      return value;
    }

    std::string DebugString() {
      static const std::array<std::string_view, 4> kOpNames = {
        "", "&&", "||", "^^"
      };
      if (op == kLiteral) return std::string(name);
      
      return absl::StrCat("(", left->DebugString(), kOpNames[op], right->DebugString(), ")");
    }
  };

  bool ValidateZ(int z_bit, Node* z);
  bool ValidateZCarryTop(int z_bit, Node* carry);
  bool ValidateZCarryBottom(int z_bit, Node* carry);
  Node* FindValid(std::function<bool(Node*)> validator);
  bool TryReplace(Node* bad, std::function<bool(Node*)> validator);

  Node* Ensure(std::string_view name) {
    auto it = owned_.find(name);
    if (it != owned_.end()) return it->second.get();
    return owned_.emplace(name, std::make_unique<Node>(Node{.name = name})).first->second.get();
  }
  Node* Find(std::string_view name) {
    auto it = owned_.find(name);
    if (it != owned_.end()) return it->second.get();
    return nullptr;
  }

  absl::flat_hash_map<std::string_view, std::unique_ptr<Node>> owned_;

  absl::flat_hash_set<std::pair<std::string_view, std::string_view>> swaps_;
};

EvalTree::Node* EvalTree::FindValid(std::function<bool(Node*)> validator) {
  for (const auto& [name, ptr] : owned_) {
    if (validator(ptr.get())) {
      return ptr.get();
    }
  }
  return nullptr;
}

bool EvalTree::TryReplace(Node* bad, std::function<bool(Node*)> is_good) {
  LOG(INFO) << "Finding replacement for " << bad->name;
  Node* find = FindValid(is_good);
  if (find != nullptr) {
    LOG(INFO) << "Swap: " << bad->name << "," << find->name;
    swaps_.insert({bad->name, find->name});
    std::swap(*bad, *find);
    return true;
  }
  return false;
}

// Look for (x-1|y-1) & Carry(n-1).
bool EvalTree::ValidateZCarryBottom(int i, Node* carry) {
  if (carry->op != Node::kAnd) {
    return false;
  }

  if (carry->left->IsXOpY(Node::kXor, i - 1)) {
    if (ValidateZCarryTop(i - 1, carry->right)) return true;
    return TryReplace(carry->right, [&](Node* n) { return ValidateZCarryTop(i - 1, n); });

  } else if (carry->right->IsXOpY(Node::kXor, i - 1)) {
    if (ValidateZCarryTop(i - 1, carry->left)) return true;
    return TryReplace(carry->left, [&](Node* n) { return ValidateZCarryTop(i - 1, n); });

  // No valid x^y, see if we have the other half and can swap one in.
  // We skip 2 because the test for ValidateZCarryTop(1, n) is too simple and
  // generates false positives.
  } else if (i > 2 && ValidateZCarryTop(i - 1, carry->left)) {
    return TryReplace(carry->right, [&](Node* n) { return n->IsXOpY(Node::kXor, i - 1); });

  } else if (i > 2 && ValidateZCarryTop(i - 1, carry->right)) {
    return TryReplace(carry->left, [&](Node* n) { return n->IsXOpY(Node::kXor, i - 1); });
  }

  return false;
}

// Look for (x&y) | ((x|y) & Carry(n-1)).
bool EvalTree::ValidateZCarryTop(int i, Node* carry) {
  if (i == 1) {
    return carry->IsXOpY(Node::kAnd, 0);
  }

  if (carry->op != Node::kOr) {
    return false;
  }

  if (carry->left->IsXOpY(Node::kAnd, i - 1)) {
    if (ValidateZCarryBottom(i, carry->right)) return true;
    return TryReplace(carry->right, [&](Node* n) { return ValidateZCarryTop(i, n); });

  } else if (carry->right->IsXOpY(Node::kAnd, i - 1)) {
    if (ValidateZCarryBottom(i, carry->left)) return true;
    return TryReplace(carry->left, [&](Node* n) { return ValidateZCarryTop(i, n); });

  // No valid x&y, see if we have the other half and can swap one in.
  } else if (ValidateZCarryBottom(i, carry->left)) {
    return TryReplace(carry->right, [&](Node* n) { return n->IsXOpY(Node::kAnd, i - 1); });
  } else if (ValidateZCarryBottom(i, carry->right)) {
    return TryReplace(carry->left, [&](Node* n) { return n->IsXOpY(Node::kAnd, i - 1); });
  }

  return false;
}

// Look for (x^y) ^ Carry().
bool EvalTree::ValidateZ(int i, Node* z) {
  if (i == 0) {
    return z->IsXOpY(Node::kXor, 0);
  }

  if (z->op != Node::kXor) {
    return false;
  }

  if (z->left->IsXOpY(Node::kXor, i)) {
    if (ValidateZCarryTop(i, z->right)) return true;
    return TryReplace(z->right, [&](Node* n) { return ValidateZCarryTop(i, n); });

  } else if (z->right->IsXOpY(Node::kXor, i)) {
    if (ValidateZCarryTop(i, z->left)) return true;
    return TryReplace(z->left, [&](Node* n) { return ValidateZCarryTop(i, n); });

   // No valid x^y, see if we have a carry and can swap one in.
  } else if (ValidateZCarryTop(i, z->left)) {
    return TryReplace(z->right, [&](Node* n) { return n->IsXOpY(Node::kXor, i); });

  } else if (ValidateZCarryTop(i, z->right)) {
    return TryReplace(z->left, [&](Node* n) { return n->IsXOpY(Node::kXor, i); });
  }

  return false;
}

absl::StatusOr<std::vector<std::string_view>> EvalTree::FixAdder() {
  for (int i = 0; true; ++i) {
    Node* z = Find(absl::StrFormat("z%02d", i));
    if (z == nullptr) return absl::InvalidArgumentError("Could not find z");
    Node* z_next = Find(absl::StrFormat("z%02d", i + 1));
    if (z_next == nullptr) {
      // Last bit is just a carry.
      if (ValidateZCarryTop(i, z)) break;
      if (TryReplace(z, [&](Node* n) { return ValidateZCarryTop(i, n); })) break;
      return absl::InvalidArgumentError("Could not fix");
    }
    if (ValidateZ(i, z)) continue;
    if (TryReplace(z, [&](Node* n) { return ValidateZ(i, n); })) continue;
    return absl::InvalidArgumentError("Could not fix");
  }

  LOG(INFO) << swaps_.size();
  absl::flat_hash_set<std::string_view> invalid;
  for (const auto& pair : swaps_) {
    LOG(INFO) << pair.first << " <-> " << pair.second;
    invalid.insert(pair.first);
    invalid.insert(pair.second);    
  }

  return std::vector<std::string_view>(invalid.begin(), invalid.end());
}

}  // namespace

absl::StatusOr<std::string> Day_2024_24::Part1(
    absl::Span<std::string_view> input) const {
  EvalTree eval_tree;
  bool parse_values = true;
  for (std::string_view line : input) {
    if (parse_values) {
      if (line.empty()) {
        parse_values = false;
        continue;
      }

      if (line.size() != 6) return absl::InvalidArgumentError("bad line");
      if (line.substr(3, 2) != ": ") return absl::InvalidArgumentError("bad line");
      std::string_view node = line.substr(0, 3);
      int val = (line.substr(5, 1) == "1");
      eval_tree.AddLiteral(node, val);
      continue;
    }

    std::string_view n1, op, n2, out;
    if (!RE2::FullMatch(line, "(\\S+) (\\S+) (\\S+) -> (\\S+)", &n1, &op, &n2, &out)) {
      return absl::InvalidArgumentError("bad line");
    }
    eval_tree.AddRule(out, n1, n2, op);
  }
  int64_t out = 0;
  for (int i = 0; true; ++i) {
    std::optional<bool> bit = eval_tree.Evaluate(absl::StrFormat("z%02d", i));
    if (!bit) break;
    out |= int64_t{*bit} << i;
  }  

  return AdventReturn(out);
}

absl::StatusOr<std::string> Day_2024_24::Part2(
    absl::Span<std::string_view> input) const {
  EvalTree eval_tree;
  bool parse_values = true;
  for (std::string_view line : input) {
    if (parse_values) {
      if (line.empty()) {
        parse_values = false;
        continue;
      }

      if (line.size() != 6) return absl::InvalidArgumentError("bad line");
      if (line.substr(3, 2) != ": ") return absl::InvalidArgumentError("bad line");
      std::string_view node = line.substr(0, 3);
      int val = (line.substr(5, 1) == "1");
      eval_tree.AddLiteral(node, val);
      continue;
    }

    std::string_view n1, op, n2, out;
    if (!RE2::FullMatch(line, "(\\S+) (\\S+) (\\S+) -> (\\S+)", &n1, &op, &n2, &out)) {
      return absl::InvalidArgumentError("bad line");
    }
    eval_tree.AddRule(out, n1, n2, op);
  }

  ASSIGN_OR_RETURN(std::vector<std::string_view> swapped,
                   eval_tree.FixAdder());
  absl::c_sort(swapped);

  return AdventReturn(absl::StrJoin(swapped, ","));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/24,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_24()); });

}  // namespace advent_of_code
