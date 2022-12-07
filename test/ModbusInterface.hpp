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

#pragma once

#include <modbus/modbus.h>


namespace modbus :: testing
{
    struct ModbusInterface
    {
        virtual modbus_t * modbus_new_tcp(const char * ip_address, int port) = 0;
        virtual const char * modbus_strerror(int errnum) = 0;
        virtual void modbus_free(modbus_t * ctx) = 0;
    };
}