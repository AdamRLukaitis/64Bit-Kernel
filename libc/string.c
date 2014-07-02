
#include <string.h>
int  parse(char *line, char **argv)
{
    int len =0; 
    while (*line != '\0') 
    {
          len++;
          /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv++ = "\0";                   /* mark the end of argument list  */ 
     return len;
}

int strlen(const char *str)
{
        const char *s;

        for (s = str; *s; ++s)
                ;
        return (s - str);
}


int strcmp(const char *s1,const char *s2)
{
       while (*s1 == *s2++) {
          if (*s1++ == 0){ 
               return (0);
                 }
            }
        return (*(unsigned char *) s1 - *(unsigned char *) --s2);
}

char * strcpy(char *destination, const char *source) {
{
    char *ret = destination;
    while ((*destination++ = *source++))
        {;}
    return ret;
}
}

char *substring(char* dest , const char *s, int startIndex, int endIndex)
{
  //char* result = dest;
  /* check for null s */
  if (NULL == s)
    return NULL;

  if (startIndex > endIndex)
    return NULL;
	
 
  /* n < 0 or m < 0 is invalid */
  if (startIndex < 0 || endIndex < 0)
    return NULL;
	
  int i = 0;
  for ( ; endIndex-startIndex > 0; startIndex++, i++)
    if (*(s+startIndex) != '\0'){
	  *(dest+i) = *(s+startIndex);	
	}else{
	  break;
	}
  *(dest+i) = 0;
  return dest;
}


int starts_with(const char * base, const char *prefix) //returns 1 if match is found
{
    while(*prefix)
    {
        if(*prefix++ != *base++)
            return 0;
    }
    return 1;
}