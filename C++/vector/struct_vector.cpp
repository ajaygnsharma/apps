//: C02:Intvector.cpp
// From Thinking in C++, 2nd Edition
// Available at http://www.BruceEckel.com
// (c) Bruce Eckel 2000
// Copyright notice in Copyright.txt
// Creating a vector that holds integers
#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;

enum{
  wr,
  rd
};

typedef struct{
	bool rw;
	int sttLPN;
	int endLPN;
	int hdl;
}cmd_ram_t;

int main() {
  vector<cmd_ram_t> v;
  cmd_ram_t cmd_ram[10];

  for(int i = 0; i < 10; i++){
	cmd_ram[i].rw= 0;
	cmd_ram[i].sttLPN=10+i;
	cmd_ram[i].sttLPN=100+i;
	cmd_ram[i].hdl=i;
  }

  for(int i = 0; i < 10; i++)
    v.push_back(cmd_ram[i]);

  cout << v.size();
  cout << endl;

  for(int i = 0; i < v.size(); i++){
    cout << v[i].rw << ", ";
    cout << v[i].sttLPN << ", ";
    cout << endl;
  }

  v.erase(v.begin()+3);
  cout << endl;

  for(int i = 0; i < v.size(); i++){
      cout << v[i].rw << ", ";
      cout << v[i].sttLPN << ", ";
      cout << endl;
  }

  /*for(int i = 0; i < v.size(); i++)
    v[i] = v[i] * 10; // Assignment  

  for(int i = 0; i < v.size(); i++)
    cout << v[i] << ", ";
  cout << endl; */
} ///:~
