    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    // ----------------------------------------------------------
     
    void *PrintHello(void *arg);
    // ----------------------------------------------------------
     
    int main(int argc, char *argv[]){
        pthread_t thread[10];
        int i;
        void *status;

        status = NULL;

        for(i=0 ; i<10 ; ++i) {
            int rc = pthread_create(&thread[i], NULL, PrintHello, i);
            if(rc){
                printf("Return code: %d\n", rc);
                exit(-1);
            }
            pthread_join(thread[i], status);
        }
        printf("OVER\n");
        return 0;
    }
    // ----------------------------------------------------------
     
    void *PrintHello(void *arg) {
        printf("thread %d executing...\n", (int)arg);
        usleep(200000);
        printf("-thread %d executed...\n", (int)arg);
        return 0;
    }

