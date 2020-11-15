#include "../src/ignitech.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
	printf("Starting to read\n");
	IGNITECH bike ("advance-tty");
	int read_status;
	for (int i=0;i<256;i++){
		read_status = bike.read_sync();
		if (read_status < 0 ) {
		    printf("%d,",i);
		    return -1;
		}
		else {
			if ( bike.get_advance1() != i - 100 ) {
				printf("Advance1: %d,i: %d\n",bike.get_advance1(),i);
				return -1;
			}
			if ( bike.get_advance2() != i - 100 ) {
				printf("Advance2: %d,i: %d\n",bike.get_advance2(),i);
				return -1;
			}
			if ( bike.get_advance3() != i - 100 ) {
				printf("Advance3: %d,i: %d\n",bike.get_advance3(),i);
				return -1;
			}
			if ( bike.get_advance4() != i - 100 ) {
				printf("Advance4: %d,i: %d\n",bike.get_advance4(),i);
				return -1;
			}
		}
	}
	return 0;
}
