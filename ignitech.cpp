/*
    libIgnitech - Library to communicate with Ignitech TCIP-4 ignition controller
    Copyright (C) 2019-2020  Jacob Geigle

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "ignitech.h"

unsigned char const IGNITECH_QUERY[102] = {0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x58,0x97};

void IGNITECH::initialize() {
	ignition.rpm=0;
	ignition.battery_mV=0;
	ignition.map_mV=0;
	ignition.map_kpa=0;
	file_descriptor=-1;
}

/*
	Synchronously reads from the controller
	Call read_async in a loop until sucessful read or error.
*/

int IGNITECH::read_sync (ignitech_t& ignitech_data ) {
	IGN_async_status ret_val = IGN_AGAIN;
	while ( ret_val == IGN_AGAIN ) {
		ret_val = read_async(ignitech_data);
	}
	return ret_val;
}

/*
	Asynchronously reads from the controller
	First it sends an initialization string
	then it reads back the response
	If it reads short, or writes short it will return return -1
*/

IGN_async_status IGNITECH::read_async (ignitech_t& ignitech_data ) {
	time_t const reset_timeout = 1;
	static time_t reset_last_read = time(0);
	static unsigned char buf[IGNITECH_PACKET_SIZE];
	static bool found_header = false;
	static bool sent_header = false;
	static size_t total_read = 0;
	static size_t good_read = 0;
	static size_t total_left = IGNITECH_PACKET_SIZE;

	if ( time(0) > reset_last_read + reset_timeout ) {
		// No response for timeout, reset
		perror ( "IGNITECH::read_async: No response from controller within timeout. Resetting.");
		total_read = 0;
		total_left = IGNITECH_PACKET_SIZE;
		good_read = 0;
		found_header = false;
		sent_header = false;
		buf[0] = 0;
	}

	if ( file_descriptor < 0 ) {
		// Bad file descriptor, reset
		total_read = 0;
		total_left = IGNITECH_PACKET_SIZE;
		good_read = 0;
		found_header = false;
		sent_header = false;
		buf[0] = 0;
	}

	if ( total_read >= IGNITECH_PACKET_SIZE ) {
		// Read more than expected, reset
		total_read = 0;
		total_left = IGNITECH_PACKET_SIZE;
		good_read = 0;
		found_header = false;
		sent_header = false;
		buf[0] = 0;
	}

	if ( !sent_header && total_read == 0 ) {
		// On reset, or after successful reading of packet
		if ( query_device() < 0 )
			return IGN_ERR;
		sent_header = true;
	}
	
	if ( !found_header ) {
		// Search for header
		if ( buf[0] != 0xb0 ) {
			int	select_result = 0,
			max_fd = 0;
			fd_set	readset;

			FD_ZERO(&readset);
			FD_SET(file_descriptor,&readset);
			max_fd = (max_fd>file_descriptor?max_fd:file_descriptor);
			struct timeval	timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			select_result = select(max_fd+1, &readset, NULL, NULL, &timeout);
			if (select_result < 0) {
				perror ( "IGNNITECH::read_async:select");
			}
			else if (select_result == 0){} // Timeout
			else if (select_result > 0) {
				if (FD_ISSET(file_descriptor,&readset)) {
					size_t b_read = read(file_descriptor,buf,1);
					if (b_read <= 0 ) {			// Failure or done
						if (b_read < 0) {		// Failure
							perror("IGNITECH::read_async:header");
							return IGN_ERR;
						}
						return IGN_AGAIN;
					}
					reset_last_read = time(0);
					total_read += b_read;
					sent_header = false;
					if (buf[0] == 0xb0 ) {		// Found header
						found_header = true;
						sent_header = false;
						good_read += 1;
						total_left -= 1;
					}
				}
			}
		}
	}
	if ( found_header ) {
		// read the rest
		if ( total_left > 0 ) {

			int	select_result = 0,
			max_fd = 0;
			fd_set	readset;

			FD_ZERO(&readset);
			FD_SET(file_descriptor,&readset);
			max_fd = (max_fd>file_descriptor?max_fd:file_descriptor);
			struct timeval	timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			select_result = select(max_fd+1, &readset, NULL, NULL, &timeout);
			if (select_result < 0) {
				perror ( "IGNNITECH::read_async:select");
			}
			else if (select_result == 0){} // Timeout
			else if (select_result > 0) {
				if (FD_ISSET(file_descriptor,&readset)) {
					size_t b_read = read(file_descriptor,&buf[good_read],total_left);
					if (b_read <= 0 ) {
						if (b_read < 0) {// Failure or none
							perror("IGNITECH::read_async:packet");
							return IGN_ERR;
						}
						return IGN_AGAIN;
					}
					else {
						reset_last_read = time(0);
						good_read += b_read;
						total_read += b_read;
						total_left -= b_read;
					}
				}
				else
					IGN_AGAIN;
			}
			else
				return IGN_AGAIN;
		}
	}
	else
		return IGN_AGAIN;				// No error, not done
	
	if ( total_left == 0 ) {
		ignitech_data.rpm = buf[2] + buf[3] * 0x100u;
		ignitech_data.map_mV = buf[4] + buf[5] * 0x100u;
		ignitech_data.battery_mV = buf[6] + buf[7] * 0x100u;
		ignitech_data.map_kpa = buf[22] + buf[23] *0x100u;

		total_read = 0;
		good_read = 0;
		total_left = IGNITECH_PACKET_SIZE;
		found_header = false;
		buf[0] = 0;
		return IGN_SUC;
	}
	else
		return IGN_AGAIN;
}

/*
	Reads synchronously into internal data structure
*/
int IGNITECH::read_sync() {
	return read_sync(ignition);
}

/*
	Reads Asynchronously into internal data structure
*/
IGN_async_status IGNITECH::read_async() {
	return read_async(ignition);
}

/*
	Send query string to device to solicit respons
*/
int IGNITECH::query_device() {
	// Check if device is open
	if ( file_descriptor < 0 ) {		// device not open
		if ( open_device() < 0 )		// try opening it 
			return -1;
	}
	// Query controller for status
	size_t b_written = write(file_descriptor,IGNITECH_QUERY,IGNITECH_PACKET_SIZE);
	if (b_written == IGNITECH_PACKET_SIZE )// check for success
		return 0; // Good :)
	return -1; // Bad  :(
}


/*
	Open device and set attribs
*/
int IGNITECH::open_device() {
	int fd;
	fd = open (device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		perror("IGNITECH:open_device:");
		file_descriptor = fd;
		return -1;
	}

	// Set interface parameters for front controls
	set_interface_attribs (fd, B57600, 0);  // set speed to 57,600 bps, 8n1 (no parity)
	set_blocking (fd, 0);				// set no blocking

	file_descriptor = fd;
	return 0;
}

/*
	Constructor that takes pathname to device file
	Opens given pathname and sets attributes
	as necessary. Initializes variables.
*/
IGNITECH::IGNITECH( char const *file) {
	device = strdup(file);
	initialize();
	open_device();
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

