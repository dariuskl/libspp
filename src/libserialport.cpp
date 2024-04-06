// Darius Kellermann <kellermann@proton.me>, March 2024

#include <libserialport.hpp>

#include <libserialport.h>

#include <memory>
#include <utility>
#include <vector>

namespace {
  sp::status_t status_;

  std::shared_ptr<sp_port> manage(sp_port *const raw_ptr) {
    return {raw_ptr, [](sp_port *const p) { sp_free_port(p); }};
  }

  std::shared_ptr<sp_port_config> manage(sp_port_config *const raw_ptr) {
    return {raw_ptr, [](sp_port_config *const p) { sp_free_config(p); }};
  }

  std::shared_ptr<const char> manage(char *const raw_ptr) {
    return {raw_ptr, [](char *const p) { sp_free_error_message(p); }};
  }

  std::shared_ptr<const char> manage_noop(const char *const raw_ptr) {
    return {raw_ptr, []([[maybe_unused]] const char *const p) {}};
  }

  const char *empty_if_null(const char *const str) {
    if (str != nullptr) {
      return str;
    }
    return "";
  }
} // namespace

sp::status_t sp::get_status() noexcept { return status_; }

std::shared_ptr<const char> sp::last_error_message() {
  switch (status_) {
  case status_t::OK:
    return manage_noop("no error");
  case status_t::InvalidArgument:
    return manage_noop("invalid argument");
  case status_t::SystemError:
    return manage(sp_last_error_message());
  case status_t::NotSupported:
    return manage_noop("not supported");
  }
  return manage_noop("unknown error code");
}

sp::port_t sp::get_port_by_name(const char *const port_name) {
  auto result = port_t{};
  sp_port *p = nullptr;

  if (status_ = status_t{sp_get_port_by_name(port_name, &p)};
      status_ == status_t::OK) {
    result = manage(p);
  }

  return result;
}

std::vector<sp::port_t> sp::list_ports() {
  auto result = std::vector<port_t>{};
  sp_port **ports = nullptr;

  if (status_ = status_t{sp_list_ports(&ports)}; status_ == status_t::OK) {
    for (auto i = 0; ports[i] != nullptr; ++i) {
      result.emplace_back(manage(ports[i]));
    }
    ports[0] = nullptr;
    sp_free_port_list(ports);
  }

  return result;
}

const char *sp::get_name(const port_t &p) noexcept {
  return empty_if_null(sp_get_port_name(p.get()));
}

const char *sp::get_description(const port_t &p) noexcept {
  return empty_if_null(sp_get_port_description(p.get()));
}

sp::transport_t sp::get_transport(const port_t &p) noexcept {
  return static_cast<transport_t>(sp_get_port_transport(p.get()));
}

sp::usb_bus_address_t sp::get_usb_bus_address(const port_t &p) noexcept {
  auto result = usb_bus_address_t{-1, -1};
  status_ = status_t{
      sp_get_port_usb_bus_address(p.get(), &result.bus, &result.address)};
  return result;
}

sp::usb_vid_pid_t sp::get_usb_vid_pid(const port_t &p) noexcept {
  auto result = usb_vid_pid_t{-1, -1};
  status_ = status_t{
      sp_get_port_usb_vid_pid(p.get(), &result.vid, &result.pid)};
  return result;
}

const char *sp::get_usb_manufacturer(const port_t &p) noexcept {
  return empty_if_null(sp_get_port_usb_manufacturer(p.get()));
}

const char *sp::get_usb_product(const port_t &p) noexcept {
  return empty_if_null(sp_get_port_usb_product(p.get()));
}

const char *sp::get_usb_serial_no(const port_t &p) noexcept {
  return empty_if_null(sp_get_port_usb_serial(p.get()));
}

const char *sp::get_bluetooth_address(const port_t &p) noexcept {
  return empty_if_null(sp_get_port_bluetooth_address(p.get()));
}

sp::status_t sp::get_native_handle(const port_t &p, void *const result_ptr) {
  return status_t{sp_get_port_handle(p.get(), result_ptr)};
}

sp::connection::connection(port_t p, const mode_t m, const port_config_t &cfg) {
  if (status_ = status_t{sp_open(p.get(), static_cast<sp_mode>(m))};
      status_ != status_t::OK) {
    throw connection_exc{last_error_message()};
  }
  p_ = std::move(p);
  set_config(cfg);
}

sp::connection::~connection() { sp_close(p_.get()); }

sp::port_config_t sp::connection::get_config() const {
  auto *cfg_raw_ptr = static_cast<sp_port_config *>(nullptr);
  sp_new_config(&cfg_raw_ptr);
  auto port_cfg = manage(cfg_raw_ptr);

  status_ = status_t{sp_get_config(p_.get(), cfg_raw_ptr)};

  auto cfg = port_config_t{};
  status_ = status_t{sp_get_config_baudrate(cfg_raw_ptr, &cfg.baud_rate)};
  status_ = status_t{sp_get_config_bits(cfg_raw_ptr, &cfg.bits)};
  auto parity = sp_parity{};
  status_ = status_t{sp_get_config_parity(cfg_raw_ptr, &parity)};
  cfg.parity = static_cast<parity_t>(parity);
  status_ = status_t{sp_get_config_stopbits(cfg_raw_ptr, &cfg.stop_bits)};
  return cfg;
}

sp::status_t sp::connection::set_config(const port_config_t &cfg) {
  auto *cfg_raw_ptr = static_cast<sp_port_config *>(nullptr);
  sp_new_config(&cfg_raw_ptr);
  auto port_cfg = manage(cfg_raw_ptr);

  status_ = status_t{sp_set_config_baudrate(cfg_raw_ptr, cfg.baud_rate)};
  status_ = status_t{sp_set_config_bits(cfg_raw_ptr, cfg.bits)};
  status_ = status_t{
      sp_set_config_parity(cfg_raw_ptr, static_cast<sp_parity>(cfg.parity))};
  status_ = status_t{sp_set_config_stopbits(cfg_raw_ptr, cfg.stop_bits)};
  return status_t{sp_set_config(p_.get(), cfg_raw_ptr)};
}

int sp::connection::read_blocking(void *const buf, const int count,
                                  const long timeout_ms) {
  if (count <= 0 || timeout_ms < 0) {
    status_ = status_t::InvalidArgument;
    return -1;
  }
  const auto ret = sp_blocking_read(p_.get(), buf,
                                    static_cast<std::size_t>(count),
                                    static_cast<unsigned>(timeout_ms));
  if (ret < 0) {
    status_ = status_t{ret};
    return -1;
  }
  return ret;
}

int sp::connection::read_next_blocking(void *const buf, const int count,
                                       const long timeout_ms) {
  if (count <= 0 || timeout_ms < 0) {
    status_ = status_t::InvalidArgument;
    return -1;
  }
  const auto ret = sp_blocking_read_next(p_.get(), buf,
                                         static_cast<std::size_t>(count),
                                         static_cast<unsigned>(timeout_ms));
  if (ret < 0) {
    status_ = status_t{ret};
    return -1;
  }
  return ret;
}

int sp::connection::read_nonblocking(void *const buf, const int count) {
  if (count <= 0) {
    status_ = status_t::InvalidArgument;
    return -1;
  }
  const auto ret = sp_nonblocking_read(p_.get(), buf,
                                       static_cast<std::size_t>(count));
  if (ret < 0) {
    status_ = status_t{ret};
    return -1;
  }
  return ret;
}

int sp::connection::input_waiting() {
  const auto ret = sp_input_waiting(p_.get());
  if (ret < 0) {
    status_ = status_t{ret};
    return -1;
  }
  return ret;
}

int sp::connection::write_blocking(const void *const buf, const int count,
                                   const long timeout_ms) {
  if (count <= 0 || timeout_ms < 0) {
    status_ = status_t::InvalidArgument;
    return -1;
  }
  const auto ret = sp_blocking_write(p_.get(), buf,
                                     static_cast<std::size_t>(count),
                                     static_cast<unsigned>(timeout_ms));
  if (ret < 0) {
    status_ = status_t{ret};
    return -1;
  }
  return ret;
}

int sp::connection::write_nonblocking(const void *const buf, const int count) {
  if (count <= 0) {
    status_ = status_t::InvalidArgument;
    return -1;
  }
  const auto ret = sp_nonblocking_write(p_.get(), buf,
                                        static_cast<std::size_t>(count));
  if (ret < 0) {
    status_ = status_t{ret};
    return -1;
  }
  return ret;
}

int sp::connection::output_waiting() {
  const auto ret = sp_output_waiting(p_.get());
  if (ret < 0) {
    status_ = status_t{ret};
    return -1;
  }
  return ret;
}

sp::status_t sp::connection::flush(buffer_t buffers_to_flush) {
  status_ = status_t{
      sp_flush(p_.get(), static_cast<sp_buffer>(buffers_to_flush))};
  return status_;
}

sp::status_t sp::connection::drain() {
  status_ = status_t{sp_drain(p_.get())};
  return status_;
}
