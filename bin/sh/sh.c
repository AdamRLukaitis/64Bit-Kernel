#include <stdio.h> 
#include <syscall.h> 
#include <string.h>



void readcommand(char * line)
{
   do {
      read(0, line, 100);
   } while ( * line == '\0');

}

void getFullPath(char * inp, char * fullPath)
{
   char path[100];

   getPath(path);

   int len = strlen(path + 1);
   path[len] = '\0';
   char * pathPtr = path + 1;
   while ( * pathPtr != '\0') * fullPath++ = * pathPtr++;


   if ( * inp == '.')
      inp = inp + 2;
   while ( * inp != '\0') * fullPath++ = * inp++;

}


int main(int argc, char * argv[])
{

   char *env[] = {"PATH=bin/","\0"};
   //char * env[] = {
   //   "\0"
   //};


   int pid = 0;

   while (1)
   {
      char path[100];
      char buff[50] = "";
      getPath(path);
      printf("\n%s$", path);

      readcommand(buff);
      char copy[100] = "";
      char fullpath[100] = "";

      strcpy(copy, buff);


      char * tempPtr = NULL;
      if ( * buff == '/') //absolute
         tempPtr = buff + 1;
      else
      {
         getFullPath(buff, fullpath);
         tempPtr = fullpath;
      }

      //checking if it is a shebang file first
      int fd = open(tempPtr, 7);

      if (fd > 2)
      {

         int isShebang = 0;
         char temp1[2] = "";

         int readbytes = read(fd, temp1, 2);

         readbytes = readbytes;
         if (strcmp("#!", temp1) == 0)
            isShebang = 1;

         isShebang = isShebang;


         if (isShebang == 1)
         {

            char executable[50] = "";
            char a = '\0';
            char * ptr = & a;
            char * exePtr = executable;
            do { * ptr = '\0';

               readbytes = read(fd, ptr, 1);
               * exePtr++ = a;

            } while ((readbytes != 0) && a != '\n');

            if (starts_with(executable, "/bin/sh") == 1)
            {

               char * args[] = {
                  "         ", "          ", "          ", "           "
               };


               while ((readbytes != 0))
               {
                  char line[50] = "";
                  char a = '\0';
                  char * ptr = & a;
                  char * linePtr = line;
                  do { * ptr = '\0';

                     readbytes = read(fd, ptr, 1);
                     * linePtr++ = a;

                  } while ((readbytes != 0) && a != '\n');

                  if (starts_with(line, "exit") == 1)
                  {
                     break;
                  }

                  int res = parse(line, args);

                  if (strcmp(args[0], "cd") == 0)
                  {
                     updatePath(args[1]);
                  }
                  else if (strcmp(args[0], "ulimit") == 0)
                  {
                     if (strcmp(args[1], "-s") == 0)
                     {
                        int size = atoi(args[2]);
                        ulimit(size);
                     }
                  }
                  else if (strcmp(args[0], "setenv") == 0)
                  {
                     updateEnvPath(args[1]);
                  }
                  else
                  {

                     int isBgProcess = 0;
                     if (strlen(args[res - 1]) == 1)
                        if (args[res - 1][0] == '&')
                           isBgProcess = 1;
                     pid = fork();

                     if (pid > 0)
                     {
                        if (isBgProcess != 1)
                           waitpid(pid);

                     }
                     else
                     {
                        int res = execvpe(args[0], args, env);

                        if (res == -1)
                        {
                           printf("\n%s : Command not found!!!!", args[0]);
                           exit(0);
                        }
                     }
                  }
               }
            }
	     else
	     {
		
		char * args[] = {
                  "         ", "          ", "          ", "           "
               };

		while ((readbytes != 0))
               {
                  char line[50] = "";
                  char a = '\0';
                  char * ptr = & a;
                  char * linePtr = line;
                  do { * ptr = '\0';

                     readbytes = read(fd, ptr, 1);
                     if(a != '\n')
				*linePtr++ = a;

                  } while ((readbytes != 0) && a != '\n');

                  if (starts_with(line, "exit") == 1)
                  {
                     break;
                  }
		    else
		    {

			char tempFull[100] = "";

			char *start = executable;
			char *full = tempFull;
			char *lin = line;
			while(*start)
				*full ++ = *start++;

			*full ++ = ' ';

			while(*lin)
				*full ++ = *lin++;

			
  			parse(tempFull, args);
			
			int pid = fork();

                     if (pid > 0)
                     {
                           waitpid(pid);
                     }
                     else
                     {
                        int res = execvpe(args[0], args, env);

                        if (res == -1)
                        {
                           printf("\n%s : Command not found!!!!", args[0]);
                           exit(0);
                        }
                     }


		    }
		}

	     }


            close(fd);
            continue;
         }

      }



      int res = parse(copy, argv);




      if (strcmp(argv[0], "cd") == 0)
      {
         updatePath(argv[1]);
      }

      else if (strcmp(argv[0], "ulimit") == 0)
      {
         if (strcmp(argv[1], "-s") == 0)
         {
            int size = atoi(argv[2]);
            ulimit(size);
         }
      }
      else if (strcmp(argv[0], "exit") == 0)
      {
         if (GetProcessId() != 1)
         {
            exit(0);
         }
      }
      else if (strcmp(argv[0], "setenv") == 0)
      {
         updateEnvPath(argv[1]);
      }
      else
      {

         int isBgProcess = 0;
         if (strlen(argv[res - 1]) == 1)
            if (argv[res - 1][0] == '&')
               isBgProcess = 1;
         pid = fork();

         if (pid > 0)
         {
            if (isBgProcess != 1)
               waitpid(pid);

         }
         else
         {
            int res = execvpe(argv[0], argv, env);

            if (res == -1)
            {
               printf("\n%s : Command not found!!!!", argv[0]);
               exit(0);
            }
         }
      }
   }

   return 0;
}