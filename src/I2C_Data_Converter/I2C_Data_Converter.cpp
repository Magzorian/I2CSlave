#include "I2C_Data_Converter.h"

void I2C_Data_Converter::put_unsigned_char(unsigned char data, unsigned char* array, uint8_t index){
	array[index] = data;
}

unsigned char I2C_Data_Converter::get_unsigned_char(unsigned char* array, uint8_t index){
	return array[index];
}

void I2C_Data_Converter::put_bool(bool data, unsigned char* array, uint8_t index){
	if(data){
		array[index] = 0x01;
	}
	else{
		array[index] = 0x00;
	}
}

bool I2C_Data_Converter::get_bool(unsigned char* array, uint8_t index){
	if(array[index] == 0x01){
		return true;
	}
	else{
		return false;
	}
}

void I2C_Data_Converter::put_uint16_t(uint16_t data, unsigned char* array, uint8_t starting_index){
	array[starting_index] = (data >> 8) & 0xFF; 
	array[starting_index+1] = data & 0xFF;  
}

uint16_t I2C_Data_Converter::get_uint16_t(unsigned char* array, uint8_t starting_index){
	return ((array[starting_index] & 0xFF) << 8) | array[starting_index + 1];
}
		
