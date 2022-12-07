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

#include "ModbusInterface.hpp"

#include <modbus/modbus.h>
#include <stdexcept>


namespace modbus :: testing
{
    static ModbusInterface * implementation_ = nullptr;


    void implementation(ModbusInterface * impl)
    {
        implementation_ = impl;
    }


    static ModbusInterface& validImplementation()
    {
        if (!implementation_)
            throw std::logic_error {"Modbus implementation not set!"};

        return *implementation_;
    }
}


extern "C"
{
    using namespace modbus::testing;

    MODBUS_API modbus_t * modbus_new_tcp(const char * ip_address, int port)
    {
        return validImplementation().modbus_new_tcp(ip_address, port);
    }


    MODBUS_API const char * modbus_strerror(int errnum)
    {
        return validImplementation().modbus_strerror(errnum);
    }


    MODBUS_API void modbus_free(modbus_t * ctx)
    {
        validImplementation().modbus_free(ctx);
    }
}
