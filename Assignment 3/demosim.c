#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

pthread_barrier_t barrier;
sem_t student, demoassistant, demostudent, finishstudent, finishassistant;

pthread_mutex_t generalMutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t generalMutex2 = PTHREAD_MUTEX_INITIALIZER;

int enterStudents=0;
int enterAssistants=0;

int demoStudents=0;
int demoAssistants=0;

int finishStudents=0;
int finishAssistants=0;

int Sparticipate=0;
int Aparticipate=0;



void *assistantfunc(void *arg){

     printf("Thread ID: %lu, Role:Assistant, I entered the classroom.\n", pthread_self());
         
     bool waker;
     sem_post(&student);
     sem_post(&student);
         
     pthread_mutex_lock(&generalMutex1);
     enterAssistants = enterAssistants+1;
     if(demoStudents>=2){
	waker=true;
        sem_post(&demostudent);
        sem_post(&demostudent);
	demoStudents=demoStudents-2;
	
     }   	
     else{
	waker=false;
        demoAssistants = demoAssistants+1;
        pthread_mutex_unlock(&generalMutex1);
	sem_wait(&demoassistant);
     }
   
     pthread_barrier_wait(&barrier);
     printf("Thread ID: %lu, Role:Assistant, I am now participating.\n", pthread_self());
     

     if(waker==true){
     	pthread_mutex_unlock(&generalMutex1);
     }

     pthread_mutex_lock(&generalMutex1);   
     Aparticipate=Aparticipate+1;
     pthread_mutex_unlock(&generalMutex1);

     while(((Aparticipate + Sparticipate)%3!=0) || Sparticipate < 2 || Aparticipate < 1){
        sem_wait(&finishassistant);
     }
     
     printf("Thread ID: %lu, Role:Assistant, demo is over.\n", pthread_self());
     
     pthread_mutex_lock(&generalMutex2);
     
     bool terminate=true;
     while(terminate){
     	if((enterStudents==0) && (enterAssistants-1==0)){
	  terminate=false;
	}
	else if((3*(enterAssistants-1)) > enterStudents){
	  terminate=false;
	}
	else{
	
        }

     }
        
     enterAssistants=enterAssistants-1;
     printf("Thread ID: %lu, Role:Assistant, I left the classroom.\n", pthread_self());
     sem_post(&finishstudent);
     sem_post(&finishstudent); 
     pthread_mutex_unlock(&generalMutex2);
}


void *studentfunc(void *arg){
   
   
   printf("Thread ID: %lu, Role:Student, I want to enter the classroom.\n", pthread_self());
   //sem_wait(&student);
  
   bool terminate=true;
     while(terminate){
        sem_wait(&student); 	
        if((enterStudents==0) && (enterAssistants==0)){
          terminate=false;
        }
	else if((3*(enterAssistants)) > (enterStudents+1)){
         terminate=false;
        }
        else{

        }
     } 
   printf("Thread ID: %lu, Role:Student, I entered the classroom.\n", pthread_self());

   
   bool waker;
  
   pthread_mutex_lock(&generalMutex1);
   enterStudents = enterStudents+1;
   if(demoAssistants>=1 && demoStudents>=1){
	waker=true;      	
        sem_post(&demoassistant);
	sem_post(&demostudent);
	demoAssistants=demoAssistants-1;
	demoStudents=demoStudents-1;

   }
   else{ 
        demoStudents = demoStudents+1;
	waker=false;
        pthread_mutex_unlock(&generalMutex1);
   	sem_wait(&demostudent);
   }   
   
   pthread_barrier_wait(&barrier);
   printf("Thread ID: %lu, Role:Student, I am now participating.\n", pthread_self());
   
   if(waker==true){
   	pthread_mutex_unlock(&generalMutex1);
   }

   pthread_mutex_lock(&generalMutex1);
   Sparticipate=Sparticipate+1;
   pthread_mutex_unlock(&generalMutex1);

   while(((Aparticipate + Sparticipate)%3!=0) || Sparticipate < 2 || Aparticipate < 1){
   	sem_post(&finishstudent);
   }
   
   
   pthread_mutex_lock(&generalMutex1);
   printf("Thread ID: %lu, Role:Student, I am now leaving.\n", pthread_self());
   enterStudents=enterStudents-1;
   sem_post(&finishstudent);
   sem_post(&finishassistant);
   sem_post(&student);
   pthread_mutex_unlock(&generalMutex1);
   
}



int main(int argc, char *argv[]){

	
   int numStudent, numAssistant;

   numAssistant = atoi(argv[1]);
   numStudent = atoi(argv[2]);
   
   printf("My program compiles with all conditions, it has 1 possible busy-waiting problem area for the assistant.\n");

   if((numAssistant * 2 != numStudent) || (numAssistant < 0) || (numStudent < 0)){
   	printf("The Main terminates.\n");
   }
   else{
	
	sem_init(&student, 0, 0);
        sem_init(&demostudent, 0, 0);
        sem_init(&demoassistant, 0, 0);
	sem_init(&finishstudent, 0, 1);
	sem_init(&finishassistant, 0, 1);

	pthread_barrier_init(&barrier,NULL,3);
	int totalThreads = numStudent + numAssistant;
	pthread_t allThreads[totalThreads];
	
	for (int i=0; i<numAssistant; i++){
		pthread_create (&allThreads[i], NULL, assistantfunc, NULL);
	}
	for (int i=numAssistant; i<totalThreads; i++){
		pthread_create (&allThreads[i], NULL, studentfunc, NULL);
	}
	for(int k=0; k<totalThreads; k++){
                pthread_join (allThreads[k],NULL);
        }

	printf("The Main terminates.\n");
   }
}
