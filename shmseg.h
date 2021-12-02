#ifndef SHMSEG_H
#define SHMSEG_H

class ShmSeg {
	private:
		int resources[20][2];

	public: 
		struct Clock {
			int seconds;
			int nanoseconds;
		} clock;
	
		void initResources (int temp [][2]) {
			for (int i; i < 20; i++) {
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

		void returnResource (int r, int ask) {
			this->resources[r][0] += ask;
		}
};

#endif
