/*
 * main.cpp
 *
 *  Created on: Dec 14, 2012
 *      Author: sharmaj
 */
#include <iostream>

class B{
public:
  void print(){
    std::cout << "Hello World!\n";
  }
};

class A{
  B* pb;
  int x;

public:
  A(int y) : x (y){
    pb = new B();
  }

  //without this ~A() is called twice to delete pb
  A(const A& other) { // copy constructor
    x = other.x;
    pb = new B();
  }

  ~A(){
	delete pb; //called twice since a1 and a are created.
  }// destructor

  //without this ~A() is called twice to delete pb
  A& operator=(const A& other){ // copy assignment operator
    x = other.x;
    delete pb;
    // clean up the junk in the existing object!
    pb = new B();
    return *this;
  }

  void printB(){
	pb->print();
  }
};

//function argument creates a copy of the object using copy constructor
void foo(A a){
  a.printB();
}

int main() {
  A a1(5);
  a1.printB();
  foo(a1);
  return 0;
}

