#include <libserialport.hpp>

#include <cstring>
#include <iostream>

// Example of how to send and receive data.
// It is based on the example `send_receive.c` for libserialport.
//
// This example file is released to the public domain.

int main(const int argc, char *argv[]) try {
  // This example will send data and try to receive it on the same port.
  // This can be done by connecting a single wire between the Tx and Rx pins
  //  of the port.

  // get the port name from the command line
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " <port 1>\n";
    return -1;
  }
  const char *port_name = argv[1];

  const auto port = sp::get_port_by_name(port_name);

  if (!port) {
    std::cout << "could not find port with name `" << port_name << "`\n";
    return -1;
  }

  auto conn = sp::connection{port,
                             sp::mode_t::ReadWrite,
                             {.baud_rate = 9600,
                              .bits = 8,
                              .stop_bits = 1,
                              .parity = sp::parity_t::None}};

  const auto *const data = "Hello!";
  const auto size = std::strlen(data);

  const auto tx_ret = conn.write_blocking(data, size, 1000);

  if (tx_ret < 0) {
    std::cout << "error: " << sp::last_error_message() << "\n";
    return 1;
  }
  if (tx_ret == size) {
    std::cout << "sent " << tx_ret << " bytes successfully\n";
  } else {
    std::cout << "timed out, " << tx_ret << "/" << size << " bytes sent\n";
  }

  auto rx_buf = std::string{};
  rx_buf.resize(static_cast<unsigned>(tx_ret + 1));

  std::cout << "receiving " << size << " bytes on " << sp::get_name(port)
            << "\n";

  const auto rx_ret = conn.read_blocking(rx_buf.data(), size, 1000);

  if (rx_ret < 0) {
    std::cout << "error: " << sp::last_error_message() << "\n";
    return 1;
  }
  if (rx_ret == size) {
    std::cout << "received " << rx_ret << " bytes successfully\n";
  } else {
    std::cout << "timed out, " << rx_ret << "/" << size << " bytes received\n";
  }

  std::cout << "received: `" << rx_buf << "`\n";

  return 0;
} catch (const sp::connection_exc &exc) {
  std::cout << "connection aborted: " << exc.what() << "\n";
  return -1;
}
