find_package(Catch2 3 REQUIRED)

add_library(test_ INTERFACE)
target_compile_features(test_ INTERFACE cxx_std_20)
target_compile_options(test_ INTERFACE
        -Wall -Wextra -pedantic -Wconversion -Wsign-conversion
        -Werror -pedantic-errors
        -Wswitch
)
target_link_libraries(test_ INTERFACE  Catch2::Catch2WithMain)

add_library(unit_test_ STATIC)
target_compile_definitions(unit_test_ PRIVATE SP_PRIV=)
target_include_directories(unit_test_
        PRIVATE ${libserialport_SOURCE_DIR}
        PUBLIC ${PROJECT_SOURCE_DIR}/inc
)
target_link_libraries(unit_test_ PUBLIC test_)
target_sources(unit_test_
        PRIVATE libserialport_mock.cpp ../src/libserialport.cpp
        PUBLIC libserialport_mock.hpp
)

add_executable(unit_test_memory unit_test_memory.cpp)
target_compile_options(unit_test_memory PRIVATE -fsanitize=address)
target_link_libraries(unit_test_memory PRIVATE unit_test_)
target_link_options(unit_test_memory PRIVATE -fsanitize=address)
add_test(NAME unit_test_memory COMMAND $<TARGET_FILE:unit_test_memory>)

add_executable(unit_test_exceptions unit_test_exceptions.cpp)
target_compile_options(unit_test_exceptions PRIVATE -fsanitize=undefined)
target_link_libraries(unit_test_exceptions PRIVATE unit_test_)
target_link_options(unit_test_exceptions PRIVATE -fsanitize=undefined)
add_test(NAME unit_test_exceptions COMMAND $<TARGET_FILE:unit_test_exceptions>)

add_executable(unit_test_logic unit_test_logic.cpp)
target_compile_options(unit_test_logic PRIVATE -fsanitize=undefined)
target_link_libraries(unit_test_logic PRIVATE unit_test_)
target_link_options(unit_test_logic PRIVATE -fsanitize=undefined)
add_test(NAME unit_test_logic COMMAND $<TARGET_FILE:unit_test_logic>)

add_executable(integration_test integration_test.cpp)
target_link_libraries(integration_test PRIVATE test_ libspp)
add_test(NAME integration_test COMMAND integration_test)
