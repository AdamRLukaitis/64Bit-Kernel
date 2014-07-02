#include <stdio.h>
#include<syscall.h>

int main(int argc, char* argv[]) 
{

       char a[100];

	printf("\nEnter name : ");
	scanf("%s",a);

	printf("\nYour name is %s",a);

	return 0;
}

