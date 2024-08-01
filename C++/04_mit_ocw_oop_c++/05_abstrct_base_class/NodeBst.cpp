/*
 * Bst.cpp
 *
 *  Created on: Dec 13, 2012
 *      Author: sharmaj
 * NodeBST class needs to be implement all
 * the pure virtual functions in base class
 * otherwise the compiler gives error.
 */
#include "NodeBST.h"

NodeBST::NodeBST(void){
	cout << "creating NodeBST" << endl;
}

NodeBST::~NodeBST(void){
	cout << "deleting NodeBST" << endl;
}

// implementation of the insert method using nodes
void NodeBST::insert(int val) {
	cout << "creating new node" <<endl;
}

bool NodeBST::find(int val) {
	cout << "found " << val <<endl;
	return true;
}

void NodeBST::print_inorder() {
	cout << "printing inorder" <<endl;
}
