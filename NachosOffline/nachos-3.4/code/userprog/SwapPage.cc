/*
 * SwapPage.cc
 *
 *  Created on: May 29, 2017
 *      Author: mamun
 */

#include "SwapPage.h"

//SwapPage::

SwapPage::SwapPage(){
	bytes = new unsigned char[PageSize];
	valid = TRUE;
}

SwapPage::~SwapPage(){
	if(bytes)
	delete[] bytes;
}

int SwapPage::writeByte(int position, char data){
	if(position<0 || position > PageSize) {printf("bug at SwapPage::writeByte() "); return -1; }
	bytes[position] = (unsigned char) data;
	return position;
}

void SwapPage::assignPage(int processId, int vpn){
	valid = TRUE;
	this->processId = processId;
	this->vpn = vpn;
}

void SwapPage::invalidate(){
	valid = FALSE;
}

