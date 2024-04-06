// Darius Kellermann <kellermann@pm.me>, March 2024

// Implements a mock of the libserialport C API.

// needs to be included as the very first header,
// because it sets some POSIX macros
extern "C" {
#include <libserialport_internal.h>
}

#include "libserialport_mock.hpp"

#include <vector>

namespace {
  auto allocated_lists_ = std::vector<sp_port **>{};
  auto allocated_ports_ = std::vector<sp_port *>{};
  auto allocated_configs_ = std::vector<sp_port_config *>{};
  auto allocated_messages_ = std::vector<char *>{};
  auto next_status_ = sp_return{SP_OK};
} // namespace

long sp_mock::number_of_allocated_lists() {
  return ssize(allocated_lists_);
}

long sp_mock::number_of_allocated_ports() {
  return ssize(allocated_ports_);
}

long sp_mock::number_of_allocated_configs() {
  return ssize(allocated_configs_);
}

long sp_mock::number_of_allocated_messages() {
  return ssize(allocated_messages_);
}

void sp_mock::set_next_status(const sp::status_t status) {
  next_status_ = static_cast<sp_return>(status);
}

sp_return sp_list_ports(sp_port ***list_ptr) {
  *list_ptr = new sp_port *[3];
  sp_get_port_by_name("", *list_ptr);
  sp_get_port_by_name("", *list_ptr + 1);
  (*list_ptr)[2] = nullptr;
  allocated_lists_.push_back(*list_ptr);
  return next_status_;
}

void sp_free_port_list(sp_port **ports) {
  std::erase(allocated_lists_, ports);
  delete[] ports;
}

sp_return sp_get_port_by_name(const char *const portname, sp_port **port_ptr) {
  (void)portname;
  *port_ptr = new sp_port;
  allocated_ports_.push_back(*port_ptr);
  return next_status_;
}

void sp_free_port(sp_port *port) {
  std::erase(allocated_ports_, port);
  delete port;
}

sp_return sp_new_config(sp_port_config **config_ptr) {
  *config_ptr = new sp_port_config;
  allocated_configs_.push_back(*config_ptr);
  return next_status_;
}

void sp_free_config(sp_port_config *const cfg) {
  std::erase(allocated_configs_, cfg);
  delete cfg;
}

char *sp_last_error_message() {
  char *msg = new char;
  allocated_messages_.push_back(msg);
  return msg;
}

void sp_free_error_message(char *message) {
  std::erase(allocated_messages_, message);
  delete message;
}

sp_return sp_open(sp_port *port, sp_mode flags) {
  (void)port;
  (void)flags;
  return next_status_;
}

sp_return sp_close(sp_port *port) {
  (void)port;
  return next_status_;
}

char *sp_get_port_name(const sp_port *p) {
  (void)p;
  return NULL;
}

char *sp_get_port_description(const sp_port *p) {
  (void)p;
  return NULL;
}

sp_transport sp_get_port_transport(const sp_port *p) {
  (void)p;
  return SP_TRANSPORT_NATIVE;
}

sp_return sp_get_port_usb_bus_address(const sp_port *port, int *usb_bus,
                                      int *usb_address) {
  (void)port;
  (void)usb_bus;
  (void)usb_address;
  return next_status_;
}

sp_return sp_get_port_usb_vid_pid(const sp_port *port, int *usb_vid,
                                  int *usb_pid) {
  (void)port;
  (void)usb_vid;
  (void)usb_pid;
  return next_status_;
}

char *sp_get_port_usb_manufacturer(const sp_port *port) {
  (void)port;
  return NULL;
}

char *sp_get_port_usb_product(const sp_port *port) {
  (void)port;
  return NULL;
}

char *sp_get_port_usb_serial(const sp_port *port) {
  (void)port;
  return NULL;
}

char *sp_get_port_bluetooth_address(const sp_port *port) {
  (void)port;
  return NULL;
}

sp_return sp_get_port_handle(const sp_port *port, void *result_ptr) {
  (void)port;
  (void)result_ptr;
  return next_status_;
}

sp_return sp_get_config(sp_port *port, sp_port_config *config) {
  (void)port;
  (void)config;
  return next_status_;
}

sp_return sp_set_config(sp_port *port, const sp_port_config *config) {
  (void)port;
  (void)config;
  return next_status_;
}

sp_return sp_set_baudrate(sp_port *port, int baudrate) {
  (void)port;
  (void)baudrate;
  return next_status_;
}

sp_return sp_get_config_baudrate(const sp_port_config *config,
                                 int *baudrate_ptr) {
  (void)config;
  (void)baudrate_ptr;
  return next_status_;
}

sp_return sp_set_config_baudrate(sp_port_config *config, int baudrate) {
  (void)config;
  (void)baudrate;
  return next_status_;
}

sp_return sp_get_config_bits(const sp_port_config *config, int *bits_ptr) {
  (void)config;
  (void)bits_ptr;
  return next_status_;
}

sp_return sp_set_config_bits(sp_port_config *config, int bits) {
  (void)config;
  (void)bits;
  return next_status_;
}

sp_return sp_get_config_parity(const sp_port_config *config,
                               sp_parity *parity_ptr) {
  (void)config;
  (void)parity_ptr;
  return next_status_;
}

sp_return sp_set_config_parity(sp_port_config *config, sp_parity parity) {
  (void)config;
  (void)parity;
  return next_status_;
}

sp_return sp_get_config_stopbits(const sp_port_config *config,
                                 int *stopbits_ptr) {
  (void)config;
  (void)stopbits_ptr;
  return next_status_;
}

sp_return sp_set_config_stopbits(sp_port_config *config, int stopbits) {
  (void)config;
  (void)stopbits;
  return next_status_;
}

sp_return sp_blocking_read(sp_port *port, void *buf, size_t count,
                           unsigned int timeout_ms) {
  (void)port;
  (void)buf;
  (void)count;
  (void)timeout_ms;
  return next_status_;
}

sp_return sp_blocking_read_next(sp_port *port, void *buf, size_t count,
                                unsigned int timeout_ms) {
  (void)port;
  (void)buf;
  (void)count;
  (void)timeout_ms;
  return next_status_;
}

sp_return sp_nonblocking_read(sp_port *port, void *buf, size_t count) {
  (void)port;
  (void)buf;
  (void)count;
  return next_status_;
}

sp_return sp_blocking_write(sp_port *port, const void *buf, size_t count,
                            unsigned int timeout_ms) {
  (void)port;
  (void)buf;
  (void)count;
  (void)timeout_ms;
  return next_status_;
}

sp_return sp_nonblocking_write(sp_port *port, const void *buf, size_t count) {
  (void)port;
  (void)buf;
  (void)count;
  return next_status_;
}

sp_return sp_input_waiting(sp_port *port) {
  (void)port;
  return next_status_;
}

sp_return sp_output_waiting(sp_port *port) {
  (void)port;
  return next_status_;
}

sp_return sp_flush(sp_port *port, sp_buffer buffers) {
  (void)port;
  (void)buffers;
  return next_status_;
}

sp_return sp_drain(sp_port *port) {
  (void)port;
  return next_status_;
}
