#include<syscall.h>

void ulimit(uint64_t stackSize)
{
	__syscall1(SYSCALL_ULIMIT,stackSize);
}

