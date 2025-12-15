#include <sys/sem.h>
#include <sys/ipc.h>
#ifndef SEMAPHORES_H
#define SEMAPHORES_H


int P(int _semid, int _semnum);
int V(int semid, int semnum);
int sem_init(key_t key, int nsems);
int sem_getValue(int _semid, int _semnum);
int sem_detroy(int _semid);


#endif