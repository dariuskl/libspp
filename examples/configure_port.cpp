#include <libserialport.hpp>

#include <iostream>

// Example of how to configure a serial port.
// It is based on the example `port_config.c` for libserialport.
//
// This example file is released to the public domain.

int main(const int argc, char *argv[]) try {
  // get the port name from the command-line
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " <port name>\n";
    return -1;
  }
  const char *port_name = argv[1];

  std::cout << "looking for port " << port_name << "\n";

  // call sp::get_port_by_name() to find the port
  const auto port = sp::get_port_by_name(port_name);
  if (!port) {
    std::cout << "error: " << sp::last_error_message() << "\n";
    return -1;
  }

  // display some basic information about the port
  std::cout << "port name: " << sp::get_name(port) << "\n";
  std::cout << "port description: " << sp::get_description(port) << "\n";

  // the port must be open to access its configuration
  std::cout << "opening port and configuring to 115200 8N1\n";

  // TODO describe connection object & exception
  // Unlike libserialport, there is one way to configure the port.
  // TODO

  auto conn = sp::connection{port,
                             sp::mode_t::ReadWrite,
                             {.baud_rate = 115200,
                              .bits = 8,
                              .stop_bits = 1,
                              .parity = sp::parity_t::None}};

  const auto initial_config = conn.get_config();

  std::cout << "Baudrate: " << initial_config.baud_rate
            << ", data bits: " << initial_config.bits << ", parity: " <<
      [&initial_config] {
        switch (initial_config.parity) {
        case sp::parity_t::Invalid:
          return "invalid";
        case sp::parity_t::None:
          return "none";
        case sp::parity_t::Odd:
          return "odd";
        case sp::parity_t::Even:
          return "even";
        case sp::parity_t::Mark:
          return "mark";
        case sp::parity_t::Space:
          return "space";
        }
        return "unknown";
      }()
            << ", stop bits: " << initial_config.stop_bits << "\n";

  // TODO change config forth and back

  return 0;
} catch (const sp::connection_exc &exc) {
  std::cout << "connection aborted: " << exc.what() << "\n";
  return -1;
}
