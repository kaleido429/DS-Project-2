#pragma once
#include "EmployeeData.h"

class BpTreeNode {
private:
	BpTreeNode* pParent;
	BpTreeNode* pMostLeftChild;

public:
	// constructor
	BpTreeNode() {
		pParent = NULL;
		pMostLeftChild = NULL;
	}

	// destructor
	~BpTreeNode() {
		// B+ tree's destructor( BpTree::~BpTree() ) releases memory
	}

	void setMostLeftChild(BpTreeNode* pN) { pMostLeftChild = pN; } // set pN is most left child
	void setParent(BpTreeNode* pN) { pParent = pN; } // set pN is parent

	BpTreeNode* getParent() { return pParent; } 
	BpTreeNode* getMostLeftChild() { return pMostLeftChild; }

	virtual void setNext(BpTreeNode* pN) {} // pN is next node
	virtual void setPrev(BpTreeNode* pN) {} // pN is previous node
	virtual BpTreeNode* getNext() { return NULL; }
	virtual BpTreeNode* getPrev() { return NULL; }

	virtual void insertDataMap(string n, EmployeeData* pN) {}
	virtual void insertIndexMap(string n, BpTreeNode* pN) {}
	virtual void deleteMap(string n) {}

	/* note
	return {}; => return value initialize
	this case : return {}; = return nullptr;
	*/
	virtual map<string, BpTreeNode*>* getIndexMap() { return {}; }
	virtual map<string, EmployeeData*>* getDataMap() { return {}; }

};
