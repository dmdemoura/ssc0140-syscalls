#include <linux/unistd.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <pwd.h>

#include <stdio.h>

#include <errno.h>
#include <string.h>

int main(int argc, char** argv)
{
	if (argc > 2) {

		printf("Usage:\n\tbad_su <user>\nIf <user> is not given, root is assumed.\n");
		return 1;
	}

	const char* username = argc == 2 ? argv[1] : "root";
	errno = 0;
	const struct passwd* user_info = getpwnam(argv[1]);

	if (!user_info) {

		printf("Failed to uid for username: %s\n", argv[1]);
		goto error;
	}

	const uid_t new_uid = user_info->pw_uid;

	printf("Trying to change user to %s with UID %d!\n", username, new_uid);

	const long result = syscall(__NR_bad_setuid, new_uid);

	if (result >= 0) {
		printf("Changed user!\n");	
	} else {

		printf("Failed to change user!\n");
		errno = -result;
		goto error;
	}

	char* args[] = { "sh" };
	execv(user_info->pw_shell, args);

	return 0;
error:
	printf("\tDue to: %s\n", strerror(errno));
	return 1;
}
