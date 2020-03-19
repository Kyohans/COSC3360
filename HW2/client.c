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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define BUFFER_LIMIT 64
#define REPLY_LIMIT 256

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

    struct sockaddr_in serv_addr;   // Structure containing an internet address (defined in netinet/in.h)
    struct hostent *server;         // Defines the host

    char buffer[BUFFER_LIMIT];  // Store characters read from user input
    char reply[REPLY_LIMIT];    // Contains the server's reply
    
    // Finds hostname of client
    printf("Enter server hostname: ");
    scanf("%s", buffer);
    server = gethostbyname(buffer);
    if (server == NULL)
        error("ERROR: No such host found");
    bzero(buffer, BUFFER_LIMIT);

    // Acquires port number and opens a socket to communicate with server
    printf("Enter server port number: ");
    scanf("%s", buffer);
    portno = atoi(buffer);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR: Failed to open socket");
    bzero(buffer, BUFFER_LIMIT);

    bzero((char *) &serv_addr, sizeof(serv_addr)); // Sets all values in serv_addr to zero
    serv_addr.sin_family = AF_INET;

    // Copies length of bytes from host address to server address
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    // Attempt to connect to server
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR");
    
    // Prompts user to enter a name of a city
    printf("Enter a city name: ");

    // Clears input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    // Gets user input and removes trailing newline (Appears after user presses ENTER)
    fgets(buffer, BUFFER_LIMIT, stdin);
    strtok(buffer, "\n");

    // Sends user input to server
    n = write(sockfd, buffer, strlen(buffer));
    if(n < 0)
        error("ERROR: Failed to write to socket");
    bzero(buffer, BUFFER_LIMIT);
    
    // Reads response from server and prints it to console
    bzero(reply, REPLY_LIMIT);
    n = read(sockfd, reply, REPLY_LIMIT);
    if(n < 0)
        error("ERROR: Failed to read from socket");
    printf("%s\n", reply);
    bzero(reply, REPLY_LIMIT);

    close(sockfd);
    return 0;
}
