#ifndef AMN_SSC0140_SYSCALL_HELLO_WORLD
#define AMN_SSC0140_SYSCALL_HELLO_WORLD

#include <linux/unistd.h>
#include <unistd.h>

#include <errno.h>

/*
 * Prints a message to the kernel log.
 *
 * @param msg An ASCII encoded string to be output to dmesg. Must be null terminated.  
 * @return Returns 0 on success, -1 on failure (Also sets errno).
 */
static inline int hello_world(const char* msg) {
	errno = 0;
	const long result = syscall(__NR_hello_world, msg);
	
	if (result >= 0) {
		return 0;
	} else {
		errno = -result;
		return -1;
	}
}

#endif //AMN_SSC0140_SYSCALL_HELLO_WORLD
