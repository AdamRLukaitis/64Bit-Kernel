#include <stdlib.h>
#include<string.h>
#include<stdio.h>
#include<defs.h>

void _start(char *arguements,char *envs) {
	int argc = 1;
	char* argv[10];

	if(arguements != NULL)	
		argc = parse(arguements,argv);

	char* envp[10];
	int res;
	res = main(argc, argv, envp);
	exit(res);
}
