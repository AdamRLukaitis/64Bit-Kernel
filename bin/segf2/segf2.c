#include <stdio.h>
#include<syscall.h>

int main(int argc, char* argv[]) 
{
	printf("\nTrying to write to Kernel address 0xfffffffff99fffff");

	char *a = (char*)0xfffffffff99fffff;

	*a = 'A';

	return 0;
}