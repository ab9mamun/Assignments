#include "Consumer.h"
#include "copyright.h"
#include "system.h"

#include <sstream>
#include <string>

using namespace std;


Consumer::Consumer(int id, Container* container){
	this->id = id;
	this->container = container;
}



int Consumer::consume(){
	return container->get();


}
