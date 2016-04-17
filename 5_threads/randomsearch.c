#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
// ----------------------------------------------------------
 
#define NUM_THREADS 5
#define TARGET 100
// ----------------------------------------------------------
 
struct thread_params {
    int target;
    int thread_idx;
};
// ----------------------------------------------------------
 
struct thread_params tp[NUM_THREADS];
pthread_t threads[NUM_THREADS];
pthread_mutex_t mutex;
int tries;
 
void *search(void *arg);
void cleanup(void *args);
// ----------------------------------------------------------
 
int main (int argc, char *argv[]){
    int ti;
    int target=TARGET;
 
    tries = 0;
    pthread_mutex_init(&mutex, NULL); 
 
    printf("Searching for: %d\n", target);
 
    for (ti=0;ti<NUM_THREADS;++ti){
      tp[ti].target = target;
      tp[ti].thread_idx = ti;
      pthread_create(&threads[ti], NULL, search, &tp[ti]);
    }
 
    for (ti=0;ti<NUM_THREADS;ti++){ 
        pthread_join(threads[ti], NULL);
    }
    //pthread_cleanup_push(cleanup, NULL);

    printf("Number of all iterations: %d.\n", tries);
    pthread_mutex_destroy(&mutex);
    return 0;
}
// ----------------------------------------------------------
 
void cleanup(void *args) {
  printf("cleaning ...\n");
}

void *search(void *arg){
    struct thread_params *params = (struct thread_params*)arg;
    int threadIdx = params->thread_idx; // indeks watku
    int toFind = params->target;   // wartosc do odszukania
    int ti = 0;
    int rnd;
    printf("Thread %d searching for %d\n", threadIdx, toFind);

    pthread_t tid = pthread_self();
    srand(tid);
 
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    while(1){
      while(pthread_mutex_trylock(&mutex) == EBUSY){
        pthread_testcancel();
      }
      ++tries;
      pthread_mutex_unlock(&mutex);

      rnd = (int)(rand()%1000);
      if (toFind == rnd) {
          while(pthread_mutex_trylock(&mutex) == EBUSY){
              pthread_testcancel();
          }
          printf("Number found by %d!\n", threadIdx);
          for(ti=0;ti<NUM_THREADS;ti++){
              if(ti == threadIdx)      // kasowanie innych watkow - ten watek zakoczy sie normalnie
                  continue;
              printf("cancelling thread %d\n", ti);
              pthread_cancel(threads[ti]);
          }
          pthread_mutex_unlock(&mutex);
          break;
      }
    }
 
    return((void *)0);
}
// ----------------------------------------------------------

