//: C03:PointerToFunction.cpp
// Defining and using a pointer to a function
#include <iostream>
/*3: The C in C++
215*/
using namespace std;
void func() {
cout << "func() called..." << endl;
}
int main() {
  void (*fp)(); // Define a function pointer
  fp = func;    // Initialize it
  (*fp)();      // Dereferencing calls the function

  void (*fp2)() = func; // reDefine and initialize
  (*fp2)();
} ///:~

