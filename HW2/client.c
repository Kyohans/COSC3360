/*
    Khalyl Smith
    ID: 1894480
    Date: 3/15/20

    -- Assignment 2 --

    This is assignment is made up into two parts: a client program and a server program.
    This file contains the client which communicates with the server program to retrieve data.

    The client allows the user to search for a city name and if it's on the file read by the server, then it will receive the weather report of said city.

    A hostname and port number are taken as arguments to establish a connection with the server.
    For example: ./client localhost 51717
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    /*
        sockfd = Store values returned by the socket system call
        portno = Port number
        n = Return value for read() and write() calls
    */
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;         // Defines the host

    char buffer[256];
    if(argc < 3)
    {
        fprintf(stderr, "usage %s hostname port", argv[0]);
    }
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR: Failed to open socket");

    
}