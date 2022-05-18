#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
struct instr {
	string label;
	string action;
	string positions[3];
};
struct state {
	string label;
	string history = "000";
	string states[8] = { "SN","SN","SN","SN","SN","SN","SN","SN" };
	int numMisprediction = 0;
	int entry = 0;
};
struct variable {
	string name;
	int value;
};
int existVariable(vector <variable> variableDatabase, string name);
int existState(vector <state> stateDatabase, string label);
int toDecimal(string binary);
int main() {
	ifstream in;
	in.open("input.txt");
	string line, str;
	vector <instr> instrDatabase;
	vector <state> stateDatabase;
	vector <variable> variableDatabase;
	state tempState;

	while (getline(in, line)) {    //transform the data into the specific format
		instr tempInstr;
		istringstream stream(line);
		stream >> str;
		tempInstr.label = str;
		if (line[0] == '\t') {
			tempInstr.label = str.substr(0, str.length() - 1);
			instrDatabase.push_back(tempInstr);
			continue;
		}
		stream >> str;
		tempInstr.action = str;
		stream >> str;
		int temp = 0;
		int count = 0;
		for (int i = 0; i < str.length(); i++)
			if (str[i] == ',') {
				tempInstr.positions[count] = str.substr(temp, i - temp);
				temp = i + 1;
				count++;
			}
		tempInstr.positions[count] = str.substr(temp, str.length() - temp);
		instrDatabase.push_back(tempInstr);
	}

	int entry;
	cout << "Please input entry(entry > 0):" << endl;
	cin >> entry;

	variable tempVarible;    //add R0(x0) to the variableDatabase
	tempVarible.name = "R0";
	tempVarible.value = 0;
	variableDatabase.push_back(tempVarible);

	for (int i = 0; i < instrDatabase.size(); i++) {    // 6 solutions include "li" , "add" , "addi" , "andi" , "beq" , "bne"
		if (instrDatabase[i].action == "li") {
			int record = existVariable(variableDatabase, instrDatabase[i].positions[0]);
			if (record == -1) {
				variable tempVarible;
				tempVarible.name = instrDatabase[i].positions[0];
				tempVarible.value = stoi(instrDatabase[i].positions[1]);
				variableDatabase.push_back(tempVarible);
			}
			else
				variableDatabase[record].value = stoi(instrDatabase[i].positions[1]);
		}
		else if (instrDatabase[i].action == "add") {
			int record = existVariable(variableDatabase, instrDatabase[i].positions[0]);
			if (record == -1) {
				variable tempVarible;
				tempVarible.name = instrDatabase[i].positions[0];
				variableDatabase.push_back(tempVarible);
			}
			variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value = variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value + variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[2])].value;
		}
		else if (instrDatabase[i].action == "addi") {
			int record = existVariable(variableDatabase, instrDatabase[i].positions[0]);
			if (record == -1) {
				variable tempVarible;
				tempVarible.name = instrDatabase[i].positions[0];
				variableDatabase.push_back(tempVarible);
			}
			variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value = variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value + stoi(instrDatabase[i].positions[2]);
		}
		else if (instrDatabase[i].action == "andi") {
			int record = existVariable(variableDatabase, instrDatabase[i].positions[0]);
			if (record == -1) {
				variable tempVarible;
				tempVarible.name = instrDatabase[i].positions[0];
				variableDatabase.push_back(tempVarible);
			}
			variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value = variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value % (stoi(instrDatabase[i].positions[2]) + 1);
		}
		else if (instrDatabase[i].action == "beq" || instrDatabase[i].action == "bne") {
			int tempi = i;
			int record = existState(stateDatabase, instrDatabase[i].label);
			string predictorResult;
			string result;
			if (record == -1) {
				state tempState;
				tempState.label = instrDatabase[i].label;
				stateDatabase.push_back(tempState);
				record = existState(stateDatabase, instrDatabase[i].label);
			}

			string tempState_ = stateDatabase[record].states[toDecimal(stateDatabase[record].history)];    //predictor result
			if (tempState_ == "SN" || tempState_ == "WN")
				predictorResult = "N";
			else if (tempState_ == "ST" || tempState_ == "WT")
				predictorResult = "T";

			if (instrDatabase[i].action == "beq") {    //true result
				if (variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value == variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value) {  
					result = "T";
					for (int j = 0; j < instrDatabase.size(); j++)
						if (instrDatabase[j].label == instrDatabase[i].positions[2]) {
							i = j;
							break;
						}
				}
				else
					result = "N";
			}
			else {
				if (variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value != variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value) {   
					result = "T";
					for (int j = 0; j < instrDatabase.size(); j++)
						if (instrDatabase[j].label == instrDatabase[i].positions[2]) {
							i = j;
							break;
						}
				}
				else
					result = "N";
			}

			if (predictorResult != result)    //misprediction
				stateDatabase[record].numMisprediction++;

			cout << "entry: " << stateDatabase[record].entry << "       " << instrDatabase[tempi].action << " " << instrDatabase[tempi].positions[0] << "," << instrDatabase[tempi].positions[1] << "," << instrDatabase[tempi].positions[2] << endl;    //output
			cout << "(" << stateDatabase[record].history << ", " << stateDatabase[record].states[0] << ", " << stateDatabase[record].states[1] << ", " << stateDatabase[record].states[2] << ", " << stateDatabase[record].states[3] << ", " << stateDatabase[record].states[4] << ", "
				<< stateDatabase[record].states[5] << ", " << stateDatabase[record].states[6] << ", " << stateDatabase[record].states[7] << ") " << predictorResult << " " << result << "  misprediction: " << stateDatabase[record].numMisprediction << endl << endl;

			if (result == "T") {    //state
				string _tempState = stateDatabase[record].states[toDecimal(stateDatabase[record].history)];
				if (_tempState == "WT") stateDatabase[record].states[toDecimal(stateDatabase[record].history)] = "ST";
				else if (_tempState == "WN") stateDatabase[record].states[toDecimal(stateDatabase[record].history)] = "WT";
				else if (_tempState == "SN") stateDatabase[record].states[toDecimal(stateDatabase[record].history)] = "WN";
			}
			else {
				string _tempState = stateDatabase[record].states[toDecimal(stateDatabase[record].history)];
				if (_tempState == "WN") stateDatabase[record].states[toDecimal(stateDatabase[record].history)] = "SN";
				else if (_tempState == "WT") stateDatabase[record].states[toDecimal(stateDatabase[record].history)] = "WN";
				else if (_tempState == "ST") stateDatabase[record].states[toDecimal(stateDatabase[record].history)] = "WT";
			}

			stateDatabase[record].history[0] = stateDatabase[record].history[1];    //history
			stateDatabase[record].history[1] = stateDatabase[record].history[2];
			if (result == "T")
				stateDatabase[record].history[2] = '1';
			else
				stateDatabase[record].history[2] = '0';

		}
	}
}
int existVariable(vector <variable> variableDatabase, string name) {
	for (int i = 0; i < variableDatabase.size(); i++) 
		if (name == variableDatabase[i].name)
			return i;
	return -1;
}
int existState(vector <state> stateDatabase, string label) {
	for (int i = 0; i < stateDatabase.size(); i++)
		if (label == stateDatabase[i].label)
			return i;
	return -1;
}
int toDecimal(string binary) {
	int sum = 0;
	sum += 1 * (binary[2] - '0') + 2 * (binary[1] - '0') + 2 * 2 * (binary[0] - '0');
	return sum;
}
