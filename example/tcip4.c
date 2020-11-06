#define _XOPEN_SOURCE 600
#define _XOPEN_SOURCE_EXTENDED

#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include "error_handling.h"

#define MAX_PACKET_SIZE 152

typedef enum{
	RPM,
	SPEED,
	MAPKPA,
	MAPMV,
	BATTMV,
	PROGRAMMINGS
} sweep_t;

unsigned char * next_packet(int version, sweep_t sweep);

volatile sig_atomic_t time_to_quit;
void signalHandler( int signum ) {
	//error_message(INFO, "Interrupt signal %d received", signum);
	time_to_quit = true;
}

int main() {
	// register signal SIGINT and signal handler  
	signal(SIGINT, signalHandler);

	// TODO make these variables options
	int version = 88;
	sweep_t sweep = RPM;
	LEVEL_DEBUG = DEBUG;
	char const* datafilename = "serial_dumps/test.bin";
	char const* pipename = "./virtual-tty";
	// END TODO

	//mkfifo(pipename,0666);
	int pts = posix_openpt(O_RDWR);
	grantpt(pts);
	unlockpt(pts);
	char const *pts_name = ptsname(pts);
	if ( symlink(pts_name,pipename) < 0 ) {
		perror("Unable symlink pts");
		return -1;
	}
	printf("Vitual serial port opened at: %s\n",pipename);

	const unsigned char response[102] = {0xb0,0x00,0x44,0x04,0x4a,0x0f,0x4d,0x30,0x9c,0x00,0x00,0x00,0x17,0x00,0xce,0x00,0xc3,0x00,0x00,0x00,0x00,0x00,0x5d,0x00,0x10,0x00,0x4a,0x00,0x00,0x00,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0xf0,0xf1,0xff,0xff,0x6e,0x6f,0x00,0x00,0x00,0x6c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x58,0x30};
	unsigned char w_buf[102];
	FILE *ptr;
	ptr = fopen(datafilename,"rb");  // r for read, b for binary
	if (ptr < 0) {
		error_message (ERROR, "error %d : %s", errno, strerror (errno));
	}
	int packet_size=0;
	if (version == 88 ) {
		packet_size = 102;
	}
	else if (version == 96 ) {
		packet_size = 152;
	}

	int total_read = 0;
	while(!time_to_quit) {
		// wait for query then respond
		fd_set	readset;
		FD_ZERO(&readset);
		FD_SET(pts,&readset);
		int select_result = 0;
		struct timeval	timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		select_result = select(pts+1, &readset, NULL, NULL, &timeout);
		if (select_result < 0) {
			if (errno == EINTR) {
				continue;
			}
			error_message (WARN, "error %d : %s", errno, strerror (errno));
			return -1;
		}
		if (select_result > 0) {
			if (FD_ISSET(pts,&readset)) {
				static unsigned char r_buf[MAX_PACKET_SIZE];
				int read_bytes = read(pts,&r_buf[total_read],packet_size - total_read);
				if (read_bytes < 0) {
					if (errno == EINTR) {
						continue;
					}
					error_message (WARN, "error %d : %s", errno, strerror (errno));
					return -1;
		        }

				else if (read_bytes > 0) {
					error_message(DEBUG,"Read %d Bytes from tty.", read_bytes);
					total_read += read_bytes;
				}
				if (total_read == packet_size) {
					total_read = 0;
					if ( r_buf[0] == 0x30 && r_buf[100] == 0x58 && r_buf[101] == 0x97 ) {
						error_message(DEBUG,"Sending Response");
						int write_bytes = write(pts,next_packet(version,sweep),packet_size);
						error_message (DEBUG, "Bytes Sent: %d", write_bytes);
						// TODO create our own custom response packet here (in a separate function) (option)
						/*
						int file_read_bytes = fread(w_buf,1,packet_size,ptr); // read 102 bytes to our buffer
						if (file_read_bytes == packet_size) {
							int write_bytes = write(1,w_buf,packet_size);
							error_message (DEBUG, "Bytes Sent: %d", write_bytes);
						}
						else if (file_read_bytes < packet_size ) {
							error_message (DEBUG, "error %d : %s:: Bytes Read: %d, resetting file.", errno, strerror (errno),file_read_bytes);
							rewind(ptr);
							// The previous read failed, so after resetting the file
							// read and write again
							file_read_bytes = fread(w_buf,1,packet_size,ptr);
							if (file_read_bytes == packet_size) {
								int write_bytes = write(1,w_buf,packet_size);
								error_message (DEBUG, "Bytes Sent: %d", write_bytes);
							}
							else {
								// After going to front of file, still short read
								// Serious issue, give up
								return -1;
							}

						}
						*/
					}
				}
				else if (total_read > packet_size ) {
					total_read = 0;
				}
			}
		}

	}
	printf("Exiting!\n");
	close(pts);
	fclose(ptr);
	remove(pipename);
}

unsigned char * next_packet(int version, sweep_t sweep) {
	static unsigned char packet[MAX_PACKET_SIZE];
	static uint16_t counter = 0;
	
	if ( version == 88 ) {
		packet[0] = 0xb0;
		packet[1] = 0x00;
		*(uint16_t*)&packet[2] = counter;
		packet[4] = 0x00;
		packet[5] = 0x00;
		packet[6] = 0x00;
		packet[7] = 0x00;
		packet[8] = 0x00;
		packet[9] = 0x00;
		packet[10] = 0x00;
		packet[11] = 0x00;
		packet[12] = 0x00;
		packet[13] = 0x00;
		packet[14] = 0x00;
		packet[15] = 0x00;
		packet[16] = 0x00;
		packet[17] = 0x00;
		packet[18] = 0x00;
		packet[19] = 0x00;
		packet[20] = 0x00;
		packet[21] = 0x00;
		packet[22] = 0x00;
		packet[23] = 0x00;
		packet[24] = 0x00;
		packet[25] = 0x00;
		packet[26] = 0x00;
		packet[27] = 0x00;
		packet[28] = 0x00;
		packet[29] = 0x00;
		packet[30] = 0x00;
		packet[31] = 0x00;
		packet[32] = 0x00;
		packet[33] = 0x00;
		packet[34] = 0x00;
		packet[35] = 0x00;
		packet[36] = 0x00;
		packet[37] = 0x00;
		packet[38] = 0x00;
		packet[39] = 0x00;
		packet[40] = 0x00;
		packet[41] = 0x00;
		packet[42] = 0x00;
		packet[43] = 0x00;
		packet[44] = 0x00;
		packet[45] = 0x00;
		packet[46] = 0x00;
		packet[47] = 0x00;
		packet[48] = 0x00;
		packet[49] = 0x00;
		packet[50] = 0x00;
		packet[51] = 0x00;
		packet[52] = 0x00;
		packet[53] = 0x00;
		packet[54] = 0x00;
		packet[55] = 0x00;
		packet[56] = 0x00;
		packet[57] = 0x00;
		packet[58] = 0x00;
		packet[59] = 0x00;
		packet[60] = 0x00;
		packet[61] = 0x00;
		packet[62] = 0x00;
		packet[63] = 0x00;
		packet[64] = 0x00;
		packet[65] = 0x00;
		packet[66] = 0x00;
		packet[67] = 0x00;
		packet[68] = 0x00;
		packet[69] = 0x00;
		packet[70] = 0x00;
		packet[71] = 0x00;
		packet[72] = 0x00;
		packet[73] = 0x00;
		packet[74] = 0x00;
		packet[75] = 0x00;
		packet[76] = 0x00;
		packet[77] = 0x00;
		packet[78] = 0x00;
		packet[79] = 0x00;
		packet[80] = 0x00;
		packet[81] = 0x00;
		packet[82] = 0x00;
		packet[83] = 0x00;
		packet[84] = 0x00;
		packet[85] = 0x00;
		packet[86] = 0x00;
		packet[87] = 0x00;
		packet[88] = 0x00;
		packet[89] = 0x00;
		packet[90] = 0x00;
		packet[91] = 0x00;
		packet[92] = 0x00;
		packet[93] = 0x00;
		packet[94] = 0x00;
		packet[95] = 0x00;
		packet[96] = 0x00;
		packet[97] = 0x00;
		packet[98] = 0x00;
		packet[99] = 0x00;
		packet[100] = 0x58;
		unsigned char cs = 0;
		for ( int i = 0; i < 102 - 1; i++ ) {
			cs ^= packet[i];
		}
		cs ^= 0xFF;
		packet[101] = cs;
	}
	counter++;
	return packet;
}
