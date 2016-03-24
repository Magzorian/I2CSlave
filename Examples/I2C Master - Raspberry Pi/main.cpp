#include <iostream>
#include <stdlib.h> 
#include <wiringPi.h>
#include <MagzorI2C/MagzorI2C.h>

#include "My_I2CDevice.h"

//Address for the board
static const uint8_t MY_BOARD_I2C_ADDRESS = 0x04;

//My_I2CDevice object
My_I2CDevice my_i2cdevice0(MY_BOARD_I2C_ADDRESS);

const uint8_t reset_pin = 4;
const uint8_t interrupt_pin = 0;

using namespace std;

int main(int argc, char** argv) {
    //Seeding the random number generator for function2 parameters
    srand (time(NULL));
    
    //Setting up the wiringPi library
    if (wiringPiSetup() < 0) {
        fprintf(stderr, "Setup failed\n");
        exit(1);
    }

    //Setting up the MagzorI2C library
    MagzorI2C::setup(reset_pin, interrupt_pin);
    //Enable I2C communication print out. Comment this out if you don't wish to see it.
    //MLogger::set_all_enabled();


    //Call our first function which just sends the command code to the I2C Slave device
    my_i2cdevice0.function1();
    cout << "Turning on LED connected to P1.0 on the MSP430G2 Launchpad." << endl;

    //Delay 5 seconds so we can see the LED on before we run the next command
    cout << "Delaying 5 seconds before running the next command..." << endl;
    delay(5000);
    
    //Call our second function which sends the command code and an additional 3 bytes of data to the I2C Slave device
    //Generate 3 random numbers
    uint8_t random1 = rand() % 256;
    uint8_t random2 = rand() % 256;
    uint8_t random3 = rand() % 256;
    
    //Call function2
    my_i2cdevice0.function2(random1, random2, random3);
    
    cout    << "Your 3 random numbers for function2 are: "
            << unsigned(random1) << " "
            << unsigned(random2) << " "
            << unsigned(random3) << " "
            << endl;
    
    unsigned int total_sum = random1 + random2 + random3;
    cout << "The sum is " << unsigned(total_sum) << "." << endl;
    
    //Check if the result is even, which means the LED would be turned off
    if((random1 % 2) == 0){
        cout << "Random1 is " << unsigned(random1) << " which is even and the LED will be turned off." << endl;
    }
    //Not even, so LED was not turned off
    else{
        cout << "Random1 is " << unsigned(random1) << " which is odd and the LED will be left on." << endl;
    }

    //Call our third function which sends the command code and requests back 1 byte of data from the I2C Slave device
    unsigned int func3_calltimes = my_i2cdevice0.function3();
    cout    << "Function 3 was called " << unsigned(func3_calltimes) 
            << " times since the I2C Slave device has started up." << endl;
    

    //Call our fourth function which sends the command code and 1 additional byte of data to the I2C Slave device
    //then requests 3 bytes of data from the I2C Slave device
    //the function then adds those 3 bytes of data and returns that value
    unsigned int returned_total_sum = my_i2cdevice0.function4(0);
    cout    << "Function 4 has returned a total sum of " << returned_total_sum
            << " which should be the same number as above." << endl;
    
    //delay 1 second to finish printing stuff from MLogger
    delay(1000);
    
    return 0;
}