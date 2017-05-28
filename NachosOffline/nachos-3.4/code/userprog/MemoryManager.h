#ifndef MEMMANAGER_H
#define MEMMANAGER_H

#include "bitmap.h"
#include "synch.h"
#include "SwapPage.h"




class MemoryManager{
/* Create a manager to track the allocation of numPages of physical memory.
   You will create one by calling the constructor with NumPhysPages as
   the parameter.  All physical pages start as free, unallocated pages. */
	Lock* lock;
	BitMap* map;
	BitMap* swapMap;
	SwapPage** swapSpace;
	int saveIntoSwapSpacePrivate(AddrSpace* space, int vpn);
public:

MemoryManager(int numPages, int numSwapPages);

/* Allocate a free page, returning its physical page number or -1
   if there are no free pages available. */
int AllocPage();

/* Free the physical page and make it available for future allocation. */
void FreePage(int physPageNum);

/* True if the physical page is allocated, false otherwise. */
bool PageIsAllocated(int physPageNum);

///added for 3rd assignment----
int Alloc(int processNo, int vpn);
int AllocByForce(int processNo, int vpn
		);

int* processMap;
int* vpnMap;
int numPages;
int numSwapPages;
int saveIntoSwapSpace(AddrSpace* space, int vpn);

~MemoryManager();




};







#endif
