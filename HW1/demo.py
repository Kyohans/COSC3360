#!/usr/bin/python3
""" This program simulates arrivals of customers at a post officce with two clerks.

    Each customer is identified by a single line contaning it arrival time and 
    how long it will take for him or her to get what he or she needs.  Try
0 5
1 6
2 7
3 3

    While the problem had little to do with your first 3360 assignment, it provides
    a good example of how to engineer a discrete event simulation.

    Author: J.-F. Paris
    January 25, 2020
"""

clock = 0          # the global simulated time
process_list = []     # the global event list
CORES = 2        # total number of postal employees at the counter
FreeCores = 0    # current number of free clerks
INTQ = []
nonINTQ= []  #customer waiting line
processID = 0  # customer count
corebusytime = 0 # sum of clerk busy times

def initialize() :
    # reads in the inputs and schedules the *arrival* events
    global nClerks, nFreeClerks, customerCount, eventList
    import fileinput
    nFreeClerks = nClerks # important!
    for line in fileinput.input():
        (arrivalTime, serviceTime) = line.split()
        arrivalTime = int (arrivalTime)
        serviceTime = int (serviceTime)
        customerCount += 1
        print("Scheduling an *arrival* event for customer %d at t = %d" %
                (customerCount, arrivalTime))
        eventList.append((arrivalTime, 'arrival', customerCount, serviceTime))

def customerArrival(customerID, serviceTime) :
    # One of the two event routines
    # Handles customer arrivals
    global clock, nFreeClerks, eventList, customerQueue, clerkBusyTimes
    print("Customer %d requests clerk attention at t = %d minutes for %d minutes" %
            (customerID, clock, serviceTime))
    if nFreeClerks >  0 :
        nFreeClerks -= 1
        clerkBusyTimes += serviceTime
        print("Customer %d getting service at t = %d minutes" % (customerID, clock))
        print("Scheduling a *departure* event for customer %d at t = %d minutes"  %
                (customerID, clock + serviceTime))
        eventList.append((clock + serviceTime, 'departure', customerID, 0))
        eventList.sort()
    else :
        print("Customer %d must wait for a clerk" % customerID)
        customerQueue.append((customerID, serviceTime))
        print("%d customer(s) now waiting for a clerk" % len(customerQueue))

def customerDeparture (customerID) :
    # One of the two event routines
    # Handles request completions
    global clock, nFreeClerks, eventList, customerQueue, clerkBusyTimes
    print("Customer %d is done at t = %d minutes" % (customerID, clock))
    if customerQueue == [] :
        nFreeClerks += 1
    else :
        (nextCustomer, serviceTime) = customerQueue.pop(0)
        clerkBusyTimes += serviceTime
        print("Customer %d getting service at t = %d minutes" % (nextCustomer, clock))
        print("Scheduling a *departure* event for customer %d at t = %d minutes" %
                 (nextCustomer, clock + serviceTime))
        eventList.append((clock + serviceTime, 'departure', nextCustomer, 0))
        eventList.sort()
         
def printSummary() :
    global clock, customerCount, clerkBusyTimes
    print("\nSUMMARY:")
    print("Simulated time: %d minutes" % clock)
    print("Number of customers that came to the post office: %d" % customerCount)
    print("Average number of busy clerks %.3f" % (clerkBusyTimes/clock))

# START MAIN PROGRAM
initialize()

# MAIN LOOP
while eventList != [] :

    (time, event, customerID, serviceTime) = eventList.pop(0)

    clock = time # First thing to do!

    if event == 'arrival' :
        print("Processing a *%s* event for customer %d at t = %d minutes" %
               (event, customerID, time))
        customerArrival(customerID, serviceTime)


    elif event == 'departure' :
        print("Processing a *%s* event for customer %d at t = %d minutes" %
               (event, customerID, time))
        customerDeparture (customerID)

        
    else :
        print("PANIC: WRONG %s EVENT" % event)

# PRINT SUMMARY
printSummary()
