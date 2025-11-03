#pragma once

#include "EmployeeData.h"
#include <vector>

class EmployeeHeap {
private:
    int data_num; // now element number
    EmployeeData** heap_arr;
    int max_capacity = 10; // heap size

public:
    // constructor
    EmployeeHeap() {
        data_num = 0;
        heap_arr = new EmployeeData*[max_capacity + 1]; // index 0 is unused -> size +1
        for (int i = 0; i <= max_capacity; ++i) {
            heap_arr[i] = nullptr;
        }
    }

    // destructor
    ~EmployeeHeap() {
        for (int i = 1; i <= data_num; ++i) {
            delete heap_arr[i];
        }
        delete[] heap_arr;
    }
 
    void Insert(EmployeeData* data);
    EmployeeData* Top();
    void Delete(); // heap = FIFO -> delete top

    bool IsEmpty();

    void UpHeap(int index);
    void DownHeap(int index);
    void ResizeArray();
    
    // for UpHeap, DownHeap, left >= right
    bool IsLeftGreater(EmployeeData* left, EmployeeData* right) {
	    if (left == nullptr) return false;
	    if (right == nullptr) return true;

        // in selection tree, Max Heap / Max Winner Tree by income
	    if (left->getIncome() != right->getIncome()) {
		    return left->getIncome() > right->getIncome();
	    }
	    if (left->getName() != right->getName()) {
		    return left->getName() < right->getName();
	    }
	    return left->getID() < right->getID();
    }

    // for selection tree
    void Collect(std::vector<EmployeeData*>& buffer);
    EmployeeData* RemoveTop();
};