#include "MemoryManager.h"
/* Create a manager to track the allocation of numPages of physical memory.
   You will create one by calling the constructor with NumPhysPages as
   the parameter.  All physical pages start as free, unallocated pages. */

#include "ProcessTable.h"
extern ProcessTable* processTable;

MemoryManager::MemoryManager(int numPages, int numSwapPages){
	this->numPages = numPages;
	this->numSwapPages = numSwapPages;
	lock = new Lock("MemManagerLock");
	map = new BitMap(numPages);
	vpnMap = new int[numPages];
	processMap = new int[numPages];
	swapSpace = new SwapPage*[numSwapPages];
	for(int i=0; i<numSwapPages; i++){
		swapSpace[i] = new SwapPage();
	}
	swapMap = new BitMap(numSwapPages);

}

/* Allocate a free page, returning its physical page number or -1
   if there are no free pages available. */
int MemoryManager::AllocPage(){
	lock->Acquire();
	int pageNum = map->Find();
	lock->Release();
	return pageNum;
}

/* Free the physical page and make it available for future allocation. */
void MemoryManager::FreePage(int physPageNum){
	lock->Acquire();
	map->Clear(physPageNum);
	lock->Release();

}

/* True if the physical page is allocated, false otherwise. */
bool MemoryManager::PageIsAllocated(int physPageNum){
	lock->Acquire();
	bool result = map->Test(physPageNum);
	lock->Release();
	return result;
}

int MemoryManager::Alloc(int processNo, int vpn){
	lock->Acquire();
	int pageNum = map->Find();
	if(pageNum>=0){
		processMap[pageNum] = processNo;
		vpnMap[pageNum] = vpn;
	}
	lock->Release();
	return pageNum;
}

int MemoryManager::AllocByForce(int processNo, int vpn
		){
	lock->Acquire();
	int pageNum = Random()%numPages; ///Random-------------replacement

	int oldProcess = processMap[pageNum];
	int oldVpn = processMap[pageNum];
	Thread* oldThread = (Thread*) processTable->Get(oldProcess);
	if(oldThread){
		printf("Swapping out a page of Thread: %s\n",oldThread->getName());
		saveIntoSwapSpacePrivate(oldThread->space, oldVpn);
	}

	processMap[pageNum] = processNo;
	vpnMap[pageNum] = vpn;

	lock->Release();
	return pageNum;
}

MemoryManager::~MemoryManager(){

	delete lock;
	delete map;
	delete[] vpnMap;
	delete[] processMap;
	delete swapMap;

	for(int i=0; i<numSwapPages; i++){
		if(swapSpace[i])
			delete swapSpace[i];
	}
	delete swapSpace;

}


int MemoryManager::saveIntoSwapSpacePrivate(AddrSpace* space, int vpn){
	TranslationEntry* pageTable = space->pageTable;
	int swapPageNo = space->swapPageMap[vpn];
	if(swapPageNo<0){
		swapPageNo = swapMap->Find();
		space->pageTable[vpn].dirty = TRUE;
	}
	ASSERT(swapPageNo>=0);
	if(space->pageTable[vpn].dirty){
		SwapPage* swapPage= swapSpace[swapPageNo];
		swapPage->writePage(pageTable[vpn].physicalPage);
	}
	return swapPageNo;
}

int MemoryManager::saveIntoSwapSpace(AddrSpace* space, int vpn){
	lock->Acquire();
	int swapPageNo = saveIntoSwapSpacePrivate(space, vpn);
	lock->Release();
	return swapPageNo;
}
