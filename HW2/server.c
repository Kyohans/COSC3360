/*
    Khalyl Smith
    ID: 1894480
    Date: 3/15/20

    -- Assignment 2 --

    This is assignment is made up into two parts: a client program and a server program.
    This file contains the server which opens a connection for the client and will receive data from the client.

    It will read in a file of city names and their corresponding weather reports. The client will be able to search for a city name a receive its weather report.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFFER_LIMIT 64
#define REPLY_LIMIT 256

struct city
{
    char cityName[BUFFER_LIMIT];
    int temp;
    char weather[BUFFER_LIMIT];
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

    char buffer[BUFFER_LIMIT];                  // Store characters read from client
    char reply[REPLY_LIMIT];                    // Contains message to be sent to client
    struct sockaddr_in serv_addr, cli_addr;     // Structure containing an internet address (defined in netinet/in.h)

    // Opens file
    printf("Enter filename: ");
    scanf("%s", buffer);
    FILE *file = fopen(buffer,"r");
    if(file == NULL)
        error("ERROR");
    bzero(buffer, BUFFER_LIMIT);

    // Creates a linked list to access and assign the city names to their corresponding temperatures and weather conditions
    struct city* head = (struct city*)malloc(sizeof(struct city));
    struct city* current = head;

    for(;;)
    {
        struct city* node = (struct city*)malloc(sizeof(struct city));
        bzero(node->cityName, BUFFER_LIMIT);
        node->next = NULL;
        current->next = node;
        current = node;

        char c;         // Used to acquire city names and weather conditions
        int i = 0;      // Index

        // Assigns city names to each node
        while((c = getc(file)) != ',' && (c != EOF) && (i != BUFFER_LIMIT))
        {
            node->cityName[i] = c;
            i++;
        }

        // Assigns temperature to each node
        fscanf(file, "%d,", &node->temp);

        // Assigns weather condition to each node
        i = 0;
        while ((c = getc(file)) != '\n' && (c != EOF))
        {
            node->weather[i] = c;
            i++;
        }

        // Ends loop if reading char reads the end of the file
        if (c == EOF)
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

    bzero((char *)&serv_addr, sizeof(serv_addr));   // Sets all values in serv_addr to zero

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
        printf("\nConnection accepted\n");

        // Clears buffer and reads input from client
        bzero(buffer, BUFFER_LIMIT);
        n = read(newsockfd, buffer, BUFFER_LIMIT);
        if (n < 0)
            error("ERROR: Failed to read from socket");
        printf("Weather report for %s\n", buffer);

        // Empties reply buffer and store "0" in it as default for error checking purposes
        bzero(reply, REPLY_LIMIT);
        sprintf(reply, "0");

        // Set current pointer to head of linked list and checks each node for a matching city name
        current = head;
        while((current = current->next) != NULL)
        {
            // Compares current node's city name with the name sent by the client
            if(strcmp(buffer, current->cityName) == 0)
            {
                // Empties reply buffer and store it with the cities temperature and weather condition
                bzero(reply, REPLY_LIMIT);
                sprintf(reply, "Tomorrow's maximum temperature: %d\nTomorrow's weather condition: %s\n", current->temp, current->weather);
                printf("%s", reply);
                break;
            }
        }

        /*  If no city specified by client can be found then send "No data" to client
            else, reply to client with the temperature and weather condition of said city */
        if(strcmp(reply,"0") == 0)
        {
            n = write(newsockfd, "No data\n", 10);
            if(n < 0)
                error("ERROR: Failed to write to socket");
            printf("No data\n");
        }
        else
        {
            n = write(newsockfd, reply, REPLY_LIMIT);
            if (n < 0)
                error("ERROR: Failed to write to socket");
        }

        // Close socket and clears it for a new incoming connection
        close(newsockfd);
    }

    return 0;
}