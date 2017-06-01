#ifndef HASHTABLE_H
#define HASHTABLE_H

#define null 0
#define DEFAULT_SIZE 20

class Node{
	Node* next;
	int key;
	void *item;
public:
	Node(int key, void* item);
	void setNext(Node* next);
	Node* getNext();
	int getKey();
	void* getItem();
	~Node();
};

class HashTable
{
	Node** table;
	int size;


	///--------private methods----------------------
	int hashcode(int key);

	///-------------public methods-----------------------------

public:

	HashTable(int size);

	~HashTable();


	bool put(int key, void* value);

	void* get(int key);
	bool remove(int key);
	//--------------------------------------------------some more functions-------------------------------------------



};


#endif;
/*
int main(){

   StringHashTable<int>* table = new StringHashTable<int>(20);
    int x = 20;
  table->put("abc", &x);
  int* y = table->get("abc");
  cout<<y<<endl;
    //cout<<table->get("abc")<<"  "<<table->get("ack");

    return 0;

}
*/
