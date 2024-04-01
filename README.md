# libspp -- A C++ wrapper for libserialport

The motivation for this is to further simplify the use of serial ports from
C++ code. The main features are RAII and memory management by `std::shared_ptr`.

Once a stable API has materialized, below example will show the general usage.

```cpp
    #include <libserialport.hpp>

    int main() {
        // list serial ports
        // connect to one serial port
        // write and receive data
        return 0;
    }
```

## Build & Versioning

Everything is built with CMake, both the original libserialport and the C++ part.
Upstream libserialport uses autotools, which I have a distaste for.
While this simplifies the build system (in terms of readability at the very least),
not all platforms that libserialport supports are supported.

To build a shared library, pass `BUILD_SHARED_LIBS=ON` to CMake.

I aim to achieve a good test coverage for at least two major Linux distributions
and Windows.

The libserialport upstream code is fetched during the build in a fixed version.
This project applies the same versioning as upstream,
which also means that versions need to be bumped manually.
