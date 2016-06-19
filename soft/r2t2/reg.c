#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


int setMem(uint32_t addr, uint32_t val) {
	uint32_t *pmem = NULL;
	uint32_t page_addr = addr & ~(sysconf(_SC_PAGESIZE)-1);
	uint32_t offs = addr - page_addr;
	int ret = -1;
	int fd;

#ifdef DEBUG
	printf ("set %08x to %08x\n",addr, val);
#endif

	if ((fd=open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		perror("error open /dev/mem:");
		return -1;
	}
	pmem = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_WRITE, MAP_SHARED, fd, page_addr);

	if (pmem==MAP_FAILED) {
		perror("error mmap:");
		goto error;
	}

	pmem[offs/sizeof(uint32_t)] = val;

	ret = 0;
error:
	if (pmem) {
		ret = munmap(pmem, sysconf(_SC_PAGESIZE));
		if (ret) 
			perror("error munmap:");
	}
	ret = close(fd);
	if (ret) 
		perror("error close:");
	return ret;
}

uint32_t readMem(uint32_t addr) {
	uint32_t *pmem = NULL;
	uint32_t page_addr = addr & ~(sysconf(_SC_PAGESIZE)-1);
	uint32_t offs = addr - page_addr;
	int ret=0, fd;

	if ((fd=open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		perror("error open /dev/mem:");
		return 0;
	}
	pmem = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_WRITE, MAP_SHARED, fd, page_addr);

	if (!pmem) {
		perror("error mmap:");
		goto error;
	}

	ret = pmem[offs/sizeof(uint32_t)];

error:
	if (pmem && munmap(pmem, sysconf(_SC_PAGESIZE))!=0) {
		perror("error munmap:");
		return 0;
	}
	if (close(fd)!=0) {
		perror("error close:");
		return 0;
	}
	return ret;
}

