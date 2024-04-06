// Darius Kellermann <kellermann@pm.me>, March 2024

// Tests the operation of the library down to the hardware.

#include <libserialport.hpp>

#include <catch2/catch_test_macros.hpp>

// TODO provide hardware and test e.g. configuration, misconfiguration, etc.

SCENARIO("opening a port") {
  GIVEN("an invalid port name") {
    const auto *const port_name = "/not/a/serial/port";
    WHEN("getting the port") {
      auto port = sp::get_port_by_name(port_name);
      THEN("the returned port is invalid") {
        CHECK_FALSE(port);
        CHECK(sp::get_status() == sp::status_t::InvalidArgument);
      }
    }
  }

  GIVEN("a valid node that is not a serial port") {
    const auto *const port_name = "/dev/null";
    WHEN("getting the port") {
      auto port = sp::get_port_by_name(port_name);
      THEN("the returned port is invalid") {
        CHECK_FALSE(port);
        CHECK(sp::get_status() == sp::status_t::InvalidArgument);
      }
    }
  }
}

SCENARIO("requesting information about a port") {
  GIVEN("an invalid port") {
    auto port = sp::port_t{};
    WHEN("requesting information about the port") {
      REQUIRE(*sp::get_name(port) == '\0');
      REQUIRE(*sp::get_description(port) == '\0');
      REQUIRE(sp::get_transport(port) == sp::transport_t::Native);
      REQUIRE(sp::get_usb_bus_address(port) == sp::usb_bus_address_t{-1, -1});
      REQUIRE(*sp::get_usb_manufacturer(port) == '\0');
      REQUIRE(*sp::get_usb_product(port) == '\0');
      REQUIRE(*sp::get_usb_serial_no(port) == '\0');
      REQUIRE(*sp::get_bluetooth_address(port) == '\0');
    }
  }

  GIVEN("a valid port") {
    auto port = sp::get_port_by_name("");
  }
}
