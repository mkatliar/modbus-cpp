// Copyright (C) 2022 Mikhail Katliar
// 
// modbus-cpp is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// modbus-cpp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with modbus-cpp. If not, see <http://www.gnu.org/licenses/>.

#include <gmock/gmock-spec-builders.h>
#include <modbus/Modbus.hpp>

#include "ModbusInterface.hpp"
#include "ModbusAdapter.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <system_error>


namespace modbus::testing
{
    using namespace ::testing;


    class MockModbus
    :   public ModbusInterface
    {
    public:
        MOCK_METHOD(modbus_t *, modbus_new_tcp, (const char * ip_address, int port), (override));
        MOCK_METHOD(const char *, modbus_strerror, (int errnum), (override));
        MOCK_METHOD(void, modbus_free, (modbus_t * ctx), (override));
    };


    class ModbusTest
    :   public Test
    {
        void SetUp() override
        {
            implementation(&mock_);
        }


        void TearDown() override
        {
            implementation(nullptr);
        }

    protected:
        MockModbus mock_;
    };


    TEST_F(ModbusTest, testNewTcp)
    {
        modbus_t * modbus_ptr = reinterpret_cast<modbus_t *>(0x10);

        EXPECT_CALL(mock_, modbus_new_tcp(StrEq("192.168.0.100"), 42)).WillOnce(Return(modbus_ptr));
        EXPECT_CALL(mock_, modbus_free(modbus_ptr)).Times(1);

        Context context = new_tcp("192.168.0.100", 42);
    }


    TEST_F(ModbusTest, testNewTcpEinval)
    {
        modbus_t * modbus_ptr = reinterpret_cast<modbus_t *>(0x10);

        EXPECT_CALL(mock_, modbus_new_tcp(StrEq("192.168.0.100"), 42)).WillOnce(SetErrnoAndReturn(EINVAL, nullptr));
        EXPECT_CALL(mock_, modbus_strerror(EINVAL)).WillOnce(Return("EINVAL"));
        EXPECT_CALL(mock_, modbus_free(modbus_ptr)).Times(0);

        EXPECT_THROW(new_tcp("192.168.0.100", 42), std::system_error);
    }
}
