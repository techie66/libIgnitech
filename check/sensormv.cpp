#include "../ignitech.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
	IGNITECH bike ("../example/virtual-tty");
	int read_status;
	for (int i=0;i<65535;i++){
		read_status = bike.read_sync();
		if (read_status < 0 ) {
		    printf("%d,",i);
		}
		else {
			if ( bike.get_sensor_mV() != i ) {
				printf("sensor: %d,i: %d\n",bike.get_sensor_mV(),i);
			}
		}
	}
}
