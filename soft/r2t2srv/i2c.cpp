#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stropts.h>
#include <linux/i2c-dev.h>
#include "i2c.h"

int writeI2C(int8_t bus, int8_t addr, uint8_t *val, int len) {
	char devstr[32];
	int fd,ret;

	sprintf (devstr, "/dev/i2c-%i", bus);
	fd = open(devstr, O_RDWR);

	if (fd < 0) {
		printf("Error opening file %s: %s\n", devstr, strerror(errno));
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		printf("ioctl write error: %s\n", strerror(errno));
		return -1;
	}

	ret = write(fd, val, len);

	close(fd);
	return ret;
} 

int readI2C(int8_t bus, int8_t addr, uint8_t reg) {
	char devstr[32];
	uint8_t val;
	int fd,ret;

	sprintf (devstr, "/dev/i2c-%i", bus);
	fd = open(devstr, O_RDWR);

	if (fd < 0) {
		printf("Error opening file: %s\n", strerror(errno));
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		printf("ioctl read error: %s\n", strerror(errno));
		return -1;
	}

	ret = write(fd, &reg, 1);
	if (ret<=0)
		return ret;

	ret = read(fd, &val, 1);
	if (ret<=0)
		return ret;
	return val;

	close(fd);
} 

