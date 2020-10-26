#include "../ignitech.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
	IGNITECH bike ("./virtual-tty");
	//IGNITECH bike ("/dev/ignitech");
	ignitech_t bike_status;
	int read_status;
	//bike.enable_raw_dump("dump.bin");
	bike.enable_debug();
	while(1){
	printf("Example passing our own ignitech_t struct.\n");
	read_status = bike.read_sync(bike_status);
	if (read_status < 0 ) {
	    printf("Error: %d\n", read_status);
	}
	else {
		printf("RPM: %d\n",bike_status.rpm);
		printf("MAP (kpa): %d\n",bike_status.map_kpa);
		printf("MAP (mV): %d\n",bike_status.map_mV);
		printf("Battery: %2.1f\n",(bike_status.battery_mV/1000.0));
	}

	printf("Example using internal ignitech_t struct.\n");
	read_status = bike.read_sync();
	if (read_status < 0 ) {
	    printf("Error: %d\n",read_status);
	}
	else {
		printf("RPM: %d\n",bike.get_rpm());
		printf("MAP (kpa): %d\n",bike.get_map_kpa());
		printf("MAP (mV): %d\n",bike.get_map_mV());
		printf("Battery: %2.1f\n",(bike.get_battery_mV()/1000.0));
	}
	usleep(1000000);
	}
}
