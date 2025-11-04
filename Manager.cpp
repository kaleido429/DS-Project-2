#include "Manager.h"
#include <sstream>
#include <vector>
#include <string>
#include <fstream>

namespace {
	void TrimCarriageReturn(std::string& line) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
	}
}

void Manager::run(const char* command) {
	if (bptree == NULL || stree == NULL) {
		return;
	}
	fin.open(command);
	if (!fin.is_open()) {
		return;
	}
	std::string line;
	while (std::getline(fin, line)) {
		TrimCarriageReturn(line);
		if (line.empty()) {
			continue;
		}
		// Parse a single command line from command.txt.
		std::stringstream ss(line);
		std::string cmdName;
		ss >> cmdName;
		if (cmdName.empty()) {
			continue;
		}
		std::vector<std::string> params;
		std::string token;
		while (ss >> token) {
			params.push_back(token);
		}
		parsedArgs = params;

		if (cmdName == "LOAD") {
			LOAD();
		} else if (cmdName == "ADD_BP") {
			ADD_BP();
		} else if (cmdName == "SEARCH_BP") {
			if (params.size() == 1) {
				SEARCH_BP_NAME(params[0]);
			} else if (params.size() == 2) {
				SEARCH_BP_RANGE(params[0], params[1]);
			} else {
				printErrorCode(300);
			}
		} else if (cmdName == "PRINT_BP") {
			PRINT_BP();
		} else if (cmdName == "ADD_ST") {
			if (params.size() != 2) {
				printErrorCode(500);
				continue;
			}
			if (params[0] == "dept_no") {
				try {
					int dept = std::stoi(params[1]);
					ADD_ST_DEPTNO(dept);
				} catch (...) {
					printErrorCode(500);
				}
			} else if (params[0] == "name") {
				ADD_ST_NAME(params[1]);
			} else {
				printErrorCode(500);
			}
		} else if (cmdName == "PRINT_ST") {
			PRINT_ST();
		} else if (cmdName == "DELETE") {
			DELETE();
		} else if (cmdName == "EXIT") {
			if (!params.empty()) {
				printErrorCode(800);
				continue;
			}
			flog << "========EXIT========\n";
			flog << "Success\n";
			flog << "====================\n\n";
			break;
		} else {
			printErrorCode(800);
		}
	}
	fin.close();
}

void Manager::LOAD() {
	if (!parsedArgs.empty()) {
		printErrorCode(100);
		return;
	}
	if (isLoaded || bptree->getFirstDataNode() != NULL) {
		printErrorCode(100);
		return;
	}
	std::ifstream data("employee.txt");
	if (!data.is_open()) {
		printErrorCode(100);
		return;
	}
	std::string line;
	bool hasData = false;
	while (std::getline(data, line)) {
		TrimCarriageReturn(line);
		if (line.empty()) {
			continue;
		}
		std::stringstream ss(line);
		std::string name;
		int dept = 0;
		int id = 0;
		int income = 0;
		if (!(ss >> name >> dept >> id >> income)) {
			continue;
		}
		EmployeeData* employee = new EmployeeData();
		employee->setData(name, dept, id, income);
		bptree->Insert(employee);
		hasData = true;
	}
	data.close();
	if (!hasData) {
		isLoaded = true;
		flog << "========LOAD========\n";
		flog << "Success\n";
		flog << "====================\n\n";
		return;
	}
	isLoaded = true;
	flog << "========LOAD========\n";
	flog << "Success\n";
	flog << "====================\n\n";
}

void Manager::ADD_BP() {
	if (parsedArgs.size() != 4) {
		printErrorCode(200);
		return;
	}
	std::string name = parsedArgs[0];
	std::string deptStr = parsedArgs[1];
	std::string idStr = parsedArgs[2];
	std::string incomeStr = parsedArgs[3];
	int dept = 0;
	int id = 0;
	int income = 0;
	try {
		dept = std::stoi(deptStr);
		id = std::stoi(idStr);
		income = std::stoi(incomeStr);
	} catch (...) {
		printErrorCode(200);
		return;
	}
	if (!isValidDepartment(dept)) {
		printErrorCode(200);
		return;
	}
	EmployeeData* employee = new EmployeeData();
	employee->setData(name, dept, id, income);
	if (!bptree->Insert(employee)) {
		delete employee;
		printErrorCode(200);
		return;
	}
	flog << "========ADD_BP======\n";
	flog << name << "/" << dept << "/" << id << "/" << income << "\n";
	flog << "====================\n\n";
}

void Manager::SEARCH_BP_NAME(string name) {
	EmployeeData* data = bptree->search(name);
	if (data == NULL) {
		printErrorCode(300);
		return;
	}
	flog << "======SEARCH_BP=====\n";
	flog << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	flog << "====================\n\n";
}

void Manager::SEARCH_BP_RANGE(string start, string end) {
	if (start > end) {
		printErrorCode(300);
		return;
	}
	std::vector<EmployeeData*> buffer;
	bptree->collectRange(start, end, buffer);
	if (buffer.empty()) {
		printErrorCode(300);
		return;
	}
	flog << "=====SEARCH_BP======\n";
	for (EmployeeData* data : buffer) {
		flog << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	}
	flog << "====================\n\n";
}

void Manager::ADD_ST_DEPTNO(int dept_no) {
	if (!isValidDepartment(dept_no)) {
		printErrorCode(500);
		return;
	}
	std::vector<EmployeeData*> matches;
	BpTreeDataNode* node = bptree->getFirstDataNode();
	while (node) {
		auto dataMap = node->getDataMap();
		for (auto& entry : *dataMap) {
			if (entry.second->getDeptNo() == dept_no) {
				matches.push_back(entry.second);
			}
		}
		node = dynamic_cast<BpTreeDataNode*>(node->getNext());
	}
	if (matches.empty()) {
		printErrorCode(500);
		return;
	}
	for (EmployeeData* data : matches) {
		EmployeeData* copy = new EmployeeData();
		copy->setData(data->getName(), data->getDeptNo(), data->getID(), data->getIncome());
		if (!stree->Insert(copy)) {
			delete copy;
		}
	}
	printSuccessCode("ADD_ST");
}

void Manager::ADD_ST_NAME(string name) {
	EmployeeData* data = bptree->search(name);
	if (data == NULL) {
		printErrorCode(500);
		return;
	}
	EmployeeData* copy = new EmployeeData();
	copy->setData(data->getName(), data->getDeptNo(), data->getID(), data->getIncome());
	if (!stree->Insert(copy)) {
		delete copy;
		printErrorCode(500);
		return;
	}
	printSuccessCode("ADD_ST");
}

void Manager::PRINT_BP() {
	if (!parsedArgs.empty()) {
		printErrorCode(400);
		return;
	}
	std::vector<EmployeeData*> list;
	BpTreeDataNode* node = bptree->getFirstDataNode();
	while (node) {
		auto dataMap = node->getDataMap();
		for (auto& entry : *dataMap) {
			list.push_back(entry.second);
		}
		node = dynamic_cast<BpTreeDataNode*>(node->getNext());
	}
	if (list.empty()) {
		printErrorCode(400);
		return;
	}
	flog << "======PRINT_BP======\n";
	for (EmployeeData* data : list) {
		flog << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	}
	flog << "====================\n\n";
}

void Manager::PRINT_ST() {
	if (parsedArgs.size() != 1) {
		printErrorCode(600);
		return;
	}
	try {
		int dept = std::stoi(parsedArgs[0]);
		if (!isValidDepartment(dept)) {
			printErrorCode(600);
			return;
		}
		std::vector<EmployeeData*> buffer;
		if (!stree->collectEmployeeData(dept, buffer)) {
			printErrorCode(600);
			return;
		}
		flog << "=====PRINT_ST=======\n";
		for (EmployeeData* data : buffer) {
			flog << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
		}
		flog << "====================\n\n";
	} catch (...) {
		printErrorCode(600);
	}
}

void Manager::DELETE() {
	if (!parsedArgs.empty()) {
		printErrorCode(700);
		return;
	}
	if (!stree->Delete()) {
		printErrorCode(700);
		return;
	}
	printSuccessCode("DELETE");
}

void Manager::printErrorCode(int n) {
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

void Manager::printSuccessCode(string success) {
	flog << "=======" << success << "=======\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}

bool Manager::isValidDepartment(int dept_no) {
	if (dept_no % 100 != 0) {
		return false;
	}
	int key = dept_no / 100;
	return key >= 1 && key <= 8;
}