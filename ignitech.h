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
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#define IGNITECH_BOGUS 0x3000
#define IGNITECH_PACKET_SIZE 102
// Endianness Dependant
//  Multi-byte comparisons need correct order
#ifdef WORDS_BIGENDIAN
// Big-Endian
//#define IGNITECH_READ_QUERY 0x975800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030

#else
// Litle-Endian
//#define IGNITECH_READ_QUERY 0x300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005897

#endif /* WORDS_BIGENDIAN */

extern unsigned char const IGNITECH_QUERY[];
typedef enum IGN_async_status{
	IGN_ERR=-1,
	IGN_SUC,
	IGN_AGAIN
} IGN_async_status;

struct ignitech_t{
	uint16_t	rpm;
	uint16_t	battery_mV;
	uint16_t	map_mV;
	uint16_t	map_kpa;
};

class IGNITECH {
	public:
		int read_sync( ignitech_t& ignitech_data );
		int read_sync( );
		IGN_async_status read_async( ignitech_t& ignitech_data );
		IGN_async_status read_async( );
		int get_rpm();
		int get_map_kpa();
		int get_map_mV();
		int get_battery_mV();
		IGNITECH( char const *file );
		IGNITECH( int fd );
	private:
		void initialize();
		int open_device();
		int query_device();
		int file_descriptor;
		ignitech_t ignition;
		bool isArduino;
		char const *device;
};


#endif
