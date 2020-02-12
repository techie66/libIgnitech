#include "ignitech.h"

int IGNITECH::read (ignitech_t& ignitech_data ) 
{
	// TODO
	ignitech_data.rpm=500;
	ignitech_data.battery_dV=141;
	ignitech_data.map_mV=497;
	ignitech_data.map_kpa=56;

	return 0;
}

int IGNITECH::setup( char* file)
{
	/* 
	Open the given tty
	set proper attributes
	return the file descriptor as well as save it for internal use
	*/

	int fd;
	fd = open (file, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		//error_message (WARN,"error %d opening %s: %s", errno, LC2_PORT, strerror (errno));
		return -1;
	}

	// Set interface parameters for front controls
	set_interface_attribs (fd, B19200, 0);  // set speed to 19,200 bps, 8n1 (no parity)
	set_blocking (fd, 0);                // set no blocking

	IGNITECH::file_descriptor = fd;
	return fd;
}
