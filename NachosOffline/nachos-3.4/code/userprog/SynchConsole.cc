/*
 * SynchConsole.cc
 *
 *  Created on: May 12, 2017
 *      Author: mamun
 */

#include "SynchConsole.h"

 extern Semaphore* readAvail;
 extern Semaphore* writeDone;
 extern Console* console;


SynchConsole::SynchConsole(){

	lock = new Lock("Console lock");
}

SynchConsole::~SynchConsole(){
	delete lock;
}


int SynchConsole::Read(char* input, int size, int id){
	lock->Acquire();
	printf("\nCONSOLE READING (Please enter something).....\n");
	int char_read = 0;
	while(char_read<size){
		readAvail->P();
		char ch = console->GetChar();
		if(ch=='\n') break;
		input[char_read] = ch;
		char_read++;
	}
	input[char_read] = 0;

	lock->Release();
	return char_read;

}
int SynchConsole::Write(const char* output, int size, int id){
	lock->Acquire();
	printf("\nCONSOLE WRITING.....\n");
		int char_written = 0;
		while(char_written<size){
			char ch = output[char_written];
			if(ch==0) break;
			console->PutChar(ch);
			writeDone->P() ;
			char_written++;
		}
		console->PutChar('\n');
		writeDone->P() ;

		lock->Release();
		return char_written;

}

