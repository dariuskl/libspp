// Darius Kellermann <kellermann@pm.me>, March 2024

// Unit tests the C++ interface by mocking the libserialport API.
// These scenarios focus on the automatic memory management.

#include <libserialport.hpp>

#include "libserialport_mock.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("port list memory is managed automatically") {
  REQUIRE(sp_mock::number_of_allocated_lists() == 0);
  REQUIRE(sp_mock::number_of_allocated_ports() == 0);

  GIVEN("a list of ports") {
    auto list = sp::list_ports();
    WHEN("the list has been acquired") {
      THEN("ports are allocated, list is free'd already") {
        CHECK(sp_mock::number_of_allocated_lists() == 0);
        CHECK(sp_mock::number_of_allocated_ports() == 2);
      }
      AND_WHEN("the list is cleared") {
        list.clear();
        THEN("no ports or lists are allocated anymore") {
          CHECK(sp_mock::number_of_allocated_lists() == 0);
          CHECK(sp_mock::number_of_allocated_ports() == 0);
        }
      }
    }
  }
}

SCENARIO("port memory is managed automatically") {
  REQUIRE(sp_mock::number_of_allocated_ports() == 0);

  GIVEN("a port") {
    auto port = sp::get_port_by_name("");

    WHEN("acquired") {
      THEN("memory is allocated") {
        REQUIRE(sp_mock::number_of_allocated_ports() == 1);
      }

      WHEN("smart pointer is reset") {
        port.reset();
        THEN("memory is freed") {
          REQUIRE(sp_mock::number_of_allocated_ports() == 0);
        }
      }
    }
  }
}

SCENARIO("config memory is managed automatically") {
  {
    auto port = sp::get_port_by_name("");
    REQUIRE(sp_mock::number_of_allocated_ports() == 1);
    auto conn = sp::connection{port, {}, {}};
    auto cfg = conn.get_config();
    conn.set_config(cfg);
    REQUIRE(sp_mock::number_of_allocated_configs() == 0);
  }
  REQUIRE(sp_mock::number_of_allocated_ports() == 0);
}

SCENARIO("error message is free'd via the `shared_ptr`") {
  GIVEN("an error with static message") {
    sp_mock::set_next_status(sp::status_t::OK);
    sp::get_usb_bus_address({}); // call a random function that sets the status
    REQUIRE(sp::get_status() == sp::status_t::OK);

    auto msg = sp::last_error_message();
    REQUIRE(sp_mock::number_of_allocated_messages() == 0);

    WHEN("resetting the smart pointer") {
      msg.reset();
      THEN("no memory is freed, because none was allocated") {}
    }
  }

  GIVEN("an error with allocated message") {
    sp_mock::set_next_status(sp::status_t::SystemError);
    sp::get_usb_bus_address({}); // call a random function that sets the status
    REQUIRE(sp::get_status() == sp::status_t::SystemError);
    sp_mock::set_next_status(sp::status_t::OK); // reset next status

    auto msg = sp::last_error_message();
    REQUIRE(sp_mock::number_of_allocated_messages() == 1);

    WHEN("resetting the smart pointer") {
      msg.reset();
      THEN("memory is freed") {
        REQUIRE(sp_mock::number_of_allocated_messages() == 0);
      }
    }
  }
}
