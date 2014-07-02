#include <sys/tarfsReader.h>
#include <sys/elf.h>
#include <string.h>


#define BLOCKSIZE 512




int power(int x, int y)
{
    if( y == 0)
        return 1;
    else if (y%2 == 0)
        return power(x, y/2)*power(x, y/2);
    else
        return x*power(x, y/2)*power(x, y/2);
 
}


//convert to int
int getSize(char *p) 
{
 int k = 0;
 while (*p) {
 k = (k<<3)+(k<<1)+(*p)-'0';
 p++;
 }


 int decimal=0, i=0, rem; 
 while (k!=0) 
 { 
  rem = k%10; 
  k/=10; 
  decimal += rem*power(8,i); 
  ++i; 
 }
 
 return decimal;
}


char* ReadFile(char *fileName)
{
    struct posix_header_ustar * entry = (struct posix_header_ustar*)(&_binary_tarfs_start);
    
    int padding =0;

    while(strcmp(fileName,entry ->name) != 0 &&(uint64_t)entry < (uint64_t)&_binary_tarfs_end)
        {
    	   int size = getSize(entry->size);
           
		   

           if(strcmp(entry->typeflag,"5")  == 0){
		}else if(strcmp(entry->typeflag,"0")==0){
			}else if(entry->typeflag == NULL){
			}else{
			}
           //increasing by size of the structure + size of file
           entry = (struct posix_header_ustar *)((char*)entry + sizeof(struct posix_header_ustar) + size );
		   if(size > 0){
				padding = BLOCKSIZE - size%BLOCKSIZE;
								if((char*)&_binary_tarfs_end - (char*)entry >=BLOCKSIZE)
				{
					entry = (struct posix_header_ustar *)((char*)entry + padding);
				}	 
			}	
		}


		char *ptr = ((char*)(entry)+ sizeof(struct posix_header_ustar));
	
		return ptr;

}