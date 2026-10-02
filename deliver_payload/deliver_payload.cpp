#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>
#include <memory>
#include <string>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string &bin_name) {
  std::cerr << "No coords given to " << bin_name << "\n";
}

int main(int argc, char **argv) {
  if (argc != 3) {
    usage(argv[0]);
    return 1;
  }

  // Convert coord args to float
  double latitude = std::stod(argv[1]);
  double longitude = std::stod(argv[2]);
  float takeoff_altitude = 100;

  std::cout << "Coords: " << argv[1] << " , " << argv[2] << std::endl;

  Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
  ConnectionResult connection_result = mavsdk.add_any_connection(
      /*"udpin://127.0.0.1:14552"*/ "udpin://0.0.0.0:14550");

  if (connection_result != ConnectionResult::Success) {
    std::cerr << "Connection failed: " << connection_result << '\n';
    return 1;
  }

  auto system = mavsdk.first_autopilot(10.0);
  if (!system) {
    std::cerr << "Timed out waiting for system\n";
    return 1;
  }

  // Instantiate plugins
  auto telemetry = Telemetry{system.value()};
  auto action = Action{system.value()};

  // Listen to altitude of drone at 1 Hz.
  const auto set_rate_result = telemetry.set_rate_position(1.0);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cerr << "Setting rate failed: " << set_rate_result << '\n';
    return 1;
  }

  // Set callback to monitor altitude while in flight
  telemetry.subscribe_position([](Telemetry::Position position) {
    std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
  });

  // Check until vehicle ready to arm
  while (telemetry.health_all_ok() != true) {
    std::cout << "Far Star healt: Bad\n";
    sleep_for(seconds(1));
  }

  // Arm vehicle
  std::cout << "Arming...\n";
  const Action::Result arm_result = action.arm();

  if (arm_result != Action::Result::Success) {
    std::cerr << "Arming failed: " << arm_result << '\n';
    return 1;
  }

  std::cout << "Setting takeoff alt to: " << takeoff_altitude;
  const Action::Result set_takeoff_altitude_result =
      action.set_takeoff_altitude(takeoff_altitude);
  if (set_takeoff_altitude_result != Action::Result::Success) {
    std::cerr << "set_takeoff_altitude(float) failed: "
              << set_takeoff_altitude_result << std::endl;
    return 1;
  }

  // Take off
  std::cout << "Taking off...\n";
  const Action::Result takeoff_result = action.takeoff();
  if (takeoff_result != Action::Result::Success) {
    std::cerr << "Takeoff failed: " << takeoff_result << '\n';
    return 1;
  }

  std::cout << "Starting winch turn";
  const Action::Result set_actuator_result = action.set_actuator(7, 20);
  if (set_actuator_result != Action::Result::Success) {
    std::cerr << "winch turn: " << set_actuator_result << '\n';
    return 1;
  }

  while (telemetry.position().relative_altitude_m < takeoff_altitude) {
    std::cout << "altitude: " << telemetry.position().relative_altitude_m
              << std::endl;
    sleep_for(seconds(1));
  }

  // Let hover
  sleep_for(seconds(10));

  std::cout << "Going to " << argv[1] << " , " << argv[2] << std::endl;
  const Action::Result go_to_result =
      action.goto_location(latitude, longitude, takeoff_altitude, 25);
  if (go_to_result != Action::Result::Success) {
    std::cerr << "Go to failed: " << go_to_result << std::endl;
    return 1;
  }
  // Let hover
  // sleep_for(seconds(100));

  std::cout << "Landing...\n";
  const Action::Result land_result = action.land();
  if (land_result != Action::Result::Success) {
    std::cerr << "Land failed: " << land_result << '\n';
    return 1;
  }

  // Check if still in air
  while (telemetry.in_air()) {
    std::cout << "Vehicle is landing...\n";
    sleep_for(seconds(1));
  }

  std::cout << "Landed!\n";

  // Relying on auto-disarming but keep watching telemetry
  sleep_for(seconds(30));
  std::cout << "Finished...\n";

  return 0;
}
