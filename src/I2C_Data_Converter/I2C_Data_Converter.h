#ifndef _I2C_DATA_CONVERTER_
#define _I2C_DATA_CONVERTER_

#include <stdint.h>

class I2C_Data_Converter {
	private:
		
	protected:

	public:
		static void put_unsigned_char(unsigned char data, unsigned char* array, uint8_t index);
		static unsigned char get_unsigned_char(unsigned char* array, uint8_t index);
		
		static void put_bool(bool data, unsigned char* array, uint8_t index);
		static bool get_bool(unsigned char* array, uint8_t index);
		
		static void put_uint16_t(uint16_t data, unsigned char* array, uint8_t starting_index);
		static uint16_t get_uint16_t(unsigned char* array, uint8_t starting_index);
};

#endif