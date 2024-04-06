add_library(libspp)
add_library(libspp::libspp ALIAS libspp)
target_compile_features(libspp PUBLIC cxx_std_20)
target_compile_options(libspp PRIVATE
        -Wall -Wextra -pedantic -Wconversion -Wsign-conversion
        -Werror -pedantic-errors
        -Wswitch
)
target_link_libraries(libspp PRIVATE libserialport)
target_sources(libspp
        PRIVATE
        $<TARGET_OBJECTS:libserialport>
        libserialport.cpp
        PUBLIC FILE_SET hpps TYPE HEADERS BASE_DIRS ${PROJECT_SOURCE_DIR}/inc FILES
        ${PROJECT_SOURCE_DIR}/inc/libserialport.hpp
)
set_target_properties(libspp PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 0:0:0)

add_library(libserialport OBJECT)
target_compile_definitions(libserialport PUBLIC
        $<BUILD_INTERFACE:LIBSERIALPORT_ATBUILD>
        $<BUILD_INTERFACE:SP_API=>
    PRIVATE SP_PRIV=
)
target_compile_features(libserialport PUBLIC $<BUILD_INTERFACE:c_std_99>)
target_include_directories(libserialport
    PUBLIC
        $<BUILD_INTERFACE:${libserialport_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_options(libserialport PRIVATE
        LINKER:-no-undefined
)
target_sources(libserialport
    PUBLIC
        $<BUILD_INTERFACE:${libserialport_SOURCE_DIR}/libserialport.h>
        $<BUILD_INTERFACE:${libserialport_SOURCE_DIR}/libserialport_internal.h>
    PRIVATE
        ${libserialport_SOURCE_DIR}/serialport.c
        ${libserialport_SOURCE_DIR}/timing.c
)

if (UNIX)
    target_compile_options(libserialport PRIVATE
            -Wall -Wextra -pedantic -Wmissing-prototypes -Wshadow
    )
    if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        target_sources(libserialport PRIVATE
                ${libserialport_SOURCE_DIR}/freebsd.c
        )
    else ()
        target_sources(libserialport PRIVATE
                ${libserialport_SOURCE_DIR}/linux.c
                ${libserialport_SOURCE_DIR}/linux_termios.c
                ${libserialport_SOURCE_DIR}/linux_termios.h
        )
    endif ()
elseif (WIN32)
    target_link_libraries(libserialport PRIVATE
            setupapi
    )
    target_sources(libserialport PRIVATE
            ${libserialport_SOURCE_DIR}/windows.c
    )
    set(NO_ENUMERATION 1)
    set(NO_PORT_METADATA 1)
elseif (APPLE)
    target_link_options(libserialport PRIVATE
            LINKER:-framework IOKit
            LINKER:-framework CoreFoundation
    )
    target_sources(libserialport PRIVATE
            ${libserialport_SOURCE_DIR}/macosx.c
    )
endif ()

include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckStructHasMember)
include(CheckSymbolExists)
include(CheckTypeSize)

macro(sp_check_header HEADER)
    string(REPLACE "/" "_" VAR_HEADER ${HEADER})
    string(TOUPPER ${VAR_HEADER} VAR_HEADER)
    check_include_file("${HEADER}.h" "HAVE_${VAR_HEADER}_H")
endmacro()

macro(sp_check_function FN EXTRA_INCLUDES)
    string(TOUPPER ${FN} VAR_FN)
    set(CMAKE_EXTRA_INCLUDE_FILES ${EXTRA_INCLUDES})
    check_function_exists(${FN} "HAVE_${VAR_FN}")
    set(CMAKE_EXTRA_INCLUDE_FILES)
endmacro()

macro(sp_check_type TYP EXTRA_INCLUDES)
    string(TOUPPER ${TYP} VAR_TYP)
    string(REPLACE " " "_" VAR_TYP ${VAR_TYP})
    set(CMAKE_EXTRA_INCLUDE_FILES ${EXTRA_INCLUDES})
    check_type_size(${TYP} ${VAR_TYP} LANGUAGE C)
    set(CMAKE_EXTRA_INCLUDE_FILES)
    if (HAVE_${VAR_TYP})
        set(HAVE_${VAR_TYP} 1)
    else ()
        set(HAVE_${VAR_TYP} 0)
    endif ()
endmacro()

sp_check_function("clock_gettime" "time.h")
check_symbol_exists("BOTHER" "asm/termbits.h" HAVE_DECL_BOTHER)
sp_check_function("flock" "")
sp_check_function("realpath" "")
sp_check_type("struct serial_struct" "linux/serial.h")
sp_check_type("struct termios2" "asm/termbits.h")
check_struct_has_member("struct termios2" "c_ispeed" "asm/termbits.h" HAVE_STRUCT_TERMIOS2_C_ISPEED LANGUAGE C)
check_struct_has_member("struct termios2" "c_ospeed" "asm/termbits.h" HAVE_STRUCT_TERMIOS2_C_OSPEED LANGUAGE C)
check_struct_has_member("struct termios" "c_ispeed" "asm/termbits.h" HAVE_STRUCT_TERMIOS_C_ISPEED LANGUAGE C)
check_struct_has_member("struct termios" "c_ospeed" "asm/termbits.h" HAVE_STRUCT_TERMIOS_C_OSPEED LANGUAGE C)
sp_check_header("sys/file")
sp_check_header("sys/stat")
sp_check_header("sys/types")
sp_check_header("unistd")

configure_file(config.h.in config.h)
