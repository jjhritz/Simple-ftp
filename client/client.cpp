//Name of Authros: John Hritz & Santiago Quinio
//Course Number and Name: CSE434, Computer Networks
//Semester: Fall 2016
//Project Part 2
//Time Spent: 4 hours 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ios>
#include <sstream>
#include <climits>

//contains definitions of a number of data types used in system calls
#include <sys/types.h>

//definitions of structures needed for sockets
#include <sys/socket.h>

//in.h contains constants and structures needed for internet domain addresses
#include <netinet/in.h>
//netdb.h defines the structure hostent,
#include <netdb.h>


//variable declarations

//sockfd is a file descriptor
//portno stores the port number on which the server accepts connections.
int sockfd, portno, n;

//serv_addr will contain the address of the server
struct sockaddr_in serv_addr;

//variable server is a pointer to a structure of type hostent
/*
struct  hostent
{
  char    *h_name;        // official name of host
  char    **h_aliases;    // alias list
  int     h_addrtype;     // host address type
  int     h_length;       // length of address
  char    **h_addr_list;  // list of addresses from name server
  #define h_addr  h_addr_list[0]  // address, for backward compatiblity
};
*/
struct hostent *server;

char buffer[256];

void error(const char *msg)
{
	perror(msg);
	exit(0);
}


void print_server()
{
    //print server response
    std::cout << "Server says: " << std::endl;
    printf("%s\n", buffer);
}

void read_server()
{
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);

    if (n < 0)
    {
        error("ERROR reading from socket");
    }

    if (n == 0)
    {
        error("ERROR You have been disconnected by the server");
    }

    print_server();
}

int main(int argc, char *argv[])
{
    //if too few/many arguements are provided
    if (argc != 4)
    {
        error("ERROR: Incorrect command. Correct command usage is: client <hostname> <client number> <port>.");
    }

    //if argv[2] is not an integer
    if (strtol(argv[2], NULL, 0) == 0)
    {
        error("ERROR: Invalid client number specified. Client must be an integer. Correct command usage is: client <hostname> <client number> <port>.");
    }

    //if argv[3] is not an integer
    if (strtol(argv[3], NULL, 0) == 0)
    {
        error("ERROR: Invalid port number specified. Port must be an integer. Correct command usage is: client <hostname> <client number> <port>.");
    }

    //if argv[3] is <= 1024
    if (atoi(argv[3]) <= 1024)
    {
        error("ERROR: Ports numbered 1024 and below are reserved by the OS.");
    }



    //atoi() function can be used to convert port number from a string of digits to an integer, if your input is in the form of a string.
    portno = atoi(argv[3]);

    std::cout << "Port: " << portno << std::endl;
    //create socket
    //it take three arguments - address domain, type of socket, protocol (zero allows the OS to choose thye appropriate protocols based on type of socket)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    std::cout << "Socket opened." << std::endl;

    std::cout << "Attempting to connect." << std::endl;
    //funcion gethostbyname() takes name as an argument and returns a pointer to a hostent containing information about that host
    //name is taken as an argument from the user
    server = gethostbyname(argv[1]);

    //If hostent structure is NULL (after the above assignment), the system could not locate a host with this name
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    std::cout << "Connected to host: " << server->h_name << std::endl;

    //set server address buffer with zeros using bzero or memset
    //two arguments - pointer to buffer and sizeof buffer
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //following code sets the fields in serv_addr
    //contains a code for the address family
    serv_addr.sin_family = AF_INET;

    //copies length bytes from s1 to s2
    bcopy((char *) server->h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          server->h_length);

    //contain the port number
    serv_addr.sin_port = htons(portno);

    //connect function is called by the client to establish a connection to the server. It takes three arguments, the socket file descriptor, the address of the host to which it wants to connect (including the port number), and the size of this address
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    //After a connection a client has successfully connected to the server
    //initialize the buffer using the bzero() function
    bzero(buffer, 256);

    //both the server can read and write after a connection has been established.
    //everything written by the client will be read by the server, and everything written by the server will be read by the client.

    //READ/WRITE 1
    //write from the argv[2] into the socket to send the client number to the serve
    n = write(sockfd, argv[2], strlen(argv[2]));

    //check of the write function was successful or not
    if (n < 0)
        error("ERROR writing to socket");

    //Check if number of clients is maximum
    //std::cout << "Checking if server will accept more clients..." << std::endl;

    //READ/WRITE 2
    //read response from server
    //should be "Max active clients reached."
    //or "There's room for you"
    //read_server();

    //READ/WRITE 3
    //check if server closed connection
    //should return 0
    //Or say "Checking your client ID..."
    //read_server();

    //check if client number is valid
    //std::cout << "Checking if client number " << argv[2] << " is valid." << std::endl;

    //READ/WRITE 4
    //read response from server
    //Should say "Client ID already online"
    //Or "You have been registered."
    //read_server();

    //READ/WRITE 5
    //check if server closed connection
    //should return 0
    //Or say "Ready for requests."
    read_server();

    bool more_commands = false;
    bool input_valid = false;

    //while the user has commands, send requests and receive responses
    do
    {
        //empty buffer
        bzero(buffer, 256);

        //prompt for input
        std::cout << "Your message: " << std::endl;

        //getinput
        fgets(buffer,255,stdin);

        //send request
        n = write(sockfd,buffer,strlen(buffer));

        //check of the write function was successful or not
        if (n < 0)
            error("ERROR writing to socket");

        bzero(buffer, 256);

        //get response
        read_server();

        do
        {
            std::cout << std::endl;
            //prompt for more commands
            std::cout << "Another command? y/n" << std::endl;

            std::string input;

            std::cin >> input;

            if(input.compare("y") == 0)
            {
                std::cout << std::endl;
                std::cout << "Continuing." << std::endl;
                input_valid = true;
                more_commands = true;
            }
            else if(input.compare("n") == 0)
            {
                std::cout << std::endl;
                std::cout << "Terminating connection.  Goodbye." << std::endl;
                input_valid = true;
                more_commands = false;
            }
            else
            {
                std::cout << std::endl;
                std::cout << "Invalid input.  Try again." << std::endl;
                input_valid = false;
            }
        }
        while(input_valid == false);

        //clear the cin buffer
        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
    }
    while(more_commands == true);

     //close connections using file descriptors
     close(sockfd);
    return 0;
}





