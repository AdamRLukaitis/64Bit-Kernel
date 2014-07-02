#include <stdio.h>
#include<syscall.h>

int main(int argc, char* argv[]) 
{
	printf("\nTrying to read from Kernel address 0xfffffffff99fffff");

	char *a = (char*)0xfffffffff99fffff;

	char b = *a;
	
	printf("%c",b);

	return 0;
}