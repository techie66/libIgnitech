#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum e_lvl{NONE,ERROR,WARN,INFO,DEBUG} e_lvl;
extern e_lvl LEVEL_DEBUG;

void error_message(e_lvl err_lvl,char const *fmt, ...);

#endif
