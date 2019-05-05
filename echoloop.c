#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>

struct mess{
	long type;
};

struct arg{
	long type;
	char* data;
}args;

int main(int argc, char** argv){
	struct mess buf1, buf2, ready, start;
	int ind_n = 0;
	int st;
	int post = 10, i = 0;
	int key = 10;
	int id = msgget(key, 0777|IPC_CREAT);

	int semid = semget(key, 1, 0777|IPC_CREAT);
	int semind = 1;
	struct sembuf sops[3];

	sops[0].sem_num = 0;
	sops[0].sem_op = 0;
	sops[0].sem_flg = 0;

	sops[1].sem_num = 0;
	sops[1].sem_op = 1;
	sops[1].sem_flg = 0;

	sops[2].sem_num = 0;
	sops[2].sem_op = -1;
	sops[2].sem_flg = 0;

	start.type = 1;
	ready.type = 2;
	semop(semid, sops, 2);
	int ind = msgrcv(id, &buf1, 0, 1, IPC_NOWAIT);
	int gind = msgrcv(id, &buf2, 0, 2, IPC_NOWAIT);
	if ((ind == -1) && (gind == -1)){
		st = msgsnd(id, &start, 0, 0);
		ind_n = 1;
	}
	else{
		if (ind != -1)
			st = msgsnd(id, &start, 0, 0);
		if (gind != -1)
			st = msgsnd(id, &ready, 0, 0);
		st = msgsnd(id, &ready, 0, 0);
		ind_n = 2;
	}
	semop(semid, &sops[2], 1);
	if (ind_n == 1){
		while(post > 0){
			for(i = 1; i < argc; i++){
				printf("%s ", argv[i]);
			}
			post--;
			sleep(1);
		}
		st = msgrcv(id, &start, 0, 1, 0);
	}
	if (ind_n == 2){
		
		printf("echoloop for ");
		for (i = 1; i < argc; i++){
			printf("%s ", argv[i]);
		}
		printf("finished!");
		st = msgrcv(id, &ready, 0, 2, 0);
	}
	return 0;
}