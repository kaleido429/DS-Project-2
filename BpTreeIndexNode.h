#pragma once
#ifndef _BPTREEINDEXNODE_H_
#define _BPTREEINDEXNODE_H_

#include "BpTreeNode.h"
#include <map>

class BpTreeIndexNode : public BpTreeNode {
private:
	map <string, BpTreeNode*> mapIndex; // index map <key,value> => <name, BpTreeNode*>

public:
	// constructor
	BpTreeIndexNode() {}

	// destructor
	~BpTreeIndexNode() {
		// B+ tree's destructor( BpTree::~BpTree() ) releases memory
	}

	// insert index map, <name, BpTreeNode*> => <string, BpTreeNode*>
	void insertIndexMap(string name, BpTreeNode* pN) {
		/* note
		1. find leaf node, using key(name)
		2. insert <name, BpTreeNode*> into mapIndex
		3. if need to split, middle node's key is promoted to parent node
		4. repeat 3 until don't need to split or reach root
		*/
		mapIndex.insert(map<string, BpTreeNode*>::value_type(name, pN));
	}

	// delete index map(key: name)
	void deleteMap(string name) {
		mapIndex.erase(name);
	}

	// return index map
	map <string, BpTreeNode*>* getIndexMap() { return &mapIndex; }

};

#endif