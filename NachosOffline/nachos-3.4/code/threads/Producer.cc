#include "Producer.h"
#include "copyright.h"
#include "system.h"

#include <string>
#include <sstream>

using namespace std;


Producer::Producer(int id, Container* container){//, Container container){
	this->id = id;
	this->container = container;


}

void Producer::produce(int item){
	container->put(item);
}
