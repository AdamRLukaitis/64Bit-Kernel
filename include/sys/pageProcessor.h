
#ifndef PAGEPROC
#define PAGEPROC


#include <defs.h>
#include <sys/writeUtil.h>
#include<sys/memoryProcessor.h>


#define PR       1               // Present(1)
#define RW      2               // Read(0)/Write(1)
#define US      4               // User(1)/Supervisor(0)


struct PML4 {
    uint64_t data[512];
};

struct PDP {
    uint64_t data[512];
};

struct PD {
    uint64_t  data[512];
};

struct PT {
    uint64_t  data[512];
};



void SetUpKernelMemory();

void* SetUpMemoryForNewProcess();

void SetCr3Value(struct PML4 *pml4);

void MapPhysicaltoVirtual(struct PML4 *pml4,uint64_t virtAddr,uint64_t physAddr,uint64_t flags);

void test();

void MapMemory(struct PML4 *pml4);


uint64_t getCr3Value();

void* AllocateVirtualMemory();

void* AssignPhysicalToVirtual(uint64_t virtAddr,int pages,int flags);

uint64_t GetMappedPhysicalAddress(uint64_t virtAddr);

void* CopyPageTables();

#endif
