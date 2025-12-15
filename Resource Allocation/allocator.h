#include <stdbool.h>
#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#define MACHINES 5


int Allocate(bool *shm_table,int _semid, int _mutex, int _nfree);
int Release(bool *shm_table, int _semid, int _mutex, int _nfree, int _num);

#endif