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

  std::vector<std::string_view>  FindWrong();

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

  Node* Ensure(std::string_view name) {
    auto it = owned_.find(name);
    if (it != owned_.end()) return it->second.get();
    return owned_.emplace(name, std::make_unique<Node>(Node{.name = name})).first->second.get();
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

bool EvalTree::ValidateZCarryBottom(int i, Node* carry) {
  Node* sub_carry = nullptr;
  if (carry->op != Node::kAnd) {
    // LOG(INFO) << "Bad sub-carry: " << i << ": " << carry->DebugString();
    return false;
  }
  if (carry->left->IsXOpY(Node::kXor, i - 1)) {
    sub_carry = carry->right;
  } else if (carry->right->IsXOpY(Node::kXor, i - 1)) {
    sub_carry = carry->left;
  } else {
    // LOG(INFO) << "No sub-carry-xor: " << i << ": " << carry->DebugString();
    return false;
  }

  if (!ValidateZCarryTop(i - 1, sub_carry)) {
    LOG(INFO) << "Finding replacement for " << sub_carry->name;
    Node* find = FindValid([&](Node* n) { return ValidateZCarryTop(i - 1, sub_carry); });
    if (find != nullptr) {
      LOG(INFO) << "Swap: " << sub_carry->name << "," << find->name;
      swaps_.insert({sub_carry->name, find->name});
      std::swap(*sub_carry, *find);
      return true;
    }
    return false;
  }

  return true;
}

bool EvalTree::ValidateZCarryTop(int i, Node* carry) {
  if (i == 1) {
    if (!carry->IsXOpY(Node::kAnd, 0)) {
      // LOG(INFO) << "Bad carry1: " << i << ": " << carry->DebugString();
      return false;
    }
    return true;
  }

  if (carry->op != Node::kOr) {
    // LOG(INFO) << "Bad carry: " << i << ": " << carry->DebugString();
    return false;
  }
  Node* sub_carry = nullptr;
  if (carry->left->IsXOpY(Node::kAnd, i - 1)) {
    sub_carry = carry->right;
  } else if (carry->right->IsXOpY(Node::kAnd, i - 1)) {
    sub_carry = carry->left;
  } else {
    // LOG(INFO) << "No carry-and: " << i << ": " << carry->DebugString();
    return false;
  }

  if (!ValidateZCarryBottom(i, sub_carry)) {
    LOG(INFO) << "Finding replacement for " << sub_carry->name;
    Node* find = FindValid([&](Node* n) { return ValidateZCarryBottom(i, sub_carry); });
    if (find != nullptr) {
      LOG(INFO) << "Swap: " << sub_carry->name << "," << find->name;
      swaps_.insert({sub_carry->name, find->name});
      std::swap(*sub_carry, *find);
      return true;
    }
    return false;
  }
  return true;
}

bool EvalTree::ValidateZ(int i, Node* z) {
  if (i == 0) {
    if (!z->IsXOpY(Node::kXor, 0)) {
      LOG(INFO) << "Bad Z" << i << ": " << z->DebugString();
      return false;
    }
    return true;
  }

  if (z->op != Node::kXor) {
    LOG(INFO) << "Bad Z" << i << ": " << z->DebugString();
    return false;
  }
  Node* carry = nullptr;
  if (z->left->IsXOpY(Node::kXor, i)) {
    carry = z->right;
  } else if (z->right->IsXOpY(Node::kXor, i)) {
    carry = z->left;
  } else {
    LOG(INFO) << "No z-Xor: " << i << ": " << z->DebugString();
    return false;
  }

  if (!ValidateZCarryTop(i, carry)) {
    LOG(INFO) << "Finding replacement for " << carry->name;
    Node* find = FindValid([&](Node* n) { return ValidateZCarryTop(i, carry); });
    if (find != nullptr) {
      LOG(INFO) << "Swap: " << carry->name << "," << find->name;
      swaps_.insert({carry->name, find->name});
      std::swap(*carry, *find);
      return true;
    }
    return false;
  }
  return true;
}

std::vector<std::string_view> EvalTree::FindWrong() {
  // 45 is special, but not broken in my input (maybe?).
  for (int i = 0; i <= 44; ++i) {
    Node* z = Ensure(absl::StrFormat("z%02d", i));
    LOG(INFO) << i << ": " << z->DebugString();
    if (!ValidateZ(i, z)) {
      LOG(INFO) << "Finding replacement for " << z->name;
      Node* find = FindValid([&](Node* n) { return ValidateZ(i, n); });
      if (find != nullptr) {
        LOG(INFO) << "Swap: " << z->name << "," << find->name;
        swaps_.insert({z->name, find->name});
        std::swap(*z, *find);
      } else {
        Node* find2 = nullptr;
        if (z->op == Node::kXor) {
          if (ValidateZCarryTop(i, z->left)) {
            find2 = FindValid([&](Node* n) { return n->IsXOpY(Node::kXor, i); });
            LOG(INFO) << "Swap: " << z->right->name << "," << find2->name;
            swaps_.insert({z->right->name, find2->name});
            std::swap(*z->right, *find2);
          } else if (ValidateZCarryTop(i, z->right)) {
            find2 = FindValid([&](Node* n) { return n->IsXOpY(Node::kXor, i); });
            LOG(INFO) << "Swap: " << z->left->name << "," << find2->name;
            swaps_.insert({z->left->name, find2->name});
            std::swap(*z->left, *find2);
          } else {
            LOG(FATAL) << "Could not fix";
          }
        }
        if (find2 != nullptr) {

        } else {
          LOG(FATAL) << "Could not fix";
        }
      }
    }
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

  std::vector<std::string_view> swapped = eval_tree.FindWrong();
  absl::c_sort(swapped);

  return AdventReturn(absl::StrJoin(swapped, ","));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/24,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_24()); });

}  // namespace advent_of_code
