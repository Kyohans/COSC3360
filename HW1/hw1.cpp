/*
Khalyl Smith
ID: 1894480
COSC 3360
Due 2/19/20

This program simulates the scheduling of processes by a multi-core processor
*/


#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <iomanip>
#include <vector>

using namespace std;

int totTime = 0;        // Total Time
int arrivalTime;        // The arrival time of a process
int ssdAccess = 0;      // Number of times SSD is accessed by process
int procID;             // Process ID
int coreTime = 0;       // Total time core is accessed by processes
int ssdTime = 0;        // Total time SSD is accessed by processes
int cores;
int freeCores = 0;

struct process
{
    int procID;
    string requestType;
    int startTime;
    string state = " ";
    process* next;
};

void coreRequest(int procID, int arrivalTime);

int main()
{
    string sCores = " ";
    cin >> sCores;
    if(sCores == "NCORES")
    {   
        sCores.clear();
        cin >> cores;
    }
    freeCores = cores;

    list<process> processList;
    vector<process> eventList;

    string word;
    int number;
    while(cin >> word)
    {
        if(word == "START")
        {
            process* proc = new process;
            cin >> number;
            proc->startTime = number;

            cin >> word;
            proc->requestType = word;

            cin >> number;
            proc->procID = number;
            cout << "Process " << proc->procID << " arrived at " << proc->startTime << "ms" << endl;

            cin >> word;
            while(word != "START")
            {
                cin >> number;

                if(word == "END")
                    break;

                process* newProc = proc;
                proc->next = newProc;
                newProc->requestType = word;
                newProc->startTime = number;
                cout << "PID: "  << newProc->procID << " " <<newProc->requestType << " " << newProc->startTime << " " << proc->next << endl;

                cin >> word;
            }
        }

    }
}

void printSummary()
{
    /* FIXME: ADD CODE HERE */
}

void coreRequest(int procID, int arrivalTime)
{
    if(freeCores > 0)
    {
        coreTime += arrivalTime;
        freeCores--;
        cout << "Process " << procID << " arrived at core at " << totTime << "ms" << endl;
        cout << "Scheduling completion at " << arrivalTime << "ms" << endl;
    }
    else
    {
        cout <<"Core is full. Process " << procID << " is blocked" << endl;
    }
    
}

/*
#include <iostream>
#include <list>
#include <vector>

using namespace std;

struct node
{
    int runtime;
    string requestType;
    node *next;
};

int main()
{
    std::cout << "Hello World!\n";

    list<node> linkedlist;

    linkedlist.pop_front();

    cin >> cores;

    vector<node> vec;

    cin >> word;

    while (word != "END")
    {

        if (word == "START")

            node *n = new node;
        n->requestTypee
            n->statime;

        while (word != "START" || word != "END")
            cin >> number;

        node *newNode = new node;
        newNode->requestType = word;
        newNode->runtime = number;

        linkedlist.push_back(newNode);

        cin >> word
    }

    vec.push_back(linkedlist);
}
}
}
*/