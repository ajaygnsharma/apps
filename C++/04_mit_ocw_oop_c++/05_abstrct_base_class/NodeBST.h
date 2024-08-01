/*
 * NodeBST.h
 *
 *  Created on: Dec 13, 2012
 *      Author: sharmaj
 */

#ifndef NODEBST_H_
#define NODEBST_H_

#include <iostream>
#include "Bst.h"
using namespace std;

class NodeBST : public BST {

public:
	NodeBST();
	~NodeBST();

	void insert(int val);
	bool find(int val);
	void print_inorder();
};

#endif /* NODEBST_H_ */
