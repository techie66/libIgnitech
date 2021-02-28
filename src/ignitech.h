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

/*
API for libIgnitech
-Method to read (controller ID)
-Method to setup serial (other) connection
-Overloaded to suport Serial/File/Arduino/etc.
*/

#ifndef IGNITECH_H
#define IGNITECH_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>


#define IGNITECH_BOGUS 0x3000
#define IGNITECH_PACKET_SIZE_V88 102
#define IGNITECH_PACKET_SIZE_V96 152
#define IGNITECH_VERSION_BYTE_V88 0x58
#define IGNITECH_VERSION_BYTE_V96 0x60
#define IGNITECH_HEADER_DATA  0xb0
#define IGNITECH_HEADER_DIAG  0x80
#define IGNITECH_HEADER_DATA_V88  0x00
#define IGNITECH_HEADER_DATA_V96  0x07
#define IGNITECH_MAX_RESETS  10

// Endianness Dependant
//  Multi-byte comparisons need correct order
#ifdef WORDS_BIGENDIAN
// Big-Endian
//#define IGNITECH_READ_QUERY 0x975800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030

#else
// Litle-Endian
//#define IGNITECH_READ_QUERY 0x300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005897

#endif /* WORDS_BIGENDIAN */

extern unsigned char const IGNITECH_QUERY_V88[];
typedef enum IGN_async_status{
	IGN_ERR=-1,
	IGN_SUC,
	IGN_BAD,
	IGN_AGAIN
} IGN_async_status;

typedef enum inputtype_t{
	UNUSED,
	KILL,
	BLOCK,
	QUICKSHIFT,
	RETARD,
	START_LIMITER,
	SECOND_CHART
} inputtype_t;

typedef enum sensortype_t {
	SENSOR_NONE,
	SENSOR_TPS,
	SENSOR_IAP
} sensortype_t;

struct ignitech_t{
	uint16_t	rpm;
	uint16_t	battery_mV;
	sensortype_t sensor_type;
	uint16_t	sensor_mV;
	uint16_t	sensor_value;
	bool	sensor_error;
	uint16_t	programmings;
	uint16_t	advance_max_1_deg;
	uint16_t	advance_max_2_deg;
	uint16_t	advance_max_3_deg;
	uint16_t	advance_max_4_deg;
	uint16_t	dwell_opt_ms;
	uint16_t	dwell_ms;
	uint16_t	runtime_min;
	uint16_t	message_number;
	int16_t	advance_1_deg;
	int16_t	advance_2_deg;
	int16_t	advance_3_deg;
	int16_t	advance_4_deg;
	uint16_t	servo_measured;
	uint16_t	servo_requested;
	bool	servo_enabled;
	int		servo_percent;
	bool	programmed_data_error;
	bool	dwell_error;
	bool	input_kill;
	bool	input_block;
	bool	input_quickshift;
	bool	input_retard;
	bool	input_start_limiter;
	bool	input_2nd_chart;
	bool	input_kill_enabled;
	bool	input_block_enabled;
	bool	input_quickshift_enabled;
	bool	input_retard_enabled;
	bool	input_start_limiter_enabled;
	bool	input_2nd_chart_enabled;
	bool	input_inverted;
	bool	input1_active;
	bool	input2_active;
	bool	output_1_on;
	bool	output_2_on;
	bool	output_1_enabled;
	bool	output_2_enabled;
	bool	limiter_active;
	bool	start_limiter_ready;
	bool	pickup1_system_error;
	bool	pickup1_signal;
	bool	pickup1_neg_polarity;
	bool	pickup1_polarity_error;
	bool	pickup2_system_error;
	bool	pickup2_signal;
	bool	pickup2_neg_polarity;
	bool	pickup2_polarity_error;
	bool	motor_off;
	uint16_t	proprietary_monitor_1;
	uint16_t	proprietary_monitor_2;
	uint16_t	proprietary_monitor_3;
	uint16_t	proprietary_monitor_4;
	uint16_t	proprietary_monitor_5;
	uint16_t	over_voltage_onboard;
	inputtype_t input1_type;
	inputtype_t input2_type;
	uint8_t	number_cylinders;
};

class IGNITECH {
	public:
		int read_sync( ignitech_t& ignitech_data );
		int read_sync( );
		IGN_async_status read_async( ignitech_t& ignitech_data );
		IGN_async_status read_async( );
		int get_rpm();
		int get_sensor_value();
		int get_sensor_mV();
		sensortype_t get_sensor_type();
		int get_battery_mV();
		int get_advance1();
		int get_advance2();
		int get_advance3();
		int get_advance4();
		int get_dwell();
		int get_dwell_opt();
		int get_runtime();
		int get_message_number();
		int get_programmings();
		int get_advance1_max();
		int get_advance2_max();
		int get_advance3_max();
		int get_advance4_max();
		int get_servo_measured();

		IGNITECH( char const *file );
		IGNITECH( int fd );
		void enable_debug();
		void disable_debug();
		void enable_raw_dump( char const *file );
		void disable_raw_dump();
		IGN_async_status get_status();

	private:
		void initialize();
		void reset(bool force=false);
		int open_device();
		int query_device();
		bool checksum_is_good(unsigned char *buf, int length);
		bool packet_version_matches(unsigned char *buf, int length);
		float running_map_ratio( ignitech_t& ignitech_data );
		int set_interface_attribs (int fd, int speed, int parity);
		int file_descriptor;
		ignitech_t ignition;
		bool isArduino;
		bool DEBUG_IGNITECH;
		bool raw_dump;
		FILE* dump_file;
		int num_resets;
		char const *device;
		enum version{
			VERSION_V88,
			VERSION_V96
		} version;
		IGN_async_status status;
};


#endif
