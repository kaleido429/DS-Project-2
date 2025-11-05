#pragma once
#include "SelectionTreeNode.h"
#include <fstream>
#include <vector>

class SelectionTree {
private:
    SelectionTreeNode* root;
    std::ofstream* fout; 
    SelectionTreeNode* run[8]; // 8 runs -> 8 leaf nodes

public:
    // constructor
    SelectionTree(std::ofstream* fout) {
        this->fout = fout;
        root = nullptr;
        for (int i = 0; i < 8; ++i) { // initialize run nodes
            run[i] = nullptr;
        }
        setTree();
    }

    // destructor
    ~SelectionTree() {
        if (!root) {
            return;
        }
        vector<SelectionTreeNode*> nodes;
        nodes.push_back(root);
        for (size_t i = 0; i < nodes.size(); ++i) { // traverse tree, BFS
            SelectionTreeNode* node = nodes[i];
            if (node->getLeftChild()) {
                nodes.push_back(node->getLeftChild());
            }
            if (node->getRightChild()) {
                nodes.push_back(node->getRightChild());
            }
        }
        for (SelectionTreeNode* node : nodes) { // delete all nodes
            delete node;
        }
        root = nullptr;
    }

    void setRoot(SelectionTreeNode* pN) { this->root = pN; }
    SelectionTreeNode* getRoot() { return root; }

    void setTree();

    bool Insert(EmployeeData* newData);
    bool Delete();
    bool printEmployeeData(int dept_no);
    bool collectEmployeeData(int dept_no, std::vector<EmployeeData*>& buffer);

private:
    int getRunIndex(int dept_no);
    void updateWinners(SelectionTreeNode* node);
    EmployeeData* chooseWinner(EmployeeData* left, EmployeeData* right);
    void collectByName(EmployeeHeap* heap, vector<EmployeeData*>& buffer);
};