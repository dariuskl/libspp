#include <libserialport.hpp>

#include <iostream>

// Example of how to get a list of serial ports on the system.
// It is based on the example `list_ports.c` for libserialport.
//
// This example file is released to the public domain.

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  std::cout << "getting port list\n";

  // call sp::list_ports() to get a vector of ports.
  const auto ports = sp::list_ports();

  if (ports.empty() && sp::get_status() != sp::status_t::OK) {
    std::cout << "sp::list_ports() failed!\n";
    return -1;
  }

  // iterate through the ports
  for (const auto &port : ports) {
    // get the name of the port
    std::cout << "found port: " << sp::get_name(port) << "\n"
              << " description: " << sp::get_description(port) << "\n"
              << " transport: " <<
        [&port] {
          switch (sp::get_transport(port)) {
          case sp::transport_t::Native:
            return "native";
          case sp::transport_t::USB:
            return "USB";
          case sp::transport_t::Bluetooth:
            return "Bluetooth";
          }
          return "unknown";
        }() << "\n"
              << " USB bus address: Bus " << sp::get_usb_bus_address(port).bus
              << " Device " << sp::get_usb_bus_address(port).address << "\n"
              << " USB VID & PID: " << std::hex << sp::get_usb_vid_pid(port).vid
              << ":" << sp::get_usb_vid_pid(port).pid << "\n"
              << " USB manufacturer: " << sp::get_usb_manufacturer(port) << "\n"
              << " USB product: " << sp::get_usb_product(port) << "\n"
              << " USB serial number: " << sp::get_usb_serial_no(port) << "\n"
              << " Bluetooth address: " << sp::get_bluetooth_address(port)
              << "\n";
  }

  std::cout << "found " << ports.size() << " ports\n";

  return 0;
}
