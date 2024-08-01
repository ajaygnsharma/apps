
#include <cfgErr.h>

Cfg_err cfg_err = Cfg_err();

void Cfg_err::add(string msg){
	msgs.push_back(msg);
}

bool Cfg_err::any(void){
	return msgs.size();
}

void Cfg_err::log(void){
	for(vector<string>::iterator it = msgs.begin(); it != msgs.end(); ++it){
		cerr << *it;
		//ibuc_log()
	}
}
