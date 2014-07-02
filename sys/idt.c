/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#include <defs.h>
#include <sys/idt.h>
typedef struct
{
    uint16_t bits015;
    uint16_t selector;
    unsigned res : 3 ;
    unsigned zero1 : 5;
    unsigned type : 4;
    unsigned ss : 1;
    unsigned dpl : 2;
    unsigned p : 1;
    uint16_t bits1631;
    uint32_t bits3263;
    uint32_t zero2;	

} __attribute__((packed)) idt_t;


typedef struct 
{
	uint16_t size;
	uint64_t addr;
}__attribute__((packed)) idtr ;



idtr IDTR;
idt_t IDT[256];

void loadIDTR()
{
    IDTR.size= 256*(sizeof(idt_t));
    IDTR.addr = (uint64_t)IDT;

    idtr *IDTRptr = &IDTR;

    __asm volatile("LIDT (%0) ": :"p" (IDTRptr));
}


void AddInt(int number, void (*handler)(),int dplEntry)
{
	 uint16_t selector = 0x08;
	 uint64_t offset = (uint64_t)handler;
        
	
	 IDT[number].bits015   = (offset & 0x0000ffff);
	 IDT[number].selector     = selector;
	 IDT[number].res     = 0;
	 IDT[number].zero1 = 0;
	 IDT[number].type    = 0xE;
	 IDT[number].ss      = 0;
	 IDT[number].dpl      = dplEntry;
	 IDT[number].p     = 1;
	 
        IDT[number].bits1631  = ((offset >>16) & 0x0000ffff);
	 
        IDT[number].bits3263  = ((offset >>32)& 0xffffffff);
	 IDT[number].zero2=0;

}

