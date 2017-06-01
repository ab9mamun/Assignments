/*
 * SynchConsole.h
 *
 *  Created on: May 12, 2017
 *      Author: mamun
 */


#ifndef USERPROG_SYNCHCONSOLE_H_
#define USERPROG_SYNCHCONSOLE_H_


#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "console.h"

#define CONSOLE_BUFFER_SIZE 50



class SynchConsole{
Lock* lock;

public:
SynchConsole();

~SynchConsole();

int Read(char* input, int size, int id);
int Write(const char* output, int size, int id);

};


#endif /* USERPROG_SYNCHCONSOLE_H_ */
