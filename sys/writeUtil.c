/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#include <sys/writeUtil.h>
#include<sys/sysCallManager.h>
#include <sys/switchManager.h>
#include <sys/mmManager.h>
#include <sys/file.h>

extern uint64_t base;
extern task_struct *currentTask;
uint64_t *g_pStartptr= (uint64_t*)0xb8000;

int nXpos =0;
int nYpos =0;

int isBufferEmpty = 0;

extern struct registers_t* g_regVals;



void clrscr()
{
  int nRows,nCols;
  char* pCurPos= (char*)g_pStartptr;
  for(nRows=0;nRows<MAX_ROWS;nRows++)
    for(nCols=0;nCols<MAX_COLS;nCols++)
      {
	*pCurPos++ = 0;
	*pCurPos++ = 0xF;
      }
  nXpos =nYpos =0;
}

void outb(unsigned int port,unsigned char value) 
{
  __asm volatile ("outb %%al,%%dx": :"d"(port),"a"(value));
}

unsigned char inb( unsigned short port )
{
  unsigned char ret;
  __asm volatile( "inb %1, %0"
		: "=a"(ret) : "Nd"(port) );
  return ret;
}


void putchar(char inp)
{

 volatile register char *temp1, *temp2;
 

switch(inp)
 {
  case '\n':
    nXpos=0;
    nYpos++;
    break;
  case '\t':
    nXpos +=8;
    break;
  case '\b':

    if(isBufferEmpty  == 1)
    {
	volatile register char *temp = (char*)g_pStartptr+ 2*(nXpos-1+MAX_COLS*nYpos);
	if(*temp == '$')
		return;
    }
    nXpos-=1;
    update_cursor(nYpos,nXpos);
    putchar(' ');
    nXpos-=1;
    update_cursor(nYpos,nXpos);
    return; 
	
  default:
      if(nXpos > MAX_COLS-1)
      {
        nYpos+= 1;
        nXpos= 0;
      }

      if(nYpos < MAX_ROWS-1)// && nXpos < MAX_COLS)
      {
      	temp1 = &inp;
      	temp2 = (char*)g_pStartptr+ 2*(nXpos+MAX_COLS*nYpos);
      	*temp2 = *temp1;
     	nXpos++;
      }
      else
	{
        	//moving data one line up, line by line
        	char* p = (char*)g_pStartptr;
        	int i;
        	for (i = 0; i < (MAX_ROWS -2)*MAX_COLS*2; i++)
        	{
            		*(p+i) = *(p+i+160);
        	}
 
		//making last line blank
        	for (i = (MAX_ROWS -2)*MAX_COLS*2+2; i < (MAX_ROWS-1)*MAX_COLS*2; i+=2)
        	{
                 	*(p+i) = ' ';
               	*(p+i+1) = 0xF ;
        	}

        	nYpos = MAX_ROWS -2;		
		putchar(inp);	
	}
      
 }; 

  update_cursor(nYpos,nXpos);

 
}



char * itoa( uint64_t value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}




void puts(char * ptr)
{
  while(*ptr != '\0')
      {
        putchar(*ptr);
        ptr+=1;
      }
}



void update_cursor(int row, int col)
{
  unsigned short position=(row*MAX_COLS) + col;

   // cursor LOW port to vga INDEX register
  outb(0x3D4, 0x0F);
  outb(0x3D5, (unsigned char)(position&0xFF));
  // cursor HIGH port to vga INDEX register
  outb(0x3D4, 0x0E);
  outb(0x3D5, (unsigned char )((position>>8)&0xFF));

}


void printf(char *fmt, ...)
{
  va_list ap;
  char *p, *sval;
  va_start(ap,fmt); 
  for (p = fmt; *p; p++) 
    {
    if (*p != '%') 
      {
      putchar(*p);
      continue;
      }
    switch (*++p) 
      {
      case 'd':
      {      
       int ival = va_arg(ap, int);
       int count =0;
       int temp = ival;
       for(;temp!=0;temp=temp/10)
         count++;
       char val[count+1];
       itoa(ival,val,10);
       puts(val);
       break;
      }      
      case 'x':
      {
	int ival = va_arg(ap, int);
	int count =0;
	int temp = ival;
	for(;temp!=0;temp=temp/10)
	  count++;
	char val[count+1];
	itoa(ival,val,16);
	puts(val);
	break;
      }

      case 'p':
      {
	uint64_t ival = (uint64_t)va_arg(ap, uint64_t);
	int count =0;
	uint64_t temp = ival;
	for(;temp!=0;temp=temp/10)
	  count++;
	char val[count];
	itoa(ival,val,16);
	puts("0x");
	puts(val);
	break;
      }

      case 'c':
      {
       char cval = (char)va_arg(ap, int);
       putchar(cval);
       break;
      }
      case 's':
      {
       for (sval = va_arg(ap, char *); *sval; sval++)
       putchar(*sval);
       break;
      }
      default:
       putchar(*p);
      break;
      };
    }
  va_end(ap); 
}



char upperCase[128] =
	{
  		0,  27, '!', '@', '#', '$', '%', '^', '&', '*','(', ')', '_', '+', '\b','\t',
  		'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',0,'A', 'S', 
  		'D', 'F', 'G', 'H', 'J', 'K', 'L', ':','\"', '~',   0,'|', 'Z', 'X', 'C', 'V',
  		'B', 'N','M', '<', '>', '?',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,'+',
  		0,0,0,0,0,0,0,0,0,0,0,
	};

char lowerCase[128] =
	{
  		0,  27, '1', '2', '3', '4', '5', '6', '7', '8','9', '0', '-', '=', '\b','\t','q', 
  		'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',0,'a', 's', 'd', 'f', 
  		'g', 'h', 'j', 'k', 'l', ';','\'', '`',0,'\\', 'z', 'x', 'c', 'v', 'b', 'n','m', 
  		',', '.', '/','r','*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,'+',
  		0,0,0,0,0,0,0,0,0,0,0,
	};


int shiftPressed = 0;
int ctrlPressed = 0;


char buf[256]="\0";
char *buffer = buf;
int isNewline =0;


void printfCharacter()
{

	volatile register char *temp1, *temp2;
	int xpos = 58;
	int ypos = 24;

	
	uint8_t b = inb(0x60);

	if(b & 0x80)
 	 {
		if(b- 0x80 == 42 || b- 0x80 == 54)
      			shiftPressed  = 0;
		else if(b- 0x80 == 29)
			ctrlPressed  =  0;
	 }
	else
	{
		if(b== 42 || b== 54)
    		{
      			shiftPressed = 1;
      			return;
    		}

		if(b== 29)
    		{
      			ctrlPressed  = 1;
      			return;
    		}

		char dispchar;


		if(shiftPressed  == 0)
			dispchar = lowerCase[b];
		else
			dispchar = upperCase[b];

		if(dispchar != '\b')
		{
			*buffer++ = dispchar;
		}

		if(ctrlPressed  == 1)
			dispchar = upperCase[b];
		
		if(dispchar == '\n')
			isNewline = 1;

		if(dispchar == '\b' && (buffer - buf) > 0 )
		{
			*(--buffer) = '\0';
		}	

		if(buffer == buf)
			isBufferEmpty  = 1;
		else
			isBufferEmpty  = 0;


		putchar(dispchar);
		
		temp1 = &dispchar ;
      		temp2 = (char*)g_pStartptr+ 2*(xpos +80*ypos );
      		*temp2 = *temp1;

		if(ctrlPressed  == 1)
		{
			char exp = '^';
			temp1 = &exp ;
      			temp2 = (char*)g_pStartptr+ 2*(xpos-1 +80*ypos );
      			*temp2 = *temp1;
		}
		else
		{
			char exp = ' ';
			temp1 = &exp ;
      			temp2 = (char*)g_pStartptr+ 2*(xpos-1 +80*ypos );
      			*temp2 = *temp1;
		}

	}
}



char temp[256]={0};

char* gets()
{
    int i=0,j=0;
    int k=0;

    for(k=0;k<256;k++)
        temp[k] = '\0';
   
    if(!isNewline)
        return NULL;
    else
    {
        while(buf[i]!='\n')
        {
            temp[i]=buf[i];
            i++;
        }

        isNewline =0;
        temp[i++]='\0';
    }

    while(buf[i])
    {
        buf[j++]=buffer[i++];
    }
 
    for(i=j;i<256;i++)
        buf[i] = '\0';

    buffer = &buf[j];

    return temp;
}


void writeSysCall()
 {
   int fd = g_regVals->rbx;
   char *str =(char*)g_regVals->rcx;
  long size = g_regVals->rdx;
   if (fd == 1 || fd == 2 )
   {
     char *temp = str;
   while((size--)> 0 && *str++);
   *str = '\0';
     puts(temp);
   }
 }


void readSysCall()
{
    if(currentTask->memMap->isBgProcess == 1)
    {                                                                         
      printf("\n%s : Background process attempting read, Exiting...",currentTask->memMap->name);
      ExitProcess();
    }
    else
    {
   	int fd = g_regVals->rbx;
   	char *str = (char*)g_regVals->rcx;
   	int size = g_regVals->rdx;
   	int readBytes = 0;
   	if(fd < 3)
	{
	  char *str1 = gets();
	  if(str1 == 0)
	  {
		*str = '\0';
	  }
	  else
	  {
	   while(*str1)
          {
		 *str++ = *str1++;
		 readBytes++;
 	   }
	   *str = '\0';
	   readBytes++;
	  }
	}else
	{
		readBytes = t_read(fd, str, size);
	}
	g_regVals->rax = readBytes;

    }
}


