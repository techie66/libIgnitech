#include "../ignitech.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
	IGNITECH bike ("virtual-tty");
	int read_status;
	for (int i=0;i<65535;i++){
		read_status = bike.read_sync();
		if (read_status < 0 ) {
		    printf("%d,",i);
		    return -1;
		}
		else {
			if ( bike.get_rpm() != i ) {
				printf("RPM: %d,i: %d\n",bike.get_rpm(),i);
				return -1;
			}
		}
	}
	return 0;
}
