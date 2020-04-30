/*
    Name: Khalyl Smith
    ID: 1894880
    Class: MW 4-5:30

    Assignment 3 - The Poorly Ventilated Tunnel

    Compile with: g++ -fpermissive main.cpp -lpthread
    Run with IO Redirection: ./a.out < input.txt
*/

#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

// Global Variables
static pthread_mutex_t lock;
static pthread_cond_t wake = PTHREAD_COND_INITIALIZER;
static int maxCars;
static int maxNCars;
static int maxSCars;
static int northInside = 0;
static int southInside = 0;
static int numNorth = 0;
static int numSouth = 0;
static int delayed = 0;
static int inside = 0;

struct car {
    int carNo;
    char direction;
    int time;
};

void* carThread(void* arg)
{
    bool wait = false;
    car* temp = (struct car*)arg;
    string direction;
    
    if(temp->direction == 'N')
    {
        direction = "Northbound";
        numNorth++;
    }
    else if(temp->direction == 'S')
    {
        direction = "Southbound";
        numSouth++;
    }

    cout << direction << " car # " << temp->carNo << " arrives at the tunnel" << endl;
    pthread_mutex_lock(&lock);

    // Loop checks if car has to wait to enter the tunnel
    while(inside >= maxCars)
    {
        switch(direction[0])
        {
            case 'N':
            {
                if(northInside >= maxNCars)
                    wait = true;
                else
                    wait = false;

                break;
            }
            case 'S':
            {
                if(southInside >= maxSCars)
                    wait = true;
                else
                    wait = false;  
                
                break;
            }
        }

        if(inside >= maxCars)
            wait = true;
        else
            wait = false;

        pthread_cond_wait(&wake, &lock);
    }

    if(direction == "Northbound")
    {
        if(northInside < maxNCars && northInside < maxCars)
        {
            cout << direction << " car # " << temp->carNo << " enters the tunnel." << endl;
            ++inside;
            ++northInside;
            
            pthread_mutex_unlock(&lock);
            sleep(temp->time);
            pthread_mutex_lock(&lock);

            cout << direction << " car # " << temp->carNo << " exits the tunnel." << endl;
            northInside--;
            inside--;
        }
        else
            wait = true;
    }
    else if (direction == "Southbound")
    {
        if (southInside < maxSCars && southInside < maxCars)
        {
            cout << direction << " car # " << temp->carNo << " enters the tunnel." << endl;
            ++inside;
            ++southInside;

            pthread_mutex_unlock(&lock);
            sleep(temp->time);
            pthread_mutex_lock(&lock);

            cout << direction << " car # " << temp->carNo << " exits the tunnel." << endl;
            southInside--;
            inside--;
        }
        else
            wait = true;
    }

    if(wait)
        delayed++;
    
    pthread_cond_broadcast(&wake);
    pthread_mutex_unlock(&lock);
    return temp;
}


int main()
{
    // Get maximum number of: cars allowed in tunnel, northbound cars allowed, and southbound cars allowed
    cin >> maxCars;
    cout << "Maximum number of cars in the tunnel: " << maxCars << endl;
    cin >> maxNCars;
    cout << "Maximum number of northbound cars: " << maxNCars << endl;
    cin >> maxSCars;
    cout << "Maximum number of southbound cars: " << maxSCars << endl;

    pthread_t threads[128];
    pthread_mutex_init(&lock, NULL);
    int numOfCars = 0;

    string input;
    while(cin >> input)
    {
        sleep(stoi(input));
        car* newCar = new car;

        cin >> input;
        newCar->direction = input[0];

        cin >> input;
        newCar->time = stoi(input);

        switch(newCar->direction)
        {
            case 'N':
                newCar->carNo = numNorth + 1;
                break;
            case 'S':
                newCar->carNo = numSouth + 1;
                break;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, carThread, (void*)newCar);
        threads[numOfCars] = tid;
        numOfCars++;
    }

    for(int i = 0; i < numOfCars; i++)
    {
        pthread_join(threads[i], NULL);
    }

    cout << numNorth << " northbound car(s) crossed the tunnel." << endl;
    cout << numSouth << " southbound car(s) crossed the tunnel." << endl;
    cout << delayed << " car(s) had to wait." << endl;
}

