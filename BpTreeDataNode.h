#pragma once
#ifndef _BpTreeDataNode_H_
#define _BpTreeDataNode_H_

#include "BpTreeNode.h"

class BpTreeDataNode : public BpTreeNode {
private:
	map <string, EmployeeData*> mapData; // data map <key,value> => <name, EmployeeData*>
	BpTreeNode* pNext;
	BpTreeNode* pPrev;
public:
	// constructor
	BpTreeDataNode() {
		pNext = NULL;
		pPrev = NULL;
	}

	// destructor
	~BpTreeDataNode() {
		// B+ tree's destructor( BpTree::~BpTree() ) releases memory
	}

	void setNext(BpTreeNode* pN) { pNext = pN; } // set pN is next node
	void setPrev(BpTreeNode* pN) { pPrev = pN; } // set pN is previous node
	BpTreeNode* getNext() { return pNext; } // return pN(next node)
	BpTreeNode* getPrev() { return pPrev; } // return pN(previous node)

	// insert datamap, <name, EmployeeData*> => <string, EmployeeData*>  
	void insertDataMap(string name, EmployeeData* pN) {
		/*
		1. 
		
		*/
		mapData.insert(map<string, EmployeeData*>::value_type(name, pN));
	}

	// delete datamap(key: name)
	void deleteMap(string name) {
		mapData.erase(name);
	}

	// return datamap
	map<string, EmployeeData*>* getDataMap() { return &mapData; }
};

#endif
