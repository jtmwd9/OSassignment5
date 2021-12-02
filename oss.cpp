#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/msg.h>
#include <cstdlib>
#include <unistd.h>
#include "shmseg.h"
#include "msgq.h"

using namespace std;

struct mesg_buffer {
	long mesg_type;
	char mesg_text;
} messageRun;

int validateArguments (int num) {
	return num;
}

int initSharedMemory (ShmSeg *&l) {		
	key_t key = ftok("/tmp", 'J');
	int shmid = shmget(key, sizeof(l), 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Shared memory");
		return -1;
	}
	l = (ShmSeg*)shmat(shmid, (void*)0, 0);
	if (l == (void*) -1) {
		perror("Shared memory attach");
		return -1;
	}

	l->clock.seconds = 0;
	l->clock.nanoseconds = 0;	

	return shmid;
}

void initResources (ShmSeg *&l) {
	int temp [20][2];
	int scale;
	for (int i = 0; i < 20; i++) {
		temp[i][0] = rand() % 10 + 1;
		scale = rand() % 5 + 1;
		if (scale == 3) {
			temp[i][1] = 1;
		}
cout << temp[i][0] << endl;
	}
	l->initResources(temp);
	l->p = 0;
	l->lines = 0;
cout << "! " << l->resources[1][0] << endl;
}

void updateClock (ShmSeg *&l, int seconds, int nanoseconds) {
	l->clock.seconds += seconds;
	l->clock.nanoseconds += nanoseconds;
	if (l->clock.nanoseconds >= 1000000000) {
		l->clock.seconds++;
		l->clock.nanoseconds -= 1000000000;
	}
}

void detachSharedMemory (ShmSeg *&l) {
	shmdt(l);
}

int destroySharedMemory (int shmid) {
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		return 1;
	} 
	return 0;
}

int initMessageQueue () {

	key_t key;
	int msgid;
	key = ftok("JamesMessage", 69);

	msgid = msgget(key, 0666 | IPC_CREAT);
	messageRun.mesg_type = 1;
	messageRun.mesg_text = '1';
	msgsnd(msgid, (void*)&messageRun, sizeof(messageRun), 0);

	return msgid;
}

void destroyMessageQueue (int msgid) {
	msgctl(msgid, IPC_RMID, NULL);
}

void spawn (ShmSeg *&l) {
	pid_t pid;
	pid = fork();
	if (pid == -1) {

	} else if (pid == 0) {//child
		char* argv_list [] = {"./user_proc", NULL};
		execv("./user_proc", argv_list);
		exit(0);
	} 
}

int main (int argc, char* argv[]) {
	ShmSeg *segment;
	srand(time(NULL));
	int shmid, destroySuccess, timer;
	shmid = initSharedMemory(segment);
	if (shmid == 1) {
		cout << "error";
		return 1;
	}

	initResources(segment);
cout << "oss r " << segment->resources[1][0] << endl;
	while(segment->lines < 10000 || segment->clock.seconds < 5 || segment->p < 12) {
		timer = rand() % 500000000000 + 1;
		updateClock(segment, 0 , timer);
		if (segment->p < 12) {
			segment->p++;
			spawn(segment);
		}
	};

	detachSharedMemory(segment);
	destroySuccess = destroySharedMemory(shmid);
	if (destroySuccess == 1) {
		return 1;
	}

	int msgid = initMessageQueue();
	destroyMessageQueue(msgid);

	return 0;
}
