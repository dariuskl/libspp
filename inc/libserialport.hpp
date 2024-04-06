// Darius Kellermann <kellermann@pm.me>, March 2024

#ifndef LIBSERIALPORT_HPP_INCLUDED
#define LIBSERIALPORT_HPP_INCLUDED

#include <cstdint>
#include <exception>
#include <memory>
#include <vector>

extern "C" struct sp_port;

namespace sp {

  enum class status_t : int {
    OK = 0,
    InvalidArgument = -1,
    SystemError = -2,
    NotSupported = -4
  };

  enum class transport_t : std::int8_t { Native, USB, Bluetooth };

  enum class mode_t : std::uint8_t { Read = 1U, Write = 2U, ReadWrite = 3U };

  enum class parity_t : std::int8_t {
    Invalid = -1, // special value to indicate setting should be left alone
    None,
    Odd,
    Even,
    Mark,
    Space
  };

  enum class buffer_t : std::uint8_t { None = 0U, Rx = 1U, Tx = 2U, Both = 3U };

  // ports are managed as shared pointers to the opaque `sp_port` structure
  using port_t = std::shared_ptr<sp_port>;

  struct usb_bus_address_t {
    int bus;
    int address;
  };

  constexpr bool operator==(const usb_bus_address_t &lhs,
                            const usb_bus_address_t &rhs) {
    return lhs.bus == rhs.bus && lhs.address == rhs.address;
  }

  struct usb_vid_pid_t {
    int vid;
    int pid;
  };

  constexpr bool operator==(const usb_vid_pid_t &lhs,
                            const usb_vid_pid_t &rhs) {
    return lhs.vid == rhs.vid && lhs.pid == rhs.pid;
  }

  // gets the status of the most recent call
  status_t get_status() noexcept;

  // gets the error message for the most recent call
  // if the most recent call was successful, `no error` will be returned
  std::shared_ptr<const char> last_error_message();

  // get a port structure for the given port name
  // if no port with that name can be found an invalid port is returned
  port_t get_port_by_name(const char *port_name);

  // get a list of available ports
  // if no ports are available, or there is an error, an empty list is returned
  std::vector<port_t> list_ports();

  // get the name of the given port (e.g. `COM1` or `/dev/ttyUSB0`)
  // for an invalid port, the empty string is returned
  // if valid, the return value points into the port structure
  const char *get_name(const port_t &p) noexcept;

  // get a description of the given port for end users
  // for an invalid port, the empty string is returned
  // if valid, the return value points into the port structure
  const char *get_description(const port_t &p) noexcept;

  // get the type of transport of the given port
  // for an invalid port, `transport_t::Native` is returned
  transport_t get_transport(const port_t &p) noexcept;

  // get the USB bus and device address of the given port
  // for an invalid or non-USB port, `-1`s are returned
  usb_bus_address_t get_usb_bus_address(const port_t &p) noexcept;

  // get the USB VID and PID of the given port
  // for an invalid or non-USB port, `-1`s are returned
  usb_vid_pid_t get_usb_vid_pid(const port_t &p) noexcept;

  // get the USB manufacturer of the given port
  // for an invalid or non-USB port, the empty string is returned
  // if valid, the return value points into the port structure
  const char *get_usb_manufacturer(const port_t &p) noexcept;

  // get the USB product of the given port
  // for an invalid or non-USB port, the empty string is returned
  // if valid, the return value points into the port structure
  const char *get_usb_product(const port_t &p) noexcept;

  // get the USB serial number of the given port
  // for an invalid or non-USB port, the empty string is returned
  // if valid, the return value points into the port structure
  const char *get_usb_serial_no(const port_t &p) noexcept;

  // get the Bluetooth address of the given port
  // for an invalid or non-Bluetooth port, the empty string is returned
  // if valid, the return value points into the port structure
  const char *get_bluetooth_address(const port_t &p) noexcept;

  // get the OS-specific handle of the given port
  // the handle will be written into the memory pointed to by `result_ptr`
  status_t get_native_handle(const port_t &p, void *result_ptr);

  class connection_exc final : public std::exception {
   public:
    explicit connection_exc(std::shared_ptr<const char> cause)
        : cause_{std::move(cause)} {}

    const char *what() const noexcept override {
      return cause_.get();
    }

   private:
    std::shared_ptr<const char> cause_;
  };

  struct port_config_t {
    int baud_rate{-1};
    int bits{-1};
    int stop_bits{-1};
    parity_t parity{parity_t::Invalid};
  };

  class connection {
   public:
    connection(port_t p, mode_t m, const port_config_t &cfg = {});
    ~connection();

    connection(const connection &) = delete;
    connection &operator=(const connection &) = delete;

    connection(connection &&) = default;
    connection &operator=(connection &&) = default;

    port_config_t get_config() const;
    status_t set_config(const port_config_t &cfg);

    // blocks until `count` bytes are available, or the timeout has expired
    // returns the number of bytes read, or -1 on error
    int read_blocking(void *buf, int count, long timeout_ms);

    // blocks until any data is available or the timeout has expired
    // returns the number of bytes read, or -1 on error
    int read_next_blocking(void *buf, int count, long timeout_ms);

    // returns the number of bytes read, or -1 on error
    int read_nonblocking(void *buf, int count);

    // returns the number of bytes waiting in the input buffer
    int input_waiting();

    // blocks until `count` bytes have been written, or the timeout has expired
    // returns the number of bytes written, or -1 on error
    int write_blocking(const void *buf, int count, long timeout_ms);

    // returns the number of bytes written, or -1 on error
    int write_nonblocking(const void *buf, int count);

    // returns the number of bytes waiting in the output buffer
    int output_waiting();

    // discards any data in the rx & tx buffers
    status_t flush(buffer_t buffers_to_flush);

    // blocks until all data has been transmitted
    status_t drain();

   private:
    port_t p_;
  };

} // namespace sp

#endif // LIBSERIALPORT_HPP_INCLUDED
