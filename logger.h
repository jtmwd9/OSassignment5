#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include "shmseg.h"

using namespace std;

void log(string str, int second, int nanosecond) {
	ofstream myfile("logfile", ios_base::app);
	myfile << str << " at time " << second << ":" << nanosecond << endl;
}

#endif
