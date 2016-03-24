//Define device functionality here

//Setup for this device
void device_setup(){

}

//Register command codes for this device
void add_device_command_codes(){
	//Register command codes, CRC enabled, and read and write packet sizes
	//Device specific command code registered
	//More used, higher priority ones should be at the top of the list
	//I2C_packet_handler.register_command_code(uint8_t command_code, cc_func func, bool enable_CRC, uint8_t read_size, uint8_t write_size);
	I2C_packet_handler.register_command_code(COMMAND_CODE_1, test_func1, true, 0, 1);
	I2C_packet_handler.register_command_code(COMMAND_CODE_2, test_func2, true, 1, 2);
}

void test_func1(unsigned char* data_read, unsigned char* data_write){
	//Echoes back the command code
	data_write[0] = data_read[0];
}

void test_func2(unsigned char* data_read, unsigned char* data_write){
	//Echoes back the command code and the 1 byte of data sent along
	data_write[0] = data_read[0];
	data_write[1] = data_read[1];
}