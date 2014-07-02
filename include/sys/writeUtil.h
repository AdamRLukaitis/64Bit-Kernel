/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#ifndef _WRITEUTIL_H
#define _WRITEUTIL_H

#include <defs.h>

extern uint64_t* g_pStartptr;

extern int nXpos;
extern int nYpos;

void outb(unsigned int port,unsigned char value);
unsigned char inb( unsigned short port );
void clrscr();
void putchar(char inp);

char * itoa( uint64_t value, char * str, int base );
void puts(char * ptr);
void update_cursor(int row, int col);

void printf(char *fmt, ...);

void printfCharacter();
void tr(char c);

void writeSysCall();
void readSysCall();

#endif

