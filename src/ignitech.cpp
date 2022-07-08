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
#include "serial.h"

unsigned char const IGNITECH_QUERY_V88[102] = {0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x58,0x97};
unsigned char const IGNITECH_QUERY_V96[152] = {0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xAF};
void IGNITECH::initialize() {
	reset(true);
	raw_dump = false;
	DEBUG_IGNITECH = false;
	file_descriptor=-1;
	version = VERSION_V88;
}

void IGNITECH::reset(bool force) {
	// Build resiliency to transient failures, only actually reset after continued failure
	if ( num_resets > IGNITECH_MAX_RESETS || force) {
		ignition.rpm=0;
		ignition.battery_mV=0;
		ignition.sensor_mV=0;
		ignition.sensor_value=0;
		ignition.sensor_type=SENSOR_NONE;
		ignition.sensor_error=false;
		ignition.programmings=0;
		ignition.advance_max_1_deg=0;
		ignition.advance_max_2_deg=0;
		ignition.advance_max_3_deg=0;
		ignition.advance_max_4_deg=0;
		ignition.dwell_opt_ms=0;
		ignition.dwell_ms=0;
		ignition.runtime_min=0;
		ignition.message_number=0;
		ignition.advance_1_deg=0;
		ignition.advance_2_deg=0;
		ignition.advance_3_deg=0;
		ignition.advance_4_deg=0;
		ignition.servo_measured=0;
		ignition.servo_requested=0;
		ignition.servo_enabled=false;
		ignition.servo_percent=0;
		ignition.programmed_data_error=false;
		ignition.dwell_error=false;
		ignition.input_kill=false;
		ignition.input_block=false;
		ignition.input_quickshift=false;
		ignition.input_retard=false;
		ignition.input_start_limiter=false;
		ignition.input_2nd_chart=false;
		ignition.input_kill_enabled=false;
		ignition.input_block_enabled=false;
		ignition.input_quickshift_enabled=false;
		ignition.input_retard_enabled=false;
		ignition.input_start_limiter_enabled=false;
		ignition.input_2nd_chart_enabled=false;
		ignition.input_inverted=false;
		ignition.input1_active=false;
		ignition.input2_active=false;
		ignition.output_1_on=false;
		ignition.output_2_on=false;
		ignition.output_1_enabled=false;
		ignition.output_2_enabled=false;
		ignition.limiter_active=false;
		ignition.start_limiter_ready=false;
		ignition.pickup1_system_error=false;
		ignition.pickup1_signal=false;
		ignition.pickup1_neg_polarity=false;
		ignition.pickup1_polarity_error=false;
		ignition.pickup2_system_error=false;
		ignition.pickup2_signal=false;
		ignition.pickup2_neg_polarity=false;
		ignition.pickup2_polarity_error=false;
		ignition.motor_off=true;
		ignition.proprietary_monitor_1=0;
		ignition.proprietary_monitor_2=0;
		ignition.proprietary_monitor_3=0;
		ignition.proprietary_monitor_4=0;
		ignition.proprietary_monitor_5=0;
		ignition.over_voltage_onboard=0;
		ignition.input1_type=UNUSED;
		ignition.input2_type=UNUSED;
		ignition.number_cylinders=0;
		
		num_resets = 0;
		status = IGN_ERR;
	}
	else {
		num_resets++;
	}
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
	static time_t reset_last_read = time(NULL);
	static unsigned char buf[IGNITECH_PACKET_SIZE_V96];
	static bool found_header = false;
	static bool sent_header = false;
	static size_t total_read = 0;
	static size_t packet_size = IGNITECH_PACKET_SIZE_V88;

  errno = 0; // Reset upon entry

	if ( file_descriptor < 0 ) {
		// Bad file descriptor, reset
		if ( DEBUG_IGNITECH ) {
			perror ( "IGNITECH::read_async: Device not open. Resetting.");
		}
		reset();
		total_read = 0;
		found_header = false;
		sent_header = false;
		buf[0] = 0;
	}

	if ( time(NULL) > reset_last_read + reset_timeout ) {
		// No response for timeout, reset
		if ( DEBUG_IGNITECH ) {
			perror ( "IGNITECH::read_async: No response from controller within timeout. Resetting.");
		}
		// TODO it hasn't proved necessary, but it may be a good idea to close the port and reopen
		reset_last_read = time(NULL);
    close(file_descriptor);
    file_descriptor = -1;
		reset();
		total_read = 0;
		found_header = false;
		sent_header = false;
		buf[0] = 0;
	}

	if ( total_read > packet_size ) {
		// Read more than expected, reset
		if ( DEBUG_IGNITECH ) {
			perror ( "IGNITECH::read_async: Read more than expected. resetting");
		}
		reset();
		total_read = 0;
		found_header = false;
		sent_header = false;
		buf[0] = 0;
	}

	if ( !sent_header && total_read == 0 ) {
		// On reset, or after successful reading of packet
		if ( query_device() < 0 ) {
			return IGN_ERR;
		}
		sent_header = true;
	}
	
	if ( !found_header ) {
		// Search for header
		if ( buf[0] != IGNITECH_HEADER_DATA ) {
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
			if (select_result < 0 && DEBUG_IGNITECH) {
				perror ( "IGNNITECH::read_async:select");
			}
			else if (select_result == 0){} // Timeout
			else if (select_result > 0) {
				if (FD_ISSET(file_descriptor,&readset)) {
					size_t b_read = read(file_descriptor,buf,1);
					if (b_read <= 0 ) {			// Failure or done
						if (b_read < 0) {		// Failure
							if ( DEBUG_IGNITECH ) {
								perror("IGNITECH::read_async:header");
							}
							return IGN_ERR;
						}
						return IGN_AGAIN;
					}
					reset_last_read = time(0);
					//sent_header = false;
					if (buf[0] == IGNITECH_HEADER_DATA ) {		// Found header
						found_header = true;
						sent_header = false;
						total_read += b_read;
					}
				}
			}
		}
	}
	if ( found_header ) {
		// read the rest
		if ( packet_size - total_read > 0 ) {

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
			if (select_result < 0 && DEBUG_IGNITECH) {
				perror ( "IGNNITECH::read_async:select");
			}
			else if (select_result == 0){} // Timeout
			else if (select_result > 0) {
				if (FD_ISSET(file_descriptor,&readset)) {
					size_t b_read = read(file_descriptor,&buf[total_read],packet_size - total_read);
					if (b_read <= 0 ) {
						if (b_read < 0) {// Failure or none
							if ( DEBUG_IGNITECH ) {
								perror("IGNITECH::read_async:packet");
							}
							return IGN_ERR;
						}
						return IGN_AGAIN;
					}
					else {
						reset_last_read = time(NULL);
						total_read += b_read;
						if ( DEBUG_IGNITECH ) {
							perror("IGNITECH::read_async:read after header");
						}
					}
				}
				else
					IGN_AGAIN;
			}
			else
				return IGN_AGAIN;
		}
		if ( total_read >= IGNITECH_PACKET_SIZE_V88 && buf[100] == IGNITECH_VERSION_BYTE_V88 ) {
			if ( DEBUG_IGNITECH ) {
				perror("IGNITECH::read_async:found version 88");
			}
			version = VERSION_V88;
			packet_size = IGNITECH_PACKET_SIZE_V88;
		}
		else if ( total_read >= IGNITECH_PACKET_SIZE_V88 && buf[100] == IGNITECH_VERSION_BYTE_V96 ) {
			// Tried reading V88, but V96 responded with a packet telling us we should use V96
			if ( DEBUG_IGNITECH ) {
				perror("IGNITECH::read_async:V88 packet,found version 96");
			}
			version = VERSION_V96;
			packet_size = IGNITECH_PACKET_SIZE_V96;
			// Reset to read a new packet with correct version
			reset_last_read = time(NULL);
			total_read = 0;
			found_header = false;
                	sent_header = false;
                	buf[0] = 0;
		}
		else if ( total_read >= IGNITECH_PACKET_SIZE_V96 && buf[150] == IGNITECH_VERSION_BYTE_V96 ) {
			if ( DEBUG_IGNITECH ) {
				perror("IGNITECH::read_async:found version 96");
			}
			version = VERSION_V96;
			packet_size = IGNITECH_PACKET_SIZE_V96;
		}
	}
	else
		return IGN_AGAIN;				// No error, not done
	
	if ( packet_size - total_read == 0 ) {
		if ( raw_dump ) {
			fwrite(buf,1,total_read,dump_file);
			fflush(dump_file);
		}
		if ( checksum_is_good(buf,packet_size) && packet_version_matches(buf,packet_size) ) {
			status = IGN_SUC;
			if ( version == VERSION_V88 ) {
				ignitech_data.rpm = buf[2] + buf[3] * 0x100u;
				switch (buf[51]) {
					case 0:
						ignitech_data.sensor_type = SENSOR_NONE;
						break;
					case 1:
						ignitech_data.sensor_type = SENSOR_TPS;
						break;
					case 2:
						ignitech_data.sensor_type = SENSOR_IAP;
						break;
					default:
						ignitech_data.sensor_type = SENSOR_IAP;
				}
				ignitech_data.sensor_mV = buf[4] + buf[5] * 0x100u;
				ignitech_data.battery_mV = buf[6] + buf[7] * 0x100u;
				ignitech_data.servo_measured = buf[8] + buf[9] * 0x100u;
				ignitech_data.advance_max_1_deg = buf[14] + buf[15] * 0x100u;
				ignitech_data.advance_max_2_deg = buf[16] + buf[17] * 0x100u;
				ignitech_data.advance_max_3_deg = buf[18] + buf[19] * 0x100u;
				ignitech_data.advance_max_4_deg = buf[20] + buf[21] * 0x100u;
				ignitech_data.sensor_value = buf[22] + buf[23] *0x100u;
				ignitech_data.advance_1_deg = buf[57] - 100;
				ignitech_data.advance_2_deg = buf[58] - 100;
				ignitech_data.advance_3_deg = buf[59] - 100;
				ignitech_data.advance_4_deg = buf[60] - 100;
			}
			if ( version == VERSION_V96 ) {
				ignitech_data.rpm = buf[2] + buf[3] * 0x100u;
				switch (buf[100]) {
					case 0:
						ignitech_data.sensor_type = SENSOR_NONE;
						break;
					case 1:
						ignitech_data.sensor_type = SENSOR_TPS;
						break;
					case 2:
						ignitech_data.sensor_type = SENSOR_IAP;
						break;
					default:
						ignitech_data.sensor_type = SENSOR_IAP;
				}
				ignitech_data.sensor_mV = buf[4] + buf[5] * 0x100u;
				ignitech_data.sensor_value = buf[6] + buf[7] *0x100u;
				ignitech_data.battery_mV = buf[8] + buf[9] * 0x100u;
				ignitech_data.servo_measured = buf[10] + buf[11] * 0x100u;
				ignitech_data.programmings = buf[14] + buf[15] * 0x100u;
				ignitech_data.advance_max_1_deg = buf[18] + buf[19] * 0x100u;
				ignitech_data.advance_max_2_deg = buf[20] + buf[21] * 0x100u;
				ignitech_data.advance_max_3_deg = buf[22] + buf[23] * 0x100u;
				ignitech_data.advance_max_4_deg = buf[24] + buf[25] * 0x100u;
				ignitech_data.dwell_opt_ms = buf[26] + buf[27] * 0x100u;
				ignitech_data.dwell_ms = buf[28] + buf[29] * 0x100u;
				ignitech_data.runtime_min = buf[30] + buf[31] * 0x100u;
				ignitech_data.message_number = buf[50] + buf[51] * 0x100u;
				ignitech_data.advance_1_deg = buf[105] - 100;
				ignitech_data.advance_2_deg = buf[106] - 100;
				ignitech_data.advance_3_deg = buf[107] - 100;
				ignitech_data.advance_4_deg = buf[108] - 100;
			}

			total_read = 0;
			found_header = false;
			buf[0] = 0;
			num_resets = 0;
			return status;
		}
		else {
			// Checksum bad or version mismatch
			if ( DEBUG_IGNITECH ) {
				perror("IGNITECH::read_async: Checksum or version error. Quitting.");
			}
			reset();
			total_read = 0;
			found_header = false;
			buf[0] = 0;
			return IGN_BAD;
		}
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
	if ( version == VERSION_V88 ) {
		if ( DEBUG_IGNITECH ) {
			perror ( "IGNITECH::query_device: Send v88");
		}
	    size_t b_written = write(file_descriptor,IGNITECH_QUERY_V88,IGNITECH_PACKET_SIZE_V88);
	    if (b_written == IGNITECH_PACKET_SIZE_V88 )// check for success
		    return 0; // Good :)
	}
	if ( version == VERSION_V96 ) {
		if ( DEBUG_IGNITECH ) {
			perror ( "IGNITECH::query_device: Send v96");
		}
	    size_t b_written = write(file_descriptor,IGNITECH_QUERY_V96,IGNITECH_PACKET_SIZE_V96);
	    if (b_written == IGNITECH_PACKET_SIZE_V96 )// check for success
		    return 0; // Good :)
	}
	if ( DEBUG_IGNITECH ) {
		perror ( "IGNITECH::query_device: Write failed. Quitting.");
	}
	return -1; // Bad  :(
}


/*
	Open device and set attribs
*/
int IGNITECH::open_device() {
	int fd;
	fd = open (device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		if (DEBUG_IGNITECH ) {
			perror("IGNITECH:open_device:");
		}
		file_descriptor = fd;
		return -1;
	}

	// Set interface parameters for front controls
	set_interface_attribs (fd, B57600, 0);  // set speed to 57,600 bps, 8n1 (no parity)
	//set_blocking (fd, 0);				// set minimum byte to 0 REDUNDANT

	file_descriptor = fd;
	return 0;
}

/*
	Constructor that takes pathname to device file
	Opens given pathname and sets attributes
	as necessary. Initializes variables.
*/
IGNITECH::IGNITECH( char const *file) {
	if ( file != NULL ) {
		device = strdup(file);
	}
	else {
		device = strdup("");
	}
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
int IGNITECH::get_sensor_value() {
	return ignition.sensor_value;
}

/*
	Simple Getter
*/
int IGNITECH::get_sensor_mV() {
	return ignition.sensor_mV;
}

/*
	Simple Getter
*/
sensortype_t IGNITECH::get_sensor_type() {
	return ignition.sensor_type;
}

/*
	Simple Getter
*/
int IGNITECH::get_battery_mV() {
	return ignition.battery_mV;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance1() {
	return ignition.advance_1_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance2() {
	return ignition.advance_2_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance3() {
	return ignition.advance_3_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance4() {
	return ignition.advance_4_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance1_max() {
	return ignition.advance_max_1_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance2_max() {
	return ignition.advance_max_2_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance3_max() {
	return ignition.advance_max_3_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_advance4_max() {
	return ignition.advance_max_4_deg;
}

/*
	Simple Getter
*/
int IGNITECH::get_dwell() {
	return ignition.dwell_ms;
}

/*
	Simple Getter
*/
int IGNITECH::get_dwell_opt() {
	return ignition.dwell_opt_ms;
}

/*
	Simple Getter
*/
int IGNITECH::get_runtime() {
	return ignition.runtime_min;
}

/*
	Simple Getter
*/
int IGNITECH::get_message_number() {
	return ignition.message_number;
}

/*
	Simple Getter
*/
int IGNITECH::get_programmings() {
	return ignition.programmings;
}

/*
	Simple Getter
*/
int IGNITECH::get_servo_measured() {
	return ignition.servo_measured;
}

/*
	Simple Getter
*/
IGN_async_status IGNITECH::get_status() {
	return status;
}

void IGNITECH::enable_debug() {
	DEBUG_IGNITECH = true;
}

void IGNITECH::disable_debug() {
	DEBUG_IGNITECH = false;
}

void IGNITECH::enable_raw_dump( char const *file ) {
	dump_file = fopen(file,"wb");
	raw_dump = true;
}

void IGNITECH::disable_raw_dump() {
	fclose(dump_file);
	raw_dump = false;
}

/*
	Verify Checksum
	@return@
		true = good
		false = anything else
*/
bool IGNITECH::checksum_is_good(unsigned char *buf, int length) {
	unsigned char cs = 0;
	for ( int i = 0 ; i < length - 1 ; i++ ) {
		cs ^= buf[i];
	}
	cs ^= 0xFF;
	if (cs == buf[length-1])
		return true;
	else
		return false;
}

/*
	Run checks on packet to determine version
	@return@
		true: consistent and matches 'version'
		false: any other result
*/
bool IGNITECH::packet_version_matches(unsigned char *buf, int length) {
	if (version == VERSION_V88 ) {
		if ( buf[0] == IGNITECH_HEADER_DATA ) { 
			if ( buf[length-2] != IGNITECH_VERSION_BYTE_V88)
				return false;
		}
		else if (buf[0] == IGNITECH_HEADER_DIAG ) {
			// TODO
		}
	}
	else if (version == VERSION_V96 ) {
		if ( buf[length-2] != IGNITECH_VERSION_BYTE_V96)
			return false;
	}

	return true;
}


