#include<syscall.h>



uint64_t  fork()
{
	return __syscall0(SYSCALL_FORK);
}