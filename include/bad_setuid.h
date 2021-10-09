#ifndef AMN_SSC0140_SYSCALL_BAD_SETUID
#define AMN_SSC0140_SYSCALL_BAD_SETUID

#include <linux/unistd.h>
#include <unistd.h>

#include <errno.h>

/*
 * Works like setuid(), but with no permission checking.
 * So any user can change into any other user, regardless of normal security measures.
 *
 * @param uid The new effective, saved and filesystem uid to assume. 
 * @return Returns 0 on success, -1 on failure (Also sets errno).
 */
static inline int bad_setuid(uid_t uid) {
	errno = 0;
	const long result = syscall(__NR_bad_setuid, uid);
	
	if (result >= 0) {
		return 0;
	} else {
		errno = -result;
		return -1;
	}
}

#endif //AMN_SSC0140_SYSCALL_BAD_SETUID
