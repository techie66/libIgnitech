#include "error_handling.h"
e_lvl LEVEL_DEBUG = WARN;
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
					fprintf(stderr, "%d", i ) ;
				} else if (*fmt == 'c') {
					int c = va_arg(args, int);
					fprintf(stderr, "%c", (char)c ) ;
				} else if (*fmt == 'f') {
					double d = va_arg(args, double);
					fprintf(stderr, "%f", d ) ;
				} else if (*fmt == 's') {
					const char *str = va_arg(args, const char*);
					fprintf(stderr, "%s",str );
				}
			}
			else {
				fprintf(stderr, "%c",(char)*fmt);
			}
			++fmt;
		}
		fprintf(stderr,"\n");
	}
	va_end(args);
}

