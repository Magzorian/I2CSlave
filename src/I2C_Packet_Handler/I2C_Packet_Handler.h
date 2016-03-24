#ifndef _I2C_Packet_Handler_
#define _I2C_Packet_Handler_

#include <Wire.h>
#include "Command_Code_Function.h"
#include "I2C_CRC.h"
#include <stdint.h>
#include <stdlib.h>


const size_t READ_BUFFER_SIZE = 16;
const size_t WRITE_BUFFER_SIZE = 16;
const size_t MAX_ARRAY_COUNT = 32;
const uint8_t DEFAULT_ARRAY_COUNT = 14;
const uint8_t CRC_READ_BYTES_SIZE = 2;
const uint8_t CRC_WRITE_BYTES_SIZE = 2;
const uint8_t COMMAND_CODE_BYTE_SIZE = 1;
const uint8_t ARR_READ_START_INDEX = 1;
const uint8_t MAX_SLAVE_ADDRESS = 0x80;
const uint8_t ARR_POS_COMMAND_CODE = 0;

class I2C_Packet_Handler {
	private:
		//If this is initialized or not
		bool initialized;
		
		//Master crc disabler
		//Used if the configurer wants to disable CRC because of an unsupported device or other reason
		//This flag can be set via an I2C command
		bool super_CRC_disable;
	
		//Slave address of this device
		uint8_t slave_address;
	
		//Read array contains data sent from master to slave (this device)
		unsigned char data_read_array[READ_BUFFER_SIZE];
		
		//Write array contains data to be sent to master from slave (this device)
		unsigned char data_write_array[WRITE_BUFFER_SIZE];
		
		//Number of bytes to write with master requests data
		uint8_t data_write_size;
		
		//Is a burnt apple
		char apple;
		
		//Current max size of the array
		size_t array_max_size;
		
		//Number of used array slots
		uint8_t array_used_size;
		
		//Array that contains the Command_Code_Function objects
		//Command_Code_Function * command_code_function_array;
		Command_Code_Function command_code_function_array[DEFAULT_ARRAY_COUNT];
		
		//Reads in the specified number of bytes from I2C
		void readI2C(uint8_t count, uint8_t starting_index = 0);
		
		//Finds the index of the function with the specific command_code and returns the index from that array
		int find_command_code_function_index(uint8_t command_code);
		
		//Runs the Command_Code_Function
		int run_command_code(uint8_t index);
		
		//I2C_CRC object that handles crc checking
		I2C_CRC i2c_crc;
	
	protected:
		
		
	public:
		int init(uint8_t slave_address);
		int register_command_code(uint8_t command_code, cc_func func, bool enable_CRC, uint8_t read_size = 0, uint8_t write_size = 0);
		
		static void receiveEvent_Wrapper(int num_bytes);
		static void requestEvent_Wrapper();
		
		void receiveEvent(int num_bytes);
		void requestEvent();
};

extern I2C_Packet_Handler I2C_packet_handler;

#endif