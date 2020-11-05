#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "error_handling.h"


volatile sig_atomic_t time_to_quit;
void signalHandler( int signum ) {
	error_message(INFO, "Interrupt signal %d received", signum);
	time_to_quit = true;

	// cleanup and close up stuff here  
	// terminate program  

	//exit(signum); 
}

int main() {
	// register signal SIGINT and signal handler  
	signal(SIGINT, signalHandler);

	LEVEL_DEBUG = DEBUG;

	const unsigned char response[102] = {0xb0,0x00,0x44,0x04,0x4a,0x0f,0x4d,0x30,0x9c,0x00,0x00,0x00,0x17,0x00,0xce,0x00,0xc3,0x00,0x00,0x00,0x00,0x00,0x5d,0x00,0x10,0x00,0x4a,0x00,0x00,0x00,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0xf0,0xf1,0xff,0xff,0x6e,0x6f,0x00,0x00,0x00,0x6c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x58,0x30};
	unsigned char w_buf[102];
	FILE *ptr;
	ptr = fopen("serial_dumps/test.bin","rb");  // r for read, b for binary
	if (ptr < 0) {
		error_message (ERROR, "error %d : %s", errno, strerror (errno));
	}
	int packet_size = 102;
	int total_read = 0;
	while(!time_to_quit) {
		// wait for query then respond
		fd_set	readset;
		FD_ZERO(&readset);
		FD_SET(0,&readset);
		int select_result = 0;
		struct timeval	timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		select_result = select(1, &readset, NULL, NULL, &timeout);
		if (select_result < 0) {
			error_message (WARN, "error %d : %s", errno, strerror (errno));
		}
		if (select_result > 0) {
			if (FD_ISSET(0,&readset)) {
				unsigned char r_buf[packet_size];
				int read_bytes = read(0,&r_buf[total_read],packet_size - total_read);
				if (read_bytes < 0) {
					error_message (WARN, "error %d : %s", errno, strerror (errno));
		        }

				else if (read_bytes > 0) {
					error_message(DEBUG,"Read %d Bytes from tty.", read_bytes);
					total_read += read_bytes;
				}
				if (total_read == packet_size) {
					total_read = 0;
					if ( r_buf[0] == 0x30 && r_buf[100] == 0x58 && r_buf[101] == 0x97 ) {
						error_message(DEBUG,"Sending Response");
						// TODO create our own custom response packet here (in a separate function) (option)
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
					}
				}
				else if (total_read > packet_size ) {
					total_read = 0;
				}
			}
		}

	}
}
