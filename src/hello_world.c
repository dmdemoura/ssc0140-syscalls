#include <hello_world.h>
#include <stdio.h>

int main() {

	printf("Write a message (At most 80 characters):\n"); 

	char msg[81];
	scanf("%80s", msg);

	hello_world(msg);

	printf("syscall executed!\n"
			"Run \"dmesg\" to see output."
			"(You might need to run dmesg as root/with sudo)\n");
}
