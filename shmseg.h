#ifndef SHMSEG_H
#define SHMSEG_H
#include <iostream>
using namespace std;
class ShmSeg {
	public:
		int resources[20][2];
		int p, lines; 
		struct Clock {
			int seconds;
			int nanoseconds;
		} clock;
	
		void initResources (int temp [][2]) {
			for (int i; i < 20; i++) {
cout << "shm " << temp [i][0] << endl;
				this->resources[i][0] = temp[i][0];
				this->resources[i][1] = temp[i][1];
			}
		}

		bool checkResource (int r, int ask) {
			if (this->resources[r][1] == 1) {
				if (this->resources[r][0] < ask) {
					return false;
				} else {
					return true;
				}
			} else {
				return false;
			}
		}

		void takeResource (int r, int ask) {
			this->resources[r][0] -= ask;
		}

		void returnResource (int r, int h) {
			this->resources[r][0] += h;
		}
};

#endif
