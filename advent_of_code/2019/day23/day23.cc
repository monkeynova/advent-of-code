#include "advent_of_code/2019/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"
#include "re2/re2.h"

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
  enum class PacketState {
   kSetAddress = 1, kSetX = 2, kSetY = 3
  };

  Computer(Network* network, const IntCode& code, int64_t address) 
   : network_(network), code_(code.Clone()), address_(address) {}

  absl::Status StepExecution() {
    return code_.RunSingleOpcode(this);
  }

  bool PauseIntCode() override { return false; }

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
        if (in_packet_queue_.empty()) return absl::InternalError("Requested y with no x");
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
      default: return absl::InternalError("Bad state");
    }
    return absl::OkStatus();
  }

  absl::Status RecvPacket(Packet packet) {
    if (packet.address != address_) {
      return absl::InvalidArgumentError(absl::StrCat(
        "Wrong address: ", packet.address, " != ", address_));
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
        if (absl::Status st = computers_[0].RecvPacket(host_packet); !st.ok()) return st;
      }
    }
  }

  absl::Status SendPacket(Packet packet) {
    VLOG(1) << "Network: SendPacket: " << packet;
    if (packet.address == 255) {
      nat_packet_ = packet;
    } else {
      if (packet.address < 0 || packet.address >= computers_.size()) {
        return absl::InvalidArgumentError(absl::StrCat("Bad address: ", packet.address));
      }
      if (absl::Status st = computers_[packet.address].RecvPacket(packet); !st.ok()) return st;
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

absl::StatusOr<std::vector<std::string>> Day23_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> code = IntCode::Parse(input);
  if (!code.ok()) return code.status();

  Network network(*code);

  return IntReturn(network.RunUntilAddress255ReturnY());
}

absl::StatusOr<std::vector<std::string>> Day23_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> code = IntCode::Parse(input);
  if (!code.ok()) return code.status();

  Network network(*code);

  return IntReturn(network.RunUntilDuplicateNat());
}
