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

struct list{
	struct list* prev;
	char* data;
	struct list* next;
}*head, *point;

struct arg{
	long type;
	struct list* data;
}argh, buf_arg;

int main(int argc, char** argv){
	struct mess buf1, buf2, ready, start;
	int ind_n = 0;
	int st;
	int i = 0;
	head->prev = NULL;
	head->data = argv[1];
	head->next = NULL;
	if (argc > 2){
		struct list* buf;
		head->next = point;
		point->data = argv[2];
		point->prev = head;
		for(i = 3; i < argc; i++){
			buf = (struct list*)calloc(1, sizeof(struct list));
			buf->prev = point;
			point->next = buf;
			buf->data = argv[i];
			point = buf;
		}
		point->next = NULL;
	}
	int key = ftok(argv[0], 0);
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
		while(1){
			point = head;
			while(point->next != NULL){
				printf("%s ", point->data);
				if (strcmp(point->data, "exit") == 0){
					printf("\necholoop finished working!");
					st = msgrcv(id, &start, 0, 1, 0);
					exit;
				}
				point = point->next;
			}
			printf("%s\n", point->data);
			ind = msgrcv(id, &buf_arg, sizeof(int), 3, IPC_NOWAIT);
			if (ind != -1){
				point->next = buf_arg.data;
				(buf_arg.data)->prev = point;
			}
			sleep(1);
		}
	}
	if (ind_n == 2){
		argh.type = 3;
		argh.data = head;
		st = msgsnd(id, &argh, sizeof(struct list*), 0);
		printf("echoloop for ");
		for (i = 1; i < argc; i++){
			printf("%s ", argv[i]);
		}
		printf("finished!");
		st = msgrcv(id, &ready, 0, 2, 0);
	}
	return 0;
}