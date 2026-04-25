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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print_packet(unsigned char* r_buf, int start, int end, bool* highlights) {
  // TODO add optional parameter to highlight one byte
  //printf(ANSI_COLOR_RED     "This text is RED!"     ANSI_COLOR_RESET "\n");
  for (int i=start;i<=end;i++){
    if (highlights != NULL && highlights[i] == true) {
      printf(ANSI_COLOR_GREEN "%02X" ANSI_COLOR_RESET  " ",r_buf[i]);
    } else {
      printf("%02X ",r_buf[i]);
    }
    if ((i+1)%19 == 0) printf("\n");
  }
  printf("\n\n");
}
int open_pts(char const* pipename)
{
	int pts = posix_openpt(O_RDWR);
	if (pts >=0 ) {
		grantpt(pts);
		unlockpt(pts);
		char const *pts_name = ptsname(pts);
		if ( symlink(pts_name,pipename) < 0 ) {
			error_message (ERROR, "Unable symlink pts. %d : %s", errno, strerror (errno));
			return -1;
		}
		error_message (ERROR, "Vitual serial port opened at:  %s",pipename);
	}
	return pts;
}

unsigned char * next_packet(int version, enum enum_sweep sweep);
bool compare_buffers(bool* results, const unsigned char* vectA, const unsigned char* vectB, const int length);
void copy_packet(const unsigned char * src, unsigned char * dst, int length);
bool is_packet_header(const unsigned char byte);
bool checksum_is_good(const unsigned char *buf, int length);
unsigned char calc_checksum(const unsigned char *buf, int length);

volatile sig_atomic_t time_to_quit;
void signalHandler( int signum ) {
	//error_message(INFO, "Interrupt signal %d received", signum);
	time_to_quit = true;
}

int main(int argc, char *argv[]) {
	// register signal SIGINT and signal handler  
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGHUP, signalHandler);

	int version = 88;
	enum enum_sweep sweep = sweep_arg_RPM;
	LEVEL_DEBUG = ERROR;
	char const* datafilename = "";
	char const* pipename = "/tmp/virtual-tty";

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
	// TODO refactor ptr name
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
	int pts = open_pts(pipename);

  unsigned char id_A[152],id_B[152],prog1_A[152]
                ,prog2_A[152],prog3_A[152],prog4_A[152]
                ,prog5_A[152],prog6_A[152],prog7_A[152]
                ,prog1_B[152],prog2_B[152],prog3_B[152]
                ,prog4_B[152],prog5_B[152],prog6_B[152]
                ,prog7_B[152];

	// Hard-coded example packets
  const unsigned char response[102] = {0xb0,0x00,0x44,0x04,0x4a,0x0f,0x4d,0x30,0x9c,0x00,0x00,0x00,0x17,0x00,0xce,0x00,0xc3,
	                                     0x00,0x00,0x00,0x00,0x00,0x5d,0x00,0x10,0x00,0x4a,0x00,0x00,0x00,0xff,0x00,0xff,0x00,
	                                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
	                                     0x02,0x00,0xf0,0xf1,0xff,0xff,0x6e,0x6f,0x00,0x00,0x00,0x6c,0x00,0x00,0x00,0x00,0x00,
	                                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                                     0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x58,0x30};

	const unsigned char redirect88_96[102] = {0xb0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xd0};

	const unsigned char resp_0x40[152] = {0xc0,0x00,0x00,0x00,0x00,0x1e,0x23,0x14,0x10,0x0a,0x01,0x60,0x00,0x00,0x00,0x00,0x00,0x54,0x43,
					      0x49,0x50,0x34,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x61,0x1e,0x04,0x14,0x62,0xc1,
					      0xa2,0x5f,0x99,0xfa,0x02,0x20,0x0e,0x13,0x04,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2c};

	const unsigned char resp_0x41[152] = {0xc1,0x2a,0x01,0x75,0x10,0x15,0x00,0xc4,0x02,0x65,0x00,0x9c,0x10,0x00,0x00,0x02,0x00,0x05,0x00,
					      0x0a,0x00,0x14,0x00,0x1e,0x00,0x28,0x00,0x3c,0x00,0x50,0x00,0x64,0x00,0x00,0x00,0x02,0x00,0x05,
					      0x00,0x0a,0x00,0x14,0x00,0x1e,0x00,0x28,0x00,0x3c,0x00,0x50,0x00,0x64,0x00,0x1e,0x00,0x32,0x00,
					      0x3c,0x00,0x46,0x00,0x4b,0x00,0x50,0x00,0x55,0x00,0x5a,0x00,0x5f,0x00,0x64,0x00,0x4b,0x00,0x5a,
					      0x00,0x4b,0x00,0x5a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x28,0x00,0x3c,0x00,
					      0x50,0x00,0x14,0x00,0x28,0x00,0x3c,0x00,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0xcd};

	const unsigned char resp_0x42[152] = {0xc2,0x04,0x10,0x01,0x64,0x00,0x01,0x02,0x01,0xc1,0x41,0x01,0x01,
					      0x41,0xc1,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0x64,0x00,
					      0x00,0x32,0x14,0x06,0x03,0x1e,0x64,0x0e,0xb4,0x3c,0x06,0x01,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x0a,0x01,0x01,0x0f,0x0f,0x64,0x64,0x64,0x64,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x28,0x7d,0x8c,0x5a,0x5f,0x8c,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x0a,0x0a,0x00,0x00,0x0a,0x0a,0x00,0x00,0x64,0x64,
					      0x00,0x00,0x14,0x14,0x00,0x00,0x02,0x02,0x02,0x0e,0x0e,0x0e,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x14,0x28,0x3c,0x50,0x64,0x78,0x14,0x28,
					      0x3c,0x50,0x64,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfb};
	const unsigned char resp_0x43[152] = {0xc3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x01,0x01,0x00,0x0f,0x01,0x64,0x00,0x00,0x32,0x32,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x56};
	const unsigned char resp_0x44[152] = {0xc4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					      0x00,0x00,0x0a,0x14,0x1e,0x28,0x3c,0x46,0x50,0x5a,0x64,0x6e,0x78,
					      0x82,0x8c,0x96,0xa0,0x06,0x0f,0x1e,0x28,0x2d,0x32,0x3c,0x46,0x50,
					      0x5a,0x65,0x66,0x6e,0x78,0x8c,0x06,0x0f,0x1e,0x28,0x2d,0x32,0x3c,
					      0x46,0x50,0x5a,0x65,0x66,0x6e,0x78,0x8c,0x29};
	const unsigned char resp_0x45[152] = {0xc5,0x7d,0x78,0x81,0x8d,0x8f,0x8f,0x93,0x95,0x8f,0x8c,0x8b,0x87,
					      0x87,0x87,0x87,0x7d,0x78,0x81,0x8d,0x8f,0x8f,0x93,0x95,0x8f,0x8c,
					      0x8b,0x87,0x87,0x87,0x87,0x7d,0x7d,0x83,0x8f,0x90,0x8f,0x93,0x95,
					      0x8f,0x8c,0x8b,0x87,0x87,0x87,0x87,0x7d,0x7d,0x86,0x8f,0x91,0x8f,
					      0x93,0x95,0x8e,0x8b,0x8b,0x87,0x87,0x87,0x87,0x7d,0x7d,0x89,0x90,
					      0x91,0x90,0x94,0x92,0x8e,0x8b,0x8b,0x87,0x87,0x87,0x87,0x7d,0x7b,
					      0x8d,0x8f,0x91,0x8f,0x90,0x8f,0x8b,0x8b,0x8b,0x87,0x87,0x87,0x87,
					      0x7d,0x7b,0x8e,0x8f,0x8f,0x8e,0x8f,0x8d,0x8b,0x8b,0x8b,0x87,0x87,
					      0x87,0x87,0x7d,0x78,0x8e,0x8d,0x8c,0x8c,0x8c,0x8d,0x8b,0x8b,0x8b,
					      0x87,0x87,0x87,0x87,0x7d,0x78,0x85,0x8a,0x8a,0x8a,0x8a,0x8c,0x8c,
					      0x8c,0x8c,0x87,0x87,0x87,0x87,0x7d,0x78,0x81,0x83,0x89,0x89,0x8b,
					      0x8c,0x8c,0x8c,0x8c,0x87,0x87,0x87,0x87,0x29};
	const unsigned char resp_0x46[152] = {0xc6,0x28,0x2a,0x2c,0x2e,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
					      0x38,0x39,0x39,0x28,0x2a,0x2c,0x2e,0x30,0x31,0x32,0x33,0x34,0x35,
					      0x36,0x37,0x38,0x39,0x39,0x28,0x2a,0x2c,0x2e,0x30,0x31,0x32,0x33,
					      0x34,0x35,0x36,0x37,0x38,0x39,0x39,0x28,0x2a,0x2c,0x2e,0x30,0x31,
					      0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x39,0x28,0x2a,0x2c,0x2e,
					      0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x39,0x28,0x2a,
					      0x2c,0x2e,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x39,
					      0x28,0x2a,0x2c,0x2e,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
					      0x39,0x39,0x28,0x2a,0x2c,0x2e,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
					      0x37,0x38,0x39,0x39,0x28,0x2a,0x2c,0x2e,0x30,0x31,0x32,0x33,0x34,
					      0x35,0x36,0x37,0x38,0x39,0x39,0x28,0x2a,0x2c,0x2e,0x30,0x31,0x32,
					      0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x39,0x39};
	const unsigned char resp_0x47[152] = {0xc7,0x80,0x79,0x81,0x8d,0x8f,0x8f,0x93,0x95,0x8f,0x8c,0x8b,0x87,
					      0x87,0x87,0x87,0x80,0x79,0x81,0x8d,0x8f,0x8f,0x93,0x95,0x8f,0x8c,
					      0x8b,0x87,0x87,0x87,0x87,0x80,0x7a,0x83,0x8f,0x90,0x8f,0x93,0x95,
					      0x8f,0x8c,0x8b,0x87,0x87,0x87,0x87,0x80,0x7a,0x86,0x8f,0x91,0x8f,
					      0x93,0x95,0x8e,0x8b,0x8b,0x87,0x87,0x87,0x87,0x80,0x7e,0x89,0x90,
					      0x91,0x90,0x94,0x92,0x8e,0x8b,0x8b,0x87,0x87,0x87,0x87,0x80,0x7c,
					      0x8d,0x8f,0x91,0x8f,0x90,0x8f,0x8b,0x8b,0x8b,0x87,0x87,0x87,0x87,
					      0x80,0x7c,0x8e,0x8f,0x8f,0x8e,0x8f,0x8d,0x8b,0x8b,0x8b,0x87,0x87,
					      0x87,0x87,0x80,0x79,0x8e,0x8d,0x8c,0x8c,0x8c,0x8d,0x8b,0x8b,0x8b,
					      0x87,0x87,0x87,0x87,0x80,0x79,0x85,0x8a,0x8a,0x8a,0x8a,0x8c,0x8c,
					      0x8c,0x8c,0x87,0x87,0x87,0x87,0x80,0x79,0x81,0x83,0x89,0x89,0x8b,
					      0x8c,0x8c,0x8c,0x8c,0x87,0x87,0x87,0x87,0x29};
 
  // Copy hard-coded packets as our "initial programming"
  copy_packet(resp_0x40,id_A,sizeof(id_A));
  copy_packet(resp_0x41,prog1_A,sizeof(prog1_A));
  copy_packet(resp_0x42,prog2_A,sizeof(prog2_A));
  copy_packet(resp_0x43,prog3_A,sizeof(prog3_A));
  copy_packet(resp_0x44,prog4_A,sizeof(prog4_A));
  copy_packet(resp_0x45,prog5_A,sizeof(prog5_A));
  copy_packet(resp_0x46,prog6_A,sizeof(prog6_A));
  copy_packet(resp_0x47,prog7_A,sizeof(prog7_A));

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
        static bool found_header = false;
				static unsigned char r_buf[MAX_PACKET_SIZE];
        if (!found_header) {
				  int read_bytes = read(pts,&r_buf[0],1);
          if (read_bytes < 0) {
            if ( errno != EINTR ) {
              error_message (ERROR, "error %d : %s", errno, strerror (errno));
            }
            close(pts);
            remove(pipename);
            pts = open_pts(pipename);
            continue;
          }
          else if (read_bytes >= 0) {
            if (is_packet_header(r_buf[0])) {
              found_header = true;
              total_read = read_bytes;
            }
            else continue;
          }
        }

				int read_bytes = read(pts,&r_buf[total_read],packet_size - total_read);
				if (read_bytes < 0) {
					if ( errno != EINTR ) {
						error_message (ERROR, "error %d : %s", errno, strerror (errno));
					}
					close(pts);
					remove(pipename);
					pts = open_pts(pipename);
					continue;
		    }

				else if (read_bytes >= 0) {

					error_message(DEBUG,"Read %d Bytes from tty.", read_bytes);
					total_read += read_bytes;
				}
				if ( version == 96 && total_read == 102 && r_buf[0] == 0x30 && 
						r_buf[100] == 0x58 && r_buf[101] == 0x97 ) {
					// Write redirect packet
					int write_bytes = write(pts,redirect88_96,102);
					error_message (DEBUG, "Sent Redirect: %d", write_bytes);
					total_read = 0;
          found_header = false;
				}
				if (total_read == packet_size) {
					total_read = 0;
          found_header = false;
          if (!checksum_is_good(r_buf,packet_size)) {
						error_message(WARN,"Checksum Error");
            continue;
          }

					// Match normal query packet
					if ( r_buf[0] == 0x30 && r_buf[packet_size-2] == version ) {
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
					// TODO Match other packets

          else {
            int write_bytes = 0;
            bool highlights[MAX_PACKET_SIZE];
            switch (r_buf[0]) {
              // 0x40 starts a read of "internal" programming one packet at a time 
              //   ending with 0x47
              case 0x40:
                error_message (INFO, "Got Programming READ request (byte 1)");
                write_bytes = write(pts,id_A,packet_size);
                break;
              case 0x41:
                error_message (INFO, "Got Programming READ request (byte 2)");
                prog1_A[0] = r_buf[0] + 0x80;
                prog1_A[packet_size-1] = calc_checksum(prog1_A,packet_size);
                write_bytes = write(pts,prog1_A,packet_size);
                break;
              case 0x42:
                error_message (INFO, "Got Programming READ request (byte 3)");
                prog2_A[0] = r_buf[0] + 0x80;
                prog2_A[packet_size-1] = calc_checksum(prog2_A,packet_size);
                write_bytes = write(pts,prog2_A,packet_size);
                break;
              case 0x43:
                error_message (INFO, "Got Programming READ request (byte 4)");
                prog3_A[0] = r_buf[0] + 0x80;
                prog3_A[packet_size-1] = calc_checksum(prog3_A,packet_size);
                write_bytes = write(pts,prog3_A,packet_size);
                break;
              case 0x44:
                error_message (INFO, "Got Programming READ request (byte 5)");
                prog4_A[0] = r_buf[0] + 0x80;
                prog4_A[packet_size-1] = calc_checksum(prog4_A,packet_size);
                write_bytes = write(pts,prog4_A,packet_size);
                break;
              case 0x45:
                error_message (INFO, "Got Programming READ request (byte 6)");
                prog5_A[0] = r_buf[0] + 0x80;
                prog5_A[packet_size-1] = calc_checksum(prog5_A,packet_size);
                write_bytes = write(pts,prog5_A,packet_size);
                break;
              case 0x46:
                error_message (INFO, "Got Programming READ request (byte 7)");
                prog6_A[0] = r_buf[0] + 0x80;
                prog6_A[packet_size-1] = calc_checksum(prog6_A,packet_size);
                write_bytes = write(pts,prog6_A,packet_size);
                break;
              case 0x47:
                error_message (INFO, "Got Programming READ request (byte 8)");
                prog7_A[0] = r_buf[0] + 0x80;
                prog7_A[packet_size-1] = calc_checksum(prog7_A,packet_size);
                write_bytes = write(pts,prog7_A,packet_size);
                break;
              
              // 0x01 Through 0x07 are programming write packets
              //   save to the progX_B variables, compare to progX_A and then
              //   copy back to progX_A
              case 0x01:
                error_message (INFO, "Got Programming WRITE request (byte 1)");
                copy_packet(r_buf,prog1_B,packet_size);
                prog1_B[0] += 0x80;
                prog1_B[packet_size-1] = calc_checksum(prog1_B,packet_size);
                write_bytes = write(pts,prog1_B,packet_size);
                if (compare_buffers(highlights,prog1_A,prog1_B,packet_size)) {
					        print_packet(prog1_B,0,packet_size-1,highlights);
                }
                copy_packet(prog1_B,prog1_A,packet_size);
                break;
              case 0x02:
                error_message (INFO, "Got Programming WRITE request (byte 1)");
                copy_packet(r_buf,prog2_B,packet_size);
                prog2_B[0] += 0x80;
                prog2_B[packet_size-1] = calc_checksum(prog2_B,packet_size);
                write_bytes = write(pts,prog2_B,packet_size);
                if (compare_buffers(highlights,prog2_A,prog2_B,packet_size)) {
					        print_packet(prog2_B,0,packet_size-1,highlights);
                }
                copy_packet(prog2_B,prog2_A,packet_size);
                break;
              case 0x03:
                error_message (INFO, "Got Programming WRITE request (byte 1)");
                copy_packet(r_buf,prog3_B,packet_size);
                prog3_B[0] += 0x80;
                prog3_B[packet_size-1] = calc_checksum(prog3_B,packet_size);
                write_bytes = write(pts,prog3_B,packet_size);
                if (compare_buffers(highlights,prog3_A,prog3_B,packet_size)) {
					        print_packet(prog3_B,0,packet_size-1,highlights);
                }
                copy_packet(prog3_B,prog3_A,packet_size);
                break;
              case 0x04:
                error_message (INFO, "Got Programming WRITE request (byte 1)");
                copy_packet(r_buf,prog4_B,packet_size);
                prog4_B[0] += 0x80;
                prog4_B[packet_size-1] = calc_checksum(prog4_B,packet_size);
                write_bytes = write(pts,prog4_B,packet_size);
                if (compare_buffers(highlights,prog4_A,prog4_B,packet_size)) {
					        print_packet(prog4_B,0,packet_size-1,highlights);
                }
                copy_packet(prog4_B,prog4_A,packet_size);
                break;
              case 0x05:
                error_message (INFO, "Got Programming WRITE request (byte 1)");
                copy_packet(r_buf,prog5_B,packet_size);
                prog5_B[0] += 0x80;
                prog5_B[packet_size-1] = calc_checksum(prog5_B,packet_size);
                write_bytes = write(pts,prog5_B,packet_size);
                if (compare_buffers(highlights,prog5_A,prog5_B,packet_size)) {
					        print_packet(prog5_B,0,packet_size-1,highlights);
                }
                copy_packet(prog5_B,prog5_A,packet_size);
                break;
              case 0x06:
                error_message (INFO, "Got Programming WRITE request (byte 1)");
                copy_packet(r_buf,prog6_B,packet_size);
                prog6_B[0] += 0x80;
                prog6_B[packet_size-1] = calc_checksum(prog6_B,packet_size);
                write_bytes = write(pts,prog6_B,packet_size);
                if (compare_buffers(highlights,prog6_A,prog6_B,packet_size)) {
					        print_packet(prog6_B,0,packet_size-1,highlights);
                }
                copy_packet(prog6_B,prog6_A,packet_size);
                break;
              case 0x07:
                error_message (INFO, "Got Programming WRITE request (byte 1)");
                copy_packet(r_buf,prog7_B,packet_size);
                prog7_B[0] += 0x80;
                prog7_B[packet_size-1] = calc_checksum(prog7_B,packet_size);
                write_bytes = write(pts,prog7_B,packet_size);
                if (compare_buffers(highlights,prog7_A,prog7_B,packet_size)) {
					        print_packet(prog7_B,0,packet_size-1,highlights);
                }
                copy_packet(prog7_B,prog7_A,packet_size);
                break;

              // Other as-of-yet unknown packets for writing programming etc
              default:
					      //print_packet(r_buf,0,packet_size-1,NULL);

            }
          }
				}
				else if (total_read > packet_size ) {
					total_read = 0;
          found_header = false;
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
			case sweep_arg_ALL_MAXADVANCE:
				packet[14] = lsb;
				packet[15] = msb;
				packet[16] = lsb;
				packet[17] = msb;
				packet[18] = lsb;
				packet[19] = msb;
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
			case sweep_arg_ALL_ADVANCE:
				packet[57] = lsb;
				packet[58] = lsb;
				packet[59] = lsb;
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
			packet[110] = 0x04;
			packet[111] = 0x03;
			packet[145] = 0x30;
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
				packet[10] = lsb;
				packet[11] = msb;
				break;
			case sweep_arg_SERVO_REQUESTED:
				packet[12] = lsb;
				packet[13] = msb;
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
			case sweep_arg_ALL_MAXADVANCE:
				packet[18] = lsb;
				packet[19] = msb;
				packet[20] = lsb;
				packet[21] = msb;
				packet[22] = lsb;
				packet[23] = msb;
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
			case sweep_arg_ALL_ADVANCE:
				packet[105] = lsb;
				packet[106] = lsb;
				packet[107] = lsb;
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

void copy_packet(const unsigned char * src, unsigned char * dst, int length)
{
  for (int i = 0; i<length;i++){
    dst[i]=src[i];
  }
}

bool is_packet_header(const unsigned char byte)
{
  switch(byte) {
    // Read Byte
    case 0x30:
      return true;
      break;
    // Read programming
    case 0x40:
      return true;
      break;
    case 0x41:
      return true;
      break;
    case 0x42:
      return true;
      break;
    case 0x43:
      return true;
      break;
    case 0x44:
      return true;
      break;
    case 0x45:
      return true;
      break;
    case 0x46:
      return true;
      break;
    case 0x47:
      return true;
      break;
    // Write programming
    case 0x01:
      return true;
      break;
    case 0x02:
      return true;
      break;
    case 0x03:
      return true;
      break;
    case 0x04:
      return true;
      break;
    case 0x05:
      return true;
      break;
    case 0x06:
      return true;
      break;
    case 0x07:
      return true;
      break;
  }
  return false;
}

/**
	Verify Checksum
	@return@
		true = good
		false = anything else
*/
bool checksum_is_good(const unsigned char *buf, int length)
{
	unsigned char cs = calc_checksum(buf,length);
	if (cs == buf[length-1])
		return true;
	else
		return false;
}

/**
	Calculate Checksum
	@return <unsigned char> Calculated checksum

  @param <unsigned char *> Data to calculate checksum on
  @param@ <int>length of data (will not checksum the last byte)
*/
unsigned char calc_checksum(const unsigned char *buf, int length)
{
	unsigned char cs = 0;
	for ( int i = 0 ; i < length - 1 ; i++ ) {
		cs ^= buf[i];
	}
	cs ^= 0xFF;
  return cs;
}

/**
 * Compare two unsigned char buffers byte for byte. Ignore first and last byte. 
 * 
 * @return true if difference found
 * @param <bool *> result vector, true=not match / false=match (size >= length) 
 * @param <const unsigned char *> Data Vector 1 (size >= length)
 * @param <const unsigned char *> Data Vector 2 (size >= length)
 * @param <const int> Length
 */
bool compare_buffers(bool* results, const unsigned char* vectA, const unsigned char* vectB, const int length)
{
  bool ret=false;
  results[0]=false;
  results[length-1]=false;
  for(int i=1;i<length-1;i++) {
    if (vectA[i] == vectB[i]) {
      results[i] = false;
    }
    else {
      results[i] = true;
      ret = true;
    }
  }
  return ret;
}

