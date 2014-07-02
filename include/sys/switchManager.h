#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <defs.h>

void firstSwitch();

#define STACK_SIZE 256
typedef struct
{
	int8_t processId;
	uint64_t stack[STACK_SIZE];
	uint64_t sp;
	uint64_t cr3;
	struct mm_struct * memMap;
	struct task_struct* parentTask;
	uint64_t sleepTime;
	uint64_t sleepLimit;
       int8_t waitId;
	
}task_struct;

void schedule();
void startingPoint();
void ExitProcess();
int Fork();
void sleepSysCall();
void execSysCall();
void waitSysCall();
void waitPidSysCall();
void procListSysCall();
void ulimit();

#endif 