/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#include <sys/timeController.h>
#include <sys/pic.h>
#include <sys/writeUtil.h>

uint32_t g_TimerFrequency;

unsigned char readCMOS(uint8_t addr)
{
  unsigned char ret;
  outb(0x70, 1<< 7 |addr);
  __asm volatile("jmp 1f; 1: jmp 1f;1:");
  ret = inb(0x71);
  __asm volatile("jmp 1f; 1: jmp 1f;1:");
  return ret;
}


  
void printfClock(uint32_t sec)
{

	volatile register char *temp1, *temp2;
	int xpos = 60;
	int ypos = 24;

	int temp =sec;

	sec += t.totSec-4;

	uint8_t hrs = sec/3600;
	sec=sec%3600;
	uint8_t min=sec/60;
	sec=sec%60;

	char time[10];

	time[0] = (hrs/10) + '0';
	time[1] = (hrs%10) + '0';
	time[2] = ':';
	
	time[3] = (min/10) + '0';
	time[4] = (min%10) + '0';
	time[5] = ':';

	time[6] = (sec/10) + '0';
	time[7] = (sec%10) + '0';
	
	time[8] = ' ';	
	
	time[9] = '\0';

	char * ptr= time;
		
  	while(*ptr != '\0')		
	{	
		temp1 = ptr;
  		temp2 = (char*)g_pStartptr+ 2*(xpos +80*ypos );
  		*temp2 = *temp1;
  		xpos ++;
		ptr+=1;	
	}
	
	

	//printing the boot time as well
	char val[100];
	itoa(temp,val,10);
	
	char *ptr1 =  val;
	
	while(*ptr1 != '\0')		
	{	
		temp1 = ptr1;
  		temp2 = (char*)g_pStartptr+ 2*(xpos +80*ypos );
  		*temp2 = *temp1;
  		xpos ++;
		ptr1+=1;	
	}

}


uint8_t FormatTime(uint8_t no)
{
	return ((((no) & 15) + ((no) >> 4) * 10));
}

//Gets CMOS actual time
void getrealDatetime()
{
  
  __asm volatile("cli");

  int hr = FormatTime(readCMOS(0x04)) -4;
  if(hr < 0)
	hr+=12;
  t.hour = hr%12;
  t.sec = FormatTime(readCMOS(0x00));
  t.min = FormatTime(readCMOS(0x02));
  __asm volatile("sti");

  t.totSec = t.hour *3600 + t.min *60 + t.sec;

}


void setTimerFreq(uint32_t frequency) 
{
    g_TimerFrequency = frequency;
    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    uint32_t divisor = 1193180 / frequency;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor>>8) & 0xFF);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}


void InitializeRTC()
{
	outb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
	outb(0x71, 0x20);	// write to CMOS/RTC RAM

	outb(0x70, 0x8B);		// select register B, and disable NMI
	char prev=inb(0x71);	// read the current value of register B
	outb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
	outb(0x71, prev | 0x40);
}

