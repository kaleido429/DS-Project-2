#include "EmployeeHeap.h"

// Insert a new EmployeeData into the heap
void EmployeeHeap::Insert(EmployeeData* data) {
	if (data == nullptr) {
		return;
	}
	if (data_num + 1 >= max_capacity) { // if heap is full, resize
		ResizeArray();
	}
	heap_arr[++data_num] = data;
	UpHeap(data_num); // maintain max-heap
}

// return Top element without removing
EmployeeData* EmployeeHeap::Top() {
	if (data_num == 0) { // empty heap
		return nullptr;
	}
	return heap_arr[1];
}

// delete Top element
void EmployeeHeap::Delete() {
	EmployeeData* removed = RemoveTop();
	delete removed;
}

// remove and return Top element
EmployeeData* EmployeeHeap::RemoveTop() {
	if (data_num == 0) {
		return nullptr;
	}
	EmployeeData* top = heap_arr[1];
	heap_arr[1] = heap_arr[data_num];
	heap_arr[data_num] = nullptr; // clear last element
	--data_num;
	DownHeap(1); // maintain max-heap
	return top;
}

// check if heap is empty
bool EmployeeHeap::IsEmpty() {
	return data_num == 0;
}

// Max Heap, using after Insert
void EmployeeHeap::UpHeap(int index) {
	while (index > 1) { // array[index] ~ array[1]
		int parent = index / 2; // parent index
		if (IsLeftGreater(heap_arr[index], heap_arr[parent])) { // Max Heap -> left(child) > right(parent)
			EmployeeData* temp = heap_arr[parent];
			heap_arr[parent] = heap_arr[index];
			heap_arr[index] = temp;
			index = parent;
		} else {
			break;
		}
	}
}

// Max Heap, using after remove element
void EmployeeHeap::DownHeap(int index) {
	while (index * 2 <= data_num) { // until no child node
		int left = index * 2; // left child index
		int right = left + 1; // right child index
		int largest = left;

		// if right child exists and right > left
		if (right <= data_num && IsLeftGreater(heap_arr[right], heap_arr[left])) {
			largest = right;
		}

		// if parent < largest child, swap
		if (IsLeftGreater(heap_arr[largest], heap_arr[index])) {
			EmployeeData* temp = heap_arr[index];
			heap_arr[index] = heap_arr[largest];
			heap_arr[largest] = temp;
			index = largest;
		} else {
			break;
		}
	}
}

// heap array resize (*2)
void EmployeeHeap::ResizeArray() {
	int new_capacity = max_capacity * 2;
	EmployeeData** newArr = new EmployeeData*[new_capacity + 1];
	for (int i = 0; i <= new_capacity; ++i) {
		newArr[i] = nullptr;
	}
	for (int i = 1; i <= data_num; ++i) {
		newArr[i] = heap_arr[i];
	}
	delete[] heap_arr;
	heap_arr = newArr;
	max_capacity = new_capacity;
}

void EmployeeHeap::Collect(std::vector<EmployeeData*>& buffer) {
	for (int i = 1; i <= data_num; ++i) {
		if (heap_arr[i]) {
			buffer.push_back(heap_arr[i]);
		}
	}
}