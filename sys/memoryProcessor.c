
#include <defs.h>
#include <sys/memoryProcessor.h>


#define SET 1
#define UNSET 0
uint64_t maxSize = 8192;

uint64_t gkerEnd, physbase;
extern uint64_t base;
char * bitMap;
      
void InitializeAvailableMemory(uint64_t physbase1, uint64_t physfree1,uint64_t limit) 
{
  maxSize = (limit/1000);
  gkerEnd= physfree1;
  physbase = physbase1;
  bitMap = (char*)gkerEnd;
  int i = 0;
  for(i = 0;i<maxSize ;i++){
    bitMap[i] = UNSET;
    }
  gkerEnd+= (int)(maxSize/8);
  }


void *memset(void *s, int c, uint64_t n)
{
    unsigned char* p=(unsigned char*)((uint64_t)s|base);
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}

int isValid(uint64_t address)
{
  if(address < gkerEnd){
    return 0;
  }
  return 1;
}

void CreateFreeLists(uint64_t start, uint64_t end){
  int startingIndex = start/0x1000;
  int endingIndex = end/0x1000;
  int i = 0;

  for(i=startingIndex;i<endingIndex;i++){
    if(isValid(i*0x1000)){
       bitMap[(int)(i/8)] |=  SET<<(i%8) ;   //Marking bit of page entry as available.
    }
  }
  
}
void* AllocatePage(){
 
  bitMap = (char*)((uint64_t)bitMap|base);
  int i = 0;
  while(!bitMap[i]) 
	i++;
  char _bit_masks[] = {0x01, 0x02, 0x04, 0x08,  0x10, 0x20, 0x40, 0x80};
  int j = 0;
  for(j=0;j<8;j++)
  {
    if(bitMap[i] & _bit_masks[j])
    {
            bitMap[i] &= ~(SET<<j);
            uint64_t address = (i*0x1000*8)+0x1000*j;
            memset((uint64_t *)(base|address),0,4096);
            return (void *)address;
    }
  }
  return NULL;
}


void FreePage(uint64_t pageAddress)
{
  int index = pageAddress/0x1000;
  bitMap[(int)(index/8)] &= ~SET<<(index%8) ;
}




