#include<sys/sysCallManager.h>
#include<sys/writeUtil.h>
#include <sys/mmManager.h>
#include <sys/file.h>
#include <sys/dirent.h>
#include <string.h>

char currentPath[100] = "";
char envPaths[200] = "";
char* lastPtr = envPaths;

struct registers_t* g_regVals;



void GetCurrentPath()
{
    char *str = (char*)g_regVals->rbx;
    char *str1 = currentPath;

	*str++ = '~';
        while(*str1)
        	*str++ = *str1++;
	 *str++ = ']';
        *str = '\0';
}

//adds endind "/" if required
void addEndingSlash(char *ptr)
{
	if(strlen(ptr) > 0)
	{
		while(*ptr != '\0')
			ptr= ptr+1;
		if(*(ptr -1) != '/')
		     *ptr++ = '/';
	}
		
}

void UpdateEnvPath()
{
	char *str = (char*)g_regVals->rbx;

	//only supporting PATH
	if(starts_with(str,"PATH=") == 1)
	{
		char temp[100]="";
		substring(temp,str,5,strlen(str));

		char *newEnv = temp;
		if(*newEnv  == '/')//should be full path
		{
		
			newEnv  +=1;
			DIR * dir =  opendir(newEnv);

	 		if(dir  != NULL)
	 		{
				int i=0;
				for(i=0;i<200;i++)
					envPaths[i] = '\0';

				lastPtr =envPaths;

				while(*newEnv)
					*lastPtr ++ = *newEnv ++;

				addEndingSlash(envPaths);

			}
			else
			  printf("\n%s : Directory not found.",temp);
			
			closedir(dir );

		}
		else if(starts_with(temp,"$PATH:") == 1)
		{
			char temp1[100]="";
			substring(temp1,temp,6,strlen(temp));

			char *newenv = temp1;
			if(*newenv == '/')//should be full path
			{
				newenv +=1;
				DIR * dir =  opendir(newenv);

	 			if(dir  != NULL)
	 			{
					if(lastPtr  == envPaths)//first one
					{
						while(*newenv)
							*lastPtr ++ = *newenv++;

						addEndingSlash(envPaths);

					}
					else//new one being added
					{
						*lastPtr ++ = ':';
						while(*newenv)
							*lastPtr ++ = *newenv++;

						addEndingSlash(envPaths);
					}
				}
				else
			  		printf("\n%s : Directory not found.",temp);
			
				closedir(dir);
			}
			else
				printf("\n%s : Expects an absolute path.",temp);


		}
		else
			printf("\n%s : Expects an absolute path.",temp);

	}

}
void UpdateCurrentPath()
{
    char *newPath = (char*)g_regVals->rbx;

    if(*newPath == '/')
    {
	newPath = newPath+1;
	char temp[100] = "";
	char *tempPath = temp;
	while(*newPath)
			*tempPath++ = *newPath++;

	
	DIR * dir =  opendir(temp);

	 if(dir  != NULL)
	 {
		int i=0;
		for(i=0;i<100;i++)
			currentPath[i] = '\0';
		
		char *str1 = currentPath;
		char *tempPath = temp;
		while(*tempPath )
			*str1++ = *tempPath++;
		addEndingSlash(currentPath);
	 }
	 else
		printf("\n%s : Directory not found.",temp);
			
	closedir(dir);
    }
    else if(*newPath == '.')
    {
	 if(*(newPath+1) == '.') //parent directory case
	 {
		char temp[100] = "";
		char parent[100] = "";
		
		if(strlen(currentPath) < 1)
			return;
		
		strncpy(temp,currentPath,strlen(currentPath)-1);
		temp[strlen(currentPath)-1] = '\0';
		int index = lastIndexOf (temp, "/");
		
		if(index == -1 || index == strlen(currentPath)-1)
		{
			parent[0] = '\0';
		}

			substring(parent,currentPath, 0, index);
			char *tempPath = (parent+index);
			newPath = newPath+2;
			while(*newPath )
        			*tempPath++ = *newPath++;

			DIR * dir =  opendir(parent);

	 		if(dir  != NULL)
	 		{
				int i=0;
				for(i=0;i<100;i++)
					currentPath[i] = '\0';

				char *cur = currentPath;
      				char *tempPath = parent;

				while(*tempPath)
        				*cur++ = *tempPath++;

				addEndingSlash(currentPath);
	 		}
			else
				printf("\n%s : Directory not found.",parent);
			
			closedir(dir);

	 }
	 else //current directory case
	 {
		char temp[100] = "";
		char *cur = currentPath;
      		char *tempPath = temp;
	
		while(*cur)
        		*tempPath++ = *cur++;

		newPath = newPath+2;

		while(*newPath )
        		*tempPath++ = *newPath++;

		DIR * dir =  opendir(temp);

	 	if(dir  != NULL)
	 	{
			int i=0;
			for(i=0;i<100;i++)
				currentPath[i] = '\0';

			char *cur = currentPath;
      			char *tempPath = temp;

			while(*tempPath)
        			*cur++ = *tempPath++;
			
			addEndingSlash(currentPath);
	 	}
		else
			printf("\n%s : Directory not found.",temp);
			
		closedir(dir);
	 }
    }
    else
    {
      char temp[100] = "";
      char *cur = currentPath;
      char *tempPath = temp;
	
	while(*cur)
        	*tempPath++ = *cur++;
	
	while(*newPath)
			*tempPath++ = *newPath++;

	DIR * dir =  opendir(temp);

	 if(dir  != NULL)
	 {
		int i=0;
		for(i=0;i<100;i++)
			currentPath[i] = '\0';

		char *cur = currentPath;
      		char *tempPath = temp;

		while(*tempPath)
        	*cur++ = *tempPath++;

		addEndingSlash(currentPath);
	 }	
	 else
		printf("\n%s : Directory not found.",temp);

	
	closedir(dir);
    }

    
  

}

static void *syscalls[25] =
 {
    &schedule,
    &ExitProcess,
    &Fork,
    &writeSysCall,
    &GetCurrentProcessId,
    &sleepSysCall,
    &execSysCall,
    &memMap,
    &waitSysCall,
    &waitPidSysCall,
    &readSysCall,
    &procListSysCall,
    &openSysCall,
    &opendirSysCall,
    &readdirSysCall,
    &closedirSysCall,
    &closeSysCall,
    &GetCurrentPath,
    &UpdateCurrentPath,
    &UpdateEnvPath,
    &ulimit
 };


void sysHandler(struct registers_t *regs)
{
   g_regVals = regs;


   void *location = syscalls[regs->rax];
	__asm volatile (
     "call *%0; \n\t" 
    :: "r" (location));
}

