#include <stdio.h>
#include<syscall.h>






int main(int argc, char* argv[]) 
{
	int a = fork();
	char *env[] = {"\0"};

	if(a > 0)
	{
		printf("\nParent ProcessID : %d",GetProcessId());

		waitpid(a);

		printf("\nAfter Child Process has exited");

		printf("\nParent : Before Sleep");
	
		sleep(10);
			
		printf("\nParent : After Sleep");
		

	}
	else
	{
		printf("\nChild ProcessID : %d",GetProcessId());
		
		char *inp[] = {"bin/child","\0"};

		execvpe(inp[0],inp,env);
	}

	return 0;

}
