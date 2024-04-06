// Darius Kellermann <kellermann@pm.me>, March 2024

// Unit tests the C++ interface by mocking the libserialport API.
// These scenarios focus on the exceptions.

// TODO test that API excepts when supposed to

#include <libserialport.hpp>

#include <catch2/catch_test_macros.hpp>

SCENARIO("USB serial is unplugged during operation") {
  GIVEN("connection to a USB serial port") {
    auto conn = sp::connection{{}, {}, {}};
    WHEN("") {
      THEN("") {}
    }
  }
}
