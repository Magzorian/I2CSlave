#ifndef _COMMAND_CODE_FUNCTION_
#define _COMMAND_CODE_FUNCTION_

#include <stdint.h>

typedef void (*cc_func)(unsigned char* data_read, unsigned char* data_write);

class  Command_Code_Function {
	private:
		uint8_t command_code;
		cc_func func;
		bool enable_CRC;
		uint8_t read_size;
		uint8_t write_size;
	protected:

	public:
		Command_Code_Function();
		Command_Code_Function(uint8_t command_code, cc_func func, bool enabled_CRC, uint8_t read_size = 0, uint8_t write_size = 0);
		uint8_t get_command_code();
		cc_func get_func();
		bool get_enable_CRC();
		uint8_t get_read_size();
		uint8_t get_write_size();
		
};

#endif