# modbus-cpp
This is a C++ wrapper around [**libmodbus**](https://libmodbus.org/), a library to communicate with [Modbus](https://en.wikipedia.org/wiki/Modbus) devices. It brings you the benefit of automatic resource management due to [RAII](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) and convenient error handling due to exceptions. It is a header-only library, has zero performance overhead, and does not require installation.

The library implements only a subset of **libmodbus** interface (actually, only the functions that I needed for my projects). If you want more -- feel free to add them and create a pull request!

## Code example
```c++
#include <modbus/Modbus.hpp>
#include <iostream>

int main(int, char **)
{
    try
    {
        modbus::Context modbus = modbus::new_tcp("192.168.0.100");
        modbus::Connection connection {modbus};

        std::array<std::uint16_t, 3> data;
        connection.read_registers(0x123, data.size(), data.data());
        connection.write_register(0x456, 0x42ff);
    }
    catch (std::system_error const& e)
    {
        std::cerr << "Error: " << e.message() << std::endl;
    }

    return 0;
}
```

## Dependencies
Besides the C++ standard library, **modbus-cpp** depends on [Boost Exception](https://www.boost.org/doc/libs/1_80_0/libs/exception/doc/boost-exception.html). Although this dependency is not strictly necessary, I find it nice to have function and line info in exceptions, therefore I use `BOOST_THROW_EXCEPTION()`. Now Boost recommends to use [Boost LEAF](https://www.boost.org/doc/libs/1_80_0/libs/leaf/doc/html/index.html), so I may switch to that. I also like the idea to give the user a choice (by adding a configuration option/macro) to use only the standard C++ library.

If you want to run library tests, you will also need to install [**GoogleTest**](https://github.com/google/googletest).


## Using
1. Install **Boost Exception**. On Ubuntu and Debian Linux, the command is:
    ```bash
    sudo apt install libboost-exception-dev
    ````
2. Copy `include/modbus` where your compiler can see it.
3. Add `#include <modbus/Modbus.hpp>` in your program.
4. Don't forget to link to `libmodbus`.
5. Enjoy!

## Tests
The library is equipped with some tests. First, install **GoogleTest**:
```bash
sudo apt install googletest
```
To build and run the tests, do
```bash
sudo apt install googletest
cd modbus-cpp
mkdir build
cd build
cmake ..
make
make test
```