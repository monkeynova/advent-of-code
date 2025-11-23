// http://adventofcode.com/2024/day/13

#include "advent_of_code/2024/day13/day13.h"

#include "absl/log/log.h"
#include "advent_of_code/point.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

std::optional<int64_t> FindCost(Point a, Point b, Point prize) {
  // a_c * a_x + b_c * b_x = p_x
  // a_c * a_y + b_c * b_y = p_y

  // bc * (bx * ay - by * ax) = px * ay - py * ax
  // bc = px * ay - py * ax / (bx * ay - by * ax)

  // ac * (ax*by - ay *bx) = px * by - py * bx
  // ac = px * by - py * bx / (ax*by - ay *bx)

  int64_t b_c = prize.x * a.y - prize.y * a.x;
  int64_t b_c_d = b.x * a.y - b.y * a.x;
  if (b_c % b_c_d != 0) return std::nullopt;
  b_c /= b_c_d;
  if (b_c < 0) return std::nullopt;

  int64_t a_c = prize.x * b.y - prize.y * b.x;
  int64_t a_c_d = a.x * b.y - a.y * b.x;
  if (a_c % a_c_d != 0) return std::nullopt;
  a_c /= a_c_d;
  if (a_c < 0) return std::nullopt;

  return 3 * a_c + b_c;
}

std::optional<int64_t> FindCost2(Point a, Point b, Point prize) {
  const int64_t kDelta = 10000000000000;

  int64_t b_c = (prize.x + kDelta) * a.y - (prize.y + kDelta) * a.x;
  int64_t b_c_d = b.x * a.y - b.y * a.x;
  if (b_c % b_c_d != 0) return std::nullopt;
  b_c /= b_c_d;
  if (b_c < 0) return std::nullopt;

  int64_t a_c = (prize.x + kDelta) * b.y - (prize.y + kDelta) * b.x;
  int64_t a_c_d = a.x * b.y - a.y * b.x;
  if (a_c % a_c_d != 0) return std::nullopt;
  a_c /= a_c_d;
  if (a_c < 0) return std::nullopt;

  return 3 * a_c + b_c;
}

}  // namespace

absl::StatusOr<std::string> Day_2024_13::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() % 4 != 3) return absl::InvalidArgumentError("% 4");

  int64_t total_cost = 0;
  for (int i = 0; i < input.size(); i += 4) {
    Point a;
    Point b;
    Point prize;
    {
      Tokenizer t(input[i]);
      RETURN_IF_ERROR(t.NextAre({"Button", "A", ":", "X", "+"}));
      ASSIGN_OR_RETURN(a.x, t.NextInt());
      RETURN_IF_ERROR(t.NextAre({",", "Y", "+"}));
      ASSIGN_OR_RETURN(a.y, t.NextInt());
      RETURN_IF_ERROR(t.AssertDone());
    }
    {
      Tokenizer t(input[i + 1]);
      RETURN_IF_ERROR(t.NextAre({"Button", "B", ":", "X", "+"}));
      ASSIGN_OR_RETURN(b.x, t.NextInt());
      RETURN_IF_ERROR(t.NextAre({",", "Y", "+"}));
      ASSIGN_OR_RETURN(b.y, t.NextInt());
      RETURN_IF_ERROR(t.AssertDone());
    }
    {
      Tokenizer t(input[i + 2]);
      RETURN_IF_ERROR(t.NextAre({"Prize", ":", "X", "="}));
      ASSIGN_OR_RETURN(prize.x, t.NextInt());
      RETURN_IF_ERROR(t.NextAre({",", "Y", "="}));
      ASSIGN_OR_RETURN(prize.y, t.NextInt());
      RETURN_IF_ERROR(t.AssertDone());
    }
    std::optional<int64_t> cost = FindCost(a, b, prize);
    if (cost) total_cost += *cost;
  }
  return AdventReturn(total_cost);
}

absl::StatusOr<std::string> Day_2024_13::Part2(
    absl::Span<std::string_view> input) const {
  int64_t total_cost = 0;
  for (int i = 0; i < input.size(); i += 4) {
    Point a;
    Point b;
    Point prize;
    {
      Tokenizer t(input[i]);
      RETURN_IF_ERROR(t.NextAre({"Button", "A", ":", "X", "+"}));
      ASSIGN_OR_RETURN(a.x, t.NextInt());
      RETURN_IF_ERROR(t.NextAre({",", "Y", "+"}));
      ASSIGN_OR_RETURN(a.y, t.NextInt());
      RETURN_IF_ERROR(t.AssertDone());
    }
    {
      Tokenizer t(input[i + 1]);
      RETURN_IF_ERROR(t.NextAre({"Button", "B", ":", "X", "+"}));
      ASSIGN_OR_RETURN(b.x, t.NextInt());
      RETURN_IF_ERROR(t.NextAre({",", "Y", "+"}));
      ASSIGN_OR_RETURN(b.y, t.NextInt());
      RETURN_IF_ERROR(t.AssertDone());
    }
    {
      Tokenizer t(input[i + 2]);
      RETURN_IF_ERROR(t.NextAre({"Prize", ":", "X", "="}));
      ASSIGN_OR_RETURN(prize.x, t.NextInt());
      RETURN_IF_ERROR(t.NextAre({",", "Y", "="}));
      ASSIGN_OR_RETURN(prize.y, t.NextInt());
      RETURN_IF_ERROR(t.AssertDone());
    }
    std::optional<int64_t> cost = FindCost2(a, b, prize);
    if (cost) total_cost += *cost;
  }
  return AdventReturn(total_cost);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/13,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_13()); });

}  // namespace advent_of_code
