#include<syscall.h>   
#include<sys/dirent.h> 
#include<stdio.h>
#include<string.h>

int main(int argc, char* argv[])
{

    DIR *dir;
    struct dirent *dirfile;


	if(strcmp(argv[1],"") == 0)
	{
		char path[100];
		getPath(path);

		int len = strlen(path+1);
		path[len] = '\0';
		dir = opendir(path+1);
	}
	else
	{	char *path = argv[1];
		if(*path == '/')
			path = path+1;
		dir = opendir(path);
	}
    if(dir != NULL)
    {
    int dirLen = strlen(dir->dirname);
    while((dirfile = readdir(dir)) != NULL)
    {
	 char temp[100] = "";
        substring(temp,dirfile->d_name,dirLen,strlen(dirfile->d_name));
        printf("\n%s", temp);
    }
    closedir(dir);
    }
    else
    {
   	printf("\n%s : directory not found.",argv[1]);
    }





}