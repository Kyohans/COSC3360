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

struct city
{
    char cityName[64];
    int temp;
    char weather[64];
    struct city* next;
};

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

    char buffer[64];   // Store characters read from socket connection
    char reply[128];    // Contains message sent by server to the client
    struct sockaddr_in serv_addr, cli_addr; // Structure containing an internet address (defined in netinet/in.h)

    // Opens file
    printf("Enter filename: ");
    scanf("%s", buffer);
    FILE *file = fopen(buffer,"r");
    if(file == NULL)
        error("ERROR");
    bzero(buffer, 64);

    // Creates a linked list to access and assign the city names to their corresponding temperatures and weather conditions
    struct city* head = (struct city*)malloc(sizeof(struct city));
    struct city* current = head;

    while(1)
    {
        struct city* node = (struct city*)malloc(sizeof(struct city));
        bzero(node->cityName, 64);
        node->next = NULL;
        current->next = node;
        current = node;

        char name;      // Used to acquire city names
        int i = 0;      // Index

        // Assigns city names to each node
        while((name = getc(file)) != ',' && (name != EOF) && (i != 64))
        {
            node->cityName[i] = name;
            i++;
        }

        if(name == EOF)
            break;

        fscanf(file, "%d , %s", &node->temp, node->weather);
        while((name = getc(file)) != '\n' && (name != EOF));
        if(name == EOF)
            break;
    }
    fclose(file);

    // Acquires server port number
    printf("Enter server port number: ");
    scanf("%d", &portno);
    if((portno < 2000) || (portno > 65535))
        error("ERROR: Invalid port number");

    // Opens new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR: Failed to open socket");

    bzero((char *)&serv_addr, sizeof(serv_addr)); // Sets all values in serv_addr to zero

    serv_addr.sin_family = AF_INET;             // Assigns address family to server address
    serv_addr.sin_port = htons(portno);         // Assigns port number in host byte order to a port number in network byte order
    serv_addr.sin_addr.s_addr = INADDR_ANY;     // Assigns IP address of the host

    // Checks if the sockets of both the client and server can be bound
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR: Failed to bind socket");
    
    // Allows the process to listen on the socket for connections
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    /* Code below will execute once a connection has been established */
    for(;;)
    {
        // Waits for client to connect to server
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR: Failed to accept");
        printf("Connection accepted\n");

        // Clears buffer and reads input from client
        bzero(buffer, 64);
        n = read(newsockfd, buffer, 64);
        if (n < 0)
            error("ERROR: Failed to read from socket");
        printf("Weather report for %s\n", buffer);

        bzero(reply,128);
        sprintf(reply, "-1 -1");
        current = head;
        while((current = current->next) != NULL)
        {
            if(strcmp(buffer, current->cityName) == 0)
            {
                bzero(reply, 128);
                sprintf(reply, "Tomorrow's maximum temperature is %d F\n", current->temp);
                sprintf(reply, "Tomorrow's sky condition is %s", current->weather);
                printf("Tomorrow's maximum temperature is %d F %s\n", current->temp, current->weather);
                printf("Tomorrow's sky condition is %s", current->weather);
                break;
            }
        }
        
        n = write(newsockfd, reply, strlen(reply));
        if(n < 0)
            error("ERROR: Failed to write to socket");

        close(newsockfd);
    }

    return 0;
}