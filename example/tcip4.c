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
#include "cmdline.h"

#define MAX_PACKET_SIZE 300
#define STDIN_BUFFER 4096

unsigned char * next_packet(int version, enum enum_sweep sweep);

volatile sig_atomic_t time_to_quit;
void signalHandler( int signum ) {
	//error_message(INFO, "Interrupt signal %d received", signum);
	time_to_quit = true;
}

int main(int argc, char *argv[]) {
	// register signal SIGINT and signal handler  
	signal(SIGINT, signalHandler);

	int version = 88;
	enum enum_sweep sweep = sweep_arg_RPM;
	LEVEL_DEBUG = ERROR;
	char const* datafilename = "";
	char const* pipename = "./virtual-tty";

	struct gengetopt_args_info args_info;
	struct cmdline_parser_params *params;

	/* initialize the parameters structure */
	params = cmdline_parser_params_create();

	/* let's call our cmdline parser */
	if (cmdline_parser (argc, argv, &args_info) != 0)
		exit(1) ;

	if ( args_info.v_given )
		LEVEL_DEBUG = (e_lvl)(args_info.v_given + 1);
	if ( args_info.verbose_given )
		LEVEL_DEBUG = (e_lvl)(args_info.verbose_arg);
	if ( args_info.quiet_given )
		LEVEL_DEBUG = NONE;

	if ( args_info.sweep_given )
		sweep = args_info.sweep_arg;
	FILE *ptr;
	if ( args_info.data_file_given ) {
		datafilename = args_info.data_file_arg;
		ptr = fopen(datafilename,"rb");  // r for read, b for binary
		if (ptr < 0) {
			error_message (ERROR, "error %d : %s", errno, strerror (errno));
			return -1;
		}
	}
	if ( args_info.pipe_name_given )
		pipename = args_info.pipe_name_arg;
	if ( args_info.firmware_version_given )
		version = args_info.firmware_version_arg;
	//mkfifo(pipename,0666);
	int pts = posix_openpt(O_RDWR);
	grantpt(pts);
	unlockpt(pts);
	char const *pts_name = ptsname(pts);
	if ( symlink(pts_name,pipename) < 0 ) {
		error_message (ERROR, "Unable symlink pts. %d : %s", errno, strerror (errno));
		return -1;
	}
	error_message (ERROR, "Vitual serial port opened at:  %s",pipename);

	const unsigned char response[102] = {0xb0,0x00,0x44,0x04,0x4a,0x0f,0x4d,0x30,0x9c,0x00,0x00,0x00,0x17,0x00,0xce,0x00,0xc3,0x00,0x00,0x00,0x00,0x00,0x5d,0x00,0x10,0x00,0x4a,0x00,0x00,0x00,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0xf0,0xf1,0xff,0xff,0x6e,0x6f,0x00,0x00,0x00,0x6c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x58,0x30};
	unsigned char w_buf[MAX_PACKET_SIZE];
	unsigned char *packet = w_buf;
	int packet_size=0;
	if (version == 88 ) {
		packet_size = 102;
	}
	else if (version == 96 ) {
		packet_size = 152;
	}

	int total_read = 0;
	bool get_next_packet = true;
	while(!time_to_quit) {
		// wait for query then respond
		fd_set	readset;
		FD_ZERO(&readset);
		FD_SET(pts,&readset);
		if ( args_info.interactive_given ) {
			FD_SET(STDIN_FILENO,&readset);
		}
		else {
			get_next_packet = true;
		}
		int select_result = 0;
		struct timeval	timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		int maxfd = (pts > STDIN_FILENO)?pts:STDIN_FILENO;
		select_result = select(maxfd+1, &readset, NULL, NULL, &timeout);
		if (select_result < 0) {
			if ( errno != EINTR ) {
				error_message (ERROR, "error %d : %s", errno, strerror (errno));
			}
			time_to_quit = true;
			continue;
		}
		if (select_result > 0) {
			if (FD_ISSET(STDIN_FILENO,&readset)) {
				unsigned char buf[STDIN_BUFFER];
				int read_bytes = read(STDIN_FILENO,buf,STDIN_BUFFER);
				if (read_bytes < 0) {
					if ( errno != EINTR ) {
						error_message (WARN, "Error reading STDIN. %d : %s", errno, strerror (errno));
					}
				}
				else if (read_bytes >= 0) {
					error_message (INFO, "Read %d bytes from STDIN.",read_bytes);
					if ( buf[0] == '\n' ) {
						get_next_packet = true;
					}
				}
			}
			if (FD_ISSET(pts,&readset)) {
				static unsigned char r_buf[MAX_PACKET_SIZE];
				int read_bytes = read(pts,&r_buf[total_read],packet_size - total_read);
				if (read_bytes < 0) {
					if ( errno != EINTR ) {
						error_message (ERROR, "error %d : %s", errno, strerror (errno));
					}
					time_to_quit = true;
					continue;
		        }

				else if (read_bytes >= 0) {
					error_message(DEBUG,"Read %d Bytes from tty.", read_bytes);
					total_read += read_bytes;
				}
				if (total_read == packet_size) {
					total_read = 0;
					if ( r_buf[0] == 0x30 && r_buf[packet_size-2] == version && r_buf[packet_size-1] == (0x30 ^ version ^ 0xFF) ) {
						error_message(INFO,"Sending Response");
						if ( get_next_packet ) {
							// read / create next packet
							if ( args_info.data_file_given ) {
								packet = w_buf;
								int file_read_bytes = fread(w_buf,1,packet_size,ptr); // read 102 bytes to our buffer
								if (file_read_bytes == packet_size) {
									// good, do nothing
								}
								else if (file_read_bytes < packet_size ) {
									error_message (INFO, "error %d : %s:: Bytes Read: %d, resetting file.", errno, strerror (errno),file_read_bytes);
									rewind(ptr);
									// The previous read failed, so after resetting the file
									// read and write again
									file_read_bytes = fread(w_buf,1,packet_size,ptr);
									if (file_read_bytes == packet_size) {
									}
									else {
										// After going to front of file, still short read
										// Serious issue, give up
										return -1;
									}

								}
							}
							else {
								packet = next_packet(version,sweep);
							}
							get_next_packet = false;
						}
						int write_bytes = write(pts,packet,packet_size);
						error_message (DEBUG, "Bytes Sent: %d", write_bytes);
					}
				}
				else if (total_read > packet_size ) {
					total_read = 0;
				}
			}
		}

	}
	error_message (ERROR, "Exiting!");
	close(pts);
	remove(pipename);
	if ( args_info.data_file_given )
		fclose(ptr);
}

unsigned char * next_packet(int version, enum enum_sweep sweep) {
	static unsigned char packet[MAX_PACKET_SIZE];
	static uint16_t counter = 0;
	// Extract separate bytes from counter
	unsigned char lsb = counter & 0xff;
	unsigned char msb = counter >> 8;

	if ( version == 88 ) {
		if ( counter == 0 ) {
			for ( int i = 0; i<102;i++) {
				packet[i] = 0x00;
			}
			packet[0] = 0xb0;
			packet[62] = 0x6c;
			packet[100] = 0x58;
		}


		switch (sweep) {
			case sweep_arg_RPM:
				packet[2] = lsb;
				packet[3] = msb;
				break;
			case sweep_arg_SENSORMV:
				packet[4] = lsb;
				packet[5] = msb;
				break;
			case sweep_arg_BATTMV:
				packet[6] = lsb;
				packet[7] = msb;
				break;
			case sweep_arg_SERVO_MEASURED:
				packet[8] = lsb;
				packet[9] = msb;
				break;
			case sweep_arg_SERVO_REQUESTED:
				packet[10] = lsb;
				packet[11] = msb;
				break;
			case sweep_arg_PROGRAMMINGS:
				packet[12] = lsb;
				packet[13] = msb;
				break;
			case sweep_arg_CH1_MAXADVANCE:
				packet[14] = lsb;
				packet[15] = msb;
				break;
			case sweep_arg_CH2_MAXADVANCE:
				packet[16] = lsb;
				packet[17] = msb;
				break;
			case sweep_arg_CH3_MAXADVANCE:
				packet[18] = lsb;
				packet[19] = msb;
				break;
			case sweep_arg_CH4_MAXADVANCE:
				packet[20] = lsb;
				packet[21] = msb;
				break;
			case sweep_arg_SENSOR_VALUE:
				packet[22] = lsb;
				packet[23] = msb;
				break;
			case sweep_arg_DWELL_OPT:
				packet[24] = lsb;
				packet[25] = msb;
				break;
			case sweep_arg_DWELL:
				packet[26] = lsb;
				packet[27] = msb;
				break;
			case sweep_arg_RESPONSE_NUMBER:
				packet[44] = lsb;
				packet[45] = msb;
				break;
			case sweep_arg_CLUTCH_MASTER:
				packet[50] = lsb % 2;
				break;
			case sweep_arg_SENSOR_TYPE:
				packet[51] = lsb % 3;
				break;
			case sweep_arg_POWER_OUT:
				packet[52] = lsb % 4;
				break;
			case sweep_arg_CH1_ADVANCE:
				packet[57] = lsb;
				break;
			case sweep_arg_CH2_ADVANCE:
				packet[58] = lsb;
				break;
			case sweep_arg_CH3_ADVANCE:
				packet[59] = lsb;
				break;
			case sweep_arg_CH4_ADVANCE:
				packet[60] = lsb;
				break;
			case sweep_arg_LIMITER:
				packet[61] = lsb % 2;
				break;
			case sweep_arg_RETARD:
				packet[63] = lsb % 2;
				break;
			case sweep_arg_START_LIMITER:
				packet[64] = lsb % 2;
				break;
			case sweep_arg_FLAGS_V88_90:
				packet[90] = 1 << lsb;
				break;
			case sweep_arg_FLAGS_V88_91:
				packet[91] = 1 << lsb;
				break;
		}

		unsigned char cs = 0;
		for ( int i = 0; i < 102 - 1; i++ ) {
			cs ^= packet[i];
		}
		cs ^= 0xFF;
		packet[101] = cs;
	}
	if ( version == 96 ) {
		if ( counter == 0 ) {
			for ( int i = 0; i<102;i++) {
				packet[i] = 0x00;
			}
			packet[0] = 0xb0;
			packet[1] = 0xFF;
			packet[114] = 0x6c; // dwell calibration
			packet[119] = 0x04; // 4 cylinders
			packet[143] = 0x3f; // all input options enabled
			packet[150] = 0x60; //version
		}

		
		switch (sweep) {
			case sweep_arg_RPM:
				packet[2] = lsb;
				packet[3] = msb;
				break;
			case sweep_arg_SENSORMV:
				packet[4] = lsb;
				packet[5] = msb;
				break;
			case sweep_arg_SENSOR_VALUE:
				packet[6] = lsb;
				packet[7] = msb;
				break;
			case sweep_arg_BATTMV:
				packet[8] = lsb;
				packet[9] = msb;
				break;
			case sweep_arg_SERVO_MEASURED:
				//packet[8] = lsb;
				//packet[9] = msb;
				break;
			case sweep_arg_SERVO_REQUESTED:
				//packet[10] = lsb;
				//packet[11] = msb;
				break;
			case sweep_arg_PROGRAMMINGS:
				packet[14] = lsb;
				packet[15] = msb;
				break;
			case sweep_arg_CH1_MAXADVANCE:
				packet[18] = lsb;
				packet[19] = msb;
				break;
			case sweep_arg_CH2_MAXADVANCE:
				packet[20] = lsb;
				packet[21] = msb;
				break;
			case sweep_arg_CH3_MAXADVANCE:
				packet[22] = lsb;
				packet[23] = msb;
				break;
			case sweep_arg_CH4_MAXADVANCE:
				packet[24] = lsb;
				packet[25] = msb;
				break;
			case sweep_arg_DWELL_OPT:
				packet[26] = lsb;
				packet[27] = msb;
				break;
			case sweep_arg_DWELL:
				packet[28] = lsb;
				packet[29] = msb;
				break;
			case sweep_arg_RUNTIME:
				packet[30] = lsb;
				packet[31] = msb;
				break;
			case sweep_arg_PROP1:
				packet[42] = lsb;
				packet[43] = msb;
				break;
			case sweep_arg_PROP2:
				packet[44] = lsb;
				packet[45] = msb;
				break;
			case sweep_arg_PROP3:
				packet[46] = lsb;
				packet[47] = msb;
				break;
			case sweep_arg_PROP4:
				packet[48] = lsb;
				packet[49] = msb;
				break;
			case sweep_arg_RESPONSE_NUMBER:
				packet[50] = lsb;
				packet[51] = msb;
				break;
			case sweep_arg_SENSOR_TYPE:
				packet[100] = lsb % 3;
				break;
			case sweep_arg_POWER_OUT:
				packet[109] = lsb % 4;
				break;
			case sweep_arg_CH1_ADVANCE:
				packet[105] = lsb;
				break;
			case sweep_arg_CH2_ADVANCE:
				packet[106] = lsb;
				break;
			case sweep_arg_CH3_ADVANCE:
				packet[107] = lsb;
				break;
			case sweep_arg_CH4_ADVANCE:
				packet[108] = lsb;
				break;
			case sweep_arg_NUM_CYLINDERS:
				packet[119] = lsb % 5;
				break;
			case sweep_arg_FLAGS_V96_140:
				packet[140] = 1 << lsb;
				break;
			case sweep_arg_FLAGS_V96_141:
				packet[141] = 1 << lsb;
				break;
			case sweep_arg_FLAGS_V96_142:
				packet[142] = 1 << lsb;
				break;
			case sweep_arg_FLAGS_V96_143:
				packet[143] = 1 << lsb;
				break;
			case sweep_arg_FLAGS_V96_144:
				packet[144] = 1 << lsb;
				break;
			case sweep_arg_FLAGS_V96_145:
				packet[145] = 1 << lsb;
				break;
		}

		unsigned char cs = 0;
		for ( int i = 0; i < 152 - 1; i++ ) {
			cs ^= packet[i];
		}
		cs ^= 0xFF;
		packet[151] = cs;
	}
	counter++;
	return packet;
}
