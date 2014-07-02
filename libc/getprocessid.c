#include<syscall.h>

int GetProcessId()
{
	 return __syscall0(SYSCALL_GETID);
}