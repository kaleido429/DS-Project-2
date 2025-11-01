#pragma once

#include "EmployeeData.h"
#include <vector>

class EmployeeHeap {
private:
    int datanum;
    EmployeeData** heapArr;
    int maxCapacity = 10;

public:
    EmployeeHeap() {
        datanum = 0;
        heapArr = new EmployeeData*[maxCapacity + 1];
        for (int i = 0; i <= maxCapacity; ++i) {
            heapArr[i] = nullptr;
        }
    }
    ~EmployeeHeap() {
        for (int i = 1; i <= datanum; ++i) {
            delete heapArr[i];
        }
        delete[] heapArr;
    }
    
    void Insert(EmployeeData* data);
    EmployeeData* Top();
    void Delete();

    bool IsEmpty();

    void UpHeap(int index);
    void DownHeap(int index);
    void ResizeArray();
    void Collect(std::vector<EmployeeData*>& buffer);
    EmployeeData* RemoveTop();
};