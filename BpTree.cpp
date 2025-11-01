#include "BpTree.h"
#include <queue>
#include <vector>
#include <fstream>
#include <string>

BpTree::~BpTree() {
    if (!root) {
        return;
    }
    std::vector<BpTreeNode*> nodes;
    std::queue<BpTreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        BpTreeNode* current = q.front();
        q.pop();
        nodes.push_back(current);
        BpTreeIndexNode* indexNode = dynamic_cast<BpTreeIndexNode*>(current);
        if (indexNode) {
            if (current->getMostLeftChild()) {
                q.push(current->getMostLeftChild());
            }
            auto mapIndex = indexNode->getIndexMap();
            for (auto& entry : *mapIndex) {
                if (entry.second) {
                    q.push(entry.second);
                }
            }
        }
    }
    for (BpTreeNode* node : nodes) {
        BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(node);
        if (dataNode) {
            auto dataMap = dataNode->getDataMap();
            for (auto& entry : *dataMap) {
                delete entry.second;
            }
        }
    }
    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
        delete *it;
    }
    root = nullptr;
}

bool BpTree::Insert(EmployeeData* newData) {
	if (newData == NULL) {
		return false;
	}
	if (root == NULL) {
		BpTreeDataNode* dataNode = new BpTreeDataNode();
		dataNode->insertDataMap(newData->getName(), newData);
		root = dataNode;
		return true;
	}
	BpTreeNode* foundNode = searchDataNode(newData->getName());
	BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(foundNode);
	if (dataNode == NULL) {
		return false;
	}
	auto dataMap = dataNode->getDataMap();
	auto iter = dataMap->find(newData->getName());
	if (iter != dataMap->end()) {
		iter->second->setDeptNo(newData->getDeptNo());
		iter->second->setID(newData->getID());
		iter->second->setIncome(newData->getIncome());
		delete newData;
		return true;
	}
	dataNode->insertDataMap(newData->getName(), newData);
	if (excessDataNode(dataNode)) {
		splitDataNode(dataNode);
	}
	return true;
}

bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
	BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(pDataNode);
	if (dataNode == NULL) {
		return false;
	}
	auto dataMap = dataNode->getDataMap();
	return dataMap->size() >= static_cast<size_t>(order);
}

bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
	BpTreeIndexNode* indexNode = dynamic_cast<BpTreeIndexNode*>(pIndexNode);
	if (indexNode == NULL) {
		return false;
	}
	auto indexMap = indexNode->getIndexMap();
	return indexMap->size() >= static_cast<size_t>(order);
}

void BpTree::splitDataNode(BpTreeNode* pDataNode) {
	BpTreeDataNode* dataNode = dynamic_cast<BpTreeDataNode*>(pDataNode);
	if (dataNode == NULL) {
		return;
	}
	// Split the overflowing data node into two sibling nodes and promote the first key of the right node.
	auto dataMap = dataNode->getDataMap();
	int total = static_cast<int>(dataMap->size());
	int mid = total / 2;
	BpTreeDataNode* newNode = new BpTreeDataNode();
	std::vector<std::string> keys;
	keys.reserve(dataMap->size());
	for (auto& entry : *dataMap) {
		keys.push_back(entry.first);
	}
	for (int i = mid; i < total; ++i) {
		auto it = dataMap->find(keys[i]);
		newNode->insertDataMap(it->first, it->second);
		dataMap->erase(it);
	}
	newNode->setNext(dataNode->getNext());
	if (dataNode->getNext()) {
		dataNode->getNext()->setPrev(newNode);
	}
	dataNode->setNext(newNode);
	newNode->setPrev(dataNode);
	std::string promoteKey = newNode->getDataMap()->begin()->first;
	BpTreeNode* parent = dataNode->getParent();
	if (parent == NULL) {
		BpTreeIndexNode* newRoot = new BpTreeIndexNode();
		newRoot->setMostLeftChild(dataNode);
		dataNode->setParent(newRoot);
		newRoot->insertIndexMap(promoteKey, newNode);
		newNode->setParent(newRoot);
		root = newRoot;
	} else {
		BpTreeIndexNode* parentIndex = dynamic_cast<BpTreeIndexNode*>(parent);
		parentIndex->insertIndexMap(promoteKey, newNode);
		newNode->setParent(parentIndex);
		if (excessIndexNode(parentIndex)) {
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
	BpTreeNode* leftMost = pIndexNode->getMostLeftChild();
	children.push_back(leftMost);
	for (auto& entry : *indexMap) {
		keys.push_back(entry.first);
		children.push_back(entry.second);
	}
	int totalChildren = static_cast<int>(children.size());
	int leftCount = (totalChildren + 1) / 2;
	if (leftCount < 1) {
		leftCount = 1;
	}
	std::string promoteKey = keys[leftCount - 1];
	BpTreeNode* rightMostLeft = children[leftCount];
	BpTreeIndexNode* newIndex = new BpTreeIndexNode();
	indexMap->clear();
	indexNode->setMostLeftChild(children[0]);
	if (children[0]) {
		children[0]->setParent(indexNode);
	}
	for (int i = 0; i < leftCount - 1; ++i) {
		indexMap->insert(std::make_pair(keys[i], children[i + 1]));
		if (children[i + 1]) {
			children[i + 1]->setParent(indexNode);
		}
	}
	newIndex->setMostLeftChild(rightMostLeft);
	if (rightMostLeft) {
		rightMostLeft->setParent(newIndex);
	}
	for (size_t i = leftCount; i < keys.size(); ++i) {
		newIndex->insertIndexMap(keys[i], children[i + 1]);
		if (children[i + 1]) {
			children[i + 1]->setParent(newIndex);
		}
	}
	BpTreeNode* parent = indexNode->getParent();
	if (parent == NULL) {
		BpTreeIndexNode* newRoot = new BpTreeIndexNode();
		newRoot->setMostLeftChild(indexNode);
		indexNode->setParent(newRoot);
		newRoot->insertIndexMap(promoteKey, newIndex);
		newIndex->setParent(newRoot);
		root = newRoot;
	} else {
		BpTreeIndexNode* parentIndex = dynamic_cast<BpTreeIndexNode*>(parent);
		parentIndex->insertIndexMap(promoteKey, newIndex);
		newIndex->setParent(parentIndex);
		if (excessIndexNode(parentIndex)) {
			splitIndexNode(parentIndex);
		}
	}
}

BpTreeNode* BpTree::searchDataNode(string name) {
	BpTreeNode* current = root;
	if (current == NULL) {
		return NULL;
	}
	while (dynamic_cast<BpTreeIndexNode*>(current) != NULL) {
		BpTreeIndexNode* indexNode = dynamic_cast<BpTreeIndexNode*>(current);
		BpTreeNode* next = indexNode->getMostLeftChild();
		auto mapIndex = indexNode->getIndexMap();
		for (auto& entry : *mapIndex) {
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

void BpTree::searchRange(string start, string end) {
	if (start > end) {
		return;
	}
	BpTreeDataNode* node = dynamic_cast<BpTreeDataNode*>(searchDataNode(start));
	if (node == NULL) {
		return;
	}
	while (node) {
		auto dataMap = node->getDataMap();
		for (auto it = dataMap->lower_bound(start); it != dataMap->end(); ++it) {
			if (it->first > end) {
				return;
			}
			EmployeeData* data = it->second;
			*fout << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
		}
		node = dynamic_cast<BpTreeDataNode*>(node->getNext());
		if (!node) {
			break;
		}
		start = "";
	}
}

EmployeeData* BpTree::search(string name) {
	BpTreeDataNode* node = dynamic_cast<BpTreeDataNode*>(searchDataNode(name));
	if (node == NULL) {
		return NULL;
	}
	auto dataMap = node->getDataMap();
	auto it = dataMap->find(name);
	if (it == dataMap->end()) {
		return NULL;
	}
	return it->second;
}

BpTreeDataNode* BpTree::getFirstDataNode() {
	if (root == NULL) {
		return NULL;
	}
	BpTreeNode* node = root;
	while (dynamic_cast<BpTreeIndexNode*>(node) != NULL) {
		node = node->getMostLeftChild();
	}
	return dynamic_cast<BpTreeDataNode*>(node);
}

void BpTree::collectRange(string start, string end, std::vector<EmployeeData*>& buffer) {
	if (start > end) {
		return;
	}
	BpTreeDataNode* node = dynamic_cast<BpTreeDataNode*>(searchDataNode(start));
	if (node == NULL) {
		return;
	}
	bool firstNode = true;
	while (node) {
		auto dataMap = node->getDataMap();
		auto it = firstNode ? dataMap->lower_bound(start) : dataMap->begin();
		for (; it != dataMap->end(); ++it) {
			if (it->first > end) {
				return;
			}
			buffer.push_back(it->second);
		}
		firstNode = false;
		node = dynamic_cast<BpTreeDataNode*>(node->getNext());
	}
}