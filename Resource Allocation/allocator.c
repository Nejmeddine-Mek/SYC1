#include "./allocator.h"
#include "./semaphores.h"
#include <stdbool.h>

int Allocate(bool *shm_table,int _semid, int _mutex, int _nfree){
    int num = 0;
    P(_semid, _nfree);
    P(_semid, _mutex);
    while(shm_table[num] == true)
        num += 1;
    shm_table[num] = true;
    V(_semid, _mutex);
    return num;
}

int Release(bool *shm_table, int _semid, int _mutex, int _nfree, int _num){
    P(_semid, _mutex);
    shm_table[_num] = false;
    V(_semid, _mutex);
    V(_semid, _nfree);
    return 1;
}