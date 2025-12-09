/*
 * Bst.cpp
 *
 *  Created on: Dec 13, 2012
 *      Author: sharmaj
 * Abstract base class: one or more pure virtual functions
 * This cannot be instantiated.
 * Cannot do: BST bst = new BST(); //not allowed
 */
#ifndef BST_H_
#define BST_H_

#include <iostream>
using namespace std;

class BST{
public:
	/*The class is never instantiated,
    constructor cannot be virtual */
	//virtual BST()=0; <- not allowed
	//virtual BST(); <- not allowed
	BST(); //<- allowed not virtual or pure virtual
	//virtual ~BST(); allowed.
	//~BST(); //allowed works same as below
	virtual ~BST()=0;


	/* pure virtual functions, no implementation */
	virtual void insert(int val) = 0;
	virtual bool find(int val) = 0; //if pure, then virtual is needed
	virtual void print_inorder() = 0;
};

#endif
