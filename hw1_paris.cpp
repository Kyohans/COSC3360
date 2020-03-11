#include <iostream>
#include <fstream>
#include <string>
#include <limits.h>
#include <iomanip>

using namespace std;

struct process; 

struct instruction
{
	string instruct;
	int value;
	instruction* next = NULL;
	instruction* prev = NULL;
	process* proc = NULL;
	instruction* nexQ = NULL;
};

struct process
{
	int procNum;
	process* next = NULL;
	process* prev = NULL;
	instruction* link = NULL;
	string state = "";
};

void readInstruction(instruction* ainstruc);
void readProcess(process* aproc);
void update(int time);

void endProcess(process* toTerminate);
void printStatus();
void printSummary();

instruction* nextReady = new instruction;
instruction* readyQ = nextReady;
instruction* nextInput = new instruction;
instruction* inputQ = nextInput;
instruction* nextSSD = new instruction;
instruction* ssdQ = nextSSD;

instruction* headInstruct = new instruction;
instruction* currentInstruct = headInstruct;
process* headProcess = new process;
process* currentProcess = headProcess;

ifstream file;
ofstream output;

int cores = 0;
instruction** coreInstructs = NULL;
int* coreTimes = NULL;
instruction* ioDevice = NULL;
int ioTime = -1;
instruction* SSD = NULL;
int ssdTime = -1;
instruction** tempArr = NULL;
int processes = 0;
int timeNow = 0;

int ssdAccesses = 0;
int ssdSum = 0;
int coreSum = 0;


int main()
{
	string fileName;

	file.open(fileName);

	cout << "Found: " << fileName << endl;

	string word;
	cin >> word;
	cin >> cores;
	cores = stoi(word);
	coreInstructs = new instruction*[cores];
	coreTimes = new int[cores];
	for (int i = 0; i < cores; i++)
	{
		coreInstructs[i] = NULL;
		coreTimes[i] = -1;
	}
	tempArr = new instruction*[cores + 2];

	while (cin >> word)
	{
		instruction* newInstruct = new instruction;
		newInstruct->instruct = word;
		if (word == "NEW")
		{
			process* newProcess = new process;
			newProcess->procNum = processes;
			processes++;
			newProcess->link = newInstruct;
			newProcess->prev = currentProcess;
			currentProcess->next = newProcess;
			currentProcess = currentProcess->next;
		}
		cin >> newInstruct->value;
		newInstruct->proc = currentProcess;
		newInstruct->prev = currentInstruct;
		currentInstruct->next = newInstruct;
		currentInstruct = currentInstruct->next;
	}
	file.close();

	readProcess(headProcess->next);
	printSummary();

	return 0;
}

void readInstruction(instruction* ainstruc)
{
	switch ((ainstruc->instruct)[0])
	{
	case 'C':
	{
		coreSum += ainstruc->value;
		bool full = true;
		for (int i = 0; i < cores; i++)
			if (coreInstructs[i] == NULL)
			{
				full = false;
				ainstruc->proc->state = "RUNNING";
				coreInstructs[i] = ainstruc;
				coreTimes[i] = ainstruc->value + timeNow;
				break;
			}
		if (full)
		{
			ainstruc->proc->state = "READY";
			readyQ->nexQ = ainstruc;
			readyQ = readyQ->nexQ;
		}
		break;
	}
	case 'I':
	{
		ainstruc->proc->state = "BLOCKED";
		if (ioDevice == NULL)
		{
			ioDevice = ainstruc;
			ioTime = ainstruc->value + timeNow;
		}
		else
		{
			inputQ->nexQ = ainstruc;
			inputQ = inputQ->nexQ;
		}
		break;
	}
	case 'S':
	{
		ssdAccesses++;
		ssdSum += ainstruc->value;
		if (SSD == NULL)
		{
			ainstruc->proc->state = "RUNNING";
			SSD = ainstruc;
			ssdTime = ainstruc->value + timeNow;
		}
		else
		{
			ainstruc->proc->state = "READY";
			ssdQ->nexQ = ainstruc;
			ssdQ = ssdQ->nexQ;
		}
		break;
	}
	}
}

void readProcess(process* aproc)
{
	update(aproc->link->value);
	cout << "Process " << (aproc->procNum) << " starts at time " << timeNow << " ms" << endl;
	printStatus();
	readInstruction(aproc->link->next);

	if (aproc->next != NULL)
			readProcess(aproc->next);
	else
		update(INT_MAX);
}

void update(int time)
{
	int maxTime = -1;
	instruction* maxInstruc = NULL;
	for (int i = 0; i < cores; i++)
		if (coreTimes[i] < time)
			if (coreTimes[i] > maxTime)
			{
				maxTime = coreTimes[i];
				maxInstruc = coreInstructs[i];
			}
	if (ioTime < time)
		if (ioTime > maxTime)
		{
			maxTime = ioTime;
			maxInstruc = ioDevice;
		}
	if (ssdTime < time)
		if (ssdTime > maxTime)
		{
			maxTime = ssdTime;
			maxInstruc = SSD;
		}
	if (maxTime != -1)
	{
		switch (maxInstruc->instruct[0])
		{
		case 'C':
		{
			for (int i = 0; i < cores; i++)
			{
				if (coreInstructs[i] == maxInstruc)
				{
					update(coreTimes[i]);
					instruction* temp = coreInstructs[i];
					coreInstructs[i] = NULL;
					coreTimes[i] = -1;
					if (nextReady->nexQ != NULL)
					{
						coreInstructs[i] = nextReady->nexQ;
						coreTimes[i] = coreInstructs[i]->value + timeNow;
						coreInstructs[i]->proc->state = "RUNNING";
						nextReady = nextReady->nexQ;
					}
					if (temp->next != NULL)
					{
						if (temp->next->instruct == "NEW")
							endProcess(temp->proc);
						else
							readInstruction(temp->next);
					}
					else
						endProcess(temp->proc);
					break;
				}
			}
			break;
		}
		case 'I':
		{
			update(ioTime);
			instruction* temp = ioDevice;
			ioDevice = NULL;
			ioTime = -1;
			if (nextInput->nexQ != NULL)
			{
				ioDevice = nextInput->nexQ;
				ioTime = ioDevice->value + timeNow;
				ioDevice->proc->state = "BLOCKED";
				nextInput = nextInput->nexQ;
			}
			if (temp->next != NULL)
			{
				if (temp->next->instruct == "NEW")
					endProcess(temp->proc);
				else
					readInstruction(temp->next);
			}
			else
				endProcess(temp->proc);
			break;
		}
		case 'S':
		{
			update(ssdTime);
			instruction* temp = SSD;
			SSD = NULL;
			ssdTime = -1;
			if (nextSSD->nexQ != NULL)
			{
				SSD = nextSSD->nexQ;
				ssdTime = SSD->value + timeNow;
				SSD->proc->state = "RUNNING";
				nextSSD = nextSSD->nexQ;
			}
			if (temp->next != NULL)
			{
				if ((temp->next->instruct) == "NEW")
					endProcess(temp->proc);
				else
					readInstruction(temp->next);
			}
			else
				endProcess(temp->proc);
			break;
		}
		}
		update(time);
	}
	if (time != INT_MAX)
		timeNow = time;
}

void endProcess(process* toTerminate)
{
	toTerminate->state = "TERMINATED";
	cout << "Process " << toTerminate->procNum << " terminates at time " << timeNow << " ms" << endl;
	printStatus();
	toTerminate->state = "";
}

void printStatus()
{
	process* temp = headProcess;
	while (temp->next != NULL)
	{
		temp = temp->next;
		if (temp->state != "")
			cout << "Process " << temp->procNum << " is " << temp->state << endl;
	}
	cout << endl;
}

void printSummary()
{
	cout << "SUMMARY:" << endl;
	cout << "Number of processes that completed: " << processes << endl;
	cout << "Total number of SSD accesses: " << ssdAccesses << endl;
	if (ssdAccesses == 0)
		cout << "Average SSD access time: " << 0 << " ms" << endl;
	else
		cout << "Average SSD access time: " << setprecision(2) << fixed << (double(ssdSum) / ssdAccesses) << " ms" << endl;
	cout << "Total elapsed time: " << timeNow << " ms" << endl;
	if (timeNow == 0)
	{
		cout << "Core utilization: " << 0 << " percent" << endl;
		cout << "SSD utilization: " << 0 << " percent" << endl;
	}
	else
	{
		cout << "Core utilization: " << setprecision(2) << fixed << ((double(coreSum) / timeNow) * 100) << " percent" << endl;
		cout << "SSD utilization: " << setprecision(2) << fixed << ((double(ssdSum) / timeNow) * 100) << " percent" << endl;
	}
}
