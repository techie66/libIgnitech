#include "error_handling.h"

void error_message(e_lvl err_lvl, char const *fmt, ...) {


	// Handle error messages
	va_list args;
	va_start(args, fmt);
	if (err_lvl <= LEVEL_DEBUG ) {
		while (*fmt != '\0') {
			if (*fmt == '%') {
			++fmt;	
				if (*fmt == 'd') {
					int i = va_arg(args, int);
					printf( "%d", i ) ;
				} else if (*fmt == 'c') {
					int c = va_arg(args, int);
					printf( "%c", (char)c ) ;
				} else if (*fmt == 'f') {
					double d = va_arg(args, double);
					printf( "%f", d ) ;
				} else if (*fmt == 's') {
					const char *str = va_arg(args, const char*);
					printf( "%s",str );
				}
			}
			else {
				printf( "%c",(char)*fmt);
			}
			++fmt;
		}
		printf("\n");
	}
	va_end(args);
}

void signalHandler( int signum ) {
	error_message(INFO, "Interrupt signal %d received", signum);
	time_to_quit = true;

	// cleanup and close up stuff here  
	// terminate program  

	//exit(signum); 
}
