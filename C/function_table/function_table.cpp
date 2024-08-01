//: C03:FunctionTable.cpp
// From Thinking in C++, 2nd Edition
// Available at http://www.BruceEckel.com
// (c) Bruce Eckel 2000
// Copyright notice in Copyright.txt
// Using an array of pointers to functions
#include <iostream>
using namespace std;

class foo{

  public:

    foo();
    void tst1();
    void tst2();

#if TYPEDEF
    typedef void (foo::*func_table)();
    func_table tbl[2];
#else
    void (foo::*tbl[2])();
#endif

    void a();
    void b();
};

void foo::a(){
  cout << __FUNCTION__ <<endl;
}

void foo::b(){
  cout << __FUNCTION__ <<endl;
}


foo::foo(){
  tbl[0] = &foo::a;
  tbl[1] = &foo::b;
}

void foo::tst1(){
  (this->*tbl[0])();
}

void foo::tst2(){
  (this->*tbl[1])();
}


int main() {
  foo f1 = foo();
  f1.tst1();
  f1.tst2();
} ///:~
