#include "../src/ignitech.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
	printf("Starting to read\n");
	IGNITECH bike ("maxadvance-tty");
	int read_status;
	for (int i=0;i<256;i++){
		read_status = bike.read_sync();
		if (read_status < 0 ) {
		    printf("%d,",i);
		    return -1;
		}
		else {
			if ( bike.get_advance1_max() != i ) {
				printf("MaxAdvance1: %d,i: %d\n",bike.get_advance1_max(),i);
				return -1;
			}
			if ( bike.get_advance2_max() != i ) {
				printf("MaxAdvance2: %d,i: %d\n",bike.get_advance2_max(),i);
				return -1;
			}
			if ( bike.get_advance3_max() != i ) {
				printf("MaxAdvance3: %d,i: %d\n",bike.get_advance3_max(),i);
				return -1;
			}
			if ( bike.get_advance4_max() != i ) {
				printf("MaxAdvance4: %d,i: %d\n",bike.get_advance4_max(),i);
				return -1;
			}
		}
	}
	return 0;
}
