// Darius Kellermann <kellermann@pm.me>, March 2024

// Provides access to mock-specific information.

#ifndef LIBSERIALPORT_MOCK_HPP_INCLUDED
#define LIBSERIALPORT_MOCK_HPP_INCLUDED

#include <libserialport.hpp>

namespace sp_mock {

  long number_of_allocated_lists();
  long number_of_allocated_ports();
  long number_of_allocated_configs();
  long number_of_allocated_messages();

  void set_next_status(sp::status_t status);

} // namespace sp_mock

#endif // LIBSERIALPORT_MOCK_HPP_INCLUDED
