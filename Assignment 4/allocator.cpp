#include <iostream>
#include <mutex>
#include <unistd.h>
#include <pthread.h>

using namespace std;

struct allocNode{
    int id;
    int index;
    int size;
    allocNode* next;
};

mutex mutexLock;
class HeapManager{

    private:
        allocNode* head;  //linked list is initially null.

    public:

    HeapManager() : head(NULL) {}

    void print(){
        allocNode* currNode=head;
        while(currNode!=NULL){
	    if(currNode->next==NULL){
            	cout<<"["<<currNode->id<<"] "<<"["<<currNode->size<<"] "<<"["<<currNode->index<<"]"<<endl;
	    }
	    else{
	    	cout<<"["<<currNode->id<<"] "<<"["<<currNode->size<<"] "<<"["<<currNode->index<<"]"<<"---";
	    }
	    currNode=currNode->next;
        }
    }

    int myMalloc(int ID, int size){
	
	mutexLock.lock();

        allocNode* currNode = head;
        allocNode* prevNode = NULL;

        while (currNode != NULL) {
            if (currNode->id == -1 && currNode->size >= size) {
                allocNode* allocateNode = new allocNode;
                allocateNode->id=ID;
                allocateNode->size=size;
                allocateNode->index=currNode->index;

                if (currNode->size > size) {
                    allocNode* remainingNode = new allocNode;
                    remainingNode->id=-1; //it is a free space node
                    remainingNode->size=currNode->size - size;
                    remainingNode->index=currNode->index + size;
                    remainingNode->next=currNode->next;

                    allocateNode->next=remainingNode;
                    delete currNode;
                    if (prevNode == NULL){ //if the list only contains the head
                        head = allocateNode; //replace the head with allocateNode
                    }
                    else{
                        prevNode->next=allocateNode;
                    }
                } 
                else { //if the space is just enough make the allocateNode the currentNode
                    allocateNode->next = currNode->next;
                    delete currNode;
                    if (prevNode == NULL){
                        head=allocateNode;
                    }
                    else{
                        prevNode->next=allocateNode;
                    }
                }
		cout<<"Allocated for thread "<<ID<<endl;
                print();
		mutexLock.unlock();
                return allocateNode->index;
            }

            prevNode = currNode;
            currNode = currNode->next;
        }
	cout<<"Can not allocate, requested size "<<size<<" for thread "<<ID<<" is bigger than the remaining size."<<endl;
        print();	
	mutexLock.unlock();
        return -1; //searching the list is over, couldn't find space
    }

    int myFree(int ID, int index){

	mutexLock.lock();
        allocNode* currNode = head;
        allocNode* prevNode = NULL;

        while (currNode != NULL) {
            if(currNode->id == ID && currNode->index == index){
                currNode->id=-1;
                if(prevNode!=NULL && prevNode->id==-1){
                    prevNode->size=currNode->size+prevNode->size;
                    prevNode->next=currNode->next;
                    delete currNode;
                    currNode=prevNode;
		    cout<<"Freed for thread "<<ID<<endl;
		    print();
		    mutexLock.unlock();
		    return 1;
                }
                else if(currNode->next->id==-1 && currNode->next!=NULL){
                    currNode->size=currNode->next->size+currNode->size;
                    allocNode* temp=currNode->next;
                    currNode->next=currNode->next->next;
                    delete temp;
		    cout<<"Freed for thread "<<ID<<endl;
		    print();
		    mutexLock.unlock();
		    return 1;
                }
        	else{
		    cout<<"Freed for thread "<<ID<<endl;
                    print();
                    mutexLock.unlock();
		    return 1;    
		}
        
            }
            prevNode = currNode;
            currNode = currNode->next;
        }
        cout<<"There was an error while calling the myFree function."<<endl;
        print();		
	mutexLock.unlock();
	return -1;

    }


    int initHeap(int size){

        allocNode* freeNode = new allocNode;
        freeNode->size=size;
        freeNode->id=-1; //chunk is free
        freeNode->index=0;
        freeNode->next=NULL;

        head=freeNode;
	print();
        return 1;
    }

};
