#include "../src/ignitech.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
	printf("Starting to read\n");
	IGNITECH bike ("battmv-tty");
	int read_status;
	for (int i=0;i<65535;i++){
		read_status = bike.read_sync();
		if (read_status < 0 ) {
		    printf("%d,",i);
		    return -1;
		}
		else {
			if ( bike.get_battery_mV() != i ) {
				printf("BATT mV: %d,i: %d\n",bike.get_battery_mV(),i);
				return -1;
			}
		}
		if ( i % 16384 == 0 ) {
			int j = i/16384 * 25;
			printf("%d%%\n",j);
		}
		if ( i % 512 == 0 ) {
			printf("*");
			fflush(stdout);
		}
	}
	return 0;
}
