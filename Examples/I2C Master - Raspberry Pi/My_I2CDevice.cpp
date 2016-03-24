#include "My_I2CDevice.h"

//Generic constructor for this device
void My_I2CDevice::initDevice(){
     
}

void My_I2CDevice::function1() {
    //Command code for our first function.
    uint8_t command_code_function1 = 0x81;

    //Create the array to pass to the write function.
    uint8_t w[1];

    //Populate the  write array. Index 0 is always the command code.
    w[0] = command_code_function1;

    //Call the write function. We pass it the array and the number of bytes.
    this->write(w, 1);
}

void My_I2CDevice::function2(uint8_t data0, uint8_t data1, uint8_t data2) {
    //Command code for our second function.
    uint8_t command_code_function2 = 0x82;

    //Create the array to pass to the write function.
    uint8_t w[4];

    //Populate the array. Index 0 is always the command code. Follow by additional data.
    w[0] = command_code_function2;
    w[1] = data0;
    w[2] = data1;
    w[3] = data2;

    //Call the write function. We pass it the array and the number of bytes.
    this->write(w, 4);
}

uint8_t My_I2CDevice::function3() {
    //Command code for our third function
    uint8_t command_code_function3 = 0x83;

    //Create the arrays to pass to the write read function
    uint8_t w[1];
    uint8_t r[1];

    //Populate the  write array. Index 0 is always the command code.
    w[0] = command_code_function3;

    //Call the write read function. We pass it the arrays and the number of bytes to write and read.
    //We also have a very short delay (10us) between the write and read operations to allow the device to prepare its data.
    this->write_read(w, 1, r, 1, 10);

    //Return the read byte from the I2C device
    return r[0];
}

unsigned int My_I2CDevice::function4(uint8_t data0) {
    //Command code for the fourth function
    uint8_t command_code_function4 = 0x84;

    //Create the arrays to pass the write read function
    uint8_t w[2];
    uint8_t r[3];

    //Populate the array. Index 0 is always the command code. Follow by additional data.
    w[0] = command_code_function4;
    w[1] = data0;

    //Call the write read function. We pass it the arrays and the number of bytes to write and read.
    //We also have a very short delay (10us) between the write and read operations to allow the device to prepare its data.
    this->write_read(w, 2, r, 3, 10);

    //Return the sum of all read the bytes
    unsigned int ret_val = 0;
    ret_val += r[0];
    ret_val += r[1];
    ret_val += r[2];

    return ret_val;
}
