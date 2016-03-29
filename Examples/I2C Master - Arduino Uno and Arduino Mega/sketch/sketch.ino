#include <Wire.h>
#include <MagzorI2C.h>

#include "My_I2CDevice.h"

//Address for the board
static const uint8_t MY_BOARD_I2C_ADDRESS = 0x04;

//My_I2CDevice object
My_I2CDevice my_i2cdevice0(MY_BOARD_I2C_ADDRESS);

uint8_t reset_pin = 0;
uint8_t interrupt_pin = 2;


void setup() {
  //Wait for MICs to initialize
  delay(1500);

  //Initialize I2C library
  Wire.begin();

  //Start serial output
  Serial.begin(9600);

  //register reset and interrupt pin
  MagzorI2C::setup(reset_pin, interrupt_pin);
  //Enable I2C communication print out. Comment this out if you don't wish to see it.
  //MLogger::set_all_enabled();

  //Seeding the random number generator for function2 parameters
  randomSeed( analogRead(0) );



}

void loop() {
  //Call our first function which just sends the command code to the I2C Slave device
  my_i2cdevice0.function1();
  Serial.print("Turning on LED connected to P1.0 on the MSP430G2 Launchpad.\n");

  //Delay 5 seconds so we can see the LED on before we run the next command
  Serial.print("Delaying 5 seconds before running the next command...\n");
  delay(5000);

  //Call our second function which sends the command code and an additional 3 bytes of data to the I2C Slave device
  //Generate 3 random numbers
  uint8_t random1 = random(0, 255);
  uint8_t random2 = random(0, 255);
  uint8_t random3 = random(0, 255);

  //Call function2
  my_i2cdevice0.function2(random1, random2, random3);

  Serial.print("Your 3 random numbers for function2 are: ");
  Serial.print(random1, DEC);
  Serial.print(" ");
  Serial.print(random2, DEC);
  Serial.print(" ");
  Serial.print(random3, DEC);
  Serial.print("\n");

  unsigned int total_sum = random1 + random2 + random3;
  Serial.print("The sum is ");
  Serial.print(total_sum, DEC);
  Serial.print(".\n");

  //Check if the result is even, which means the LED would be turned off
  if ((random1 % 2) == 0) {
    Serial.print("Random1 is ");
    Serial.print(random1, DEC);
    Serial.print(" which is even and the LED will be turned off.\n");
  }
  //Not even, so LED was not turned off
  else {
    Serial.print("Random1 is ");
    Serial.print(random1, DEC);
    Serial.print(" which is odd and the LED will be left on.\n");
  }

  //Call our third function which sends the command code and requests back 1 byte of data from the I2C Slave device
  unsigned int func3_calltimes = my_i2cdevice0.function3();
  Serial.print("Function 3 was called ");
  Serial.print(func3_calltimes, DEC);
  Serial.print(" times since the I2C Slave device has started up.\n");


  //Call our fourth function which sends the command code and 1 additional byte of data to the I2C Slave device
  //then requests 3 bytes of data from the I2C Slave device
  //the function then adds those 3 bytes of data and returns that value
  unsigned int returned_total_sum = my_i2cdevice0.function4(0);
  Serial.print("Function 4 has returned a total sum of ");
  Serial.print(returned_total_sum, DEC);
  Serial.print(" which should be the same number as above.\n");

  //delay 1 second to finish printing stuff from MLogger
  delay(1000);

  //delay 10 seconds before repeating loop
  Serial.print("***Delaying 10 seconds before running loop again\n\n\n\n\n");
  delay(10000);
}
