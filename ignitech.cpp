#include "ignitech.h"

const unsigned char IGNITECH_QUERY[102] = {0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x58,0x97};

void IGNITECH::initialize() {
	ignition.rpm=0;
	ignition.battery_mV=0;
	ignition.map_mV=0;
	ignition.map_kpa=0;
	file_descriptor=-1;
}

/*
	Synchronously reads from the controller
	First it sends an initialization string
	then it reads back the response
	If it reads short, or writes short it will return return -1
*/

int IGNITECH::read_sync (ignitech_t& ignitech_data ) {
	unsigned char buf[102];

	// Query controller for status
	int b_written = write(file_descriptor,IGNITECH_QUERY,102);
	// check for success...
	if (b_written == 102) {
		// Good !
		// read response data / check validity
		int b_read = read(file_descriptor,buf,102);
		if (b_read == 102) {
			// Good !
		}
		else {
			return -1 * b_read;
		}
	}
	else {
		return -1;
	}

	ignitech_data.rpm = buf[2] + buf[3] * 0x100u;
	ignitech_data.map_mV = buf[4] + buf[5] * 0x100u;
	ignitech_data.battery_mV = buf[6] + buf[7] * 0x100u;
	ignitech_data.map_kpa = buf[22] + buf[23] *0x100u;

	return 0;
}

/*
	Reads synchronously into internal data structure
*/
int IGNITECH::read_sync() {
	return read_sync(ignition);
}

/*
	Constructor that takes pathname to device file
	Opens given pathname and sets attributes
	as necessary. Initializes variables.
*/
IGNITECH::IGNITECH( const char* file) {
	int fd;
	fd = open (file, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		perror("Error opening Ignitech device:");
		file_descriptor = fd;
		return;
	}

	// Set interface parameters for front controls
	set_interface_attribs (fd, B19200, 0);  // set speed to 19,200 bps, 8n1 (no parity)
	set_blocking (fd, 0);                // set no blocking

	file_descriptor = fd;
	return;
}

/*
	Simple Getter
*/
int IGNITECH::get_rpm() {
	return ignition.rpm;
}

/*
	Simple Getter
*/
int IGNITECH::get_map_kpa() {
	return ignition.map_kpa;
}

/*
	Simple Getter
*/
int IGNITECH::get_map_mV() {
	return ignition.map_mV;
}

/*
	Simple Getter
*/
int IGNITECH::get_battery_mV() {
	return ignition.battery_mV;
}

