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
#include "serial.h"

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
