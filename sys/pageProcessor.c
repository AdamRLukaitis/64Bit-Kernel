
#include<sys/pageProcessor.h>
#include <sys/writeUtil.h>

extern char kernmem, physbase;
extern uint64_t limit;
extern uint64_t gkerEnd;

#define BASE 0xfffffff000000000
uint64_t lastUsedAddress = 0xffffffff00000000;

uint64_t COW  = 0x10000000000000;

uint64_t ReadOnly  = ~RW;

#define KFLAGS 1 | 2

#define UFLAGS 1 | 2 | 4

uint64_t base = 0;
uint64_t GetPTIndex(long lAddress)
{
    return lAddress >> 12 & 0x1FF;
}

uint64_t GetPDIndex(long lAddress)
{
    return lAddress >> 21 & 0x1FF;
}

uint64_t GetPDPIndex(long lAddress)
{
    return lAddress >> 30 & 0x1FF;
}

uint64_t GetPML4Index(long lAddress)
{
    return lAddress >> 39 & 0x1FF;
}

void test(struct PML4 *pml4)
{

	struct PML4 *pml4tbase = (struct PML4 *)((uint64_t)pml4|BASE);

	printf("PLM4 is at %p,PDP is at %x",pml4tbase ,pml4tbase->data[511]);

	struct PT *pt = (struct PT *)((uint64_t)AllocatePage()|BASE);
	printf("%p",pt);

}

void MapMemory(struct PML4 *pml4)
{
	uint64_t start = 0;
	uint64_t startV = BASE;


	while(start < limit)
	{
		MapPhysicaltoVirtual(pml4,startV,start,KFLAGS);
		start+=0x1000;
		startV+=0x1000;
	}
}


struct PML4 *goo; 

void* SetUpMemoryForNewProcess()
{
	struct PML4 *pml4New = (struct PML4 *) AllocatePage();
	
	struct PML4 *pml4Current = (struct PML4 *)getCr3Value();
	
	//Mapping Kernel & Identity Mapping
	((struct PML4 *)((uint64_t)pml4New |base))->data[511] = ((struct PML4 *)((uint64_t)pml4Current |base))->data[511];

	return (void*)pml4New ;
} 


void* CopyPageTables()
{
	struct PML4 *pml4New = (struct PML4 *) AllocatePage();
	
	struct PML4 *pml4Current = (struct PML4 *)getCr3Value();
	

	uint64_t virAddrStart = (long)&kernmem;

	uint64_t pml4UserMax = GetPML4Index(virAddrStart);


	int pmlIndex =0;
	//below the kernel all needs to be copied
	for(pmlIndex=0;pmlIndex<512;pmlIndex++)
	{
		if(pmlIndex >= pml4UserMax) //part of kernel , clone...
		{
			((struct PML4 *)((uint64_t)pml4New |base))->data[pmlIndex] = ((struct PML4 *)((uint64_t)pml4Current |base))->data[pmlIndex];
			continue;
		}
		
		
		uint64_t pdpBaseAddr = ((struct PML4 *)((uint64_t)pml4Current |base))->data[pmlIndex];

		if(( pdpBaseAddr & PR)  == PR)
		{
			struct PDP *pdpNew = (struct PDP *) AllocatePage();
			((struct PML4 *)((uint64_t)pml4New |base))->data[pmlIndex] = (uint64_t)((uint64_t)pdpNew| UFLAGS);
			
			struct PDP *pdpCurrent = (struct PDP *)( pdpBaseAddr & 0xFFFFFFFFFFFFF000);

			int pdpIndex = 0;
			for(pdpIndex =0;pdpIndex <512;pdpIndex ++)
			{
				uint64_t pdBaseAddr =   ((struct PDP *)((uint64_t)pdpCurrent |base))->data[pdpIndex];

				if((pdBaseAddr & PR)  == PR)
				{
					struct PD *pdNew = (struct PD *) AllocatePage();
					((struct PDP *)((uint64_t)pdpNew |base))->data[pdpIndex] = (uint64_t)((uint64_t)pdNew | UFLAGS);
			
					struct PD *pdCurrent = (struct PD *)( pdBaseAddr & 0xFFFFFFFFFFFFF000);
					
					int pdIndex = 0;
					for(pdIndex =0;pdIndex <512;pdIndex ++)
					{
						uint64_t ptBaseAddr =   ((struct PD *)((uint64_t)pdCurrent |base))->data[pdIndex ];

						if((ptBaseAddr & PR)  == PR)
						{
							struct PT *ptNew = (struct PT *) AllocatePage();
							((struct PD *)((uint64_t)pdNew |base))->data[pdIndex] = (uint64_t)((uint64_t)ptNew | UFLAGS);
			
							struct PT *ptCurrent = (struct PT *)( ptBaseAddr & 0xFFFFFFFFFFFFF000);

							int ptIndex = 0;
							for(ptIndex =0;ptIndex <512;ptIndex ++)
							{
								uint64_t pageAddr =   ((struct PT *)((uint64_t)ptCurrent |base))->data[ptIndex];

								ptNew = ((struct PT *)((uint64_t)ptNew |base));
								ptCurrent = ((struct PT *)((uint64_t)ptCurrent |base));


								if((pageAddr & PR)  == PR)
								{

									//setting COW bit & read only bit	
									pageAddr  = ((pageAddr | COW) & ReadOnly );

									//same physical address the new PT
									ptNew->data[ptIndex] = pageAddr;
									
									//updating address in old PT
									ptCurrent->data[ptIndex] = pageAddr;	

									//Need to add ref count code			
			
								}		
							}							
			
						}		
					}
			
				}		
			}
		}		
	}

	return (void*)pml4New ;
} 

void SetUpKernelMemory()
{

	uint64_t virAddrStart = (long)&kernmem;
	uint64_t phyArrrStart = (long)&physbase;
	uint64_t phyAddrEnd = (long)gkerEnd;

	struct PML4 *pml4 = (struct PML4 *) AllocatePage();
	MapMemory(pml4);

	//mapping kernel memory

	uint64_t tempStart = phyArrrStart;
	
	uint64_t virAddrBase = virAddrStart & 0xfffffffff0000000;

	while(tempStart <= phyAddrEnd+0x2000)
	{

		uint64_t tempVirt = virAddrBase | tempStart;	
		MapPhysicaltoVirtual(pml4,tempVirt ,tempStart, KFLAGS);
		tempStart+=0x1000;
	}



	//to be used for virtualization
	base = BASE;

	g_pStartptr = ((uint64_t*)((uint64_t)0xb8000|base)) ;

	SetCr3Value(pml4);

}


void MapPhysicaltoVirtual(struct PML4 *pml4,uint64_t virtAddr,uint64_t physAddr,uint64_t flags)
{

	struct PDP *pdp = NULL;
	struct PD *pd = NULL;
	struct PT *pt = NULL;
	

	uint64_t pdpBaseAddr =   ((struct PML4 *)((uint64_t)pml4|base))->data[GetPML4Index(virtAddr)];
	if((pdpBaseAddr & PR) != PR)
	{
		pdp = (struct PDP *) AllocatePage();
		((struct PML4 *)((uint64_t)pml4|base))->data[GetPML4Index(virtAddr)] = (uint64_t)((uint64_t)pdp | 7);
	}
	else
	{
		pdp =(struct PDP *)( pdpBaseAddr & 0xFFFFFFFFFFFFF000);
	}

	pdp = ((struct PDP *)((uint64_t)pdp |base));

	uint64_t pdBaseAddr =   pdp->data[GetPDPIndex(virtAddr)];

	if((pdBaseAddr & PR) != PR)
	{
		pd = (struct PD *) AllocatePage();
		pdp->data[GetPDPIndex(virtAddr)] = (uint64_t)((uint64_t)pd | 7);
	}
	else
	{
		pd =(struct PD *)( pdBaseAddr & 0xFFFFFFFFFFFFF000);
	}
	
	pd = ((struct PD *)((uint64_t)pd |base));

	uint64_t ptBaseAddr =   pd->data[GetPDIndex(virtAddr)];

	if((ptBaseAddr & PR) != PR)
	{
		pt = (struct PT *) AllocatePage();
		pd->data[GetPDIndex(virtAddr)] = (uint64_t)((uint64_t)pt | 7);
	}
	else
	{
		pt = (struct PT *)(ptBaseAddr & 0xFFFFFFFFFFFFF000);
	}

	pt = ((struct PT *)((uint64_t)pt |base));
	
	//uint64_t ptValue =  pt->data[GetPTIndex(virtAddr)];

	//if((ptValue & PR) != PR)
	{
		pt->data[GetPTIndex(virtAddr)] = physAddr| flags;
	}
/*
	else
	{	
		printf("\n%x is already mapped at beacuse of %x not %x ",virtAddr,ptValue,physAddr );
	}
*/
	
}



uint64_t GetMappedPhysicalAddress(uint64_t virtAddr)
{
	struct PML4 *pml4= (struct PML4 *)getCr3Value();

	uint64_t physAddr = 0;

	struct PDP *pdp = NULL;
	struct PD *pd = NULL;
	struct PT *pt = NULL;
	

	uint64_t pdpBaseAddr =   ((struct PML4 *)((uint64_t)pml4|base))->data[GetPML4Index(virtAddr)];
	if((pdpBaseAddr & PR) == PR)
	{
		pdp =(struct PDP *)( pdpBaseAddr & 0xFFFFFFFFFFFFF000);
	}

	pdp = ((struct PDP *)((uint64_t)pdp |base));

	uint64_t pdBaseAddr =   pdp->data[GetPDPIndex(virtAddr)];

	if((pdBaseAddr & PR) == PR)
	{
		pd =(struct PD *)( pdBaseAddr & 0xFFFFFFFFFFFFF000);
	}
	
	pd = ((struct PD *)((uint64_t)pd |base));

	uint64_t ptBaseAddr =   pd->data[GetPDIndex(virtAddr)];

	if((ptBaseAddr & PR) == PR)
	{
		pt = (struct PT *)(ptBaseAddr & 0xFFFFFFFFFFFFF000);
	}


	if(pt  != NULL)
	{
		pt = ((struct PT *)((uint64_t)pt |base));
		physAddr = pt->data[GetPTIndex(virtAddr)]& 0xFFFFFFFFFFFFF000;
	}

	return physAddr ;
}




void* AllocateVirtualMemory()
{
	uint64_t phyPage =(uint64_t )AllocatePage();
	lastUsedAddress += 0x1000;
	
	struct PML4 *pml4 = (struct PML4 *)getCr3Value();
	MapPhysicaltoVirtual(pml4 ,lastUsedAddress-0x1000,phyPage,KFLAGS);
	
	return (void*)(lastUsedAddress-0x1000);

} 




void* AssignPhysicalToVirtual(uint64_t virtAddr,int pages,int flags)
{
	int cnt=0;

	if(flags == -1)
		flags = UFLAGS;

	uint64_t temp = virtAddr;
	struct PML4 *pml4 = (struct PML4 *)getCr3Value();
	for(cnt =0;cnt<pages;cnt++)
	{
		uint64_t phyPage =  (uint64_t)AllocatePage();
		MapPhysicaltoVirtual(pml4 ,temp,phyPage,flags);
		temp +=0x1000;
	}
	return (void*)(virtAddr);
} 


void SetCr3Value(struct PML4 *pml4)
{
	__asm volatile("movq %0, %%cr3":: "b"(pml4));
}

uint64_t getCr3Value()
{
	uint64_t cr3Value;
    	__asm volatile("mov %%cr3, %0" : "=r" (cr3Value));
	return cr3Value;

}



