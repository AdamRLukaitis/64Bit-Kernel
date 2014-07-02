#ifndef PROC
#define PROC


#include <defs.h>
#define STACK_SIZE 64


struct PCB 
{
	uint64_t rip;
	uint64_t rsp;
	uint8_t pid;
	uint64_t cr3;
	uint64_t stack[STACK_SIZE];	
};


//switching trial
void func1();
void func2();

void schedule();

void initializePCB (struct PCB  *proc, void *ptr);
void starter();

#endif