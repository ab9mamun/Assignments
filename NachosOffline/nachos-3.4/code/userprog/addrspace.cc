// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "synch.h"
#include "MemoryManager.h"


#ifdef HOST_SPARC
#include <strings.h>
#endif


extern MemoryManager* MMU;
extern Lock* MMU_lock;

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------



static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{

    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    this->noffH = noffH;
    this->executable = executable;
// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

//    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		pageTable[i].physicalPage = -1; //MMU->AllocPage();

		if(pageTable[i].physicalPage<0){
			///free the allocated pages-------------------------
			for(int currentVirtualPage = 0; currentVirtualPage<i; currentVirtualPage++){
	//			MMU->FreePage(pageTable[currentVirtualPage].physicalPage);
			}
		//	ASSERT(FALSE);
		}

		pageTable[i].valid = FALSE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
					// a separate page, we could set its 
					// pages to be read-only
    }
    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    //bzero(machine->mainMemory, size);

    /*
     * we cannot clear the entire memory, rather, we will clear the memory allocated to this process=============
     *
     *
     */
    MMU_lock->Acquire();
    for(int i=0; i<numPages; i++){
    	int locationInMemory = PageSize*pageTable[i].physicalPage;
   // 	bzero(machine->mainMemory+locationInMemory, PageSize);

    }

// then, copy in the code and data segments into memory
    int numPagesCS= divRoundUp(noffH.code.size, PageSize);
    int numPagesDS= divRoundUp(noffH.initData.size, PageSize);

    int codeBase = noffH.code.inFileAddr;
    int CS = 0;

    for(int i=0; i<numPagesCS; i++){
    	int where = PageSize*pageTable[CS+i].physicalPage;
    //	executable->ReadAt(machine->mainMemory+where, PageSize, codeBase+ i*PageSize);
    }

    int dataBase = noffH.initData.inFileAddr;
    int DS = CS+numPagesCS;
    for(int i=0; i<numPagesDS; i++){
        	int where = PageSize*pageTable[DS+i].physicalPage;
      //  	executable->ReadAt(machine->mainMemory+where, PageSize, dataBase+ i*PageSize);
     }

    MMU_lock->Release();

   /* if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }
    */

}
int AddrSpace::loadIntoFreePage(int addr, int physicalPage){

	int vpn = addr/PageSize;
	pageTable[vpn].physicalPage = physicalPage;
	pageTable[vpn].valid = TRUE;

	///offsets are in bytes here-------------

	int codeOffsetToStart,codeLeft, codeSizeToRead;
	int codeBase = noffH.code.inFileAddr;
	int dataOffsetToStart, dataLeft, dataSizeToRead;
	int dataBase = noffH.initData.inFileAddr;
	int uninitOffsetToStart, uninitLeft, uninitSizeToRead;
	int where;

	addr = addr/PageSize*PageSize;

	if(addr>=noffH.code.virtualAddr && addr < noffH.code.virtualAddr+noffH.code.size){
		///it means the starting was in code segment ---------

		codeOffsetToStart = (addr - noffH.code.virtualAddr)/PageSize*PageSize;
		codeLeft = noffH.code.size - codeOffsetToStart;
		codeSizeToRead = min(codeLeft, PageSize);
		where = PageSize*physicalPage;
		executable->ReadAt(machine->mainMemory+where, codeSizeToRead, codeBase+codeOffsetToStart);

		///code read finished------------
		if(codeSizeToRead<PageSize){  ///still something to read

				dataOffsetToStart = 0;
				dataLeft = noffH.initData.size - dataOffsetToStart;

				dataSizeToRead = min(dataLeft, PageSize-codeSizeToRead);
				where = PageSize*physicalPage+codeSizeToRead;
				executable->ReadAt(machine->mainMemory+where, dataSizeToRead, dataBase+dataOffsetToStart);

				///data read finished ------------
				if(codeSizeToRead + dataSizeToRead< PageSize) {
					where = PageSize*physicalPage + codeSizeToRead + dataSizeToRead;
					bzero(machine->mainMemory+where, PageSize - (codeSizeToRead+dataSizeToRead));
					///uninit read finished--------
				}
		}


	}
	else if(addr>=noffH.initData.virtualAddr && addr < noffH.initData.virtualAddr+noffH.initData.size){
		dataOffsetToStart = (addr - noffH.initData.virtualAddr)/PageSize*PageSize;
		dataLeft = noffH.initData.size - dataOffsetToStart;
		dataSizeToRead = min(dataLeft, PageSize);
		where = PageSize*physicalPage;

		executable->ReadAt(machine->mainMemory+where, dataSizeToRead, dataBase+dataOffsetToStart);

		if(dataSizeToRead< PageSize){
				where = PageSize*physicalPage + dataSizeToRead;
				bzero(machine->mainMemory+where, PageSize-dataSizeToRead);
				///uninit read finished--------
		}


	}

	else {
		where = PageSize*physicalPage;
		bzero(machine->mainMemory+where, PageSize);
		///uninit read finished--------
	}




	//}

	//int dataBase = noffH.initData.inFileAddr;
	//int DS = CS+numPagesCS;
	//for(int i=0; i<numPagesDS; i++){
		//int where = PageSize*pageTable[DS+i].physicalPage;
		//  	executable->ReadAt(machine->mainMemory+where, PageSize, dataBase+ i*PageSize);
	//}
	//int unititBase = noffH.uninitData.inFileAddr;



	//printf("page loaded\n");
	return 0;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   delete pageTable;
   delete executable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
