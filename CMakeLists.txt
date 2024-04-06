cmake_minimum_required(VERSION 3.14...3.23)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include(cmake/libserialport.cmake)

project(libspp
        VERSION ${sp_package_version_major}.${sp_package_version_minor}.${sp_package_version_micro}.0
        DESCRIPTION "a C++ wrapper for the libserialport library"
        LANGUAGES CXX C
)

option(BUILD_SHARED_LIBS "build a shared or static library" OFF)
option(SP_BUILD_TESTING "build test programs (requires Catch2 v3)" ON)
option(SP_BUILD_EXAMPLES "build example programs" ON)

if (SP_BUILD_TESTING)
    include(CTest)
endif ()

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(test)
endif ()

if (SP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

include(cmake/install.cmake)
