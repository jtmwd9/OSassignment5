#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/msg.h>
#include "shmseg.h"
#include "msgq.h"

using namespace std;

struct mesg_buffer {
	long mesg_type;
	char mesg_text;
} messageRun;

int validateArguments (int num) {
	if (num == -69) {
		//no arg
		return 10;
	}	
	if (num < 1 || num > 20) {
		//warning use 20 as num
		return 10;
	}
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

void initResourceArray (ShmSeg *&l) {
	
}

void updateClock (ShmSeg *&l, int seconds, int nanoseconds) {

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

int main (int argc, char* argv[]) {
	ShmSeg *segment;
	int shmid, destroySuccess;
	shmid = initSharedMemory(segment);
	if (shmid == 1) {
		cout << "error";
		return 1;
	}
	detachSharedMemory(segment);
	destroySuccess = destroySharedMemory(shmid);
	if (destroySuccess == 1) {
		return 1;
	}

	int msgid = initMessageQueue();
	destroyMessageQueue(msgid);

	return 0;
}
