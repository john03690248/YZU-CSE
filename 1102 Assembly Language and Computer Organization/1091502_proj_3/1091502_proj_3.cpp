#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;
struct instr {
	string action;
	string pos1, pos2, pos3;
};
struct data_ {
	string action;
	string pos1, pos2;
	int dispatch = 0;
	int writeBackCycle = 0;
	int busyOrNot = 0;
};
int main() {
	int add_sub_cycle, mul_cycle, div_cycle;    //init different cycle times for different instructions & declare variables
	cout << "Input add/addi/sub cycle time: " << endl;
	cin >> add_sub_cycle;
	cout << "Input mul cycle time: " << endl;
	cin >> mul_cycle;
	cout << "Input div cycle time: " << endl;
	cin >> div_cycle;

	int add_sub_unitReleaseCycle = 0, mul_unitReleaseCycle = 0, div_unitReleaseCycle = 0;    //unit control
	int cycle = 1;
	int change = 0;
	ifstream in;
	in.open("input.txt");
	string line, str;
	vector <int> registerFile;
	for (int i = 0; i <= 4; i++)    //init
		registerFile.push_back(i * 2);
	vector <string> registerAllocationTable;
	string temp = "";
	for (int i = 0; i < 5; i++)    //init
		registerAllocationTable.push_back(temp);
	int numRATuse = 0;    // record number of RAT
	vector <instr> instrQueue;
	data_ reservationStation1[3];     //deal with instructions(ADD, ADDI, SUB), total 3 vacancies. (RS1)
	int numRS1busy = 0;    // record number of busy RS
	data_ reservationStation2[2];    //deal with instructions(MUL, DIV), total 2 vacancies. (RS2)
	int numRS2busy = 0;	   // record number of busy RS
	int currentRS = 0;

	while (getline(in, line)) {    //transform input into specific format
		instr tempInstr;
		stringstream stream(line);
		stream >> str;
		tempInstr.action = str;
		stream >> str;
		tempInstr.pos1 = str.substr(0, str.length() - 1);
		stream >> str;
		tempInstr.pos2 = str.substr(0, str.length() - 1);
		stream >> str;
		tempInstr.pos3 = str;
		instrQueue.push_back(tempInstr);
	}
	
	while (instrQueue.size() != 0 || numRATuse != 0 || numRS1busy != 0 || numRS2busy != 0) {    //start to run
		if (instrQueue.size() != 0) {    //issue part   
			if (instrQueue[0].action == "ADD" || instrQueue[0].action == "ADDI" || instrQueue[0].action == "SUB") {
				if (numRS1busy < 3) {
					for (int i = 0; i < 3; i++)
						if (reservationStation1[i].busyOrNot == 0) {
							currentRS = i;
							break;
						}
					data_ tempData_;
					tempData_.action = instrQueue[0].action;
					tempData_.busyOrNot = 1;
					if (instrQueue[0].pos2[0] == 'F')    //check immediate or not
						if (registerAllocationTable[instrQueue[0].pos2[1] - '0' - 1] == "")    //check the target for RAT exists or not
							tempData_.pos1 = to_string(registerFile[instrQueue[0].pos2[1] - '0' - 1]);
						else 
							tempData_.pos1 = registerAllocationTable[instrQueue[0].pos2[1] - '0' - 1];
					else
						tempData_.pos1 = instrQueue[0].pos2[0];
					if (instrQueue[0].pos3[0] == 'F')    //check immediate or not
						if (registerAllocationTable[instrQueue[0].pos3[1] - '0' - 1] == "")    //check the target for RAT exists or not
							tempData_.pos2 = to_string(registerFile[instrQueue[0].pos3[1] - '0' - 1]);
						else
							tempData_.pos2 = registerAllocationTable[instrQueue[0].pos3[1] - '0' - 1];
					else
						tempData_.pos2 = instrQueue[0].pos3[0];
					if (registerAllocationTable[instrQueue[0].pos1[1] - '0' - 1] == "")
						numRATuse++;
					registerAllocationTable[instrQueue[0].pos1[1] - '0' - 1] = "RS" + to_string(currentRS + 1);    //update RAT		
					reservationStation1[currentRS] = tempData_;
					numRS1busy++;
					vector <instr>::iterator it = instrQueue.begin();
					instrQueue.erase(it);
					change = 1;
				}
			}
			else if (instrQueue[0].action == "MUL" || instrQueue[0].action == "DIV") {
				if (numRS2busy < 2) {
					for (int i = 0; i < 2; i++)
						if (reservationStation2[i].busyOrNot == 0) {
							currentRS = 3 + i;
							break;
						}
					data_ tempData_;
					tempData_.action = instrQueue[0].action;
					tempData_.busyOrNot = 1;
					if (instrQueue[0].pos2[0] == 'F')    //check immediate or not
						if (registerAllocationTable[instrQueue[0].pos2[1] - '0' - 1] == "")    //check the target for RAT exists or not
							tempData_.pos1 = to_string(registerFile[instrQueue[0].pos2[1] - '0' - 1]);
						else
							tempData_.pos1 = registerAllocationTable[instrQueue[0].pos2[1] - '0' - 1];
					else
						tempData_.pos1 = instrQueue[0].pos2[0];
					if (instrQueue[0].pos3[0] == 'F')    //check immediate or not
						if (registerAllocationTable[instrQueue[0].pos3[1] - '0' - 1] == "")    //check the target for RAT exists or not
							tempData_.pos2 = to_string(registerFile[instrQueue[0].pos3[1] - '0' - 1]);
						else
							tempData_.pos2 = registerAllocationTable[instrQueue[0].pos3[1] - '0' - 1];
					else
						tempData_.pos2 = instrQueue[0].pos3[0];
					if (registerAllocationTable[instrQueue[0].pos1[1] - '0' - 1] == "")
						numRATuse++;
					registerAllocationTable[instrQueue[0].pos1[1] - '0' - 1] = "RS" + to_string(currentRS + 1);    //update RAT
					reservationStation2[currentRS - 3] = tempData_;
					numRS2busy++;
					vector <instr>::iterator it = instrQueue.begin();
					instrQueue.erase(it);
					change = 1;
				}
			}
		}
		
		for (int i = 0; i < 3; i++)    //check every RS can dispatch or not (if can, variable(dispatch) = execute cycle, or variable(dispatch) = 0)
			if (reservationStation1[i].busyOrNot == 1 && reservationStation1[i].pos1[0] != 'R' && reservationStation1[i].pos2[0] != 'R' && reservationStation1[i].dispatch == 0) {
				if (add_sub_unitReleaseCycle == 0) {
					reservationStation1[i].dispatch = cycle + 1;
					reservationStation1[i].writeBackCycle = reservationStation1[i].dispatch + add_sub_cycle;
					add_sub_unitReleaseCycle = reservationStation1[i].writeBackCycle;
				}
				else {
					if (cycle >= add_sub_unitReleaseCycle)
						reservationStation1[i].dispatch = cycle + 1;
					else
						reservationStation1[i].dispatch = add_sub_unitReleaseCycle;
					reservationStation1[i].writeBackCycle = reservationStation1[i].dispatch + add_sub_cycle;
					add_sub_unitReleaseCycle = reservationStation1[i].writeBackCycle;
				}	
			}
		for (int i = 0; i < 2; i++)    //check every RS can dispatch or not (if can, variable(dispatch) = execute cycle, or variable(dispatch) = 0)
			if (reservationStation2[i].busyOrNot == 1 && reservationStation2[i].pos1[0] != 'R' && reservationStation2[i].pos2[0] != 'R' && reservationStation2[i].dispatch == 0) {
				if (reservationStation2[i].action == "MUL") {
					if (mul_unitReleaseCycle == 0) {
						reservationStation2[i].dispatch = cycle + 1;
						reservationStation2[i].writeBackCycle = reservationStation2[i].dispatch + mul_cycle;
						mul_unitReleaseCycle = reservationStation2[i].writeBackCycle;
					}
					else {
						if (cycle >= mul_unitReleaseCycle)
							reservationStation2[i].dispatch = cycle + 1;
						else
							reservationStation2[i].dispatch = mul_unitReleaseCycle;
						reservationStation2[i].writeBackCycle = reservationStation2[i].dispatch + mul_cycle;
						mul_unitReleaseCycle = reservationStation2[i].writeBackCycle;
					}
				}
				else if (reservationStation2[i].action == "DIV") {
					if (div_unitReleaseCycle == 0) {
						reservationStation2[i].dispatch = cycle + 1;
						reservationStation2[i].writeBackCycle = reservationStation2[i].dispatch + div_cycle;
						div_unitReleaseCycle = reservationStation2[i].writeBackCycle;
					}
					else {
						if (cycle >= div_unitReleaseCycle)
							reservationStation2[i].dispatch = cycle + 1;
						else
							reservationStation2[i].dispatch = div_unitReleaseCycle;
						reservationStation2[i].writeBackCycle = reservationStation2[i].dispatch + div_cycle;
						div_unitReleaseCycle = reservationStation2[i].writeBackCycle;
					}
				}
			}

		int answer = 0;    //write back part
		for (int i = 0; i < 3; i++)
			if (reservationStation1[i].writeBackCycle != 0 && cycle == reservationStation1[i].writeBackCycle) {
				if (reservationStation1[i].action == "ADD" || reservationStation1[i].action == "ADDI")    //cal answer
					answer = stoi(reservationStation1[i].pos1) + stoi(reservationStation1[i].pos2);
				else if (reservationStation1[i].action == "SUB")
					answer = stoi(reservationStation1[i].pos1) - stoi(reservationStation1[i].pos2);
				reservationStation1[i].busyOrNot = 0;
				numRS1busy--;
				string tempRS = "RS" + to_string(i + 1);    //update RAT & RF
				for (int j = 0; j < 5; j++)
					if (registerAllocationTable[j] == tempRS) {
						registerAllocationTable[j] = "";
						numRATuse--;
						registerFile[j] = answer;
					}
				for (int j = 0; j < 3; j++)     //update RS1
					if (reservationStation1[j].busyOrNot == 1) {
						if (reservationStation1[j].pos1 == tempRS) reservationStation1[j].pos1 = to_string(answer);
						if (reservationStation1[j].pos2 == tempRS) reservationStation1[j].pos2 = to_string(answer);
					}
				for (int j = 0; j < 2; j++)     //update RS2
					if (reservationStation2[j].busyOrNot == 1) {
						if (reservationStation2[j].pos1 == tempRS) reservationStation2[j].pos1 = to_string(answer);
						if (reservationStation2[j].pos2 == tempRS) reservationStation2[j].pos2 = to_string(answer);
					}
				data_ temp_clear;    
				reservationStation1[i] = temp_clear;    //update RS (clear RS);
				change = 1;
			}
		for (int i = 0; i < 2; i++)
			if (reservationStation2[i].writeBackCycle != 0 && cycle == reservationStation2[i].writeBackCycle) {
				if (reservationStation2[i].action == "MUL")    //cal answer
					answer = stoi(reservationStation2[i].pos1) * stoi(reservationStation2[i].pos2);
				else if (reservationStation2[i].action == "DIV")    //cal answer
					answer = stoi(reservationStation2[i].pos1) / stoi(reservationStation2[i].pos2);
				reservationStation2[i].busyOrNot = 0;
				numRS2busy--;
				string tempRS = "RS" + to_string(i + 4);    //update RAT & RF
				for (int j = 0; j < 5; j++)
					if (registerAllocationTable[j] == tempRS) {
						registerAllocationTable[j] = "";
						numRATuse--;
						registerFile[j] = answer;
					}
				for (int j = 0; j < 3; j++)    //update RS1
					if (reservationStation1[j].busyOrNot == 1) {
						if (reservationStation1[j].pos1 == tempRS) reservationStation1[j].pos1 = to_string(answer);
						if (reservationStation1[j].pos2 == tempRS) reservationStation1[j].pos2 = to_string(answer);
					}
				for (int j = 0; j < 2; j++)     //update RS2
					if (reservationStation2[j].busyOrNot == 1) {
						if (reservationStation2[j].pos1 == tempRS) reservationStation2[j].pos1 = to_string(answer);
						if (reservationStation2[j].pos2 == tempRS) reservationStation2[j].pos2 = to_string(answer);
					}
				data_ temp_clear;
				reservationStation2[i] = temp_clear;    //update RS (clear RS);
				change = 1;
			}

		if (change == 1) {    //output
			cout << endl << endl;
			cout << "Cycle: " << cycle << endl << endl;    //cycle
			cout << "   _ RF ___" << endl;    //RF
			for (int i = 0; i < 5; i++)
				cout << "F" << i + 1 << " | " << setw(4) << setfill(' ') << registerFile[i] << " |" << endl;
			cout << "   --------" << endl << endl << endl;
			cout << "   _ RAT __" << endl;    //RAT
			for (int i = 0; i < 5; i++)
				cout << "F" << i + 1 << " | " << setw(4) << setfill(' ') << registerAllocationTable[i] << " |" << endl;
			cout << "   --------" << endl << endl << endl;
			cout << "    _RS______________________" << endl;   //RS1
			for (int i = 0; i < 3; i++) {
				cout << "RS" << i + 1 << " |   ";
				if (reservationStation1[i].action == "ADD" || reservationStation1[i].action == "ADDI") cout << "+";
				else if (reservationStation1[i].action == "SUB") cout << "-";
				else if (reservationStation1[i].action == "") cout << " ";
				cout << "   |";
				cout << " " << setw(4) << setfill(' ') << reservationStation1[i].pos1 << "  |";
				cout << " " << setw(4) << setfill(' ') << reservationStation1[i].pos2 << "  |" << endl;
			}
			cout << "    -------------------------" << endl << endl << endl;
			cout << "    _________________________" << endl;  //RS2
			for (int i = 0; i < 2; i++) {
				cout << "RS" << i + 4 << " |   ";
				if (reservationStation2[i].action == "MUL") cout << "*";
				else if (reservationStation2[i].action == "DIV") cout << "/";
				else if (reservationStation2[i].action == "") cout << " ";
				cout << "   |";
				cout << " " << setw(4) << setfill(' ') << reservationStation2[i].pos1 << "  |";
				cout << " " << setw(4) << setfill(' ') << reservationStation2[i].pos2 << "  |" << endl;
			}
			cout << "    -------------------------" << endl;
			change = 0;
		}
		cycle++;
	}
}