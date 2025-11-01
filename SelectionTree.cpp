#include "SelectionTree.h"
#include <algorithm>

namespace {
	bool IsBetter(EmployeeData* lhs, EmployeeData* rhs) {
		if (lhs == nullptr) {
			return false;
		}
		if (rhs == nullptr) {
			return true;
		}
		if (lhs->getIncome() != rhs->getIncome()) {
			return lhs->getIncome() > rhs->getIncome();
		}
		if (lhs->getName() != rhs->getName()) {
			return lhs->getName() < rhs->getName();
		}
		return lhs->getID() < rhs->getID();
	}
}

void SelectionTree::setTree() {
	if (root) {
		return;
	}
	std::vector<SelectionTreeNode*> leaves;
	leaves.reserve(8);
	for (int i = 0; i < 8; ++i) {
		SelectionTreeNode* node = new SelectionTreeNode();
		node->HeapInit();
		leaves.push_back(node);
		run[i] = node;
	}

	std::vector<SelectionTreeNode*> current = leaves;
	while (current.size() > 1) {
		std::vector<SelectionTreeNode*> next;
		for (size_t i = 0; i < current.size(); i += 2) {
			SelectionTreeNode* parent = new SelectionTreeNode();
			SelectionTreeNode* leftChild = current[i];
			SelectionTreeNode* rightChild = current[i + 1];
			parent->setLeftChild(leftChild);
			parent->setRightChild(rightChild);
			leftChild->setParent(parent);
			rightChild->setParent(parent);
			next.push_back(parent);
		}
		current = next;
	}
	root = current.front();
}

bool SelectionTree::Insert(EmployeeData* newData) {
	if (newData == nullptr) {
		return false;
	}
	int index = getRunIndex(newData->getDeptNo());
	if (index < 0) {
		return false;
	}
	SelectionTreeNode* leaf = run[index];
	if (leaf == nullptr) {
		return false;
	}
	if (leaf->getHeap() == nullptr) {
		leaf->HeapInit();
	}
	EmployeeHeap* heap = leaf->getHeap();
	heap->Insert(newData);
	leaf->setEmployeeData(heap->Top());
	updateWinners(leaf->getParent());
	return true;
}

bool SelectionTree::Delete() {
	if (!root || !root->getEmployeeData()) {
		return false;
	}
	EmployeeData* target = root->getEmployeeData();
	int index = getRunIndex(target->getDeptNo());
	if (index < 0) {
		return false;
	}
	SelectionTreeNode* leaf = run[index];
	if (!leaf || !leaf->getHeap() || leaf->getHeap()->IsEmpty()) {
		return false;
	}
	EmployeeHeap* heap = leaf->getHeap();
	// Remove the winner from the corresponding heap and rebuild the winner path.
	EmployeeData* removed = heap->RemoveTop();
	delete removed;
	leaf->setEmployeeData(heap->Top());
	updateWinners(leaf->getParent());
	return true;
}

bool SelectionTree::printEmployeeData(int dept_no) {
	std::vector<EmployeeData*> items;
	if (!collectEmployeeData(dept_no, items)) {
		return false;
	}
	for (EmployeeData* data : items) {
		*fout << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	}
	return true;
}

int SelectionTree::getRunIndex(int dept_no) {
	if (dept_no % 100 != 0) {
		return -1;
	}
	int index = (dept_no / 100) - 1;
	if (index < 0 || index >= 8) {
		return -1;
	}
	return index;
}

void SelectionTree::updateWinners(SelectionTreeNode* node) {
	while (node) {
		SelectionTreeNode* left = node->getLeftChild();
		SelectionTreeNode* right = node->getRightChild();
		EmployeeData* winner = chooseWinner(left ? left->getEmployeeData() : nullptr,
											right ? right->getEmployeeData() : nullptr);
		node->setEmployeeData(winner);
		node = node->getParent();
	}
}

EmployeeData* SelectionTree::chooseWinner(EmployeeData* left, EmployeeData* right) {
	if (IsBetter(left, right)) {
		return left;
	}
	return right;
}

void SelectionTree::collectByName(EmployeeHeap* heap, std::vector<EmployeeData*>& buffer) {
	if (!heap) {
		return;
	}
	heap->Collect(buffer);
}

bool SelectionTree::collectEmployeeData(int dept_no, std::vector<EmployeeData*>& buffer) {
	int index = getRunIndex(dept_no);
	if (index < 0) {
		return false;
	}
	SelectionTreeNode* leaf = run[index];
	if (!leaf || !leaf->getHeap() || leaf->getHeap()->IsEmpty()) {
		return false;
	}
	collectByName(leaf->getHeap(), buffer);
	if (buffer.empty()) {
		return false;
	}
	// Sort by name to match the required output order for PRINT_ST.
	std::sort(buffer.begin(), buffer.end(), [](EmployeeData* lhs, EmployeeData* rhs) {
		if (lhs->getName() != rhs->getName()) {
			return lhs->getName() < rhs->getName();
		}
		return lhs->getID() < rhs->getID();
	});
	return true;
}