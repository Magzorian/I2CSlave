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