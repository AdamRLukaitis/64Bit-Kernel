/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#include <sys/pic.h>
#include <sys/writeUtil.h>


void setupPIC(int pic1, int pic2)
{
	//setting up pic 

	byte md,sd;

	md=inb(MASTERDATA);                     
	sd=inb(SLAVEDATA);                      

	outb(MASTER, EOI);                      

	outb(MASTER, ICW1_INIT+ICW1_ICW4);      
	outb(SLAVE, ICW1_INIT+ICW1_ICW4);       

	outb(MASTERDATA, pic1);                 
	outb(SLAVEDATA, pic2);                   

	outb(MASTERDATA, 0x04);                  
	outb(SLAVEDATA, 0x02);

	outb(MASTERDATA, 0x01);            
	outb(SLAVEDATA, 0x01);

	outb(MASTERDATA,md);                    
	outb(SLAVEDATA,sd);                     

	//initially masking all interrupts
	int nCnt=0;
	for(nCnt =0;nCnt<16;nCnt++)
		IRQ_set_mask(nCnt);
}
	

void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = MASTERDATA;
    } else {
        port = SLAVEDATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = MASTERDATA;
    } else {
        port = SLAVEDATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}


 /* enables/disables software interrupts */
void INTS(bool on)
{
	if(on)
	{
		__asm("sti");
	}
	else
	{
		__asm("cli");
	}
}



