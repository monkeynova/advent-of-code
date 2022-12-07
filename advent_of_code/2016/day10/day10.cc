// https://adventofcode.com/2016/day/10
//
// --- Day 10: Balance Bots ---
// ----------------------------
// 
// You come upon a factory in which many robots are zooming around
// handing small microchips to each other.
// 
// Upon closer examination, you notice that each bot only proceeds when
// it has two microchips, and once it does, it gives each one to a
// different bot or puts it in a marked "output" bin. Sometimes, bots
// take microchips from "input" bins, too.
// 
// Inspecting one of the microchips, it seems like they each contain a
// single number; the bots must use some logic to decide what to do with
// each chip. You access the local control computer and download the
// bots' instructions (your puzzle input).
// 
// Some of the instructions specify that a specific-valued microchip
// should be given to a specific bot; the rest of the instructions
// indicate what a given bot should do with its lower-value or
// higher-value chip.
// 
// For example, consider the following instructions:
// 
// value 5 goes to bot 2
// bot 2 gives low to bot 1 and high to bot 0
// value 3 goes to bot 1
// bot 1 gives low to output 1 and high to bot 0
// bot 0 gives low to output 2 and high to output 0
// value 2 goes to bot 2     
// 
// * Initially, bot 1 starts with a value-3 chip, and bot 2 starts with
// a value-2 chip and a value-5 chip.
// 
// * Because bot 2 has two microchips, it gives its lower one (2) to
// bot 1 and its higher one (5) to bot 0.
// 
// * Then, bot 1 has two microchips; it puts the value-2 chip in output
// 1 and gives the value-3 chip to bot 0.
// 
// * Finally, bot 0 has two microchips; it puts the 3 in output 2 and
// the 5 in output 0.
// 
// In the end, output bin 0 contains a value-5 microchip, output bin 1
// contains a value-2 microchip, and output bin 2 contains a value-3
// microchip. In this configuration, bot number 2 is responsible for
// comparing value-5 microchips with value-2 microchips.
// 
// Based on your instructions, what is the number of the bot that is
// responsible for comparing value-61 microchips with value-17
// microchips?
//
// --- Part Two ---
// ----------------
// 
// What do you get if you multiply together the values of one chip in
// each of outputs 0, 1, and 2?


#include "advent_of_code/2016/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

struct Input {
  int value = -1;
  int bot_num = -1;
};

struct Bot {
  int low_bot_num = -1;
  int high_bot_num = -1;
  int low_output_num = -1;
  int high_output_num = -1;
  std::vector<Input> inputs;
};

absl::StatusOr<int> Eval(absl::flat_hash_map<int, Bot>* bots, int bot_num,
                         Input* in) {
  VLOG(1) << "Evaling: {" << in->value << ", " << in->bot_num << "} for "
          << bot_num;
  if (in->value != -1) return in->value;

  auto it = bots->find(in->bot_num);
  if (it == bots->end()) return Error("No bot: ", in->bot_num);
  Bot& in_bot = it->second;

  absl::StatusOr<int> v1 = Eval(bots, in->bot_num, &in_bot.inputs[0]);
  if (!v1.ok()) return v1.status();
  absl::StatusOr<int> v2 = Eval(bots, in->bot_num, &in_bot.inputs[1]);
  if (!v2.ok()) return v2.status();

  if (in_bot.low_bot_num == bot_num) {
    in->value = std::min(*v1, *v2);
  } else if (in_bot.high_bot_num == bot_num) {
    in->value = std::max(*v1, *v2);
  } else {
    return Error("Bot is neither high nor low");
  }

  VLOG(1) << "Assigning: " << in->value << " to " << bot_num;
  return in->value;
}

absl::StatusOr<int> EvalOutput(absl::flat_hash_map<int, Bot>* bots,
                               int output_num, Input* in) {
  VLOG(1) << "Evaling: {" << in->value << ", " << in->bot_num
          << "} for O:" << output_num;
  if (in->value != -1) return in->value;

  auto it = bots->find(in->bot_num);
  if (it == bots->end()) return Error("No bot: ", in->bot_num);
  Bot& in_bot = it->second;

  absl::StatusOr<int> v1 = Eval(bots, in->bot_num, &in_bot.inputs[0]);
  if (!v1.ok()) return v1.status();
  absl::StatusOr<int> v2 = Eval(bots, in->bot_num, &in_bot.inputs[1]);
  if (!v2.ok()) return v2.status();

  if (in_bot.low_output_num == output_num) {
    in->value = std::min(*v1, *v2);
  } else if (in_bot.high_output_num == output_num) {
    in->value = std::max(*v1, *v2);
  } else {
    return Error("Bot is neither high nor low");
  }

  VLOG(1) << "Assigning: " << in->value << " to O:" << output_num;
  return in->value;
}

absl::StatusOr<int> FindCmp(absl::flat_hash_map<int, Bot>* bots, int test_v1,
                            int test_v2) {
  for (auto& [bot_num, bot] : *bots) {
    if (bot.inputs.size() != 2) {
      return Error("Bad input: ", bot_num, ": ", bot.inputs.size());
    }
    absl::StatusOr<int> v1 = Eval(bots, bot_num, &bot.inputs[0]);
    if (!v1.ok()) return v1.status();
    absl::StatusOr<int> v2 = Eval(bots, bot_num, &bot.inputs[1]);
    if (!v2.ok()) return v2.status();
    if (*v1 == test_v1 && *v2 == test_v2) return bot_num;
    if (*v2 == test_v1 && *v1 == test_v2) return bot_num;
  }

  return Error("Not found");
}

}  // namespace

absl::StatusOr<std::string> Day_2016_10::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int, Bot> bots;
  for (absl::string_view ins : input) {
    Input i;
    int bot_num;
    int low_bot_num;
    int high_bot_num;
    int low_output_num;
    int high_output_num;
    if (RE2::FullMatch(ins, "value (\\d+) goes to bot (\\d+)", &i.value,
                       &bot_num)) {
      bots[bot_num].inputs.push_back(i);
    } else if (RE2::FullMatch(
                   ins,
                   "bot (\\d+) gives low to bot (\\d+) and high to bot (\\d+)",
                   &bot_num, &low_bot_num, &high_bot_num)) {
      bots[bot_num].high_bot_num = high_bot_num;
      bots[high_bot_num].inputs.push_back({.bot_num = bot_num});
      bots[bot_num].low_bot_num = low_bot_num;
      bots[low_bot_num].inputs.push_back({.bot_num = bot_num});
    } else if (RE2::FullMatch(ins,
                              "bot (\\d+) gives low to output (\\d+) and high "
                              "to bot (\\d+)",
                              &bot_num, &low_output_num, &high_bot_num)) {
      bots[bot_num].high_bot_num = high_bot_num;
      bots[high_bot_num].inputs.push_back({.bot_num = bot_num});
      bots[bot_num].low_output_num = low_output_num;
    } else if (RE2::FullMatch(
                   ins,
                   "bot (\\d+) gives low to bot (\\d+) and high to bot (\\d+)",
                   &bot_num, &low_bot_num, &high_output_num)) {
      bots[bot_num].high_output_num = high_output_num;
      bots[bot_num].low_bot_num = low_bot_num;
      bots[low_bot_num].inputs.push_back({.bot_num = bot_num});
    } else if (RE2::FullMatch(ins,
                              "bot (\\d+) gives low to output (\\d+) and high "
                              "to output (\\d+)",
                              &bot_num, &low_output_num, &high_output_num)) {
      // Nothing for the tree.
    } else {
      return Error("Bad instruction: ", ins);
    }
  }

  bool saw_61 = false;
  for (const auto& [_, bot] : bots) {
    for (const Input& i : bot.inputs) {
      if (i.value == 61) saw_61 = true;
    }
  }

  if (saw_61) {
    return IntReturn(FindCmp(&bots, 17, 61));
  }
  return IntReturn(FindCmp(&bots, 2, 5));
}

absl::StatusOr<std::string> Day_2016_10::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int, Bot> bots;
  absl::flat_hash_map<int, Input> outputs;
  for (absl::string_view ins : input) {
    Input i;
    int bot_num;
    int low_bot_num;
    int high_bot_num;
    int low_output_num;
    int high_output_num;
    if (RE2::FullMatch(ins, "value (\\d+) goes to bot (\\d+)", &i.value,
                       &bot_num)) {
      bots[bot_num].inputs.push_back(i);
    } else if (RE2::FullMatch(
                   ins,
                   "bot (\\d+) gives low to bot (\\d+) and high to bot (\\d+)",
                   &bot_num, &low_bot_num, &high_bot_num)) {
      bots[bot_num].high_bot_num = high_bot_num;
      bots[high_bot_num].inputs.push_back({.bot_num = bot_num});
      bots[bot_num].low_bot_num = low_bot_num;
      bots[low_bot_num].inputs.push_back({.bot_num = bot_num});
    } else if (RE2::FullMatch(ins,
                              "bot (\\d+) gives low to output (\\d+) and high "
                              "to bot (\\d+)",
                              &bot_num, &low_output_num, &high_bot_num)) {
      bots[bot_num].high_bot_num = high_bot_num;
      bots[high_bot_num].inputs.push_back({.bot_num = bot_num});
      bots[bot_num].low_output_num = low_output_num;
      if (outputs.contains(low_output_num))
        return Error("Dupe output: ", low_output_num);
      outputs[low_output_num] = Input{.bot_num = bot_num};
    } else if (RE2::FullMatch(
                   ins,
                   "bot (\\d+) gives low to bot (\\d+) and high to bot (\\d+)",
                   &bot_num, &low_bot_num, &high_output_num)) {
      bots[bot_num].high_output_num = high_output_num;
      if (outputs.contains(high_output_num))
        return Error("Dupe output: ", high_output_num);
      outputs[high_output_num] = Input{.bot_num = bot_num};
      bots[bot_num].low_bot_num = low_bot_num;
      bots[low_bot_num].inputs.push_back({.bot_num = bot_num});
    } else if (RE2::FullMatch(ins,
                              "bot (\\d+) gives low to output (\\d+) and high "
                              "to output (\\d+)",
                              &bot_num, &low_output_num, &high_output_num)) {
      bots[bot_num].high_output_num = high_output_num;
      if (outputs.contains(high_output_num))
        return Error("Dupe output: ", high_output_num);
      outputs[high_output_num] = Input{.bot_num = bot_num};
      bots[bot_num].low_output_num = low_output_num;
      if (outputs.contains(low_output_num))
        return Error("Dupe output: ", low_output_num);
      outputs[low_output_num] = Input{.bot_num = bot_num};
      // Nothing for the tree.
    } else {
      return Error("Bad instruction: ", ins);
    }
  }

  absl::StatusOr<int> v0 = EvalOutput(&bots, 0, &outputs[0]);
  if (!v0.ok()) return v0.status();
  absl::StatusOr<int> v1 = EvalOutput(&bots, 1, &outputs[1]);
  if (!v1.ok()) return v1.status();
  absl::StatusOr<int> v2 = EvalOutput(&bots, 2, &outputs[2]);
  if (!v2.ok()) return v2.status();

  return IntReturn(*v0 * *v1 * *v2);
}

}  // namespace advent_of_code
