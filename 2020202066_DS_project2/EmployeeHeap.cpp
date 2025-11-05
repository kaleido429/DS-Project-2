#include "EmployeeHeap.h"

// Insert a new EmployeeData into the heap
void EmployeeHeap::Insert(EmployeeData* data) {
	if (data == nullptr) {
		return;
	}
	// de-duplicate by ID: if already exists in this heap, skip and delete to avoid leak
	if (ContainsById(data->getID())) {
		delete data;
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

// check if any element in heap has the given ID
bool EmployeeHeap::ContainsById(int id) const {
	for (int i = 1; i <= data_num; ++i) {
		if (heap_arr[i] && heap_arr[i]->getID() == id) {
			return true;
		}
	}
	return false;
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
	EmployeeData** new_arr = new EmployeeData*[new_capacity + 1];
	// initialize new array
	for (int i = 0; i <= new_capacity; ++i) {
		new_arr[i] = nullptr;
	}

	// copy element, heap arr -> new arr
	for (int i = 1; i <= data_num; ++i) {
		new_arr[i] = heap_arr[i];
	}
	delete[] heap_arr; // delete old array
	heap_arr = new_arr; // change array pointer
	max_capacity = new_capacity; // change capacity
}

// call all EmployeeData pointers in the heap
void EmployeeHeap::Collect(std::vector<EmployeeData*>& buffer) {
	// copy all elements heap -> buffer
	for (int i = 1; i <= data_num; ++i) {
		if (heap_arr[i]) { // heap arr[i] exists
			buffer.push_back(heap_arr[i]);
		}
	}
}