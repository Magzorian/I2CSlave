#ifndef MY_I2CDEVICE_H
#define MY_I2CDEVICE_H

#include <MagzorI2C/AbstractI2CDevice.h>
#include <stdint.h>

class My_I2CDevice : public AbstractI2CDevice {
private:
    void initDevice();

protected:

public:

    My_I2CDevice() : AbstractI2CDevice() {
        initDevice();
    };

    My_I2CDevice(uint8_t i2c_address) : AbstractI2CDevice(i2c_address) {
        initDevice();
    };

    void function1();
    void function2(uint8_t data0, uint8_t data1, uint8_t data2);
    uint8_t function3();
    unsigned int function4(uint8_t data0);
};

#endif

