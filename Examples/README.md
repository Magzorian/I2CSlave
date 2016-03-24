# Creating your own I2C device using the MSP430 and controlling it with a Raspberry Pi or an Arduino


## Introduction

This tutorial will cover how to write your own I2C slave device using the I2CSlave library and control it by defining the I2C device on your I2C master using the MagzorI2C library.
The I2C slave device will be running on the MSP430G2553 using the I2CSlave library for Energia. Energia is an Arduino-like IDE for the MSP430 and other TI Launchpads.

## Requirements
#### Hardware
* Supported Raspberry Pi or Supported Arduino
* MSP430G2 Launchpad with MSPG2553 MCU in the socket
* Bread Board
* Jumper Wires
* 2x 10kOhm Pull Up Resistors
* for the Arduino:
    * Additional 2x 10kOhm Pull Up Resistors
    * 1x 200kOhm Pull Up Resistor
    * Level Shifter - PCA9306

#### Software
* Raspbian on the Raspberry Pi or Arduino IDE
* MagzorI2C Library, for [Raspberry Pi](https://github.com/Magzorian/MagzorI2C-RaspberryPi/) or for [Arduino](https://github.com/Magzorian/MagzorI2C-Arduino/)
* [Energia](http://energia.nu/), recommended version: 0101E0016
* [I2CSlave](https://github.com/Magzorian/I2CSlave) library

## Hardware Setup
First things first. Setting up the hardware. Connect your I2C SDA and SCL from the slave to the master. Ensure you have proper pull up resistors and on your I2C SDA and SCL lines.
Raspberry Pi and MSP430G2553 operate at 3.3V. A pull up on the I2C lines to 3.3V source is required. Arduino and MSP430G2553 operate at 5V and 3.3V respectively and therefore a level shifter is required or the MSP430G2553 can possibly be damaged. 
Respective pull-ups are also needed.

**Raspberry Pi 2 Hardware Setup**
![Hello!](https://github.com/Magzorian/I2CSlave/blob/master/Examples/images/arduinouno_setup.png | width=300)
**Arduino Uno Hardware Setup**
![Hi!](https://raw.githubusercontent.com/Magzorian/I2CSlave/master/Examples/images/arduinouno_setup.png | width=300)

## Background
I2C is a serial communication protocol where an I2C Master controls several I2C slaves. It is a master-slave protocol which means that only the I2C Master can start communication. The I2C Slave cannot directly initiate communication.
The major advantage of I2C is the addition of slave devices doesn't require any more pins. It is just as simple as connecting the slave device's I2C lines to the bus.

## Software Setup
* Install MagzorI2C library onto your [Raspberry Pi](https://github.com/Magzorian/MagzorI2C-RaspberryPi) or the [Supported Arduino](https://github.com/Magzorian/MagzorI2C-Arduino).
* Install the [I2CSlave](https://github.com/Magzorian/I2CSlave) onto Energia
	* Extract the I2CSlave.zip contents into your _C:\Users\\**USER_NAME**\Documents\Energia\libraries_ folder.

## Communciation Protocol
The communication protocol followed by this system is as follows.  
Brackets [] denotes a single byte. Descriptions within the bracket defines what the byte represents.

**IMPORTANT NOTE: Data is sent and retrieved as bytes. Therefore, if you wish to send or retrieve other datatypes you are responsible for breaking up the datatype into bytes and recombining it in the other end.**

I2C Master writes to I2C Slave. This means that I2C Master writes data bytes to the I2C Slave. The I2C Slave then can perform a task if set up to.
A Command Code Byte is always required. It determines which function will be called on the I2C Slave once it receives it. Optional bytes of data can be added to the packet if required for the function on the I2C Slave.  
**I2C Master -> I2C Slave**  
``[Command Code Byte][Data Byte 0][Data Byte 1]...[Data Byte n]``

I2C Master reads from I2C Slave. This means that the I2C Master requests data from the I2C Slave. The data must be ready to read before performing this request.
The I2C Slave cannot initiate sending data to the I2C Master. Only the I2C Master can request data from the I2C Slave. Usually, the library will first perform a I2C Master write followed by an I2C Master read.
The I2C Master requests a certain number of bytes of data from the I2C Slave.  
**I2C Master <- I2C Slave**  
``[Data Byte 0][Data Byte 1]...[Data Byte n]``

I2C Master writes to I2C Slave followed by I2C Master read from I2C Slave. This means that the I2C Master first writes to the I2C Slave followed by the I2C Master requesting data from the I2C Slave. This a combination of the above two.
The purpose of this is that we first write to the I2C Slave and then the I2C Slave prepares its data for when the I2C Master request data from it.  
**I2C Master -> I2C Slave**  
``[Command Code Byte][Data Byte 0][Data Byte 1]...[Data Byte n]``  
**I2C Master <- I2C Slave**  
``[Data Byte 0][Data Byte 1]...[Data Byte n]``

		
## Configuring Your I2C Slave Device
The I2CSlave library is designed to aid in configuring your I2C Slave device. It encapsulates the communication layer. Thus, you will primary focus on the behavior of your I2C Slave device.

The two functions you will use are:
```
int init(uint8_t slave_address);
```
##### Description
Initiates the library with the passed in slave address.
##### Parameters
**slave_address**: the slave address you are setting this I2C slave device to be.
##### Returns
1: Successfully initialized this library  
-1: Slave address out of range. Failed to initialize.
```
int register_command_code(uint8_t command_code, cc_func func, bool enable_CRC, uint8_t read_size = 0, uint8_t write_size = 0);
```
##### Description
Registers your function that will be trigger by the command code sent by the I2C master.
##### Parameters
**command_code**: arbitrary value that will call your function **func**.  
**func**: function that is called when the command_code packet is sent. **func** should have the function signature of: ``void function(unsigned char* data_read, unsigned char* data_write)``  
**enable_CRC**: true to enable CRC packet error handling. false to disable.  
**read_size**: number of bytes that will be sent from the I2C master to this device (slave).  
**write_size**: number of bytes that will be sent from this device (slave) to the I2C master.
##### Returns
1: Successfully registered your command code.  
-1: The library is not initialized. Call ``init(uint8_t slave_address)`` before using the register command code function.  
-2: Cannot add anymore command codes due to memory limitations. **Limit is 14**. Unexpected behavior may occur if too much RAM is used even before this limit is reached.  
-3: Command code has already been registered.  
-4: read size is too large and will overflow read array.  
-5: write size is too large and will overflow write array.

The ``init`` function will initialize the library and configure the device to be that slave address you passed in. ``init`` should only be called once. Ideally in the ``setup()`` of your sketch.

The ``register_command_code`` function can only be used after initializing the library with the ``init`` function. This registers your function with an accompanying command code value. 

Command code values are arbitrary and don't have any meaning. They are simply a value used to pair with your function call.  
The ``register_command_code`` function takes:
* Your command code value between 0-255.
* Your function you wish to call when that command code arrives. The function must have the function signature of:  
``void function(unsigned char* data_read, unsigned char* data_write)``
* true or false to enable CRC checking. CRC checking allows for checking if data corruption has occured. This can be used if you want to ensure the incoming data is not corrupted.
* The number of additional bytes that will be written from I2C Master to I2C Slave for this command code
* The number of bytes that will be requested from I2C Master from the I2C Slave for this command code

An example below will show how to set up both I2C Slave and I2C Master.

## Configuring Your I2C Master Device

The MagzorI2C library is designed to aid in confguring your I2C Master to communicate with your I2C Slave device. It encapsulates the communication layer. Thus, you will primary focus on creating function calls to your I2C Slave device.

You will have to inherit from an Abstract class which will be the basis for your I2C Slave device definition. To learn more about class inhertiance and Abstract classes check out this [tutorial](http://www.cplusplus.com/doc/tutorial/inheritance/). You can also directly access the ``write`` and ``read`` I2C functions from **I2C_Interface** class.  
Once you have inherited from the AbstractI2CDevice, you will have access to I2C communication functions:
```	
int write(uint8_t* w, uint8_t w_size);
```
##### Description
This function writes a number of bytes from an array to the I2C Address defined in the constructor of this object.
##### Parameters
**w**: an array of bytes that will be written to the I2C Slave.  
**w_size**: the number of bytes to write to the I2C Slave from **w**.
##### Returns
Number of bytes written

```		
int read(uint8_t* r, uint8_t r_size);
```
##### Description
This function reads a number of bytes from the I2C Address defined in the constructor of this object into an array.
##### Parameters
**r**: an array of bytes that will store the requested data from the I2C Slave.  
**r_size**: the number of bytes to request from the I2C Slave which will be stored in **r**.
##### Returns
Number of bytes read
```
int write_read(uint8_t* w, uint8_t w_size, uint8_t* r, uint8_t r_size, uint16_t microsecond_wr_delay = 0);
```
##### Description
This function performs a ``write`` followed by a ``read``. There is also a defined delay time in microseconds if the device requires time to prepare for the requested data.
##### Parameters
**w**: an array of bytes that will be written to the I2C Slave.  
**w_size**: the number of bytes to write to the I2C Slave from **w**.  
**r**: an array of bytes that will store the requested data from the I2C Slave.  
**r_size**: the number of bytes to request from the I2C Slave which will be stored in **r**.  
**microsecond_wr_delay**: the number of microseconds to wait between the write and read operations. This allows some time for the I2C Slave to prepare for the requested data.
##### Returns
0

The ``write`` function will write a number of bytes as defined by the **w_size** parameter from the **w** array to the I2C Address defined in the object during instantiation. We want to follow the format as defined in the "Configuring Your I2C Slave Device."  
The first byte should be a command code followed by any additional data bytes.  
**I2C Master -> I2C Slave**  
``[Command Code Byte][Data Byte 0][Data Byte 1]...[Data Byte n]``
	
The ``read`` function will read a number of bytes as defined by the **r_size** parameter from the I2C Address and store it into the **r** array. From here, you can retrieve the read bytes from the **r** array.  
**I2C Master <- I2C Slave**  
``[Data Byte 0][Data Byte 1]...[Data Byte n]``
		
The ``write read`` function will perform a ``write`` then a ``read`` with a microsecond delay time between the two. This delay allows for the I2C Slave device to prepare its data before requesting it during the read phase.  
This function reduces the need to call ``write`` followed by the ``read`` since it self contained. On the Raspberry Pi it also protects against other threads within your program trying to access the I2C bus.  
**I2C Master -> I2C Slave**  
``[Command Code Byte][Data Byte 0][Data Byte 1]...[Data Byte n]``  
**I2C Master <- I2C Slave**  
``[Data Byte 0][Data Byte 1]...[Data Byte n]``

An example below will show how to set up both I2C Slave and I2C Master.
	
## Communication Overview
		
For your class on your I2C Master you want to define functions that will use the ``write`` and ``read`` fuctions to communicate with your I2C Slave device. You will have to come up with the command code (**don't reuse multiple times**) and if any additional data you wish to send. In the corresponding end on the I2C Slave side. The I2C Slave will read in the command code and any additional data you send and look for which function it will call based on your configuration.  
Your function, when called can do whatever it wishes to and it can access the additional data from the array that is passed to it as a function parameter ( **data_read** ). Your function can return data by modifying the array that is passed to it as a function parameter ( **data_write** ).  
There are limitations to the **data_write** and **data_read** arrays. They are not unlimited in size. **They are 16 bytes in size each**. The table below shows the usable bytes of the arrays in different modes.

| Array | Size of Array | Used Bytes (without CRC) | Usable Bytes (without CRC)
|:-|:-|:-|:-
| data_read  | 16 | 1 (Command Code) | 15     
| data_write | 16| 0 | 16  

| Array | Size of Array | Used Bytes (with CRC) | Usable Bytes (with CRC)
|:-|:-|:-|:-
| data_read  | 16 | 1 (Command Code) + 2 (CRC) | 13     
| data_write | 16 | 2 (CRC read) + 2 (CRC write) | 14  

**Note:** This guide just covers "without CRC."

Within the **data_read** array
* **data_read[0]**: the command code
* **data_read[1]**: additional data byte 0
* **data_read[2]**: additional data byte 1
* ...
* **data_read[n]**: additional data byte n-1
	
Within the **data_write** array
* **data_write[0]**: requested data byte 0
* **data_write[1]**: requested data byte 1
* ...
* **data_write[n]**: requested data byte n
		
##### The following break down is how things work:
1. **On I2C Master**
    * A user function is called that contains an I2C Slave device ``write``.
    * The user function generates the array that contains the command code and any additional data.
    * The user function then calls the ``write`` function and passes it the array and the size of the array to be written to the I2C Slave address.
        * If also requesting data. The user function calls the ``write read`` function and passes it the write array, write size, read array, and read size in addition to the delay time in microseconds between ``write`` and ``read``.
    * The ``write`` function performs the write to the I2C Slave device.

2. **On the I2C Slave device**
    * The device receives the data and calls your function based on the command code.
    * Your function is passed two parameters, the **read_array** and the **write_array**. Your function should know the **read_array** and **write_array** size based on your definition.
        * The **read_array** contains bytes sent from the I2C Master.
        * The **write_array** contains bytes that will be sent to the I2C Master, you are free to modify this array and the I2C Master will receive those bytes when requested.
    * Your function does something and optionally uses the **read_array** for data or modifies the **write_array** to return data when requested.

3. **On I2C Master (if I2C Master will request data from the I2C Slave)**
    * The ``write read`` function delays based on the value passed. This allows for the I2C Slave to prepare for the data that will be requested.
    * The ``write read`` function performs a ``read`` from the I2C Slave device.
    * The data is placed into the **read_array** and returns back to the original calling user function.
    * The user function can do what it wishes with the **read_array**.

[Image]

## Example

For this example we will create an I2C Slave device that will accept the 4 different type of communication packets:
* I2C Master writes a command to I2C Slave.
* I2C Master writes a command with additional data to I2C Slave.
* I2C Master writes a command followed by a request of data from the I2C Slave.
* I2C Master writes a command with additional data followed by a request of data from the I2C Slave.

**function1** will illustrate "I2C Master writes a command to I2C Slave."  
**function2** will illustrate "I2C Master writes a command with additional data to I2C Slave."  
**function3** will illustrate "I2C Master writes a command followed by a request of data from the I2C Slave."  
**function4** will illustrate "I2C Master writes a command with additional data followed by a request of data from the I2C Slave."

**function1**  
I2C Master will send a command code to the I2C Slave. No additional data is sent or requested. The I2C Slave will turn on an LED connected to P1.0.
* **I2C Master -> I2C Slave**  
``[Command Code Byte = 0x81]``

**function2**  
I2C Master will send a command code and an additional 3 bytes to I2C slave. No data is requested from the slave. The I2C Slave will turn off the LED connected to P1.0 if the first additional byte is an even number. If it is odd, the LED will be left as. The additional 3 bytes of data will be stored on the I2C Slave device and be returned if function4 is called.
* **I2C Master -> I2C Slave**  
``[Command Code Byte = 0x82][Data Byte 0][Data Byte 1][Data Byte 2]``

**function3**  
I2C Master will send a command code to the I2C Slave. No additional data is sent, but 1 byte of data will be requested. The I2C Slave will prepare 1 byte of data corresponding to the the number of times this command has been called since the I2C Slave device has started up. Therefore, calling function3 will send an incrementally increasing value.
* **I2C Master -> I2C Slave**  
``[Command Code Byte = 0x83]``
* **I2C Master <- I2C Slave**  
``[Data Byte 0]``

**function4**  
I2C Master will send a command code an an additional 1 byte to I2C slave. (The additional 1 byte send from the I2C Master is unused because the author of the guide was too lazy to come up with a way of using it but still wanted to demostrate that sending additional data is possible in this packet.) 3 bytes of data will also be requested. The I2C Slave will prepare 3 bytes of data corresponding to the values sent during a previous function2 packet. If no function2 call has been made it will send the default 0xFF for those 3 bytes. 
* **I2C Master -> I2C Slave**  
``[Command Code Byte = 0x84][Data Byte 0]``
* **I2C Master <- I2C Slave**  
``[Data Byte 0][Data Byte 1][Data Byte 2]``
### I2C Slave Code
Copy and paste the code below into a new sketch in Energia. Ensure you target the MSP430G2553. Press upload to upload the firmware to the microcontroller.
```
#include <Wire.h>
#include <stdint.h>
#include <I2C_Packet_Handler.h>
#include <Command_Code_Function.h>
#include <I2C_CRC.h>
#include <I2C_Data_Converter.h>
#include <msp430g2553.h>

//The I2C Address of this device
const uint8_t I2C_ADDRESS = 0x04;

//Command code values for our four functions
const uint8_t COMMAND_CODE_FUNCTION1 = 0x81;
const uint8_t COMMAND_CODE_FUNCTION2 = 0x82;
const uint8_t COMMAND_CODE_FUNCTION3 = 0x83;
const uint8_t COMMAND_CODE_FUNCTION4 = 0x84;

//Some variables used to store 3 bytes of data from function2 and used in function4
volatile uint8_t func2_databyte0 = 0xff;
volatile uint8_t func2_databyte1 = 0xff;
volatile uint8_t func2_databyte2 = 0xff;

//Variable used to keep track of the number of times function3 is called
volatile uint8_t func3_callcount = 0;

void setup()
{
  //Initialize I2C_packet_handler, which also initializes this device as a slave on slave_address
  I2C_packet_handler.init(I2C_ADDRESS);
  
  //Register command codes with their callback functions
  //I2C_packet_handler.register_command_code(uint8_t command_code, cc_func func, bool enable_CRC, uint8_t read_size, uint8_t write_size);
  
  //Register "COMMAND_CODE_FUNCTION1" to callback function "function1" with CRC disabled. Read 0 additional bytes from I2C Master, write 0 bytes back to I2C Master
  I2C_packet_handler.register_command_code(COMMAND_CODE_FUNCTION1, function1, false, 0, 0);
  
  //Register "COMMAND_CODE_FUNCTION2" to callback function "function2" with CRC disabled. Read 3 additional bytes from I2C Master, write 0 bytes back to I2C Master
  I2C_packet_handler.register_command_code(COMMAND_CODE_FUNCTION2, function2, false, 3, 0);
  
  //Register "COMMAND_CODE_FUNCTION3" to callback function "function3" with CRC disabled. Read 0 additional bytes from I2C Master, write 1 bytes back to I2C Master
  I2C_packet_handler.register_command_code(COMMAND_CODE_FUNCTION3, function3, false, 0, 1);
  
  //Register "COMMAND_CODE_FUNCTION4" to callback function "function4" with CRC disabled. Read 1 additional bytes from I2C Master, write 3 bytes back to I2C Master
  I2C_packet_handler.register_command_code(COMMAND_CODE_FUNCTION4, function4, false, 1, 3);
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(100); 
}

//function that is called when "COMMAND_CODE_FUNCTION1" packet arrives
//This function turns on the LED on P1.0 to indicate that the packet has been received
//data_read[0] contains the command_code, which is COMMAND_CODE_FUNCTION1
void function1(unsigned char* data_read, unsigned char* data_write){
  pinMode(P1_0, OUTPUT);
  digitalWrite(P1_0, HIGH);
}

//function that is called when "COMMAND_CODE_FUNCTION2" packet arrives
//This function turns off LED on P1.0 if the sum of the 3 data bytes are an even number
//It also stores the bytes to return to in function4
//data_read[0] contains the command_code, which is COMMAND_CODE_FUNCTION2
//data_read[1] contains additional data byte 0
//data_read[2] contains additional data byte 1
//data_read[3] contains additional data byte 2
void function2(unsigned char* data_read, unsigned char* data_write){
  //Store the bytes to future use
  func2_databyte0 = data_read[1];
  func2_databyte1 = data_read[2];
  func2_databyte2 = data_read[3];
  
  //Check if the first additional byte is even
  if( (data_read[1] % 2) == 0 ){
    //Turn off LED if even
    pinMode(P1_0, OUTPUT);
    digitalWrite(P1_0, LOW); 
  }
}

//function that is called when "COMMAND_CODE_FUNCTION3" packet arrives
//This function sets up the data_write array for the master to request 1 byte of value of the number of times this function is called
//ie. the first time it is called it will return 1, second time 2, third time 3... etc
//data_read[0] contains the command_code, which is COMMAND_CODE_FUNCTION3
//data_write[0] first value that will be sent to I2C Master when requested
void function3(unsigned char* data_read, unsigned char* data_write){
  //Increment the number of times this function has been called
  func3_callcount ++;
  
  //Prepare data for read request from the I2C Master
  data_write[0] = func3_callcount;
}

//function that is called when "COMMAND_CODE_FUNCTION4" packet arrives
//This function sets up the data_write array for the master to request 3 bytes which were read from func2, or it just sends back 0xFF if function2 was never used.
//data_read[0] contains the command_code, which is COMMAND_CODE_FUNCTION4
//data_write[0] contains requested data byte 0
//data_write[1] contains requested data byte 1
//data_write[2] contains requested data byte 2
void function4(unsigned char* data_read, unsigned char* data_write){
  //Prepare data for read request from the I2C Master
  data_write[0] = func2_databyte0;
  data_write[1] = func2_databyte1;
  data_write[2] = func2_databyte2;
}
```
### I2C Master Code for Raspberry Pi
**My_I2CDevice.h**
```
//File: My_I2CDevice.h
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
```
**My_I2CDevice.cpp**
```
//File: My_I2CDevice.cpp
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
```
**main.cpp**
```
//File: main.cpp
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
```
**Compile Command**:  
``g++ -Wall -lwiringPi -lMagzorI2C -DRASPBERRYPI My_I2CDevice.h My_I2CDevice.cpp main.cpp -o a.out``  
**Run Command**:  
``sudo ./a.out``

### I2C Master Code for Arduino Uno
**My_I2CDevice.h**

**My_I2CDevice.cpp**

**sketch.cpp**
