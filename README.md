# libspp -- A C++ wrapper for libserialport

The motivation for this is to further simplify the use of serial ports from
C++ code. The main features are RAII and memory management by `std::shared_ptr`.

Below example outlines the general usage. For more elaborate examples, refer to
the folder `examples/`.

```cpp
#include <libserialport.hpp>

int main() {
  // list serial ports
  const std::vector<sp::port_t> ports = sp::list_ports();

  // connect to each serial port
  for (const auto &port : ports) {
    // write and receive data
    auto conn = sp::connection{port,
                               sp::mode_t::ReadWrite,
                               {.baud_rate = 9600,
                                .bits = 8,
                                .stop_bits = 1,
                                .parity = sp::parity_t::None}};

    const auto *const data = "Hello, World!";
    const auto size = std::strlen(data);

    const int tx_ret = conn.write_blocking(data, size, 1'000);

    if (tx_ret < 0) {
      std::cout << "error: " << sp::last_error_message() << "\n";
      return 1;
    }
    if (tx_ret == size) {
      std::cout << "sent " << tx_ret << " bytes successfully\n";
    } else {
      std::cout << "timed out, " << tx_ret << "/" << size << " bytes sent\n";
      return 1;
    }

    auto rx_buf = std::string{};
    rx_buf.resize(size + 1);

    std::cout << "receiving " << size << " bytes on " << sp::get_name(port)
              << "\n";

    const int rx_ret = conn.read_blocking(rx_buf.data(), size, 1'000);

    if (rx_ret < 0) {
      std::cout << "error: " << sp::last_error_message() << "\n";
      return 1;
    }
    if (rx_ret == size) {
      std::cout << "received " << rx_ret << " bytes successfully\n";
    } else {
      std::cout << "timed out, " << rx_ret << "/" << size
                << " bytes received\n";
    }
  }
  return 0;
}
```

## Build & Versioning

Everything is built with CMake, both the original libserialport and the C++ part.
Upstream libserialport uses autotools, which I have a distaste for.
While this simplifies the build system (in terms of readability at the very least),
not all platforms that libserialport supports are supported.

To build a shared library, pass `BUILD_SHARED_LIBS=ON` to CMake.

I aim to achieve a good test coverage for at least two major Linux distributions
and Windows.

The libserialport upstream code is fetched during the build in a fixed version.
This project applies the same versioning as upstream,
which also means that versions need to be bumped manually.
To do so, changing the `GIT_TAG` in `cmake/libserialport.cmake` is sufficient.

## To do

- throw exceptions in `sp::connection` when appropriate
- write a lot of tests
- generate export header & add macros appropriately
