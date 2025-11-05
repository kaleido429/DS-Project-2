#include "Manager.h"
#include <sstream>
#include <vector>
#include <string>
#include <fstream>

void Manager::run(const char* command) {
	if (bptree == NULL || stree == NULL) { // safety check
		return;
	}
	fin.open(command);
	if (!fin.is_open()) { // file open failed
		return;
	}
	std::string line;

	// read commands line by line
	while (std::getline(fin, line)) {
		
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
		while (ss >> token) { //parsing arguments
			params.push_back(token);
		}
		parsedArgs = params;

		// execute commands
		if (cmdName == "LOAD") {
			LOAD();
		} else if (cmdName == "ADD_BP") {
			ADD_BP();
		} else if (cmdName == "SEARCH_BP") {
			// distinguish by number of parameters
			if (params.size() == 1) {
				SEARCH_BP_NAME(params[0]);
			} else if (params.size() == 2) {
				SEARCH_BP_RANGE(params[0], params[1]);
			} else {
				printErrorCode(300); // invalid parameters
			}
		} else if (cmdName == "PRINT_BP") {
			PRINT_BP();
		} else if (cmdName == "ADD_ST") {
			// need 2 parameters
			if (params.size() != 2) {
				printErrorCode(500);
				continue;
			}
			// param[0]: "dept_no", ADD_ST_DEPTNO[param[1]]
			if (params[0] == "dept_no") {
				try {
					int dept = std::stoi(params[1]);
					ADD_ST_DEPTNO(dept);
				} catch (...) {
					printErrorCode(500);
				}
			} else if (params[0] == "name") { // param[0]: "name", ADD_ST_NAME[param[1]]
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
	fin.close(); // close file
}

// load employee data from employee.txt
void Manager::LOAD() {
	if (!parsedArgs.empty()) { // no parameters allowed
		printErrorCode(100);
		return;
	}
	if (isLoaded || bptree->getFirstDataNode() != NULL) { // already loaded
		printErrorCode(100);
		return;
	}
	std::ifstream data("employee.txt");
	if (!data.is_open()) { // file open failed
		printErrorCode(100);
		return;
	}
	std::string line;
	bool hasData = false;
	while (std::getline(data, line)) { // read line by line
		
		if (line.empty()) { // skip empty lines
			continue;
		}
		std::stringstream ss(line);
		std::string name;
		int dept = 0;
		int id = 0;
		int income = 0;
		if (!(ss >> name >> dept >> id >> income)) { // parsing failed
			continue;
		}
		EmployeeData* employee = new EmployeeData();
		employee->setData(name, dept, id, income);
		bptree->Insert(employee);
		hasData = true;
	}
	data.close(); // close employee.txt
	if (!hasData) { // no valid data loaded
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

// add new employee data into B+ tree
void Manager::ADD_BP() {
	if (parsedArgs.size() != 4) { // need 4 parameters 1. name 2. department number 3. id 4. income
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
	try { // convert to integers
		dept = std::stoi(deptStr);
		id = std::stoi(idStr);
		income = std::stoi(incomeStr);
	} catch (...) {
		printErrorCode(200);
		return;
	}
	if (!isValidDepartment(dept)) { // invalid department number
		printErrorCode(200);
		return;
	}
	EmployeeData* employee = new EmployeeData();
	employee->setData(name, dept, id, income);
	if (!bptree->Insert(employee)) { // insertion failed
		delete employee;
		printErrorCode(200);
		return;
	}
	flog << "========ADD_BP======\n";
	flog << name << "/" << dept << "/" << id << "/" << income << "\n";
	flog << "====================\n\n";
}

// search employee by name in B+ tree
void Manager::SEARCH_BP_NAME(string name) {
	EmployeeData* data = bptree->search(name); // search by name
	if (data == NULL) { // not found
		printErrorCode(300);
		return;
	}
	flog << "======SEARCH_BP=====\n";
	flog << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	flog << "====================\n\n";
}

// search employee by range in B+ tree
void Manager::SEARCH_BP_RANGE(string start, string end) {
	if (start > end) { // invalid range
		printErrorCode(300);
		return;
	}
	std::vector<EmployeeData*> buffer;
	bptree->collectRange(start, end, buffer);
	if (buffer.empty()) { // no records found
		printErrorCode(300);
		return;
	}
	flog << "=====SEARCH_BP======\n";
	for (EmployeeData* data : buffer) {
		flog << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	}
	flog << "====================\n\n";
}

// add employee data of given dept number into selection tree
void Manager::ADD_ST_DEPTNO(int dept_no) {
	if (!isValidDepartment(dept_no)) { // invalid department number
		printErrorCode(500);
		return;
	}
	std::vector<EmployeeData*> matches;
	BpTreeDataNode* node = bptree->getFirstDataNode();// start from first data node
	while (node) { // iterate all data nodes
		auto dataMap = node->getDataMap();
		for (auto& entry : *dataMap) { // iterate all records in data map
			if (entry.second->getDeptNo() == dept_no) { // department number matches
				matches.push_back(entry.second);
			}
		}
		node = dynamic_cast<BpTreeDataNode*>(node->getNext());
	}
	if (matches.empty()) { // no matching records found
		printErrorCode(500);
		return;
	}
	for (EmployeeData* data : matches) { // insert all matching records into selection tree
		EmployeeData* copy = new EmployeeData();
		copy->setData(data->getName(), data->getDeptNo(), data->getID(), data->getIncome());
		if (!stree->Insert(copy)) { // insertion failed
			delete copy;
		}
	}
	printSuccessCode("ADD_ST"); // success
}

// add employee data of given name into selection tree
void Manager::ADD_ST_NAME(string name) {
	EmployeeData* data = bptree->search(name); // search by name
	if (data == NULL) { // not found
		printErrorCode(500);
		return;
	}
	EmployeeData* copy = new EmployeeData();
	copy->setData(data->getName(), data->getDeptNo(), data->getID(), data->getIncome());
	if (!stree->Insert(copy)) { // insertion failed
		delete copy;
		printErrorCode(500);
		return;
	}
	printSuccessCode("ADD_ST"); // success
}

// print all employee data in B+ tree in order
void Manager::PRINT_BP() {
	if (!parsedArgs.empty()) { // no parameters allowed
		printErrorCode(400);
		return;
	}
	std::vector<EmployeeData*> list;
	BpTreeDataNode* node = bptree->getFirstDataNode();
	while (node) { // iterate all data nodes
		auto dataMap = node->getDataMap();
		for (auto& entry : *dataMap) { // iterate all records in data map
			list.push_back(entry.second);
		}
		node = dynamic_cast<BpTreeDataNode*>(node->getNext());
	}
	if (list.empty()) { // no records found
		printErrorCode(400);
		return;
	}
	flog << "======PRINT_BP======\n";
	for (EmployeeData* data : list) { // print all records
		flog << data->getName() << "/" << data->getDeptNo() << "/" << data->getID() << "/" << data->getIncome() << "\n";
	}
	flog << "====================\n\n";
}

// print all employee data in selection tree for given department number
void Manager::PRINT_ST() {
	if (parsedArgs.size() != 1) { // need 1 parameter: department number
		printErrorCode(600);
		return;
	}
	try { // convert to integer
		int dept = std::stoi(parsedArgs[0]);
		if (!isValidDepartment(dept)) { // invalid department number
			printErrorCode(600);
			return;
		}
		std::vector<EmployeeData*> buffer;
		if (!stree->collectEmployeeData(dept, buffer)) { // collection failed
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

// delete the top employee data from selection tree
void Manager::DELETE() {
	if (!parsedArgs.empty()) { // no parameters allowed
		printErrorCode(700);
		return;
	}
	if (!stree->Delete()) { // deletion failed
		printErrorCode(700);
		return;
	}
	printSuccessCode("DELETE");
}

// // print error code n in log.txt
void Manager::printErrorCode(int n) {
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

// print success message in log.txt
void Manager::printSuccessCode(string success) {
	flog << "=======" << success << "=======\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}

// validate department number
bool Manager::isValidDepartment(int dept_no) {
	if (dept_no % 100 != 0) {
		return false;
	}
	int key = dept_no / 100;
	return key >= 1 && key <= 8;
}