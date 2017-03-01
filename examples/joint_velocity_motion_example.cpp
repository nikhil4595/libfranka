#include <cmath>
#include <iostream>

#include <franka/robot.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./generate_motion <robot-hostname>" << std::endl;
    return -1;
  }
  try {
    franka::Robot robot(argv[1]);
    std::cout << "Starting joint velocity motion generator" << std::endl;
    franka::JointVelocityMotionGenerator motion_generator =
        robot.startJointVelocityMotionGenerator();
    std::cout << "succeeded to start motion generator! " << std::endl;
    double time(0.0);
    double T(4.0);
    double omega_max(0.2);
    // double angle(M_PI / 4);

    while (robot.update()) {
      int cycle = int(pow(-1.0, (time - fmod(time, T)) / T));
      double omega = double(cycle) * omega_max / 2.0 *
                     (1.0 - std::cos(2.0 * M_PI / T * time));
      try {
        motion_generator.setDesiredVelocity(
            {{0.0, 0.0, 0.0, omega, omega, omega, omega}});
      } catch (franka::MotionGeneratorException const& e) {
        std::cout << e.what() << std::endl;
      }
      time += 0.001;
      if (time > 2 * T) {
        std::cout << std::endl
                  << "finished motion, shutting down example" << std::endl;
        break;
      }
    }
  } catch (franka::NetworkException const& e) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;
}
