include(FetchContent)

FetchContent_Declare(
        libserialport
        GIT_REPOSITORY "git://sigrok.org/libserialport"
        GIT_TAG "fd20b0fc5a34cd7f776e4af6c763f59041de223b"
)
FetchContent_MakeAvailable(libserialport)

macro(parse_m4_define VARNAME)
    string(REGEX MATCH "m4_define\\(\\[${VARNAME}\\], \\[([0-9]+)\\]\\)" _ ${CONFAC})
    set(${VARNAME} ${CMAKE_MATCH_1})
endmacro()

macro(parse_configure)
    file(READ "${libserialport_SOURCE_DIR}/configure.ac" CONFAC)
    parse_m4_define(sp_package_version_major)
    parse_m4_define(sp_package_version_minor)
    parse_m4_define(sp_package_version_micro)
endmacro(parse_configure)

parse_configure()
message(STATUS "build based on libserialport v${sp_package_version_major}.${sp_package_version_minor}.${sp_package_version_micro}")
