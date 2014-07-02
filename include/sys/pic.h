/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#ifndef PIC
#define PIC

#define byte unsigned char
#define word unsigned short
#define dword unsigned int


#include <defs.h>

#define IDE_1     14
#define IDE_2     15


#define MASTER          0x20
#define MASTERDATA      0x21
#define SLAVE           0xA0
#define SLAVEDATA       0xA1
#define EOI             0x20


#define ICW1_INIT    0x10               
#define ICW1_ICW4    0x01      
         

void setupPIC(int pic1, int pic2);      //setting up PIC
void INTS(bool on);                     //disabling or enabling interrupts

void IRQ_clear_mask(unsigned char IRQline);
void IRQ_set_mask(unsigned char IRQline);

#endif

