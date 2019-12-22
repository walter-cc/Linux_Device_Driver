#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define DEMO_DEV_NAME "/dev/demo_drv" // the name =  demo_drv in the command "sudo mknod /dev/demo_drv c 244 0"

int main()
{
	char buffer[64];
	int fd;

	fd = open(DEMO_DEV_NAME, O_RDONLY); // open the device node "DEMO_DEV_NAME"
	if (fd < 0) {
		printf("open device %s failded\n", DEMO_DEV_NAME);
		return -1;
	}

	read(fd, buffer, 64);
	close(fd);

	return 0;
}
