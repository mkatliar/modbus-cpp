# Copyright (C) 2022 Mikhail Katliar
#
# modbus-cpp is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# modbus-cpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with modbus-cpp. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.24)
project(modbus_cpp CXX)
set(CMAKE_CXX_STANDARD 20)

enable_testing()
find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS exception)

add_library(modbus_cpp INTERFACE)
target_include_directories(modbus_cpp INTERFACE "include")
target_link_libraries(modbus_cpp
    INTERFACE Boost::exception
)

add_executable(
    modbus_cpp-test
    test/ModbusTest.cpp
    test/ModbusAdapter.cpp
)

target_link_libraries(
    modbus_cpp-test
    modbus_cpp
    GTest::gtest_main
    GTest::gmock
)

gtest_discover_tests(modbus_cpp-test)