// https://adventofcode.com/2019/day/23
//
// --- Day 23: Category Six ---
// ----------------------------
// 
// The droids have finished repairing as much of the ship as they can.
// Their report indicates that this was a Category 6 disaster - not
// because it was that bad, but because it destroyed the stockpile of
// Category 6 network cables as well as most of the ship's network
// infrastructure.
// 
// You'll need to rebuild the network from scratch.
// 
// The computers on the network are standard Intcode computers that
// communicate by sending packets to each other. There are 50 of them in
// total, each running a copy of the same Network Interface Controller
// (NIC) software (your puzzle input). The computers have network
// addresses 0 through 49; when each computer boots up, it will request
// its network address via a single input instruction. Be sure to give
// each computer a unique network address.
// 
// Once a computer has received its network address, it will begin doing
// work and communicating over the network by sending and receiving
// packets. All packets contain two values named X and Y. Packets sent to
// a computer are queued by the recipient and read in the order they are
// received.
// 
// To send a packet to another computer, the NIC will use three output
// instructions that provide the destination address of the packet
// followed by its X and Y values. For example, three output instructions
// that provide the values 10, 20, 30 would send a packet with X=20 and
// Y=30 to the computer with address 10.
// 
// To receive a packet from another computer, the NIC will use an input
// instruction. If the incoming packet queue is empty, provide -1.
// Otherwise, provide the X value of the next packet; the computer will
// then use a second input instruction to receive the Y value for the
// same packet. Once both values of the packet are read in this way, the
// packet is removed from the queue.
// 
// Note that these input and output instructions never block.
// Specifically, output instructions do not wait for the sent packet to
// be received - the computer might send multiple packets before
// receiving any. Similarly, input instructions do not wait for a packet
// to arrive - if no packet is waiting, input instructions should receive
// -1.
// 
// Boot up all 50 computers and attach them to your network. What is the
// Y value of the first packet sent to address 255?
//
// --- Part Two ---
// ----------------
// 
// Packets sent to address 255 are handled by a device called a NAT (Not
// Always Transmitting). The NAT is responsible for managing power
// consumption of the network by blocking certain packets and watching
// for idle periods in the computers.
// 
// If a packet would be sent to address 255, the NAT receives it instead.
// The NAT remembers only the last packet it receives; that is, the data
// in each packet it receives overwrites the NAT's packet memory with the
// new packet's X and Y values.
// 
// The NAT also monitors all computers on the network. If all computers
// have empty incoming packet queues and are continuously trying to
// receive packets without sending packets, the network is considered
// idle.
// 
// Once the network is idle, the NAT sends only the last packet it
// received to address 0; this will cause the computers on the network to
// resume activity. In this way, the NAT can throttle power consumption
// of the network when the ship needs power in other areas.
// 
// Monitor packets released to the computer at address 0 by the NAT. What
// is the first Y value delivered by the NAT to the computer at address 0
// twice in a row?


#include "advent_of_code/2019/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Packet {
  int64_t address;
  int64_t x;
  int64_t y;
};

std::ostream& operator<<(std::ostream& out, const Packet& p) {
  return out << "{a:" << p.address << ",x:" << p.x << ",y:" << p.y << "}";
}

class Network;

class Computer : public IntCode::IOModule {
 public:
  enum class PacketState { kSetAddress = 1, kSetX = 2, kSetY = 3 };

  Computer(Network* network, const IntCode& code, int64_t address)
      : network_(network), code_(code.Clone()), address_(address) {}

  absl::Status StepExecution() {
    run_step_ = true;
    return code_.Run(this);
  }

  bool PauseIntCode() override {
    bool pause = !run_step_;
    run_step_ = false;
    return pause;
  }

  bool idle() const { return idle_; }

  absl::StatusOr<int64_t> Fetch() override {
    switch (in_packet_state_) {
      case PacketState::kSetAddress: {
        VLOG(1) << "Fetch Address: " << address_;
        in_packet_state_ = PacketState::kSetX;
        return address_;
      }
      case PacketState::kSetX: {
        if (in_packet_queue_.empty()) {
          idle_ = true;
          return -1;
        }
        VLOG(2) << "Fetch X: " << in_packet_queue_.front();
        in_packet_state_ = PacketState::kSetY;
        return in_packet_queue_.front().x;
      }
      case PacketState::kSetY: {
        in_packet_state_ = PacketState::kSetX;
        VLOG(2) << "Fetch Y: " << in_packet_queue_.front();
        if (in_packet_queue_.empty()) {
          return absl::InternalError("Requested y with no x");
        }
        int64_t ret = in_packet_queue_.front().y;
        in_packet_queue_.pop_front();
        return ret;
      }
    }
    return absl::InternalError("Bad state");
  }

  absl::Status Put(int64_t val) override {
    switch (out_packet_state_) {
      case PacketState::kSetAddress: {
        out_packet_.address = val;
        out_packet_state_ = PacketState::kSetX;
        break;
      }
      case PacketState::kSetX: {
        out_packet_.x = val;
        out_packet_state_ = PacketState::kSetY;
        break;
      }
      case PacketState::kSetY: {
        out_packet_.y = val;
        out_packet_state_ = PacketState::kSetAddress;
        if (absl::Status st = SendCurrentPacket(); !st.ok()) return st;
        break;
      }
      default:
        return absl::InternalError("Bad state");
    }
    return absl::OkStatus();
  }

  absl::Status RecvPacket(Packet packet) {
    if (packet.address != address_) {
      return absl::InvalidArgumentError(
          absl::StrCat("Wrong address: ", packet.address, " != ", address_));
    }

    VLOG(2) << "RecvPacket: " << packet;
    in_packet_queue_.push_back(packet);
    idle_ = false;
    return absl::OkStatus();
  }

  absl::Status SendCurrentPacket();

 private:
  Network* network_;
  IntCode code_;
  int64_t address_;
  PacketState out_packet_state_ = PacketState::kSetAddress;
  Packet out_packet_;
  std::deque<Packet> in_packet_queue_;
  PacketState in_packet_state_ = PacketState::kSetAddress;
  bool idle_ = false;
  bool run_step_ = true;
};

class Network {
 public:
  Network(const IntCode& code) {
    for (int i = 0; i < 49; ++i) {
      computers_.push_back(Computer(this, code, i));
    }
  }

  absl::StatusOr<int> RunUntilAddress255ReturnY() {
    while (nat_packet_.address != 255) {
      for (Computer& c : computers_) {
        if (absl::Status st = c.StepExecution(); !st.ok()) return st;
      }
    }
    return nat_packet_.y;
  }

  absl::StatusOr<int> RunUntilDuplicateNat() {
    absl::optional<int> last_y;
    while (true) {
      bool all_idle = true;
      for (Computer& c : computers_) {
        if (absl::Status st = c.StepExecution(); !st.ok()) return st;
        all_idle &= c.idle();
      }
      if (all_idle) {
        if (last_y && nat_packet_.y == *last_y) {
          return *last_y;
        }
        last_y = nat_packet_.y;
        Packet host_packet = nat_packet_;
        host_packet.address = 0;
        if (absl::Status st = computers_[0].RecvPacket(host_packet); !st.ok()) {
          return st;
        }
      }
    }
  }

  absl::Status SendPacket(Packet packet) {
    VLOG(1) << "Network: SendPacket: " << packet;
    if (packet.address == 255) {
      nat_packet_ = packet;
    } else {
      if (packet.address < 0 || packet.address >= computers_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Bad address: ", packet.address));
      }
      if (absl::Status st = computers_[packet.address].RecvPacket(packet);
          !st.ok()) {
        return st;
      }
    }
    return absl::OkStatus();
  }

 private:
  std::vector<Computer> computers_;
  Packet nat_packet_{.address = 0, .x = 123, .y = 456};
};

absl::Status Computer::SendCurrentPacket() {
  return network_->SendPacket(out_packet_);
}

}  // namespace

absl::StatusOr<std::string> Day_2019_23::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> code = IntCode::Parse(input);
  if (!code.ok()) return code.status();

  Network network(*code);

  return IntReturn(network.RunUntilAddress255ReturnY());
}

absl::StatusOr<std::string> Day_2019_23::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> code = IntCode::Parse(input);
  if (!code.ok()) return code.status();

  Network network(*code);

  return IntReturn(network.RunUntilDuplicateNat());
}

}  // namespace advent_of_code
