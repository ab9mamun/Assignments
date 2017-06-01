#include "MemoryManager.h"
/* Create a manager to track the allocation of numPhysPages of physical memory.
   You will create one by calling the constructor with NumPhysPages as
   the parameter.  All physical pages start as free, unallocated pages. */

#include "ProcessTable.h"
extern ProcessTable* processTable;
extern int currentClock;
extern int* physPageLastClock;

MemoryManager::MemoryManager(int numPhysPages, int numSwapPages){
	this->numPhysPages = numPhysPages;
	this->numSwapPages = numSwapPages;
	lock = new Lock("MemManagerLock");
	map = new BitMap(numPhysPages);
	vpnMap = new int[numPhysPages];
	processMap = new int[numPhysPages];
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
	if(pageNum>=0){
	//	printf("Allocating physical page number: %d\n", pageNum);
	}
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
		printf("Allocating physicalPage number: %d\n", pageNum);
		processMap[pageNum] = processNo;
		vpnMap[pageNum] = vpn;
	}
	lock->Release();
	return pageNum;
}

int MemoryManager::AllocByForce(int processNo, int vpn
		){
	lock->Acquire();
	int pageNum;
	/////////Random=========================
	pageNum = Random()% numPhysPages ; ///Random-------------replacement

	///////////////===========================================



	///===================LRU-----------------------
	pageNum = 0;
	int min = physPageLastClock[0];

	for(int i=1; i<numPhysPages; i++){
		if(physPageLastClock[i]<min){
			min = physPageLastClock[i];
			pageNum = i;
		}
	}
	/////////------------------------------------------------


	printf("Allocating physicalPage number: %d\n", pageNum);

	int oldProcess = processMap[pageNum];
	int oldVpn = vpnMap[pageNum];
	Thread* oldThread = (Thread*) processTable->Get(oldProcess);
	if(oldThread){
		printf("Swapping out a page of process: %d page: %d\nReplacing with  process: %d virtualPage: %d\n",
				oldProcess, oldVpn, processNo, vpn);
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

int MemoryManager::loadFromSwapSpacePrivate(AddrSpace* space, int vpn){
	TranslationEntry* pageTable = space->pageTable;
	int swapPageNo = space->swapPageMap[vpn];
	ASSERT(swapPageNo>=0);
	SwapPage* swapPage = swapSpace[swapPageNo];
	swapPage->readPage(pageTable[vpn].physicalPage);
	stats->numPageIns++;
	return swapPageNo;

}

int MemoryManager::loadFromSwapSpace(AddrSpace* space, int vpn){
	lock->Acquire();
	int swapPageNo = loadFromSwapSpacePrivate(space,vpn);

	lock->Release();
	return swapPageNo;

}


int MemoryManager::saveIntoSwapSpacePrivate(AddrSpace* space, int vpn){
	TranslationEntry* pageTable = space->pageTable;
	int swapPageNo = space->swapPageMap[vpn];
	pageTable[vpn].valid = FALSE;
	if(swapPageNo<0){
		swapPageNo = swapMap->Find();
		space->pageTable[vpn].dirty = TRUE;
		space->swapPageMap[vpn] = swapPageNo;
	}
	ASSERT(swapPageNo>=0);
	if(pageTable[vpn].dirty){
		SwapPage* swapPage= swapSpace[swapPageNo];
		swapPage->writePage(pageTable[vpn].physicalPage);
		stats->numPageOuts++;
	}
	return swapPageNo;
}

int MemoryManager::saveIntoSwapSpace(AddrSpace* space, int vpn){
	lock->Acquire();
	int swapPageNo = saveIntoSwapSpacePrivate(space, vpn);
	lock->Release();
	return swapPageNo;
}
