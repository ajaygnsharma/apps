/*
 ============================================================================
 Name        : 101_cfg_read_getline.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

#include "cfgErr.h"

using namespace std;

struct cfg_s {
  int val1;
  char str[50];
  int val2;
}cfg;

static map<string, string> m;

bool build_map(string s, string delim){
	string var;
	size_t pos = -1;

	pos = s.find(delim);
	if(pos != string::npos){ /* Found it! */
		string key = s.substr(0,pos);
		string value = s.substr(pos+1); // Ignore =

		m[key] = value;

		return true;
	}else{
		return false;
	}
}

void create_data(void){
  FILE *pFile;
  char *buf = NULL;
  ssize_t n = 0;
  struct cfg_s cfg = {10, "my_security", 20};

  pFile = fopen("test.txt", "w");

  n = asprintf(&buf, "TEMPERATURE_MAX=%d\n"
                     "SNMP_C2GROUP_SECURITY=%s\n"
                     "BURST_THOLD=%d\n",
                     cfg.val1, cfg.str, cfg.val2 );

  fwrite(buf , n, 1, pFile);
  fclose(pFile);
  return;
}

bool mapFind(string s, string &foundStr){
	if(auto search = m.find(s); search != m.end()){
		foundStr = search->second;
		return true;
	}else{
		foundStr.clear();
		return false;
	}
}


int
main(void)
{
  /* First load defaults */
  create_data();

  ifstream t("test.txt");
  stringstream buffer;
  buffer << t.rdbuf();

  vector <string> tokens;
  string token;
  while(getline(buffer, token, '\n')){
  	tokens.push_back(token);
  }


  for(vector<string>::iterator line = tokens.begin(); line != tokens.end(); ++line){
  	build_map(*line, "=");
  }

#if(0)
  for(auto &x: m){
  	cout << x.first << "," << x.second  <<  endl;
  }
#endif

  string found = "";
  if( mapFind("BURST_THOLD", found) ){    cfg.val1 = stoi(found); } else{ cfg.val1 = 1000; }
  if( mapFind("SNMP_C2GROUP_SECURITY", found) ){ strcpy(cfg.str, found.c_str()); }

  if( auto search = m.find("TEMPERATURE_MAX"); search != m.end() ){
  	cfg.val2 = stoi(search->second);
  }


  cout << cfg.val1 << endl;
  cout << cfg.str << endl;
  cout << cfg.val2 << endl;

  if(cfg_err.any()){
  	cfg_err.log();
  }

  exit(EXIT_SUCCESS);
}
