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
				unsigned char r_buf[102];
				int read_bytes = read(0,r_buf,102);
				if (read_bytes < 0) {
					error_message (WARN, "error %d : %s", errno, strerror (errno));
		        }

				else if (read_bytes > 0) {
					error_message(DEBUG,"Read %d Bytes from tty, sending response.", read_bytes);
				}
				if (read_bytes == 102) {
					int file_read_bytes = fread(w_buf,1,102,ptr); // read 102 bytes to our buffer
					if (file_read_bytes == 102) {
						int write_bytes = write(1,w_buf,102);
						error_message (DEBUG, "Bytes Sent: %d", write_bytes);
					}
					else if (file_read_bytes < 102 ) {
						error_message (DEBUG, "error %d : %s:: Bytes Read: %d, resetting file.", errno, strerror (errno),file_read_bytes);
						rewind(ptr);
					}
				}
			}
		}

	}
}
