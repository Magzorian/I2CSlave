#include "I2C_Packet_Handler.h"
#include "Energia.h"

I2C_Packet_Handler I2C_packet_handler;

int I2C_Packet_Handler::init(uint8_t slave_address){
	//Return value stays at 1 if there is no problems. :)
	int retval = 1;
	
	if(slave_address > MAX_SLAVE_ADDRESS){
		initialized = false;
		
		//Slave address is above possible range, send return code -1
		return -1;
	}
	
	this->slave_address = slave_address;
	
	//If number of command codes is greater than the MAX_ARRAY_COUNT (currently 32), then set max array size to MAX_ARRAY_COUNT
	//if(num_command_codes > MAX_ARRAY_COUNT){
	//	array_max_size = MAX_ARRAY_COUNT;
	//	
	//	//Return 2 if cannot set max number of command codes correctly.
	//	retval = 2;
	//}
	//Else set number to the request amount
	//else{
	//	array_max_size = num_command_codes;
	//
	
	array_max_size = DEFAULT_ARRAY_COUNT;
	
	// This device to join I2C bus as slave with address #4
	Wire.begin(slave_address);
	
	// Register function call for when data is received from I2C Master
	Wire.onReceive(I2C_Packet_Handler::receiveEvent_Wrapper);
	
	// Register function call for when data is requested to I2C Master	
	Wire.onRequest(I2C_Packet_Handler::requestEvent_Wrapper);
	
	//Alloc memory for storing Command_Code_Function objects
	//Something about casting the return of malloc can cause a problem. But need to cast or else won't compile.
	//command_code_function_array = (Command_Code_Function*) malloc(sizeof(Command_Code_Function) * array_max_size);
	
	//Nothing in the array is currently being used.
	array_used_size = 0;
	
	//super crc disabled, is not enabled by default. This is settable by a command over I2C
	super_CRC_disable = false;
	
	initialized = true;
	
	return retval;
}

//Wrapper for registering with the Wire.h class because pointer to object member function problems
void I2C_Packet_Handler::receiveEvent_Wrapper(int num_bytes){
	noInterrupts();
	I2C_packet_handler.receiveEvent(num_bytes);
	interrupts();
}

//Wrapper for registering with the Wire.h class because pointer to object member function problems
void I2C_Packet_Handler::requestEvent_Wrapper(){
	noInterrupts();
	I2C_packet_handler.requestEvent();
	interrupts();
}

void I2C_Packet_Handler::receiveEvent(int num_bytes){
	//Moved from requestEvent to receiveEvent
	//Start of a new transmission causes the write buffer to the master to be reset
	data_write_size = 0;

	//Check is the packet is not empty
	if(num_bytes > 0){        
		//Read in the command_code
		uint8_t current_command_code = Wire.read();
		//Store the command code in the first spot in the array
		data_read_array[ARR_POS_COMMAND_CODE] = current_command_code;
		
		//Find the index to the command code function object
		//int command_code_index = find_command_code_function_index(current_command_code);
		int command_code_index = find_command_code_function_index(current_command_code);
		
		//If the command code function exists, then run the command code
		if(command_code_index != -1){
			run_command_code(command_code_index);
		}
		
		//Clear out remaining bytes, prevents I2C from locking up
		while(Wire.available() > 0){
			Wire.read();
		}  
	}
}

void I2C_Packet_Handler::requestEvent(){
	//Write data to the I2C Master. data_write_array contains data bytes to write
	//data_write_size contains the number of bytes to write
	Wire.write(data_write_array, data_write_size);
	
	//After writing data, set the data_write_size to 0
	data_write_size = 0;	
	
	data_write_array[0] = 0;
	data_write_array[1] = 0;
	data_write_array[2] = 0;
	data_write_array[3] = 0;
	
	//Clear out array
	//memset(data_write_array, 0, sizeof(data_write_array));

}

void I2C_Packet_Handler::readI2C(uint8_t count, uint8_t starting_index){
	//Loop that reads in bytes based on the count value
	for(uint8_t i = starting_index; i < count + starting_index; ++i){
		//Read the available byte into the data_read_array
		data_read_array[i] = Wire.read();
	} 
}

int I2C_Packet_Handler::register_command_code(uint8_t command_code, cc_func func, bool enable_CRC, uint8_t read_size, uint8_t write_size){
	int retval = 1;
	
	//Only allow registering a function if I2C_Packet_Handler is initialized
	if(initialized){
		//Check if we have room to add another command code
		if(array_used_size > array_max_size){
			//Return -2 if we cannot add any more command codes due to array limits
			retval = -2;
		}
		//Check if command code value already exists.
		else if(find_command_code_function_index(command_code) != -1){
			//Return -3 if the command code value already exists
			retval = -3;
		}
		//Check if with CRC enabled that read_size is within limits of the buffer
		else if( enable_CRC && ((COMMAND_CODE_BYTE_SIZE + read_size + CRC_READ_BYTES_SIZE) > READ_BUFFER_SIZE) ){
			//Return -4 if the read_size is going to overflow the read buffer array
			return -4;
		}
		//Check if with CRC enabled that the write_size is within limits of the buffer
		else if(	enable_CRC && ((CRC_READ_BYTES_SIZE + write_size + CRC_WRITE_BYTES_SIZE) > WRITE_BUFFER_SIZE)  ){
			//Return -5 if the write_size is going to overflow the write buffer array
			return -5;
		}
		//Finally, passed all tests to be able to add command code function
		else{
			//Add command code function
			command_code_function_array[array_used_size] = Command_Code_Function(command_code, func, enable_CRC, read_size, write_size);
			//Increase array used size by 1
			array_used_size ++;
		}
	}
	else{
		//Return -1 if I2C_Packet_Handler is not initialized.
		retval = -1;
	}
	
	return retval;
}

//Searches for the index of the specified command_code. Returns -1 if it doesn't exist.
int I2C_Packet_Handler::find_command_code_function_index(uint8_t command_code){
	for(unsigned int i = 0; i < array_used_size; ++i){
		if(command_code == command_code_function_array[i].get_command_code()){
			return i;
		}
	}
	return -1;
}

int I2C_Packet_Handler::run_command_code(uint8_t index){
	//Get the amount of bytes to read from the object
	uint8_t read_amount = command_code_function_array[index].get_read_size();
	//Get the amount of bytes to write from the object
	uint8_t write_amount = command_code_function_array[index].get_write_size();
	//Grab the enable CRC flag. If super_CRC_disable is enabled, then this will also disable CRC.
	bool CRC_enabled = command_code_function_array[index].get_enable_CRC() || super_CRC_disable;
	
	
	if(CRC_enabled){
		//If CRC is enabled, read in two additional bytes because of the CRC values.
		readI2C(read_amount + CRC_READ_BYTES_SIZE, ARR_READ_START_INDEX);
		
		//Put crc of the recently read data (includes command code byte) into the data_write_array. This is the slave_CRC
		i2c_crc.putCRC(data_read_array, 0, COMMAND_CODE_BYTE_SIZE + read_amount, data_write_array, 0);
		
		//Check if master_CRC is equal to slave_CRC
		//master_crc is stored in data_read_array offset by --> read_amount + 1 (the command code byte)... and slave_CRC was just saved into the data_write_array
		if(!i2c_crc.check_equal_CRC(data_read_array + (COMMAND_CODE_BYTE_SIZE + read_amount) * sizeof(unsigned char), data_write_array)){
			//CRC check failed. Not running command code function.
			data_write_size = 0;
			return -1;

		}
		
		//Call the user's command code function with offset to data_write_array because of CRC being returned
		(command_code_function_array[index].get_func())(data_read_array, data_write_array + CRC_READ_BYTES_SIZE * sizeof(unsigned char));
	
		
		if(write_amount == 0){
			data_write_size = CRC_READ_BYTES_SIZE;
		}
		else{
			//Put the the slave_CRC of data into the write array
			i2c_crc.putCRC(data_write_array, CRC_READ_BYTES_SIZE, write_amount, data_write_array, CRC_READ_BYTES_SIZE + write_amount);
			data_write_size = CRC_READ_BYTES_SIZE + write_amount + CRC_WRITE_BYTES_SIZE;
		}
	}
	else{
		//Read in the I2C bytes with no CRC enabled
		readI2C(read_amount, ARR_READ_START_INDEX);
		
		//Call the user's command code function
		(command_code_function_array[index].get_func())(data_read_array, data_write_array);
		
		//Set the data write size with no CRC enabled
		data_write_size = write_amount;
	}
	
	
	return 0;
}