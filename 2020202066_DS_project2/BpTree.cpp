#include "BpTree.h"
#include <queue>
#include <vector>
#include <fstream>
#include <string>

// destructor
BpTree::~BpTree() {
    if (!root) { // if tree is empty
        return; // return; => do nothing
    }
    std::vector<BpTreeNode*> nodes;
    std::queue<BpTreeNode*> q;
    q.push(root);

	// until queue is empty, traverse the tree in level-order(like BFS), delete all nodes
    while (!q.empty()) { // if queue is not empty => continue
        BpTreeNode* current = q.front();
        q.pop(); // remove front node from queue
        nodes.push_back(current); // store current node for later deletion

		/* note
		dynamic_cast<T*>(ptr) is safe type casting in C++
		run-time check, if fail, return nullptr
		---
		auto is compile time type deduction
		*/
        BpTreeIndexNode* indexNode = dynamic_cast<BpTreeIndexNode*>(current);
        if (indexNode) { // if current node is index node -> push child nodes
            if (current->getMostLeftChild()) { // if most-left child exists
                q.push(current->getMostLeftChild()); // push queue
            }
            auto mapIndex = indexNode->getIndexMap(); 
			
			// entry.second -> right child node pointer, push into queue
            for (auto& entry : *mapIndex) {
                if (entry.second) { // if child node exists
                    q.push(entry.second);
                }
            }
        }
    }

	// delete EmployeeData* in data nodes first
    for (BpTreeNode* node : nodes) {
        BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(node);
        if (dataNode) { // if current node is data node
            auto dataMap = dataNode->getDataMap();
            for (auto& entry : *dataMap) { // delete EmployeeData* in dataMap
                delete entry.second;
            }
        }
    }

	/* note
	reverse BFS
	rbegin() -> last element -> leaf node
	rend() -> one before first element
	*/
    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) { // delete all BpTreeNode*
        delete *it;
    }
    root = nullptr;
}

// insert new EmployeeData into B+ tree
// steps
// 1) find target leaf by name
// 2) if name exists -> update record (dept/id/income)
// 3) else insert into leaf; if overflow, split leaf (and propagate)
bool BpTree::Insert(EmployeeData* newData) {
	if (newData == NULL) { // safety check
		return false;
	}

	// tree is empty -> first insertion -> create first data node as root
	if (root == NULL) {
		BpTreeDataNode* dataNode = new BpTreeDataNode();
		dataNode->insertDataMap(newData->getName(), newData);
		root = dataNode;
		return true;
	}

	BpTreeNode* foundNode = searchDataNode(newData->getName()); // descend to leaf for this key
	BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(foundNode); // for using dataMap
	if (dataNode == NULL) { // safety check
		return false;
	}

	auto dataMap = dataNode->getDataMap();
	auto iter = dataMap->find(newData->getName());

	// if name already exists, update ONLY income (PDF)
	if (iter != dataMap->end()) { 
		iter->second->setIncome(newData->getIncome());
		delete newData;
		return true;
	}

	// insert new EmployeeData into the found data node
	dataNode->insertDataMap(newData->getName(), newData); // insert new key into leaf
	if (excessDataNode(dataNode)) { // if leaf overflows order
		splitDataNode(dataNode); // split leaf and possibly propagate to parent
	}
	return true;
}

// check data node's order
bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
	BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(pDataNode);
	if (dataNode == NULL) { // not data node or NULL
		return false;
	}
	auto dataMap = dataNode->getDataMap();

	// size >= order -> overflow -> return true
	return dataMap->size() >= static_cast<size_t>(order);
}

// check index node's order
bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
	BpTreeIndexNode* indexNode = dynamic_cast<BpTreeIndexNode*>(pIndexNode);
	if (indexNode == NULL) {
		return false;
	}
	auto indexMap = indexNode->getIndexMap();

	// size >= order -> overflow -> return true
	return indexMap->size() >= static_cast<size_t>(order);
}

// split overflowing data node
void BpTree::splitDataNode(BpTreeNode* pDataNode) {
	BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(pDataNode);
	if (dataNode == NULL) { // safety check
		return;
	}
	// Split the overflow data node into two sibling nodes and promote the first key of the right node
	auto dataMap = dataNode->getDataMap();
	int total = static_cast<int>(dataMap->size());
	int mid = total / 2; // split point (left keeps 0..mid-1, right takes mid..end-1)
	BpTreeDataNode* newNode = new BpTreeDataNode();
	std::vector<std::string> keys;
	keys.reserve(dataMap->size());
	for (auto& entry : *dataMap) { // collect keys in sorted order (map is ordered)
		keys.push_back(entry.first);
	}
	for (int i = mid; i < total; ++i) { // move right half records to new right sibling
		auto it = dataMap->find(keys[i]);
		newNode->insertDataMap(it->first, it->second);
		dataMap->erase(it);
	}
	// fix leaf-level doubly linked list
	newNode->setNext(dataNode->getNext());
	if (dataNode->getNext()) {
		dataNode->getNext()->setPrev(newNode);
	}
	dataNode->setNext(newNode);
	newNode->setPrev(dataNode);
	// promote the first key of the right sibling to parent
	std::string promoteKey = newNode->getDataMap()->begin()->first;
	BpTreeNode* parent = dataNode->getParent();
	if (parent == NULL) {
		// no parent -> create a new root index node
		BpTreeIndexNode* newRoot = new BpTreeIndexNode();
		newRoot->setMostLeftChild(dataNode);
		dataNode->setParent(newRoot);
		newRoot->insertIndexMap(promoteKey, newNode);
		newNode->setParent(newRoot);
		root = newRoot;
	} else {
		// insert promoted key and right sibling into existing parent
		BpTreeIndexNode* parentIndex = dynamic_cast<BpTreeIndexNode*>(parent);
		parentIndex->insertIndexMap(promoteKey, newNode);
		newNode->setParent(parentIndex);
		if (excessIndexNode(parentIndex)) { // if parent overflows -> split parent
			splitIndexNode(parentIndex);
		}
	}
}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
	BpTreeIndexNode* indexNode = dynamic_cast<BpTreeIndexNode*>(pIndexNode);
	if (indexNode == NULL) {
		return;
	}
	// Rebalance an overflowing index node by creating a new right sibling and moving half of the children.
	std::vector<std::string> keys;
	std::vector<BpTreeNode*> children;
	auto indexMap = indexNode->getIndexMap();
	BpTreeNode* leftMost = pIndexNode->getMostLeftChild(); // first child (to the left of first key)
	children.push_back(leftMost);
	for (auto& entry : *indexMap) { // flatten keys and right-children in order
		keys.push_back(entry.first);
		children.push_back(entry.second);
	}
	int totalChildren = static_cast<int>(children.size());
	int leftCount = (totalChildren + 1) / 2; // number of children to keep on the left node
	if (leftCount < 1) {
		leftCount = 1;
	}
	// choose promote key (the greatest key that stays in the left node)
	std::string promoteKey = keys[leftCount - 1];
	BpTreeNode* rightMostLeft = children[leftCount]; // left child of the new right node
	BpTreeIndexNode* newIndex = new BpTreeIndexNode();
	indexMap->clear(); // rebuild left index node
	indexNode->setMostLeftChild(children[0]); // reset most-left child
	if (children[0]) {
		children[0]->setParent(indexNode);
	}
	for (int i = 0; i < leftCount - 1; ++i) { // put back left-side (key, right-child) pairs
		indexMap->insert(std::make_pair(keys[i], children[i + 1]));
		if (children[i + 1]) {
			children[i + 1]->setParent(indexNode);
		}
	}
	// build new right sibling index node
	newIndex->setMostLeftChild(rightMostLeft);
	if (rightMostLeft) {
		rightMostLeft->setParent(newIndex);
	}
	for (size_t i = leftCount; i < keys.size(); ++i) { // move remaining (key, right-child) into right node
		newIndex->insertIndexMap(keys[i], children[i + 1]);
		if (children[i + 1]) {
			children[i + 1]->setParent(newIndex);
		}
	}
	BpTreeNode* parent = indexNode->getParent();
	if (parent == NULL) {
		// no parent -> create a new root and attach left/right index nodes
		BpTreeIndexNode* newRoot = new BpTreeIndexNode();
		newRoot->setMostLeftChild(indexNode);
		indexNode->setParent(newRoot);
		newRoot->insertIndexMap(promoteKey, newIndex);
		newIndex->setParent(newRoot);
		root = newRoot;
	} else {
		// insert promoted key and right sibling into existing parent
		BpTreeIndexNode* parentIndex = dynamic_cast<BpTreeIndexNode*>(parent);
		parentIndex->insertIndexMap(promoteKey, newIndex);
		newIndex->setParent(parentIndex);
		if (excessIndexNode(parentIndex)) { // propagate split if parent overflows
			splitIndexNode(parentIndex);
		}
	}
}

// search for the data(leaf) node that may contain the given name
BpTreeNode* BpTree::searchDataNode(string name) {
	BpTreeNode* current = root;
	if (current == NULL) {
		return NULL;
	}
	// descend through index nodes until reaching a data(leaf) node
	while (dynamic_cast<BpTreeIndexNode*>(current) != NULL) {
		BpTreeIndexNode* indexNode = dynamic_cast<BpTreeIndexNode*>(current);
		BpTreeNode* next = indexNode->getMostLeftChild(); // default: go to most-left child
		auto mapIndex = indexNode->getIndexMap();
		for (auto& entry : *mapIndex) { // pick the right child for the largest key <= name
			if (name >= entry.first) {
				next = entry.second;
			} else {
				break;
			}
		}
		current = next;
	}
	return current;
}

// search for all records with names in the range [start, end]
void BpTree::searchRange(string start, string end) {
	if (start > end) { // invalid range
		return;
	}
	BpTreeDataNode* node = dynamic_cast<BpTreeDataNode*>(searchDataNode(start));
	if (node == NULL) {
		return;
	}
	while (node) { // iterate over leaf nodes via right links
		auto dataMap = node->getDataMap();
		for (auto it = dataMap->lower_bound(start); it != dataMap->end(); ++it) { // start from lower_bound in first leaf
			if (it->first > end) {
				return;
			}
			EmployeeData* data = it->second;
			*fout << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
		}
		node = dynamic_cast<BpTreeDataNode*>(node->getNext()); // move to next leaf
		if (!node) {
			break;
		}
		start = ""; // after first leaf, consume from the beginning
	}
}

// search for a single record by name
EmployeeData* BpTree::search(string name) {
	BpTreeDataNode* node = dynamic_cast<BpTreeDataNode*>(searchDataNode(name));
	if (node == NULL) {
		return NULL;
	}

	// in dataMap, search for the key(name)
	auto dataMap = node->getDataMap();
	auto it = dataMap->find(name);
	if (it == dataMap->end()) {
		return NULL;
	}
	return it->second;
}

// get the first data(leaf) node in the B+ tree
BpTreeDataNode* BpTree::getFirstDataNode() {
	if (root == NULL) {
		return NULL;
	}
	BpTreeNode* node = root; // start from root
	while (dynamic_cast<BpTreeIndexNode*>(node) != NULL) { // follow most-left pointers to the leftmost leaf
		node = node->getMostLeftChild();
	}
	return dynamic_cast<BpTreeDataNode*>(node);
}

// get all records with names in the range [start, end] into buffer
void BpTree::collectRange(string start, string end, std::vector<EmployeeData*>& buffer) {
	if (start > end) { // invalid range
		return;
	}
	BpTreeDataNode* node = dynamic_cast<BpTreeDataNode*>(searchDataNode(start));
	if (node == NULL) { // no data node found
		return;
	}
	bool firstNode = true; // use lower_bound only for the first leaf
	while (node) { // traverse leaves and collect into buffer
		auto dataMap = node->getDataMap();
		auto it = firstNode ? dataMap->lower_bound(start) : dataMap->begin();
		for (; it != dataMap->end(); ++it) { // stop when key exceeds end
			if (it->first > end) {
				return;
			}
			buffer.push_back(it->second);
		}
		firstNode = false;
		node = dynamic_cast<BpTreeDataNode*>(node->getNext()); // go to next leaf
	}
}