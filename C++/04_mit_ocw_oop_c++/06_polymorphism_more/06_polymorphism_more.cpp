//============================================================================
// Name        : 06_polymorphism_more.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================
#include <stdio.h>
#include <stdlib.h>

class Rectangle {
protected:
	float length;
	float width;

public:
	Rectangle(float length, float width);
	void setLength();
	void setWidth();
	void getLength();
	void getWidth();
};

class Square : public Rectangle {
	// representation invariant: length = width
public:
	Square(float length);
	void setLength(); // ensures length = width
	void setWidth();
	// does nothing
};

int main(void) {
	Rectangle *r = new Square(5.0);
	return EXIT_SUCCESS;
}
