#include "SelectionTree.h"
#include <algorithm>

// initialize selection tree
void SelectionTree::setTree() {
	if (root) {
		return;
	}
	std::vector<SelectionTreeNode*> leaves; // leaf nodes
	leaves.reserve(8); // for 8 runs (8 leaf nodes)

	// create 8 leaf nodes
	for (int i = 0; i < 8; ++i) {
		SelectionTreeNode* node = new SelectionTreeNode();
		node->HeapInit();
		leaves.push_back(node);
		run[i] = node;
	}

	std::vector<SelectionTreeNode*> current = leaves;

	// build tree upwards
	while (current.size() > 1) {
		std::vector<SelectionTreeNode*> next;
		for (size_t i = 0; i < current.size(); i += 2) { // pair nodes -> parent
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

// insert new EmployeeData into selection tree
bool SelectionTree::Insert(EmployeeData* newData) {
	if (newData == nullptr) { // safety check
		return false;
	}
	int index = getRunIndex(newData->getDeptNo());
	if (index < 0) { // invalid dept number
		return false;
	}
	SelectionTreeNode* leaf = run[index];
	if (leaf == nullptr) { // safety check
		return false;
	}
	if (leaf->getHeap() == nullptr) { // initialize heap if not exists
		leaf->HeapInit();
	}
	EmployeeHeap* heap = leaf->getHeap();
	heap->Insert(newData);
	leaf->setEmployeeData(heap->Top());
	updateWinners(leaf->getParent());
	return true;
}

// delete top EmployeeData from selection tree
bool SelectionTree::Delete() {
	if (!root || !root->getEmployeeData()) { // safety check
		return false;
	}
	EmployeeData* target = root->getEmployeeData();
	int index = getRunIndex(target->getDeptNo());
	if (index < 0) { // invalid dept number
		return false;
	}
	SelectionTreeNode* leaf = run[index];

	//  no leaf node, heap not initialized, or heap is empty
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

// print EmployeeData of given dept number
bool SelectionTree::printEmployeeData(int dept_no) {
	std::vector<EmployeeData*> items;
	if (!collectEmployeeData(dept_no, items)) { // collect data
		return false;
	}
	// print to fout
	for (EmployeeData* data : items) {
		*fout << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	}
	return true;
}

// get run index from dept number
int SelectionTree::getRunIndex(int dept_no) {
	if (dept_no % 100 != 0) { // invalid dept number
		return -1;
	}
	int index = (dept_no / 100) - 1;
	if (index < 0 || index >= 8) { // out of range
		return -1;
	}
	return index;
}

// update winners up to root from given node
void SelectionTree::updateWinners(SelectionTreeNode* node) {
	// traverse leaf -> root
	while (node) {
		SelectionTreeNode* left = node->getLeftChild();
		SelectionTreeNode* right = node->getRightChild();
		// choose winner between left and right
		EmployeeData* winner = chooseWinner(left ? left->getEmployeeData() : nullptr, right ? right->getEmployeeData() : nullptr);
		node->setEmployeeData(winner);
		node = node->getParent();
	}
}

// choose winner between two EmployeeData
EmployeeData* SelectionTree::chooseWinner(EmployeeData* left, EmployeeData* right) {
    
	// safety checks
	if (left == nullptr) return right;
    if (right == nullptr) return left;

	// Compare based on income, name, ID
    if (left->getIncome() != right->getIncome()) {
        return (left->getIncome() > right->getIncome()) ? left : right; // income priority
    }
    if (left->getName() != right->getName()) {
        return (left->getName() < right->getName()) ? left : right; // name priority
    }
    return (left->getID() < right->getID()) ? left : right; // ID priority
	/*note
	in PRINT_ST
	income priority, higher income wins
	name priority, smaller name wins
	id priority, smaller id wins
	*/
}

// collect EmployeeData of given dept number into buffer
void SelectionTree::collectByName(EmployeeHeap* heap, std::vector<EmployeeData*>& buffer) {
	if (!heap) { // safety check
		return;
	}
	heap->Collect(buffer);
}

// collect EmployeeData of given dept number into buffer
bool SelectionTree::collectEmployeeData(int dept_no, std::vector<EmployeeData*>& buffer) {
	int index = getRunIndex(dept_no);
	if (index < 0) { // invalid dept number
		return false;
	}
	SelectionTreeNode* leaf = run[index];
	//  no leaf node, heap not initialized, or heap is empty
	if (!leaf || !leaf->getHeap() || leaf->getHeap()->IsEmpty()) {
		return false;
	}
	collectByName(leaf->getHeap(), buffer);
	if (buffer.empty()) { // no data collected
		return false;
	}
	// Sort by PRINT_ST requirement: income desc, then name asc, then ID asc
	std::sort(buffer.begin(), buffer.end(), [](EmployeeData* lhs, EmployeeData* rhs) {
		if (lhs->getIncome() != rhs->getIncome()) {
			return lhs->getIncome() > rhs->getIncome();
		}
		if (lhs->getName() != rhs->getName()) {
			return lhs->getName() < rhs->getName();
		}
		return lhs->getID() < rhs->getID();
	});
	return true;
}