#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string>

using namespace std;

struct car {
    int id;
    char direction;
    int time;
};

