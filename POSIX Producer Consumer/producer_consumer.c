#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <threads.h>
#include <stdint.h>

#define BIGVAL 157429221
#define MAXLEN 32
#define bool uint8_t
#define true 1
#define false 0

typedef struct {
    int data;
    bool state;
}buffer;

typedef struct {
    buffer buffers[MAXLEN];
    int prodCur;
    int consumeCur;
}T;

sem_t s_empty;
sem_t s_full;
T g_buff;

int produce(){
    
    return rand() % BIGVAL + 1;
}

void consume(int x){
    printf("Consumer using %d\n\n", x);
}

void requestDeposit(){
    printf("Requesting deposit\n");
}

void Deposit(T *t, int val){
    printf("\nDepositing: %d\n",val);
    t->buffers[t->prodCur].data = val;
    t->buffers[t->prodCur].state = true;
    t->prodCur = (t->prodCur + 1) % MAXLEN;
}

void requestRemoval(){
    printf("Requesting removal\n");
}

int Remove(T *t){
    int temp = t->buffers[t->consumeCur].data;
    t->buffers[t->consumeCur].state = false;
    t->consumeCur = (t->consumeCur + 1) % MAXLEN;
    printf("\nRemoved: %d\n", temp);
    return temp;
}

int Consume(void* arg){
    int *a = (int *)arg;
    for(int i = 0; i <a[0]; i++){
        requestRemoval();
        sem_wait(&s_full);
        int res = Remove(&g_buff);
        sem_post(&s_empty);
        consume(res);
    }
    return 0;
}

int Produce(void* arg){
    int *a = (int *)arg;
    for(int i = 0; i < a[0]; i++){
        requestDeposit();
        int data = produce();
        sem_wait(&s_empty);
        Deposit(&g_buff, data);
        sem_post(&s_full);
    }
    return 0;
}


int main(){
    srand(0);
    // initially initialize the semaphores
    sem_init(&s_empty, 0, MAXLEN);
    sem_init(&s_full, 0,0);
    g_buff.consumeCur = 0;
    g_buff.prodCur = 0;
    thrd_t thread2, thread1;
    int arg = 70;
    int arg2 = 75;
    thrd_create(&thread1,&Produce,&arg2);
    thrd_create(&thread2,&Consume,&arg);
    //Produce(arg + 15);
    thrd_join(thread1,NULL);
    thrd_join(thread2,NULL);
    printf("\n\n============================================================================\n\n");
    for(int i = 0; i < MAXLEN; i++){
        printf("buffer state: %s\n", g_buff.buffers[i].state == true ? "full":"empty");
        if(g_buff.buffers[i].state)
        	printf("data in the buffer: %d\n\n", g_buff.buffers[i].data);
    }
    return 0;
}
