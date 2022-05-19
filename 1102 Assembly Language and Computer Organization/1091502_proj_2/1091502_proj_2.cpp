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
	int numPastLabel = 0;    // be used to record how many labels current instr has pasted
};
struct state {
	string history = "000";
	string states[8] = { "SN","SN","SN","SN","SN","SN","SN","SN" };
	int numMisprediction = 0;
	int entry = 0;
};
struct variable {
	string name;
	int value;
};
int existVariable(vector <variable> variableDatabase, string name);    // return the index of variable if it exist, or return -1
int toDecimal(string binary);    // transform binary to decimal
int main() {
	ifstream in;
	in.open("input.txt");
	string line, str;
	vector <instr> instrDatabase;        
	vector <state> stateDatabase;        	
	vector <variable> variableDatabase;  

	int numPastLabel_ = 0;
	while (getline(in, line)) {    //transform the data into the specific format
		instr tempInstr;
		stringstream stream(line);
		stream >> str;
		tempInstr.label = str;
		if (line[0] == '\t') {
			numPastLabel_++;
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
		tempInstr.numPastLabel = numPastLabel_;
		instrDatabase.push_back(tempInstr);
	}

	int entry;    //init every predictor (entry)
	cout << "Please input entry(entry > 0):" << endl;
	cin >> entry;
	state tempState;
	for (int i = 0; i < entry; i++) {
		tempState.entry = i;
		stateDatabase.push_back(tempState);
	}

	variable tempVarible;    //add R0(x0) to the variableDatabase
	tempVarible.name = "R0";
	tempVarible.value = 0;
	variableDatabase.push_back(tempVarible);

	string predictorResult;
	string result;
	for (int i = 0; i < instrDatabase.size(); i++) {    //( PC )start to run // 6 solutions include "li" , "add" , "addi" , "andi" , "beq" , "bne"
		int tempi = i;
		if (instrDatabase[i].action == "")    // skip label
			continue;
		if (instrDatabase[i].action == "li") {    // li
			result = "N";
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
		else if (instrDatabase[i].action == "add") {    // add
			result = "N";
			int record = existVariable(variableDatabase, instrDatabase[i].positions[0]);
			if (record == -1) {
				variable tempVarible;
				tempVarible.name = instrDatabase[i].positions[0];
				variableDatabase.push_back(tempVarible);
			}
			variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value = variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value + variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[2])].value;
		}
		else if (instrDatabase[i].action == "addi") {    // addi
			result = "N";
			int record = existVariable(variableDatabase, instrDatabase[i].positions[0]);
			if (record == -1) {
				variable tempVarible;
				tempVarible.name = instrDatabase[i].positions[0];
				variableDatabase.push_back(tempVarible);
			}
			variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value = variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value + stoi(instrDatabase[i].positions[2]);
		}
		else if (instrDatabase[i].action == "andi") {    // andi
			result = "N";
			int record = existVariable(variableDatabase, instrDatabase[i].positions[0]);
			if (record == -1) {
				variable tempVarible;
				tempVarible.name = instrDatabase[i].positions[0];
				variableDatabase.push_back(tempVarible);
			}
			variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[0])].value = variableDatabase[existVariable(variableDatabase, instrDatabase[i].positions[1])].value % (stoi(instrDatabase[i].positions[2]) + 1);
		}
		else if (instrDatabase[i].action == "beq" || instrDatabase[i].action == "bne") {    // beq or bne
			if (instrDatabase[i].action == "beq") {    // beq    
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
			else {    //bne
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
		}
		int currentEntry = (tempi - instrDatabase[tempi].numPastLabel) % entry;    //cal currentEntry (which predictor should be update)
		string tempState_ = stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)];    //predictor result
		if (tempState_ == "SN" || tempState_ == "WN")
			predictorResult = "N";
		else if (tempState_ == "ST" || tempState_ == "WT")
			predictorResult = "T";

		if (predictorResult != result)    //misprediction
			stateDatabase[currentEntry].numMisprediction++;

		cout << "entry: " << stateDatabase[currentEntry].entry << "       " << instrDatabase[tempi].action << " " << instrDatabase[tempi].positions[0] << "," << instrDatabase[tempi].positions[1];    //output  
		if (instrDatabase[tempi].positions[2] != "") 
			cout << "," << instrDatabase[tempi].positions[2];
		cout << endl;
		cout << "(" << stateDatabase[currentEntry].history << ", " << stateDatabase[currentEntry].states[0] << ", " << stateDatabase[currentEntry].states[1] << ", " << stateDatabase[currentEntry].states[2] << ", " << stateDatabase[currentEntry].states[3] << ", " << stateDatabase[currentEntry].states[4] << ", "
			<< stateDatabase[currentEntry].states[5] << ", " << stateDatabase[currentEntry].states[6] << ", " << stateDatabase[currentEntry].states[7] << ") " << predictorResult << " " << result << "  misprediction: " << stateDatabase[currentEntry].numMisprediction << endl << endl;

		if (result == "T") {    //state (update state after prediction)
			string _tempState = stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)];
			if (_tempState == "WT") stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)] = "ST";
			else if (_tempState == "WN") stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)] = "WT";
	     	else if (_tempState == "SN") stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)] = "WN";
		}
		else {
			string _tempState = stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)];
			if (_tempState == "WN") stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)] = "SN";
			else if (_tempState == "WT") stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)] = "WN";
			else if (_tempState == "ST") stateDatabase[currentEntry].states[toDecimal(stateDatabase[currentEntry].history)] = "WT";
		}

		stateDatabase[currentEntry].history[0] = stateDatabase[currentEntry].history[1];    //history (update history after prediction)
		stateDatabase[currentEntry].history[1] = stateDatabase[currentEntry].history[2];
		if (result == "T")
			stateDatabase[currentEntry].history[2] = '1';
		else
			stateDatabase[currentEntry].history[2] = '0';
	}
}
int existVariable(vector <variable> variableDatabase, string name) {
	for (int i = 0; i < variableDatabase.size(); i++) 
		if (name == variableDatabase[i].name)
			return i;
	return -1;
}
int toDecimal(string binary) {
	int sum = 0;
	sum += 1 * (binary[2] - '0') + 2 * (binary[1] - '0') + 2 * 2 * (binary[0] - '0');
	return sum;
}
