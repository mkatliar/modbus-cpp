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

#include <boost/throw_exception.hpp>

#include <memory>
#include <system_error>
#include <cerrno>
#include <cstdint>


namespace modbus
{
    namespace detail
    {
        /**
         * @brief Implements @a std::error_category for Modbus errors.
         *
         */
        class ErrorCategory
        :   public std::error_category
        {
        public:
            char const * name() const noexcept override
            {
                return "modbus";
            }


            std::string message(int condition) const override
            {
                return ::modbus_strerror(condition);
            }
        };


        /**
         * @brief Deleter class for Modbus context
         *
         */
        struct ContextDeleter
        {
            void operator()(::modbus_t * context) const noexcept
            {
                ::modbus_free(context);
            }
        };
    }


    /**
     * @brief Modbus error category object.
     *
     * @return Modbus error category singleton.
     */
    inline std::error_category const& modbus_category() noexcept
    {
        static const detail::ErrorCategory category;
        return category;
    }


    /**
     * @brief Manages a pointer to modbus_t structure.
     *
     * Automatically deallocates the structure when destroyed.
     *
     */
    using Context = std::unique_ptr<modbus_t, detail::ContextDeleter>;


    /**
     * @brief Allocates and initializes Modbus context to communicate with a Modbus TCP IPv4 server.
     *
     * https://libmodbus.org/reference/modbus_new_tcp/
     *
     * @param ip_address specifies the IP address of the server to which the client wants to establish a connection. A nullptr value can be used to listen any addresses in server mode.
     * @param port TCP port to use. It’s convenient to use a port number greater than or equal to 1024 because it’s not necessary to have administrator privileges.
     *
     * @return Newly created modbus context
     *
     * @throw @a std::system_error with an error code, if an error occurs.
     */
    inline Context new_tcp(char const * ip_address = nullptr, int port = MODBUS_TCP_DEFAULT_PORT)
    {
        Context context {::modbus_new_tcp(ip_address, port)};

        if (!context)
            BOOST_THROW_EXCEPTION((std::system_error {errno, modbus_category()}));

        return context;
    }


    /**
     * @brief Allocates and initializes Modbus context to communicate with a Modbus TCP IPv4 server.
     *
     * https://libmodbus.org/docs/v3.1.7/modbus_new_tcp.html
     *
     * @param ip_address specifies the IP address of the server to which the client wants to establish a connection.
     * @param port TCP port to use. It’s convenient to use a port number greater than or equal to 1024 because it’s not necessary to have administrator privileges.
     *
     * @return Newly created modbus context
     *
     * @throw @a std::system_error with an error code, if an error occurs.
     */
    inline Context new_tcp(std::string const& ip_address, int port = MODBUS_TCP_DEFAULT_PORT)
    {
        return new_tcp(ip_address.c_str(), port);
    }


    /**
     * @brief Set the debug flag of the Modbus context. By default, the boolean flag is set to FALSE.
     * When the flag value is set to true, many verbose messages are displayed on stdout and stderr.
     * For example, this flag is useful to display the bytes of the Modbus messages.
     *
     * https://libmodbus.org/docs/v3.1.7/modbus_set_debug.html
     *
     * @param context Modbus context
     * @param flag when set to true, verbose messages are displayed on stdout and stderr.
     *
     * @throw @a std::system_error with an error code, if an error occurs.
     */
    inline void set_debug(Context& context, bool flag)
    {
        if (::modbus_set_debug(context.get(), flag) != 0)
            BOOST_THROW_EXCEPTION((std::system_error {errno, modbus_category()}));
    }


    class Connection
    {
    public:
        /**
         * @brief Establishes a connection to a Modbus server, a network or a bus using the context information of libmodbus context given in argument.
         *
         * https://libmodbus.org/docs/v3.1.7/modbus_connect.html
         *
         * @param context Modbus context. The @a Connection object takes ownership of @a context.
         *
         * @throw @a std::system_error with an error code, if an error occurs.
         */
        explicit Connection(Context context)
        {
            if (::modbus_connect(context.get()) != 0)
                BOOST_THROW_EXCEPTION((std::system_error {errno, modbus_category()}));

            context_ = std::move(context);
        }


        /**
         * @brief Move constructor.
         *
         */
        Connection(Connection&&) = default;


        /**
         * @brief Close the connection established with the backend set in the context.
         *
         * https://libmodbus.org/docs/v3.1.7/modbus_close.html
         *
         */
        ~Connection()
        {
            ::modbus_close(context_.get());
        }


        /**
         * @brief Close the connection established with the backend set in the context,
         * release the context ownership and return the previously owned context.
         *
         * https://libmodbus.org/docs/v3.1.7/modbus_close.html
         *
         */
        Context close() noexcept
        {
            ::modbus_close(context_.get());
            return std::move(context_);
        }


        /**
         * @brief Read the content of the @a nb holding registers from the address @a addr of the remote device.
         *
         * The result of reading is stored in dest array as word values (16 bits).
         * You must take care to allocate enough memory to store the results in dest (at least nb * sizeof(uint16_t)).
         * The function uses the Modbus function code 0x03 (read holding registers).
         *
         * https://libmodbus.org/reference/modbus_read_registers/
         *
         * @param addr addres of the first register to read
         * @param nb number of registers to read
         * @param dest array to store the read data
         *
         * @return The number of read registers
         *
         * @throw @a std::system_error with an error code, if an error occurs.
         */
        int read_registers(int addr, int nb, std::uint16_t * dest) const
        {
            int const read = ::modbus_read_registers(context_.get(), addr, nb, dest);
            if (read < 0)
                BOOST_THROW_EXCEPTION((std::system_error {errno, modbus_category()}));

            return read;
        }


        /**
         * @brief Write the value of a single holding register at the address @a addr of the remote device.
         *
         * The function uses the Modbus function code 0x06 (preset single register).
         *
         * https://libmodbus.org/reference/modbus_write_register/
         *
         * @param addr holding register addres
         * @param value value to write
         *
         * @throw @a std::system_error with an error code, if an error occurs.
         */
        void write_register(int addr, std::uint16_t value)
        {
            if (::modbus_write_register(context_.get(), addr, value) != 1)
                BOOST_THROW_EXCEPTION((std::system_error {errno, modbus_category()}));
        }


    private:
        Context context_;
    };


    /**
     * @brief Extracts the high byte from a word
     *
     * https://libmodbus.org/docs/v3.1.7/
     *
     * @param word word to extract the byte from
     *
     * @return high byte of @a word
     */
    inline std::uint8_t get_high_byte(std::uint16_t word)
    {
        return MODBUS_GET_HIGH_BYTE(word);
    }


    /**
     * @brief Extracts the low byte from a word
     *
     * https://libmodbus.org/docs/v3.1.7/
     *
     * @param word word to extract the byte from
     *
     * @return low byte of @a word
     */
    inline std::uint8_t get_low_byte(std::uint16_t word)
    {
        return MODBUS_GET_LOW_BYTE(word);
    }


    /**
     * @brief Builds an int16 from two int8
     *
     * @param high_byte high byte
     * @param low_byte low byte
     *
     * @return an int16 with high byte set to @a high_byte and low byte set to @a low_byte
     */
    inline std::uint16_t get_int16_from_int8(std::uint8_t high_byte, std::uint8_t low_byte) noexcept
    {
        return static_cast<std::uint16_t>(high_byte) << 8 | static_cast<std::uint16_t>(low_byte);
    }


    /**
     * @brief Read the content of a single holding register from the address @a addr of the remote device.
     *
     * @param connection Modbus connection object
     * @param addr addres of the first register to read
     *
     * @return The content of the register
     *
     * @throw @a std::system_error with an error code, if an error occurs.
     */
    inline std::uint16_t read_register(Connection& connection, int addr)
    {
        std::uint16_t result;
        connection.read_registers(addr, 1, &result);

        return result;
    }
}

