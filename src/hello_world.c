#include <stdio.h>
#include <string.h>

#include <hello_world.h>

const int MSG_LENGTH = 81;

int main() {

	printf("Write a message (At most 80 characters, only printable ASCII characters):\n"); 

	char msg[MSG_LENGTH];
	if(!fgets(msg, MSG_LENGTH, stdin)) goto error;
	
	for (int i = 0; msg[i] != '\0'; i++) { 
		const char c = msg[i];
		if ((c < ' ' || c >= 127) && c != '\n') {
			
			printf("Character %c (Code %#x) is not allowed!\n", c, c);
			return 1;
		}
	}

	printf("Calling hello_world syscall...\n");
	if(hello_world(msg) == -1) goto error;

	printf("syscall executed!\n"
			"Run \"dmesg\" to see output."
			"(You might need to run dmesg as root/with sudo)\n");

	return 0;
error:
	printf("\tDue to: %s\n", strerror(errno));
	return 1;
}
