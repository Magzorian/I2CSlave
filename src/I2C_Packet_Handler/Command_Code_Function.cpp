#include "Command_Code_Function.h"

Command_Code_Function::Command_Code_Function(){
}

Command_Code_Function::Command_Code_Function(uint8_t command_code, cc_func func, bool enable_CRC, uint8_t read_size, uint8_t write_size){
	this->command_code = command_code;
	this->func = func;
	this->enable_CRC = enable_CRC;
	this->read_size = read_size;
	this->write_size = write_size;
}

uint8_t Command_Code_Function::get_command_code(){
	return command_code;
}

cc_func Command_Code_Function::get_func(){
	return func;
}

bool Command_Code_Function::get_enable_CRC(){
	return enable_CRC;
}

uint8_t Command_Code_Function::get_read_size(){
	return read_size;
}

uint8_t Command_Code_Function::get_write_size(){
	return write_size;
}


