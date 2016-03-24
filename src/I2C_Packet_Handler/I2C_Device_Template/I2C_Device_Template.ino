#include <Wire.h>
#include <stdint.h>
#include <I2C_Packet_Handler.h>
#include <Command_Code_Function.h>
#include <I2C_CRC.h>
#include <I2C_Data_Converter.h>

#include <msp430g2553.h>

#include "command_code_definitions.h"
#include "pin_definitions.h"

//Module id, defines the type of module
const unsigned int MODULE_ID = 1046;

//Defines the version of the module        
const unsigned int MODULE_VERSION = 1;

//Defines the base address for this device
const uint8_t BASE_I2C_ADDRESS = 0x04;

void setup(){
	//Call the device setup function
	device_setup();

	//Put flag pin to input, so it doesn't cause problems with flag line
	pinMode(FLAG_PIN, INPUT);  

	//Set up address pins for input
	pinMode(ADDRESS0_PIN, INPUT_PULLDOWN);
	pinMode(ADDRESS1_PIN, INPUT_PULLDOWN);
	pinMode(ADDRESS2_PIN, INPUT_PULLDOWN);
	
	//Add delay pins voltage to settle
	delay(1);
	
	//Grab base address
	uint8_t slave_address = BASE_I2C_ADDRESS;

	//Change slave address value based on which address pins are high
	if(digitalRead(ADDRESS0_PIN) == HIGH){
		slave_address += 1;
	}
	if(digitalRead(ADDRESS1_PIN) == HIGH){
		slave_address += 2;
	}
	if(digitalRead(ADDRESS2_PIN) == HIGH){
		slave_address += 4;
	}
	
	//Initialize I2C_packet_handler, which also initializes this device as a slave on slave_address
	I2C_packet_handler.init(slave_address);
	
	//Call function to add the device's command codes
	add_device_command_codes();

	//Default command codes registered
	//Lesser used therefore they are at the end of the list
	I2C_packet_handler.register_command_code(COMMAND_CODE_TEST, command_code_test, false, 0, 1);
	I2C_packet_handler.register_command_code(COMMAND_CODE_MODULE_ID, command_code_module_id, true, 0, 2);
	I2C_packet_handler.register_command_code(COMMAND_CODE_MODULE_VERSION, command_code_version, true, 0, 2);
	I2C_packet_handler.register_command_code(COMMAND_CODE_MODULE_RESET, command_code_reset, true, 0, 0);
}

void loop(){
	delay(100);
}

