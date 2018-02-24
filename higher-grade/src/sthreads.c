/* On Mac OS (aka OS X) the ucontext.h functions are deprecated and requires the
   following define.
*/
#define _XOPEN_SOURCE 700

/* On Mac OS when compiling with gcc (clang) the -Wno-deprecated-declarations
   flag must also be used to suppress compiler warnings.
*/

#include <signal.h>   /* SIGSTKSZ (default stack size), MINDIGSTKSZ (minimal
                         stack size) */
#include <stdio.h>    /* puts(), printf(), fprintf(), perror(), setvbuf(), _IOLBF,
                         stdout, stderr */
#include <stdlib.h>   /* exit(), EXIT_SUCCESS, EXIT_FAILURE, malloc(), free() */
#include <ucontext.h> /* ucontext_t, getcontext(), makecontext(),
                         setcontext(), swapcontext() */
#include <stdbool.h>  /* true, false */

#include "sthreads.h"

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ*100
#define MAXIMUM_NUMBER_OF_THREADS 10
/*******************************************************************************
                             Global data structures

                Add data structures to manage the threads here.
********************************************************************************/
struct queue readyQueue, waitingQueue;
tid_t  threadId = 0;


/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/



/// Initilizes and allocates a new TCB. Sets the state to ready and the T_ID to threadId
thread_t* init_thread(){
   if (threadId>=MAXIMUM_NUMBER_OF_THREADS){
       printf("THREAD POOL IS FULL");
       return NULL;}
     
     thread_t* temp = malloc(sizeof(thread_t));
     temp->state = ready;
     temp->tid = threadId;
     
     threadId++;
     return temp; 
} 

void init_context(ucontext_t *ctx){
    void *stack = malloc(STACK_SIZE); // Allocate stack for the thread.
    
    if(stack == NULL ){
    perror("STACK ALLOCATION");
    exit(EXIT_FAILURE); 
    }
    
    if(getcontext(ctx)<0){
     perror("getcontext");
     exit(EXIT_FAILURE);
    }
    
    ctx->uc_stack.ss_sp = stack;
    ctx->uc_stack.ss_size = STACK_SIZE;
    ctx->uc_stack.ss_flags = 0;
      
     //Step trough queue and links the new TCB to the last one in the queue
    //thread_t *temp = readyQueue.first->next;
   // while(temp->next != NULL){
      //    temp = temp->next;}
  
} 



/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/


int init(){
    // allocates spaces for the ready queue and sets up the first and last pointer.
    readyQueue.first = calloc(MAXIMUM_NUMBER_OF_THREADS,sizeof(thread_t)); 
    readyQueue.last = readyQueue.first;
    

  return 1;
}


tid_t spawn(thread_t thread,void (*start)()){
   
    init_context(&(thread.ctx));
    makecontext(&thread.ctx, start,0 ); // The zero indicates that the start function takes no arguments.
   
       // If there are any TCB in the ready queue, link the new one to the last in the queue.
      if (threadId!=0){
        readyQueue.last->next = &thread;} 

     // Update the queue //  
     if(readyQueue.last!=readyQueue.first || (readyQueue.first!=NULL && readyQueue.first == readyQueue.first)){
       thread.next = readyQueue.last;
       readyQueue.last = &thread;
     }

     else if(readyQueue.first==NULL){
     readyQueue.first = &thread;
     readyQueue.last = &thread;}

     return thread.tid;
  //return -1;
}

void yield(){
}

void  done(){
}

void destroyQueue(){
     free(readyQueue.first);}
     tid_t join() {
  return -1;
}
