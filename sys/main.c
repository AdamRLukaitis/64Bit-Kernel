#include <defs.h>
#include <sys/pic.h>
#include <sys/gdt.h>
#include <sys/writeUtil.h>
#include <sys/isr.h>
#include <sys/idt.h>
#include <sys/timeController.h>
#include <sys/memoryProcessor.h>
#include <sys/pageProcessor.h>
#include <sys/switchManager.h>
#include <sys/tarfsReader.h>
#include <sys/file.h>


#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
uint64_t limit;
uint64_t phyAddrEn;
struct tss_t tss;

//struct tss_t tss1;
void start(uint32_t* modulep, void* physbase, void* physfree)
{
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;

	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
		limit = smap->base + smap->length;	
		}
	}

	InitializeAvailableMemory((uint64_t)physbase,(uint64_t)physfree,limit );

	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
		CreateFreeLists(smap->base,smap->base + smap->length);
		}
	}
		

	
	SetUpKernelMemory(physfree);
	initializeFileDescriptors();

	printf("********************************Welcome to SBUOS********************************");
	printf("\nRefer \\STARTGUID for details on how to run.");

	
	startingPoint();

	
	
}


void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	reload_gdt();
	setup_tss();

	INTS(false);
    	//clearing the screen
	clrscr();
	//enabling and remapping pic to required positions
	setupPIC(0x20,0x28);

	//adding irq s
	AddInterrupts();
	
	loadIDTR();

	setTimerFreq(500);
	InitializeRTC();

	//enabling timer interrupt
	IRQ_clear_mask(0);
	
	//enabling keyboard interrupt
	IRQ_clear_mask(1);

	INTS(true);

        //getting current time & storing it to be used with interrupt 0 timer
	getrealDatetime();


	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);



	

    while(1);
}
