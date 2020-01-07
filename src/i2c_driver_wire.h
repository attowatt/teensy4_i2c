// Copyright © 2019 Richard Gemmell
// Released under the MIT License. See license.txt. (https://opensource.org/licenses/MIT)

#ifndef I2C_DRIVER_WIRE_H
#define I2C_DRIVER_WIRE_H

#include <Arduino.h>
#include <functional>
#include "i2c_driver.h"
#ifdef __IMXRT1062__
#include "imx_rt1060/imx_rt1060_i2c_driver.h"
#endif

// An implementation of the Wire library as defined at
//   https://www.arduino.cc/en/reference/wire
// This header also defines TwoWire as an alias for I2CDriverWire
// for better compatibility with Teensy libraries.
// WARNING. This implementation does not include extensions to
// Wire that were part of the Teensy 3 implementation. e.g. setSDA()
class I2CDriverWire : public Stream {
public:
    // Size of RX and TX buffers. Feel free to change sizes if necessary.
    static const size_t rx_buffer_length = 32;
    static const size_t tx_buffer_length = 32;

    // Time to wait for a read or write to complete in millis
    static const uint32_t timeout_millis = 200;

    // Indicates that there is no more data to read.
    static const int no_more_bytes = -1;

    I2CDriverWire(I2CMaster& master, I2CSlave& slave);

    // Call setClock() before calling begin() to set the I2C frequency.
    // Although you can pass any frequency, it will be converted to one
    // of the standard values of 100_000, 400_000 or 1_000_000.
    // The default is 100000.
    void setClock(uint32_t frequency);

    // Use this version of begin() to initialise a master.
    void begin();

    // Use this version of begin() to initialise a slave.
    void begin(int address);

    void end();

    void beginTransmission(int address);

    uint8_t endTransmission(int stop = true);

    size_t write(uint8_t data) override;

    size_t write(const uint8_t* data, size_t length) override;

    uint8_t requestFrom(int address, int quantity, int stop = true);

    inline int available() override {
        return (int)(rx_bytes_available - rx_next_byte_to_read);
    }

    int read() override;

    int peek() override;

    // A callback that's called by the I2C driver's interrupt
    // service routine (ISR).
    // WARNING: This method is called inside an ISR so it must be
    // very, very fast. Avoid using it if at all possible.
    inline void onReceive(void (* function)(int len)) {
        on_receive = function;
    }

    // A callback that's called by the I2C driver's interrupt
    // service routine (ISR).
    // WARNING: This method is called inside an ISR so it must be
    // very, very fast. Avoid using it if at all possible.
    // In particular, don't call write() in this method to prepare
    // the transmit buffer. It's much better to fill the transmit
    // buffer during loop().
    inline void onRequest(void (* function)()) {
        on_request = function;
    }

    using Print::write;

private:
    I2CMaster& master;
    I2CSlave& slave;
    uint32_t master_frequency = 100 * 1000U;

    void (* on_receive)(int len) = nullptr;
    void (* on_request)() = nullptr;

    uint16_t write_address = 0;
    uint8_t tx_buffer[tx_buffer_length] = {};
    size_t tx_next_byte_to_write = 0;

    uint8_t rxBuffer[rx_buffer_length] = {};
    size_t rx_bytes_available = 0;
    size_t rx_next_byte_to_read = 0;

    void before_transmit();
    void finish();
    void on_receive_wrapper(size_t num_bytes);
};

extern I2CDriverWire Wire;
extern I2CDriverWire Wire1;
extern I2CDriverWire Wire2;

// Alias for backwards compatibility with Wire.h
using TwoWire = I2CDriverWire;

#endif //I2C_DRIVER_WIRE_H
