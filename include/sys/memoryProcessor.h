


#ifndef _PMAP_H
#define _PMAP_H

void InitializeAvailableMemory(uint64_t physbase1, uint64_t physfree1,uint64_t limit);

void CreateFreeLists(uint64_t start, uint64_t end);

void FreePage(uint64_t base);
void* AllocatePage();

void *memset(void *s, int c, uint64_t n);
#endif


