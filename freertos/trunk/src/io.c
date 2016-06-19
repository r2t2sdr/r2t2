#include <unistd.h>
#include <sys/stat.h>
#include "assert.h"

// dummy function for libc

ssize_t _write(int fildes, const void *buf, size_t nbyte) {
	assert(0);
	return 0;
}

int _open(const char *path, int oflag, ...) {
	assert(0);
	return 0;
}

int _close(int fildes) {
	assert(0);
	return 0;
}

int _fstat(int fildes, struct stat *buf) {
	assert(0);
	return 0;
}

int _isatty(int fd) {
	assert(0);
	return 0;
}

off_t _lseek(int fildes, off_t offset, int whence) {
	assert(0);
	return 0;
}

ssize_t _read(int fildes, void *buf, size_t nbyte) {
	assert(0);
	return 0;
}
