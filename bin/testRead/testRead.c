#include <stdio.h>
#include<syscall.h>
#include<sys/dirent.h>
int main(int argc, char* argv[]) 
{
		int fd = 0;

		if(argc > 2 )
		{
			fd = open(argv[1],7 );
		}
		else
		   {
			printf("Will display default file 'etc/123/test.txt' 's contents.");
			fd = open("etc/123/test.txt",7 );
		   }
		char a ='\0';
		int readbytes =-1; 
		while((readbytes  != 0))
			{
			char line[50] = "";
			
			char *ptr = &a;
			char* linePtr = line;
			do	
			{
				*ptr = '\0'; 
				readbytes = read(fd,ptr,1);
				*linePtr ++ = a;
				
			}while((readbytes  != 0) && a != '\n' );

			printf("\n%s",line);
			}
		close(fd);

}