#include <fcntl.h>

int main(int ac, char **av)
{
	char buf[256] = {0};
	int fd = open("/dev/fortytwo", O_RDWR);

	read(fd, buf, 4);
	printf("buf: %s (4)\n", buf);
	read(fd, buf, 4);
	printf("buf: %s (4)\n", buf);
	printf("%d", read(fd, buf, 8));
	printf("buf: %s (8)\n", buf);
	read(fd, buf, 16);
	printf("buf: %s (16)\n", buf);

	write(fd, "ataguiro", 8);
	write(fd, "ataguirf", 8);
	write(fd, "atag", 4);
	write(fd, "ataguiroo", 9);
}
