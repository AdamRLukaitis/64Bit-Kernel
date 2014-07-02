/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#include <sys/isr.h>
#include <sys/idt.h>
#include <sys/timeController.h>
#include <sys/mmManager.h>
#include <sys/switchManager.h>
#include<sys/pageProcessor.h>

extern void interrupt0(void);
extern void interrupt1(void);
extern void interrupt14(void);
extern void interrupt32(void);
extern void interrupt33(void);
extern void interrupt128(void);
extern void interrupt130(void);
extern void def(void);
extern void schedule();
extern void ExitProcess();

extern task_struct *currentTask;
extern uint32_t g_TimerFrequency;
extern uint64_t stackHardLimit;
extern uint64_t stackSoftLimit;

uint32_t g_counter =0;
uint32_t g_seconds =0;

void AddInterrupts()
{
    AddInt(0, interrupt0,0);
    AddInt(1, interrupt1,0);
    AddInt(2, def,0);
    AddInt(3, def,0);
    AddInt(4, def,0);
    AddInt(5, def,0);
    AddInt(6, def,0);
    AddInt(7, def,0);
    AddInt(8, def,0);
    AddInt(9, def,0);
    AddInt(10, def,0);
    AddInt(11, def,0);
    AddInt(12, def,0);
    AddInt(13, def,0);
    AddInt(14, interrupt14,0);
    AddInt(15, def,0);
    AddInt(16, def,0);
    AddInt(17, def,0);
    AddInt(18, def,0);
    AddInt(19, def,0);
    AddInt(20, def,0);
    AddInt(21, def,0);
    AddInt(22, def,0);
    AddInt(23, def,0);
    AddInt(24, def,0);
    AddInt(25, def,0);
    AddInt(26, def,0);
    AddInt(27, def,0);
    AddInt(28, def,0);
    AddInt(29, def,0);
    AddInt(30, def,0);
    AddInt(31, def,0);
    AddInt(32, interrupt32,3);
    AddInt(33, interrupt33,0);
    AddInt(128, interrupt128,3);

}

//divide by 0 testing
void int_00(void)
{
	printf("Divide By Zero Error .It is working!!!\n");
}


void int_01(void)
{
	printf("Debug Error.It is working!!!");
}


//page Fault
void int_14(struct pagefault_registers *regvals)
{

	uint64_t faultAddr;
    	__asm volatile("mov %%cr2, %0" : "=r" (faultAddr));
	//printf("\nPage Fault accessing : %p",faultAddr);
	//printf("\nError Code : %d",regvals->errorCode);
	switch(regvals->errorCode)
	{
		case 0:
			CopyCodeData(faultAddr,regvals->errorCode);
			break;

		case 1:
			ExitProcess();
			break;	
		case 2:
			CopyCodeData(faultAddr,regvals->errorCode);
			break;
		case 3:
			ExitProcess();
			break;
		case 4:
		case 6:
			if(regvals->rsp - 0x8 == faultAddr)
      			{
       
		
				if((regvals->rsp <= stackHardLimit))
        			{
          				printf("\n%s : Processe's Max limit of Stack size reached. Stack Hardlimit Overflow!!!.Exitting...",currentTask->memMap->name);
          				ExitProcess();
        			}
				else if(regvals->rsp <= stackSoftLimit)
				{
					printf("\n%s : Set Ulimit of Stack size for process reached. Stack Softlimit Overflow!!!.Exitting...",currentTask->memMap->name);
          				ExitProcess();
				}
        			else
        			{

		
          				//Auto Growing Stack
          				struct vm_area_struct * temp = currentTask->memMap->head;
          				while(temp->vm_next != NULL)
            					temp = temp->vm_next;
           
          				struct vm_area_struct * newVMA=(struct vm_area_struct*)AllocateVirtualMemory();
           				newVMA->vm_end = regvals->rsp & 0xFFFFFFFFFFFFF000;
           				newVMA->vm_start = newVMA->vm_end-0x1000;                                                                                                                      
           				newVMA->vm_next = NULL;
           				newVMA->sectionOffset = -1;
           				newVMA->permFlags = 3;
           				temp->vm_next = newVMA;
        			}

      			}
			CopyCodeData(faultAddr,regvals->errorCode);
			break;
		case 5:
			//User process tried to read a present page and caused a protection fault
			printf("\n%s : Seg Fault. Process tried to read from a physical page for which it does not have privileges. Exitting...",currentTask->memMap->name);
			ExitProcess();
			break;
		case 7:;
			uint64_t COW  = 0x10000000000000UL;
			uint64_t PTentry = GetMappedPhysicalAddress(faultAddr);
			
			if((COW&PTentry)  == COW)
			{
				CopyVmaData(faultAddr);
			}
			else
			{
				printf("\n%s : Seg Fault. Process tried to write into a physical page for which it does not have privileges. Exitting...",currentTask->memMap->name);
				ExitProcess();
			}
			break;
		default :
			break;
	};
}




//timer interrupt IRQ
void int_32(void)
{
	outb(0x20,0x20);

	g_counter++;

	if((g_counter) % g_TimerFrequency ==0)
	{
		g_seconds ++;
		printfClock(g_seconds);

	}

	
	if(currentTask != NULL)
	{
		schedule();		
	}

}




//keyboard interrupt IRQ
void int_33(void)
{
	printfCharacter();
	outb(0x20,0x20);
}

void defaultFunc(void)
{
	printf("Undefined Interrupt!!!!!!!!!!");
	while(1);
}
