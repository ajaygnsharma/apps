/*
 * cfgErr.h
 *
 *  Created on: Sep 22, 2023
 *      Author: asharma
 */

#ifndef SRC_CFGERR_H_
#define SRC_CFGERR_H_

#include <vector>
#include <string>
#include <iostream>


using namespace std;

class Cfg_err{
	vector <string> msgs;

public:
	Cfg_err(){}
	void add(string msg);
	bool any(void);
	void log(void);
};

extern Cfg_err cfg_err;

#endif /* SRC_CFGERR_H_ */
