#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sstream>
#include "shmseg.h"
#include "logger.h"

using namespace std;

struct mesg_buffer {
	long mesg_type;
	int mesg_text;
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

	msgid = msgget(key, 0666);
	messageRun.mesg_type = 1;
	messageRun.mesg_text = 1;
	msgsnd(msgid, (void*)&messageRun, sizeof(messageRun), 0);

	return msgid;
}

void sendMessage (int msgid, int type, int text) {
	messageRun.mesg_type = type;
	messageRun.mesg_text = text;
	msgsnd(msgid, (void*)&messageRun, sizeof(messageRun), 0);
}

void destroyMessageQueue (int msgid) {
	msgctl(msgid, IPC_RMID, NULL);
}

void updateClock (ShmSeg *&l, int seconds, int nanoseconds) {
	l->clock.seconds += seconds;
	l->clock.nanoseconds += nanoseconds;
	if (l->clock.nanoseconds >= 1000000000) {
		l->clock.seconds++;
		l->clock.nanoseconds -= 1000000000;
	}
}

void requestResource(ShmSeg *&l, int r, int ask) {
	l->checkResource(r, ask);
	l->takeResource(r, ask);

	int pid = getpid();
	string p, rs, asks;
	stringstream a, b, c;
	a << pid; b << r; c << ask;
	p = a.str();
	rs = b.str();
	asks = c.str();
	string str = "Process with PID " + p + " requesting " + asks + " instances of resource " + rs;
	log (str, l->clock.seconds, l->clock.nanoseconds);
	l->lines++;
}

void releaseResource (ShmSeg *&l, int r, int h) {
	l->returnResource(r, h);
	int pid = getpid();
	string p, rs;
	stringstream a, b, c;
	a << pid; b << r;
	p = a.str();
	rs = b.str();
	string str = "Process with PID " + p + " releasing resource " + rs;
	log(str, l->clock.seconds, l->clock.nanoseconds);
	l->lines++;
}

void releaseAll(ShmSeg *&l, int h []) {
	for(int i = 0; i < 20; i++) {
		l->returnResource(i, h[i]);
	}
}

int main (int argc, char *argv[]) {
	srand (time(NULL));

	ShmSeg *segment;
	int shmid, destroySuccess;
	shmid = initSharedMemory(segment);
	if (shmid == 1) {
		cout << "error";
		return 1;
	}
cout << "here" << getpid() << endl;
cout << segment->resources[1][0] << endl;	
	int msgid = initMessageQueue();
	
	int timer = (segment->clock.seconds * 1000000000) + segment->clock.nanoseconds;
	int temp = timer + 1000000000;
	int check, r, ask;

	int held [] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	while (1) {
		check = rand() % 250000000 + 1;
		timer += check;
		updateClock(segment, 0, check);
		if (timer > temp) {
			check = rand() % 2 + 1;
			if (check == 1) {
				break;
			}
		}
		check = rand() % 2 + 1;
		if (check == 1) {
cout << "1 if"<< endl;
			while (check == 1) {
				r = rand() % 20;
				if (segment->resources[r][0] > 0) {		
					ask = rand() % segment->resources[r][0] + 1;
					held[r] += ask;
					requestResource(segment, r, ask);
cout <<"ask"<<endl;
					check = 0;
				}	
			}
		} else if (check == 2) {
			while (check != 0) {
				r = rand() % 20;
				if (held[r] > 0) {
					releaseResource (segment, r, held[r]);
cout<<"release"<<endl;
					held[r] = 0;
					check = 0;
				}
			}
		}
	}
	
	releaseAll(segment, held);
	segment->p++;
	detachSharedMemory(segment);
	destroySuccess = destroySharedMemory(shmid);
	if (destroySuccess == 1) {
		return 1;
	}

	destroyMessageQueue(msgid);
	
}
