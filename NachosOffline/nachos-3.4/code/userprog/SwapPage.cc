/*
 * SwapPage.cc
 *
 *  Created on: May 29, 2017
 *      Author: mamun
 */

#include "SwapPage.h"

//SwapPage::

SwapPage::SwapPage(){
	bytes = new char[PageSize];
}

SwapPage::~SwapPage(){
	if(bytes)
	delete[] bytes;
}

int SwapPage::writeByte(int position, char data){
	if(position<0 || position > PageSize) {printf("bug at SwapPage::writeByte() "); return -1; }
	bytes[position] = data;
	return position;
}


int SwapPage::writePage(int physicalPage){
	if(physicalPage<0 || physicalPage>NumPhysPages){printf("bug at SwapPage::writePage() "); return -1;}

	int where = physicalPage*PageSize;
	for(int i=0; i<PageSize; i++){
		bytes[i] =  machine->mainMemory[where+i];
	}
	return physicalPage;
}


