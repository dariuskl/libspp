add_library(example_ INTERFACE)
target_compile_options(example_ INTERFACE
        -Wall -Wextra -pedantic -Wconversion -Wsign-conversion
        -Werror -pedantic-errors
        -Wswitch
)

add_executable(list_ports list_ports.cpp)
target_link_libraries(list_ports PRIVATE libspp example_)

add_executable(configure_port configure_port.cpp)
target_link_libraries(configure_port PRIVATE libspp example_)

add_executable(send_and_receive send_and_receive.cpp)
target_link_libraries(send_and_receive PRIVATE libspp example_)
