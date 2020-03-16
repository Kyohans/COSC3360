/*
    Khalyl Smith
    ID: 1894480
    Date: 3/15/20

    -- Assignment 2 --

    This is assignment is made up into two parts: a client program and a server program.
    This file contains the server which opens a connection for the client and will receive data from the client.

    It will read in a file of city names and their corresponding weather reports. The client will be able to search for a city name a receive its weather report.

    A port number is taken as an argument to establish the server.
    For example: ./server 51717
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    /*
        sockfd = Store values returned by the socket system call
        newsockfd = Store values returned by the accept system call
        portno = Port number
        clilen = Stores the size of the address of the client
        n = Return value for read() and write() calls
    */
    int sockfd, newsockfd, portno, clilen, n;

    char buffer[256];   // Store characters read from socket connection
    struct sockaddr_in serv_addr, cli_addr; // Structure containing an internet address (defined in netinet/in.h)

    if(argc < 2)
    {
        error("ERROR: No port provided");
        exit(1);
    }

    // Opens new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR: Failed to open socket");

    bzero((char *) &serv_addr, sizeof(serv_addr)); // Sets all values in serv_addr to zero
    
    portno = atoi(argv[1]); // Converts port number argument into to an integer

    serv_addr.sin_family = AF_INET;             // Assigns address family to server address
    serv_addr.sin_port = htons(portno);         // Assigns port number in host byte order to a port number in network byte order
    serv_addr.sin_addr.s_addr = INADDR_ANY;     // Assigns IP address of the host

    // Checks if the sockets of both the client and server can be bound
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR: Failed to bind socket");
    
    // Allows the process to listen on the socket for connections
    listen(sockfd,5);

    // Waits for client to connect to server
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd < 0)
        error("ERROR: Failed to accept");


    /* Code below will execute once a connection has been established */
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 256);
    if(n < 0)
        error("ERROR: Failed to read from socket");

    return 0;
}