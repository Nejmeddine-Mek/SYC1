#include <stdio.h>
#include <stdlib.h>
#include "./semaphores.h"
#include "./allocator.h"
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
	key_t key;

	int nsems; // number of semaphores, length of the semaphores array, 0-indexed
	int semid;
} _SEMAPHORE_;

typedef struct {
	key_t key; 
	int shmsize;  // size of the shared memory
	int shmid;
}_SHMEMORY_;

int main(int agrc, char **argv){
    // first of all, let us create the shared memory segment
    // initially generate a key
    _SHMEMORY_ res_shm;
    // generate a key for the shared memory
    if((res_shm.key = ftok("./main.c", 0x11)) == -1){
        perror("failed to genreate a key!\n");
        return 1;
    }
    // now we shall allocate space we need
    res_shm.shmsize = sizeof(bool) * MACHINES;
    if((res_shm.shmid = shmget(res_shm.key, res_shm.shmsize, IPC_CREAT | 0666)) == -1){
        perror("failed to allocate a shared segment\n\n");
        return 2;
    }
    // now since the shared memory is created, we shall also create our set of semaphores
    // mutex = alloc_sem[0]
    // nlibre = alloc_sem[1]
    _SEMAPHORE_ alloc_sem = {
        .nsems = 2
    };
    // then, we first generate a key
    if( (alloc_sem.key = ftok("./allocator.c", 0x15)) == -1){
        perror("failed to generate key for the mutex semaphore\n");
        return 3;
    }
    // now, we shall get the semid
    if((alloc_sem.semid = semget(alloc_sem.key, alloc_sem.nsems, IPC_CREAT | 0666)) == -1){
        perror("falied to create mutex semaphore\n\n");
        return 4;
    }
    // now, we set their values    
    semctl(alloc_sem.semid, 0, SETVAL, 1); // this does mutex init(1)
    semctl(alloc_sem.semid, 1, SETVAL, MACHINES); // this does nlibre init(n), where n is the number of processes

    // now, we simply create children and execute their code
    // for later
    for(int i = 0; i < 10; i++){
        pid_t p = fork();
        if(p == 0){
            char s_semid[32];
            char s_shmid[32];
            char s_nsems[32];
            sprintf(s_nsems, "%d", alloc_sem.nsems);
            sprintf(s_semid, "%d", alloc_sem.semid);
            sprintf(s_shmid, "%d", res_shm.shmid);
            execl("./Pgme_client", "Pgme_client", s_shmid, s_semid, s_nsems, NULL);
            //printf("failed to call\n");
            exit(10);
        }

    }
    while(wait(NULL) != -1);

    semctl(alloc_sem.semid, 0, IPC_RMID);
    shmctl(res_shm.shmid, IPC_RMID, NULL);
    return 0;
}