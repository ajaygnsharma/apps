/*
 * main.cpp
 *
 *  Created on: Dec 13, 2012
 *      Author: sharmaj
 */
#include <iostream>
//#include <Bst.h>
#include "NodeBST.h"

int main(void){

	//BST dummy = new BST();

	NodeBST *first = new NodeBST();
	first->insert(2);
	first->find(2);
	first->print_inorder();
	delete(first);

	return 0;
}
