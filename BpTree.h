#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include "SelectionTree.h"
#include <fstream>
#include <vector>

class BpTree {
private:
	BpTreeNode* root;
	int	order;		// m children
	std::ofstream* fout;

public:
	BpTree(std::ofstream *fout, int order = 3) {
		this->fout = fout;
		root = NULL;
		this->order = order;
	}
	
	~BpTree();

	/* essential */
	bool		Insert(EmployeeData* newData);
	bool		excessDataNode(BpTreeNode* pDataNode);
	bool		excessIndexNode(BpTreeNode* pIndexNode);
	void		splitDataNode(BpTreeNode* pDataNode);
	void		splitIndexNode(BpTreeNode* pIndexNode);
	BpTreeNode* getRoot() { return root; }
	BpTreeNode* searchDataNode(string name);
	void		searchRange(string start, string end);
    EmployeeData* search(string name);
    BpTreeDataNode* getFirstDataNode();
    void collectRange(string start, string end, std::vector<EmployeeData*>& buffer);
};

#endif
