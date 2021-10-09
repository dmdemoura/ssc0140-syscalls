#ifndef AMN_SSC0140_SYSCALL_HELLO_WORLD
#define AMN_SSC0140_SYSCALL_HELLO_WORLD

#include <asm-generic/unistd.h>
#include <unistd.h>

#define hello_world(x) syscall(__NR_hello_world, x);

#endif //AMN_SSC0140_SYSCALL_HELLO_WORLD
