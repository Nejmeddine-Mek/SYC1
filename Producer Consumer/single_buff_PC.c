#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define BIGVAL 12648545
/*----------*/
//         IMPORTANT! 
// This code is a simple demo of how to use System V sempahores, shared memory and inter process communication on a single buffered producer consumer program
// no error handling is included, and those structures aren't necessary.
// no shmdt() is sued to detach the memory segment from the processes
// please make sure you read about error handling and memeory detaching for optimal use

typedef union{
	int val;
	struct semid_ds *buffs;
	unsigned short *array; // used for SETALL
    struct seminfo *__buf; // system-specific
}semun;

typedef struct {
	key_t key;
	int semflg;
	int nsems; // number of semaphores, length of the semaphores array, 0-indexed
	int semid;
} _SEMAPHORE_;

typedef struct {
	key_t key; 
	int shmsize;  // size of the shared memory
	int shmid;
	int shmflg; 
}_SHMEMORY_;

int ipproduce(){
	return rand() % BIGVAL + 1;
}
int consume(int val){
	printf("consuming: %d\n\n", val);
}


int main(int argc, char **argv){
	//srand(0);
	int rounds =  5 ; //rand() % 100 + 1;
	// create 2 semaphores
	// I think we shall create the key using ftok()
	
	_SEMAPHORE_ sem_full = {
		.key = 1, // you can directly use ftok from the ipc library
		.nsems = 1,
		.semflg = IPC_CREAT | IPC_EXCL | 0666,
		.semid = 0
	};
	_SEMAPHORE_ sem_empty = {
		.key = 2,
		.nsems = 1,
		.semflg = IPC_CREAT | IPC_EXCL | 0666,
		.semid = 0
	};
	// now, we created our sempahores, we need to set their values now
	sem_full.semid = semget(sem_full.key,sem_full.nsems,sem_full.semflg);
	sem_empty.semid = semget(sem_empty.key, sem_empty.nsems,sem_empty.semflg);
	
	semun sem_full_arg = {
		.val = 0
	};
	semun sem_empty_arg = {
		.val = 1
	};
	// now, we successfully set values of our semaphores
	semctl(sem_full.semid, 0,SETVAL,sem_full_arg);
	semctl(sem_empty.semid, 0,SETVAL, sem_empty_arg);

	// create a shared memo and put a buffer in it
	_SHMEMORY_ shared_memory = {
		.shmsize = sizeof(int),
		.shmflg = IPC_CREAT | 0666,
		.key = 10
	};
	
	shared_memory.shmid = shmget(shared_memory.key,shared_memory.shmsize,shared_memory.shmflg);
	// create a prod process and consumer process and let them use the shared memo
	pid_t pid = fork();
	// you can put this code in a diiferent file then run it useing exec* family
	// use sprintf() to convert integers to strings
	// use atoi() to cnovert strings to integers
	if(pid == 0){
		// code for the consumer
		// firstly, attach the memory to this process
		int *buffer = (int *)shmat(shared_memory.shmid,NULL, 0);
		// now we can use this memory using semaphore to synchronize access
		struct sembuf P_sem_full = {
			.sem_flg = SEM_UNDO,
			.sem_num = 0,
			.sem_op = -1
		};
		struct sembuf V_sem_empty = {
			.sem_flg = SEM_UNDO,
			.sem_num = 0,
			.sem_op = 1
		};
		for(int i = 0; i < rounds; i++){
			semop(sem_full.semid, &P_sem_full,1);
			int val = buffer[0];
			semop(sem_empty.semid,&V_sem_empty,1);
			consume(val);
		}
		shmdt(buffer);
		exit(0);
	}
	pid = fork();
	if(pid == 0){
		// code for producer
		int *buffer = (int *)shmat(shared_memory.shmid, NULL, 0);
		// now we can access from here while synchronizing using semaphores
		struct sembuf V_sem_full = {
			.sem_num = 0,
			.sem_op = 1,
			.sem_flg = SEM_UNDO
		};

		struct sembuf P_sem_empty = {
			.sem_num = 0,
			.sem_op = -1,
			.sem_flg = SEM_UNDO
		};

		for(int i = 0; i < rounds; i++){
			// here, we have to produce first
			int value = produce();
			// then use semaphore to try and access the memory
			
			semop(sem_empty.semid, &P_sem_empty,1);
			buffer[0] = value;
			semop(sem_full.semid,&V_sem_full,1);
		}
		shmdt(buffer);
		exit(0);
	}
	while(wait(NULL) != -1);
	// now we shall clean the shared memory
	shmctl(shared_memory.shmid, IPC_RMID, NULL);
	// and here we clean the semaphores
	semctl(sem_full.semid,0,IPC_RMID);
	semctl(sem_empty.semid,0,IPC_RMID);
	
	return 0;
}
