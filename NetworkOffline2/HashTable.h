#include <bits/stdc++.h>

#define null 0
#define DEFAULT_SIZE 20

using namespace std;

template<class Y>
class StringHashTable
{
	vector< pair<string, Y*>* >** table;
	int size;
	stringstream out;


	///--------private methods----------------------
	int hashcode(string key){
      //  cout<<"\""<<key<<"\""<<endl;
	    int seed = 131;
        unsigned long hash = 0;
        for(int i = 0; i < key.length(); i++){
		if(key[i]==' ' || key[i]==',') break;
                hash = (hash * seed) + key[i];
        }
        return hash % size;
	}





	///-------------public methods-----------------------------

public:

	StringHashTable(int size){
		this->size = size;
		table = new vector<pair<string, Y*>* >*[size];
		for(int i=0; i<size; i++){
            table[i] = new vector<pair<string, Y*>*>;
		}

	}
	StringHashTable(){
		this->size = DEFAULT_SIZE;
		table = new vector<pair<string, Y*>* >*[size];

		for(int i=0; i<size; i++){
            table[i] = new vector<pair<string, Y*>*>;
		}
	}
	~StringHashTable(){
		if(table==null) return;

		for(int i=0; i<size; i++){
			vector<pair<string, Y*> > temp = table[i];

			for(int i=0; i<temp->size(); i++){

				delete (*temp)[i];
			}
			delete[] temp;
		}
		delete[] table;
	}

	bool put(string key, Y* value){

		int index = hashcode(key);

		vector<pair<string, Y*>* >* temp = table[index];
		for(int i=0; i<temp->size(); i++){

            if( (*temp)[i]->first==key) return false;
		}
		temp->push_back( new pair<string, Y*> (key, value));


		return true;
	}
	Y* get(string key){
		int index = hashcode(key);
		vector <pair<string, Y*>* >*temp = table[index];

		for(int i=0; i<temp->size(); i++){
            if( (*temp)[i]->first==key) return (*temp)[i]->second;
		}
		return null;
	}



	//--------------------------------------------------some more functions-------------------------------------------



};

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
