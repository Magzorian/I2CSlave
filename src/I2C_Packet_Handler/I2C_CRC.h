#ifndef _I2C_CRC_
#define _I2C_CRC_

#include <stdint.h>

const uint8_t CRC_BYTES_SIZE = 2;

class I2C_CRC {
	private:
		static unsigned int getCRC(unsigned char *data, uint8_t length);
		static void putCRC_into_array(unsigned crc, unsigned char *write_array, uint8_t start_index);

	protected:
	
	public:
		static void putCRC(unsigned char *data, uint8_t data_start_index, uint8_t length, unsigned char *write_array, uint8_t crc_start_index);
		static bool check_equal_CRC(unsigned char *crc_array1, unsigned char *crc_array2);

};

#endif