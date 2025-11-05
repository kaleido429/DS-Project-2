#pragma once
#include "SelectionTree.h"
#include "BpTree.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class Manager {
private:
	char* cmd;
	BpTree* bptree;
	SelectionTree* stree;
    bool isLoaded;

public:
    //constructor
	Manager(int bpOrder) {
		cmd = NULL;
		bptree = NULL;
		stree = NULL;
		isLoaded = false;
		flog.open("log.txt", std::ios::out | std::ios::trunc);
		if (!flog.is_open()) {
			throw std::runtime_error("Failed to open log file");
		}
		bptree = new BpTree(&flog, bpOrder);
		stree = new SelectionTree(&flog);
	}

	//destructor
	~Manager() {
		delete bptree;
		delete stree;
		if (fin.is_open()) {
			fin.close();
		}
		if (flog.is_open()) {
			flog.close();
		}
	}

	ifstream fin;
	ofstream flog;

	void run(const char* command);
	void LOAD();
	void ADD_BP();
	void SEARCH_BP_NAME(string name);
	void SEARCH_BP_RANGE(string start, string end);
	void PRINT_BP();
	void ADD_ST_DEPTNO(int dept_no);
	void ADD_ST_NAME(string name);
	void PRINT_ST();
	void DELETE();

	void printErrorCode(int n);
	void printSuccessCode(string success);

private:
	bool isValidDepartment(int dept_no);
	std::vector<std::string> parsedArgs;
};

