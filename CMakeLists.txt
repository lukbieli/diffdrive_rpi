cmake_minimum_required(VERSION 3.5)
project(diffdrive_rpi)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Set the path to the directory containing FindPigpio.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra)
endif()

find_package(ament_cmake REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(controller_manager REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pluginlib REQUIRED)
find_package(pigpio REQUIRED)






add_library(diffdrive_rpi SHARED src/diffdrive_rpi.cpp src/wheel.cpp src/motor_ctrl.cpp src/robot_move.cpp src/rotary_encoder.cpp src/pid.cpp)

target_include_directories(
  diffdrive_rpi
  PRIVATE
  include
  
)
ament_target_dependencies(
  diffdrive_rpi
  hardware_interface
  controller_manager
  rclcpp
  pluginlib
  pigpio
)

target_link_libraries(diffdrive_rpi pigpio)

pluginlib_export_plugin_description_file(hardware_interface robot_hardware.xml)



add_library(fake_robot SHARED src/fake_robot.cpp src/wheel.cpp)
set_property(TARGET fake_robot PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(
  fake_robot
  PRIVATE
  include
)
ament_target_dependencies(
  fake_robot
  hardware_interface
  controller_manager
  rclcpp
  pluginlib
)

pluginlib_export_plugin_description_file(hardware_interface fake_robot_hardware.xml)




install(
  TARGETS diffdrive_rpi fake_robot
  DESTINATION lib
)




install(
  DIRECTORY controllers launch/
  DESTINATION share/${PROJECT_NAME}
)


ament_export_libraries(
  diffdrive_rpi
  fake_robot
)
ament_export_dependencies(
  hardware_interface
  controller_manager
  rclcpp
  pluginlib
  pigpio
)

ament_package()


