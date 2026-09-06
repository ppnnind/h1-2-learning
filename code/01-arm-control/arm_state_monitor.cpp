#include <array>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <unitree/idl/hg/LowState_.hpp>
#include <unitree/robot/channel/channel_factory.hpp>
#include <unitree/robot/channel/channel_subscriber.hpp>

namespace {

using LowState = unitree_hg::msg::dds_::LowState_;
using Clock = std::chrono::steady_clock;

constexpr char kTopicState[] = "rt/lowstate";
constexpr auto kReportPeriod = std::chrono::seconds(1);
constexpr auto kStaleThreshold = std::chrono::milliseconds(500);

struct JointInfo {
  std::size_t sdk_index;
  const char* name;
};

constexpr std::array<JointInfo, 15> kArmJoints{{
    {13, "LeftShoulderPitch"},
    {14, "LeftShoulderRoll"},
    {15, "LeftShoulderYaw"},
    {16, "LeftElbow"},
    {17, "LeftWristRoll"},
    {18, "LeftWristPitch"},
    {19, "LeftWristYaw"},
    {20, "RightShoulderPitch"},
    {21, "RightShoulderRoll"},
    {22, "RightShoulderYaw"},
    {23, "RightElbow"},
    {24, "RightWristRoll"},
    {25, "RightWristPitch"},
    {26, "RightWristYaw"},
    {12, "WaistYaw"},
}};

struct StateCache {
  std::mutex mutex;
  LowState latest_state;
  Clock::time_point last_receive_time{};
  std::uint64_t message_count{0};
  bool received{false};
};

volatile std::sig_atomic_t g_stop_requested = 0;

void HandleSignal(int) {
  g_stop_requested = 1;
}

void PrintState(const LowState& state, double receive_rate_hz,
                std::chrono::milliseconds state_age, bool fresh) {
  std::cout << "\nState: " << (fresh ? "FRESH" : "STALE")
            << " | receive rate: " << std::fixed << std::setprecision(1)
            << receive_rate_hz << " Hz"
            << " | age: " << state_age.count() << " ms\n";
  std::cout << std::left << std::setw(3) << "ID" << std::setw(22) << "Joint"
            << std::right << std::setw(11) << "q(rad)" << std::setw(12)
            << "dq(rad/s)" << std::setw(13) << "tau_est" << '\n';

  for (const auto& joint : kArmJoints) {
    const auto& motor = state.motor_state().at(joint.sdk_index);
    std::cout << std::left << std::setw(3) << joint.sdk_index << std::setw(22)
              << joint.name << std::right << std::fixed << std::setprecision(3)
              << std::setw(11) << motor.q() << std::setw(12) << motor.dq()
              << std::setw(13) << motor.tau_est() << '\n';
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " networkInterface\n";
    return 1;
  }

  std::signal(SIGINT, HandleSignal);
  std::signal(SIGTERM, HandleSignal);

  try {
    unitree::robot::ChannelFactory::Instance()->Init(0, argv[1]);

    StateCache cache;
    unitree::robot::ChannelSubscriber<LowState> subscriber(kTopicState);
    subscriber.InitChannel(
        [&cache](const void* raw_message) {
          if (raw_message == nullptr) {
            return;
          }

          const auto* received_state = static_cast<const LowState*>(raw_message);
          std::lock_guard<std::mutex> lock(cache.mutex);
          cache.latest_state = *received_state;
          cache.last_receive_time = Clock::now();
          ++cache.message_count;
          cache.received = true;
        },
        1);

    std::cout << "Monitoring " << kTopicState << " on interface " << argv[1]
              << ". This program does not publish commands.\n"
              << "Press Ctrl+C to stop.\n";

    std::uint64_t previous_message_count = 0;
    auto previous_report_time = Clock::now();

    while (!g_stop_requested) {
      std::this_thread::sleep_for(kReportPeriod);
      const auto now = Clock::now();

      LowState snapshot;
      Clock::time_point last_receive_time;
      std::uint64_t message_count = 0;
      bool received = false;

      {
        std::lock_guard<std::mutex> lock(cache.mutex);
        snapshot = cache.latest_state;
        last_receive_time = cache.last_receive_time;
        message_count = cache.message_count;
        received = cache.received;
      }

      const double report_seconds =
          std::chrono::duration<double>(now - previous_report_time).count();
      const double receive_rate_hz =
          static_cast<double>(message_count - previous_message_count) /
          report_seconds;

      if (!received) {
        std::cout << "Waiting for the first " << kTopicState
                  << " message... (0 messages received)\n";
      } else {
        const auto state_age =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_receive_time);
        PrintState(snapshot, receive_rate_hz, state_age,
                   state_age <= kStaleThreshold);
      }

      previous_message_count = message_count;
      previous_report_time = now;
    }

    subscriber.CloseChannel();
    std::cout << "State monitor stopped. No command was published.\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "Failed to start state monitor: " << error.what() << '\n';
    return 2;
  }
}
