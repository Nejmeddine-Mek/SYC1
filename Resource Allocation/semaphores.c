#include "./semaphores.h"


int P(int _semid, int _semnum){
    struct sembuf sem_P = {
        .sem_num = _semnum,
        .sem_op = -1,
        .sem_flg = 0
    };
    int res = semop(_semid,&sem_P,1);
    return res;
}

int V(int _semid, int _semnum){
    struct sembuf sem_V = {
        .sem_num = _semnum,
        .sem_op = 1,
        .sem_flg = 0
    };
    int res = semop(_semid, &sem_V, 1);
    return res;
}
// this one does simple creation, but I didn't use it
int sem_init(key_t key, int _nsems){
    return semget(key, _nsems, IPC_CREAT | IPC_EXCL | 0666);
}

int sem_getValue(int _semid, int _semnum){
    return semctl(_semid, _semnum, GETVAL);
}
//not important, I thought, I would use it, but I didn't, it needs fixing
int sem_detroy(int _semid){
    return semctl(_semid, 0, IPC_RMID);
}