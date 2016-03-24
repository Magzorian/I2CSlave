//Default command codes functions

void command_code_test(unsigned char* data_read, unsigned char* data_write){
	data_write[0] = 0x01;
}

void command_code_module_id(unsigned char* data_read, unsigned char* data_write){
	I2C_Data_Converter::put_uint16_t(MODULE_ID, data_write, 0);
}

void command_code_version(unsigned char* data_read, unsigned char* data_write){
	I2C_Data_Converter::put_uint16_t(MODULE_VERSION, data_write, 0);
}

void command_code_reset(unsigned char* data_read, unsigned char* data_write){
	//Forces a software reset
	WDTCTL = 0;  
}
