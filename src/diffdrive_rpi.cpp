#include "diffdrive_rpi/diffdrive_rpi.h"


#include "hardware_interface/types/hardware_interface_type_values.hpp"


#include <pigpio.h>
#include <unistd.h>
#include <string>
#include <sstream>

#include "diffdrive_rpi/robot_move.hpp"


DiffDriveRpi::DiffDriveRpi()
    : logger_(rclcpp::get_logger("DiffDriveRpi"))
{}





return_type DiffDriveRpi::configure(const hardware_interface::HardwareInfo & info)
{
  if (configure_default(info) != return_type::OK) {
    return return_type::ERROR;
  }

  RCLCPP_INFO(logger_, "Configuring...");

  time_ = std::chrono::system_clock::now();

  cfg_.left_wheel_name = info_.hardware_parameters["left_wheel_name"];
  cfg_.right_wheel_name = info_.hardware_parameters["right_wheel_name"];
  cfg_.loop_rate = std::stof(info_.hardware_parameters["loop_rate"]);
  cfg_.device = info_.hardware_parameters["device"];
  cfg_.baud_rate = std::stoi(info_.hardware_parameters["baud_rate"]);
  cfg_.timeout = std::stoi(info_.hardware_parameters["timeout"]);
  cfg_.enc_counts_per_rev = std::stoi(info_.hardware_parameters["enc_counts_per_rev"]);
  cfg_.wheel_radius = std::stof(info_.hardware_parameters["wheel_radius"]);

  // Set up the wheels
  l_wheel_.setup(cfg_.left_wheel_name, cfg_.enc_counts_per_rev);
  r_wheel_.setup(cfg_.right_wheel_name, cfg_.enc_counts_per_rev);

  // Set up the Arduino
//   arduino_.setup(cfg_.device, cfg_.baud_rate, cfg_.timeout);  

    if (gpioInitialise() < 0){
        RCLCPP_INFO(logger_, "Fail pigpio");
        gpioTerminate();
        return return_type::ERROR;
    } 

    robot_.config(0,1);

  RCLCPP_INFO(logger_, "Finished Configuration");

  status_ = hardware_interface::status::CONFIGURED;
  return return_type::OK;
}

std::vector<hardware_interface::StateInterface> DiffDriveRpi::export_state_interfaces()
{
  // We need to set up a position and a velocity interface for each wheel

  std::vector<hardware_interface::StateInterface> state_interfaces;

  state_interfaces.emplace_back(hardware_interface::StateInterface(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.vel));
  state_interfaces.emplace_back(hardware_interface::StateInterface(l_wheel_.name, hardware_interface::HW_IF_POSITION, &l_wheel_.pos));
  state_interfaces.emplace_back(hardware_interface::StateInterface(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.vel));
  state_interfaces.emplace_back(hardware_interface::StateInterface(r_wheel_.name, hardware_interface::HW_IF_POSITION, &r_wheel_.pos));

  return state_interfaces;
}

std::vector<hardware_interface::CommandInterface> DiffDriveRpi::export_command_interfaces()
{
  // We need to set up a velocity command interface for each wheel

  std::vector<hardware_interface::CommandInterface> command_interfaces;

  command_interfaces.emplace_back(hardware_interface::CommandInterface(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.cmd));
  command_interfaces.emplace_back(hardware_interface::CommandInterface(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.cmd));

  return command_interfaces;
}


return_type DiffDriveRpi::start()
{
  RCLCPP_INFO(logger_, "Starting Controller...");

//   arduino_.sendEmptyMsg();
  // arduino.setPidValues(9,7,0,100);
  // arduino.setPidValues(14,7,0,100);
//   arduino_.setPidValues(30, 20, 0, 100);
    
  status_ = hardware_interface::status::STARTED;

  return return_type::OK;
}

return_type DiffDriveRpi::stop()
{
  RCLCPP_INFO(logger_, "Stopping Controller...");
  gpioTerminate();
  status_ = hardware_interface::status::STOPPED;

  return return_type::OK;
}

hardware_interface::return_type DiffDriveRpi::read()
{

  // TODO fix chrono duration

  // Calculate time delta
  auto new_time = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = new_time - time_;
  double deltaSeconds = diff.count();
  time_ = new_time;


//   if (!arduino_.connected())
//   {
//     return return_type::ERROR;
//   }

//   arduino_.readEncoderValues(l_wheel_.enc, r_wheel_.enc);
    robot_.readEncoders(&l_wheel_.enc, &r_wheel_.enc);

    //left encoder is counting in other direction
  double pos_prev = l_wheel_.pos;
  l_wheel_.pos = l_wheel_.calcEncAngle();
  l_wheel_.vel = (l_wheel_.pos - pos_prev) / deltaSeconds;

  pos_prev = r_wheel_.pos;
  r_wheel_.pos = r_wheel_.calcEncAngle();
  r_wheel_.vel = (r_wheel_.pos - pos_prev) / deltaSeconds;



  return return_type::OK;

  
}

hardware_interface::return_type DiffDriveRpi::write()
{

//   if (!arduino_.connected())
//   {
//     return return_type::ERROR;
//   }

//   arduino_.setMotorValues(l_wheel_.cmd / l_wheel_.rads_per_count / cfg_.loop_rate, r_wheel_.cmd / r_wheel_.rads_per_count / cfg_.loop_rate);
    double left = l_wheel_.cmd * cfg_.wheel_radius;
    double right = r_wheel_.cmd * cfg_.wheel_radius;
    std::ostringstream log;
    log << "l_wheel: " << left << " r_wheel: " << right << std::endl;
    std::string s_log = log.str();
    // RCLCPP_INFO(logger_, s_log);
    
    std::ostringstream log2;
    log2 << "l_wheel_cmd: " << l_wheel_.cmd << " r_wheel_cmd: " << r_wheel_.cmd << std::endl;
    std::string s_log2 = log2.str();
    // RCLCPP_INFO(logger_, s_log2);
    
    robot_.move(left, right );



  return return_type::OK;


  
}



#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(
  DiffDriveRpi,
  hardware_interface::SystemInterface
)