#include "EmployeeHeap.h"

namespace {
	bool IsLeftGreater(EmployeeData* lhs, EmployeeData* rhs) {
		if (lhs == nullptr) return false;
		if (rhs == nullptr) return true;
		if (lhs->getIncome() != rhs->getIncome()) {
			return lhs->getIncome() > rhs->getIncome();
		}
		if (lhs->getName() != rhs->getName()) {
			return lhs->getName() < rhs->getName();
		}
		return lhs->getID() < rhs->getID();
	}
}

void EmployeeHeap::Insert(EmployeeData* data) {
	if (data == nullptr) {
		return;
	}
	if (datanum + 1 >= maxCapacity) {
		ResizeArray();
	}
	heapArr[++datanum] = data;
	UpHeap(datanum);
}

EmployeeData* EmployeeHeap::Top() {
	if (datanum == 0) {
		return nullptr;
	}
	return heapArr[1];
}

void EmployeeHeap::Delete() {
	EmployeeData* removed = RemoveTop();
	delete removed;
}

EmployeeData* EmployeeHeap::RemoveTop() {
	if (datanum == 0) {
		return nullptr;
	}
	EmployeeData* top = heapArr[1];
	heapArr[1] = heapArr[datanum];
	heapArr[datanum] = nullptr;
	--datanum;
	DownHeap(1);
	return top;
}

bool EmployeeHeap::IsEmpty() {
	return datanum == 0;
}

void EmployeeHeap::UpHeap(int index) {
	while (index > 1) {
		int parent = index / 2;
		if (IsLeftGreater(heapArr[index], heapArr[parent])) {
			EmployeeData* temp = heapArr[parent];
			heapArr[parent] = heapArr[index];
			heapArr[index] = temp;
			index = parent;
		} else {
			break;
		}
	}
}

void EmployeeHeap::DownHeap(int index) {
	while (index * 2 <= datanum) {
		int left = index * 2;
		int right = left + 1;
		int largest = left;
		if (right <= datanum && IsLeftGreater(heapArr[right], heapArr[left])) {
			largest = right;
		}
		if (IsLeftGreater(heapArr[largest], heapArr[index])) {
			EmployeeData* temp = heapArr[index];
			heapArr[index] = heapArr[largest];
			heapArr[largest] = temp;
			index = largest;
		} else {
			break;
		}
	}
}

void EmployeeHeap::ResizeArray() {
	int newCapacity = maxCapacity * 2;
	EmployeeData** newArr = new EmployeeData*[newCapacity + 1];
	for (int i = 0; i <= newCapacity; ++i) {
		newArr[i] = nullptr;
	}
	for (int i = 1; i <= datanum; ++i) {
		newArr[i] = heapArr[i];
	}
	delete[] heapArr;
	heapArr = newArr;
	maxCapacity = newCapacity;
}

void EmployeeHeap::Collect(std::vector<EmployeeData*>& buffer) {
	for (int i = 1; i <= datanum; ++i) {
		if (heapArr[i]) {
			buffer.push_back(heapArr[i]);
		}
	}
}