#include <stdio.h>
#include<syscall.h>

int main(int argc, char* argv[]) 
{
	
	char *a = (char *)malloc(100);

	printf("\nEnter Name :");
	scanf("%s",a);

	printf("\nYour Name is : %s",a);
	return 0;
}