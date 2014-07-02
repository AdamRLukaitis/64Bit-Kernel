
#include <sys/writeUtil.h>
#include <stdlib.h>
#include <sys/tarfs.h>
#include <sys/memoryProcessor.h>
#include<sys/pageProcessor.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/dirent.h>
#include <sys/tarfsReader.h>

#define BLOCKSIZE 512


int getSize(char *p);
int initdir (DIR *p);
const char *_getdirname (const struct dirent *dp);
void _setdirname (struct DIR *dirp);
int findfirst(char* dirname, char spec , struct dirent* data );
DIR * opendir(const char *dirname);


extern char _binary_tarfs_start;
int errno;

 DIR * opendir(const char *dirname)
{
  DIR *dirp;
  if(dirname != NULL){
	dirp = (DIR*)AllocateVirtualMemory();
	if (dirp != NULL) {
    char *p;
    strncpy (dirp->dirname, dirname, NAMEMAX);
    p = strchr (dirp->dirname, '\0'); //Return substring starting from character c
    if (dirp->dirname < p  &&
        *(p - 1) != '\\'  &&  *(p - 1) != '/'  &&  *(p - 1) != ':')
    {
      strcpy (p++, "/");
    }
	
    // open stream 
    if (initdir (dirp) == 0) {//Returns 1 if directory is found.
      // initialization failed 
      FreePage(GetMappedPhysicalAddress((uint64_t)dirp));
	  errno = ENOENT;
      return NULL;
	}
	//printf("found : %s",dirname);
	}
  return dirp;
  }
  errno = ENOTDIR;
  return NULL;
}

//to read from the directory
struct dirent * readdir (DIR *dirp)
{
  if (dirp == NULL) {
    errno = EBADF;
    return NULL;
  }
  if (dirp->dirent_filled != 0) {
    dirp->dirent_filled = 0;
  } else if (findNext(&dirp->current) != 0) {
      /* findnext will set errno to ENOENT when no
       * more entries could be retrieved. */
       return NULL;
  }
  //printf("\n\nfound next : %s",dirp->current.d_name);
  return &dirp->current;
} 


//close the directory
 int closedir (DIR *dirp)
 {   
  int retcode = 0;
 // make sure that dirp points to legal structure 
  if (dirp == NULL) {
    errno = EBADF;
    return -1;
  }
  // clear dirp structure to make sure that it cannot be used anymore
  FreePage(GetMappedPhysicalAddress((uint64_t)dirp));
  return retcode;
}


 void rewinddir (DIR *dirp)
 {   
  // make sure that dirp is legal 
  if (dirp == NULL) {
    errno = EBADF;
    return;
  }
  
  // re-open previous stream 
  if (initdir (dirp) == 0) {
    /* initialization failed but we cannot deal with error.  User will notice
     * error later when she tries to retrieve first directory entry. */
    /*EMPTY*/;
	}
 } 


 int initdir (DIR *dirp)
{ 
  dirp->dirent_filled = 0;

  if (findfirst (dirp->dirname, _A_SUBDIR | _A_RDONLY | _A_ARCH | _A_SYSTEM | _A_HIDDEN, &dirp->current) != 0)
  {
    /* findfirst will set errno to ENOENT when no 
     * more entries could be retrieved. */
    return 0;
  }
  // initialize DIR and it's first entry 
  dirp->dirent_filled = 1;
  return 1;
}


/*
 * Return implementation dependent name of the current directory entry.
 */
 const char * _getdirname (const struct dirent *dp)
{
  return dp -> d_name;
}


void displayentry(struct posix_header_ustar* entry){
	printf("\nname - %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
}


int findNext(struct dirent* current)
{
	struct posix_header_ustar* entry = (struct posix_header_ustar*)&_binary_tarfs_start + current->offset;
	if(entry != NULL){
		int padding =0; int offset = current -> offset;
		char tempstr[NAMEMAX], dirname[NAMEMAX]; int index;
		strncpy(tempstr,entry->name,strlen(entry->name));
		if(strcmp(entry->typeflag,"5") == 0 || strcmp(entry->typeflag,"0") == 0){
			strncpy(tempstr,entry->name,strlen(entry->name)-1);
			tempstr[strlen(entry->name)-1] = 0;
			index = lastIndexOf (tempstr, "/");
			if(index == -1 || index == strlen(entry->name)-1)
			dirname[0] = '\0';
			else
			substring(dirname ,entry->name , 0, index);
		}
		
		do{		
			int size = getSize(entry->size);	
			//increasing by size of the structure + size of file
			entry = (struct posix_header_ustar *)((char*)entry + sizeof(struct posix_header_ustar) + size );
			offset += 1 + (size/BLOCKSIZE);
			if(size > 0){
				padding = BLOCKSIZE - size%BLOCKSIZE;
				//printf("value - %d & padding - %d",(char*)&_binary_tarfs_end - (char*)entry, padding);
				if((char*)&_binary_tarfs_end - (char*)entry >= BLOCKSIZE && padding > 0)
				{
					entry = (struct posix_header_ustar *)((char*)entry + padding);
					offset++;
				}
				//printf(" ,address:%p",entry);
				}	
			if(starts_with( entry-> name,dirname) && strcmp(entry->typeflag,"5")  == 0) //starts_with( base,prefix)
			{
				if(indexOf_shift(entry-> name, "/", strlen(dirname)+1) == strlen(entry->name)-1){
					//indexOf_shift(base, str, startIndex)
					current -> offset = offset;	
					strncpy(current -> d_name, entry->name, NAMEMAX-1);
					//printf("found - %s",current->d_name);
					return 0;
				}
			}
			if(starts_with( entry-> name,dirname) && strcmp(entry->typeflag,"0")  == 0 ){
				if((indexOf_shift(entry-> name, "/", strlen(dirname)+1))==-1){
					current -> offset = offset;	
					strncpy(current -> d_name, entry->name, NAMEMAX-1);
					//printf("found - %s",current->d_name);
					return 0;
				}
			}	
			}while((uint64_t)entry < (uint64_t)&_binary_tarfs_end);
		}
		errno = ENOENT;
		return errno;
 }




int findfirst(char* dirname, char spec , struct dirent* data )
{	
	struct posix_header_ustar * entry = (struct posix_header_ustar *)&_binary_tarfs_start;
	
	//specs of files: not considering for now, (_A_SUBDIR | _A_RDONLY | _A_ARCH | _A_SYSTEM | _A_HIDDEN)
	if(entry != NULL){
		int padding =0, offset = 0;
		while( (uint64_t)entry < (uint64_t)&_binary_tarfs_end){
			int size = getSize(entry->size);
			if(starts_with(entry-> name,dirname) && strcmp(entry->typeflag,"5")  == 0) //starts_with( base,prefix)
			{
				if(indexOf_shift(entry-> name, "/", strlen(dirname)+1) == strlen(entry->name)-1){
					//indexOf_shift(base, str, startIndex)
					strncpy(data -> d_name,entry -> name,NAMEMAX-1);
					data -> offset = offset;
					//printf("found - %s",data->d_name);
					return 0;
				}
			}
			if(starts_with( entry-> name,dirname) && strcmp(entry->typeflag,"0")  == 0 ){
				if((indexOf_shift(entry-> name, "/", strlen(dirname)+1))==-1){
					strncpy(data -> d_name,entry -> name,NAMEMAX-1);
					data -> offset = offset;
					//printf("found - %s",data->d_name);
					return 0;
				}
			}
			//increasing by size of the structure + size of file
			entry = (struct posix_header_ustar *)((char*)entry + sizeof(struct posix_header_ustar) + size );
			offset += 1 + (size/BLOCKSIZE);
			if(size > 0){
				padding = BLOCKSIZE - size%BLOCKSIZE;
				//printf("value - %d & padding - %d",(char*)&_binary_tarfs_end - (char*)entry, padding);
				if((char*)&_binary_tarfs_end - (char*)entry >= BLOCKSIZE && padding > 0)
				{
					entry = (struct posix_header_ustar *)((char*)entry + padding);
					offset += 1;
				}
				//printf(" ,address:%p",entry);
				}	
			}
		}
		errno = ENOENT;
		return errno;
}


void list()
{
    struct posix_header_ustar * entry = (struct posix_header_ustar*)(&_binary_tarfs_start);
    
    printf("tarfs in [2q%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
    int padding =0;
	//printf("\nname - %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
    //while(strcmp(fileName,entry ->name) != 0 && (uint64_t)entry < (uint64_t)&_binary_tarfs_end)
    while((uint64_t)entry < (uint64_t)&_binary_tarfs_end)
        {
    	   int size = getSize(entry->size);
           
		   
			//printf("\nname - %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
           if(strcmp(entry->typeflag,"5")  == 0){
				//printf("  Directory -- %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
				printf(" \n Directory -- %s, size-%s, type %s ",entry ->name, entry->size, entry-> typeflag);
			}else if(strcmp(entry->typeflag,"0")==0){
				//printf("  file -- %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
				printf(" \n file -- %s, size-%s, type %s ",entry ->name, entry->size, entry-> typeflag);
			}else if(entry->typeflag == NULL){
				printf(" typeflag null");
			}else{
				//printf(" entry address : %p ,entry->typeflag %s, name %s",entry,entry->typeflag,entry->name);
			}
           //increasing by size of the structure + size of file
           entry = (struct posix_header_ustar *)((char*)entry + sizeof(struct posix_header_ustar) + size );
		   if(size > 0){
				padding = BLOCKSIZE - size%BLOCKSIZE;
				//printf("value - %d & padding - %d",(char*)&_binary_tarfs_end - (char*)entry, padding);
				if((char*)&_binary_tarfs_end - (char*)entry >=BLOCKSIZE)
				{
					entry = (struct posix_header_ustar *)((char*)entry + padding);
					//break;
				}
			 //printf(" ,address:%p",entry);
			 //add padding
			}	
		} 

}