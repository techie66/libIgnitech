#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdarg.h>
#include <csignal>
#include <stdlib.h>

enum e_lvl{NONE,ERROR,WARN,INFO,DEBUG};
extern volatile sig_atomic_t time_to_quit;
extern e_lvl LEVEL_DEBUG;

void error_message(e_lvl err_lvl,char const *fmt, ...);
void signalHandler( int signum );

#endif
