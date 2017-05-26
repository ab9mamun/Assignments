// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "ProcessTable.h"
#include "MemoryManager.h"
#include "SynchConsole.h"

extern ProcessTable* processTable;
extern Lock* syscallLock;
extern MemoryManager* MMU;
extern SynchConsole* myconsole;
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void forkProcess(int notUsed)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
// load page table register
	machine->Run(); // jump to the user progam
	ASSERT(FALSE); // machine->Run never returns;
}

void incrementPC(){
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
	int pc;
	pc=machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg,pc);
	pc=machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg,pc);
	pc += 4;
	machine->WriteRegister(NextPCReg,pc);

	(void) interrupt->SetLevel(oldLevel);	// re-enable interrupts

}

void ExecProcess(){
	syscallLock->Acquire();	// acquire lock
	    	//	printf("inside sc_exec\n");

	    		int fileNameAddress = machine->ReadRegister(4); //only 1 argument, so a0 (register 4) will get the argument
	    		char* fileName = new char[150];
	    		int temp;
	    		int i=0;
	    		while(true){
	    			if(!machine->ReadMem(fileNameAddress,1,&temp))  {
	    				syscallLock->Release();
	    				return;
	    			}
					fileName[i]=(char) temp;
					fileNameAddress++;
					i++;
					if(!temp) break;
	    		}
	    		fileName[i] = (char) 0;
	    	//	printf("inside sc_exec\n");
	    		printf("Attempting to execute %s\n", fileName);
	    		OpenFile* executable = fileSystem->Open(fileName);
	    		if(executable==0){
	    			printf("can't openFile %s\n", fileName);
	    			syscallLock->Release();
	    			return;
	    		}
	    		printf("inside sc_exec\n");
	    		AddrSpace* space = new AddrSpace(executable);
	    		Thread* newThread = new Thread("Forked processs");
	    		int processId = processTable->Alloc((void*) newThread);
	    		if(processId<0){
	    			printf("not enough space for new process\n");
	    		}
	    		else {
	    			printf("new process id: %d\n", processId);
	    		}
	    		machine->WriteRegister(2, processId);

	    		newThread->space = space;
	    		newThread->processId = processId;

	    		delete fileName;
//	    		delete executable;
	    		//printf("inside sc_exec\n");

	    		syscallLock->Release();	// release lock
	    		newThread->Fork(forkProcess, 0);
	    		/* routine task – do at last -- generally manipulate PCReg,
	    		PrevPCReg, NextPCReg so that they point to proper place*/
	    		incrementPC();
	    		//printf("inside sc_exec\n");

	    		//printf("exiting sc_exec\n");
}
void ExitProcess(){
	syscallLock->Acquire();

	int returnVal = machine->ReadRegister(4);
	printf("Exiting with return val: %d\n", returnVal);

	int pts = machine->pageTableSize;
	TranslationEntry* pageTable = machine->pageTable;
	//	printf("freeing memory\n");
	for(int i=0; i<pts; i++){
		if(pageTable[i].valid)
			MMU->FreePage(pageTable[i].physicalPage);
	}
	//printf("memory freed\n");
	//printf("processTable: %d\n", processTable);
	//printf("currentThread: %d processId: %d\n", currentThread, currentThread->processId);
	processTable->Release(currentThread->processId);
	//	printf("released from process table\n");
	 bool empt = processTable->empty();
	 syscallLock->Release();
	 if(empt) interrupt->Halt();
	 else currentThread->Finish();
}
void HandleRead(){
	syscallLock->Acquire();
	int bufferAddress = machine->ReadRegister(4); //3 arguments, so registers 4, 5, 6 will get the arguments
	int size = machine->ReadRegister(5);
	int id = machine->ReadRegister(6);

	char* myBuffer = new char[size+5];
	int temp;
	int i=0;
	int char_read = myconsole->Read(myBuffer, size, id);
	//printf("My ReadBuffer: %s\n", myBuffer);

	while(i<=char_read){
	    if(!machine-> WriteMem(bufferAddress, 1, (int) myBuffer[i])){
	    	syscallLock->Release();
	    	return;
	    }
	    bufferAddress++;
	    i++;
	    if(!myBuffer[i]) break;
	}
	delete myBuffer;
	machine->WriteRegister(2, char_read);
	syscallLock->Release();
	//printf("here i am inside read\n");
	incrementPC();

}
void HandleWrite(){
	syscallLock->Acquire();

	    	//	printf("here i am inside write\n");
	    		int bufferAddress = machine->ReadRegister(4); //3 arguments, so registers 4, 5, 6 will get the arguments
	    		int size = machine->ReadRegister(5);
	    		int id = machine->ReadRegister(6);

	      		char* myBuffer = new char[size+5];
	    		int temp;
	    		int i=0;


	    		while(i<size){
	    			if(!machine->ReadMem(bufferAddress,1,  &temp)){
	    				syscallLock->Release();
	    		    	return;
	    		    }
	    			myBuffer[i] = (char) temp;
	    			bufferAddress++;
	    		    i++;
	    		    if(!temp) break;
	    		 }
	    		myBuffer[i] = (char) 0;
	    		//printf("My WriteBuffer: %s\n", myBuffer);

	    		int char_written = myconsole->Write(myBuffer, size, id);

	    		 delete myBuffer;
	    		 machine->WriteRegister(2, char_written);
	    		 syscallLock->Release();
	    		 incrementPC();

}



///----------------------------------handling other exceptions those are not syscall--------------
void HandlePageFault(){
	int addr = machine->ReadRegister(39);
	int virtualPageNo = addr/PageSize;
	//printf("Page fault culprit: %d\n", virtualPageNo);
	int physicalPage = MMU->AllocPage();
	if(physicalPage<0){
		//free a page--------
	}
	currentThread->space->loadIntoFreePage(addr, physicalPage);

}



void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which == SyscallException) {
    	if(type==SC_Halt){
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
    	}
    	else if(type==SC_Exec){

    		ExecProcess();
    	}

    	else if(type==SC_Exit){
    		ExitProcess();
    	}


    	else if(type==SC_Read){
    		HandleRead();
    	}
    	else if(type==SC_Write){
    		HandleWrite();
    	}

    	else {
    		printf("Not supported system call exception %d %d\n", which, type);
    		//ASSERT(FALSE);
    	 }
    }
    else if(which==PageFaultException) {
    //	printf("Page Fault Exception.. killing the process\n");
    	HandlePageFault();
//    	ExitProcess();
    }
    else if(which==ReadOnlyException) {
        printf("ReadOnlyException.. killing the process\n");
       	ExitProcess();
    }
    else if(which==BusErrorException) {
    	printf("BusErrorException.. killing the process\n");
        ExitProcess();
     }
    else if(which==AddressErrorException) {
       printf("AddressErrorException.. killing the process\n");
       ExitProcess();
    }
    else if(which==OverflowException) {
       printf("OverflowException.. killing the process\n");
       ExitProcess();
     }
    else if(which==NumExceptionTypes) {
       printf("NumExceptionTypes.. killing the process\n");
       ExitProcess();
    }
    else if(which==IllegalInstrException) {
       printf("IllegalInstrException.. killing the process\n");
       ExitProcess();
    }
    else {
    	printf("Unrecognized exception %d %d ..killing the process\n", which, type);
    	ExitProcess();
    }
}
