#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "./allocator.h"
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char **argv){
    srand(0);
    // argv must be call shmid semid nsems
    if(argc < 4){
        perror("insufficient arguments\n\n");
        exit(5);
    }
    // it is very recommended to use strtol 
    int shmid = atoi(argv[1]);
    int semid = atoi(argv[2]);
    int nsems = atoi(argv[3]);

    bool *shm_table = (bool *)shmat(shmid, NULL, 0); // for read and write
    
    if(shm_table == (bool *)-1){
        perror("failed to attach memory\n\n");
        exit(6);
    }
    int machine = Allocate(shm_table,semid,0,1);
    printf("I'm the client of pid = %d, using the machine N° = %d\n\n", getpid(), machine + 1);

    sleep(rand() % 20 + 1);

    printf("The client of pid = %d, released the machine N° = %d\n\n", getpid(), machine + 1);
    Release(shm_table,semid,0,1,machine);

    shmdt((void *)shm_table); // this should work without debugging

    return 0;
}