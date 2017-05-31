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
extern int currentClock;
extern int* physPageLastClock;

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
    swapPageMap = new int[numPages];

    for (i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		pageTable[i].physicalPage = -1; //MMU->AllocPage();
		swapPageMap[i] = -1;

		pageTable[i].valid = FALSE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
					// a separate page, we could set its 
					// pages to be read-only
    }



}
int AddrSpace::loadIntoFreePage(int addr, int physicalPage){

	int mainAddr = addr;
	int vpn = addr/PageSize;
	addr = vpn*PageSize;
	int where;


	pageTable[vpn].physicalPage = physicalPage;
	pageTable[vpn].valid = TRUE;
	pageTable[vpn].dirty = FALSE;

	physPageLastClock[physicalPage] = currentClock++;

	if(doesSwapPageExist(vpn)){
		printf("Page got in swap space\n\n");
		loadFromSwapSpace(vpn);
		return 1;
	}
	printf("Page loading from executable\n");


	///offsets are in bytes here-------------

	int codeOffsetToStart,codeLeft, codeSizeToRead;
	int codeBase = noffH.code.inFileAddr;
	int dataOffsetToStart, dataLeft, dataSizeToRead;
	int dataBase = noffH.initData.inFileAddr;
	int uninitOffsetToStart, uninitLeft, uninitSizeToRead;
	int uninitBase = noffH.uninitData.inFileAddr;

	int CS = 0;
			 //noffH.code.virtualAddr;
	int CS_size = noffH.code.size;
	int DS = CS+CS_size;
			 //noffH.initData.virtualAddr;
	int DS_size = noffH.initData.size;
	int UDS = DS+DS_size;
			//noffH.uninitData.virtualAddr;
	int UDS_size = noffH.uninitData.size;
	int SS = UDS+UDS_size;
	int SS_size = UserStackSize;
	printf("Virtual addr: %d\n", mainAddr);
	printf("Page table size: %d VirtualPageNo: %d\n", numPages, vpn);
	printf("Segments:\nCS: %d to %d\nDS: %d to %d\nUDS: %d to %d\nSS: %d to %d\n", CS, CS+CS_size, DS, DS+DS_size, UDS, UDS+UDS_size, SS, SS+SS_size);
	printf("InFile:\nCS: %d -- DS: %d -- UDS: %d\n", codeBase, dataBase, uninitBase);

	if( CS_size>0 && addr>=CS && addr < CS+CS_size){
		///it means the starting was in code segment ---------
		printf("Verdict: address in Code Segment\n");

		//codeOffsetToStart = (addr - CS)/PageSize*PageSize;
		codeOffsetToStart = (addr - CS); //probable bugfix
		codeLeft = CS_size - codeOffsetToStart;
		codeSizeToRead = min(codeLeft, PageSize);
		where = PageSize*physicalPage;
		executable->ReadAt(machine->mainMemory+where, codeSizeToRead, codeBase+codeOffsetToStart);

		///code read finished------------
		if(codeSizeToRead<PageSize){  ///still something to read

				dataOffsetToStart = 0;
				dataLeft = DS_size - dataOffsetToStart;

				dataSizeToRead = min(dataLeft, PageSize-codeSizeToRead);
				where = PageSize*physicalPage+codeSizeToRead;
				if(DS_size>0)
				executable->ReadAt(machine->mainMemory+where, dataSizeToRead, dataBase+dataOffsetToStart);

				///data read finished ------------
				if(codeSizeToRead + dataSizeToRead< PageSize) {
					where = PageSize*physicalPage + codeSizeToRead + dataSizeToRead;
					bzero(machine->mainMemory+where, PageSize - (codeSizeToRead+dataSizeToRead));

					/*uninitOffsetToStart = 0;
					uninitLeft = UDS_size - uninitOffsetToStart;
					uninitSizeToRead = min(uninitLeft, PageSize);
					where = PageSize*physicalPage;
					if(UDS_size>0)
					executable->ReadAt(machine->mainMemory+where,uninitSizeToRead, uninitBase+uninitOffsetToStart);*/
					///uninit read finished--------
				}
		}


	}
	else if(DS_size>0 && addr>=DS && addr < DS+DS_size){
		printf("Verdict: address in InitData Segment\n");
		dataOffsetToStart = (addr - DS);
		dataLeft = DS_size - dataOffsetToStart;
		dataSizeToRead = min(dataLeft, PageSize);
		where = PageSize*physicalPage;

		executable->ReadAt(machine->mainMemory+where, dataSizeToRead, dataBase+dataOffsetToStart);

		if(dataSizeToRead< PageSize){
				where = PageSize*physicalPage + dataSizeToRead;
				bzero(machine->mainMemory+where, PageSize-dataSizeToRead);

				/*uninitOffsetToStart = 0;
									uninitLeft = UDS_size - uninitOffsetToStart;
									uninitSizeToRead = min(uninitLeft, PageSize);
									where = PageSize*physicalPage;
									if(UDS_size>0)
									executable->ReadAt(machine->mainMemory+where,uninitSizeToRead, uninitBase+uninitOffsetToStart);*/
				///uninit read finished--------
		}


	}

	else if(UDS_size>0 && addr>=UDS && addr < UDS+UDS_size){
		printf("Verdict: address in UninitData Segment\n");
		where = PageSize*physicalPage;
		bzero(machine->mainMemory+where, PageSize);

		/*uninitOffsetToStart = 0;
							uninitLeft = UDS_size - uninitOffsetToStart;
							uninitSizeToRead = min(uninitLeft, PageSize);
							where = PageSize*physicalPage;
							executable->ReadAt(machine->mainMemory+where,uninitSizeToRead, uninitBase+uninitOffsetToStart);*/
		///uninit read finished--------
	}
	else {

		printf("Verdict: address in Stack Segment\n");
		where = PageSize*physicalPage;
		bzero(machine->mainMemory+where, PageSize);
	}
	printf("\n");


	return 0;
}
void AddrSpace::saveIntoSwapSpace(int vpn){
	MMU->saveIntoSwapSpace(this, vpn);
}
void AddrSpace::loadFromSwapSpace(int vpn){
	MMU->loadFromSwapSpace(this, vpn);
}
bool AddrSpace::doesSwapPageExist(int vpn){
	return swapPageMap[vpn]!=-1;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   delete pageTable;
   delete executable;
   delete[] swapPageMap;
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
