//Name of Authros: John Hritz & Santiago Quinio
//Course Number and Name: CSE434, Computer Networks
//Semester: Fall 2016
//Project Part 2
//Time Spent: 4 hours

/*
 * 1.)    What are the advantages and disadvantages of a concurrent, connection oriented server?
 *  A.)
 *
 *  Advantage:
 *   Better performance than Iterative server implementations
 *   Lack of wait time for clients
 *   Overall provides a faster response time to multiple clients
 *   Works well when a response requires massive amounts of input
 *   Works well when processing time for each section varies
 *   Able to handle multiple requests at the same time
 *   Reliable delivery
 *
 *   Disadvantages:
 *   More difficult to design and build
 *   Resulting code is more complex
 *   Code is difficult to modify
 *
 *   2.)    What is the appropriate usage of a concurrent, connection oriented server?
 *   A.)
 *   It is appropriate to use a concurrent connection oriented server when multiple clients are connecting to the server at the same time.
 *
 *
 */


/*
 * Execution: server p
 *
 * p: port number for connection
 *
 * server -> argv[0]
 * p -> argv[1]
 */

//input - output declarations included in all C programs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <ios>
#include <sstream>
#include <fstream>

//contains definitions of a number of data types used in system calls
#include <sys/types.h>

//definitions of structures needed for sockets
#include <sys/socket.h>

//in.h contains constants and structures needed for internet domain addresses
#include <netinet/in.h>

//contains functions for checking the state of child processes
#include <sys/wait.h>

//vector for the user ids
std::vector<std::string> users;

//vector for the child pids
std::vector<int> child_pids;

//number of active children
int active_children = 0;

//variable declarations
int n;
int pid;

//sockfd and newsockfd are file descriptors,These two variables store the values returned by the socket system call and the accept system call.
//portno stores the port number on which the server accepts connections.
int sockfd, newsockfd, portno;



//This function is called when a system call fails. It displays a message about the error on stderr and then aborts the program.
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//This function is called when a system call in a child process fails. It displays a message about the error on stderr and then aborts the program.
void child_error(const char *msg)
{
    perror(msg);
    _exit(0);
}

std::string read_client()
{
    //Improved implementation of socket ready by Steve on StackOverflow: http://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket
    // create the buffer with space for the data
    const unsigned int MAX_BUF_LENGTH = 4096;
    std::vector<char> buffer(MAX_BUF_LENGTH);
    std::string rcv;
    int bytesReceived = 0;
    do
    {
        bytesReceived = recv(newsockfd, buffer.data(), buffer.size(), 0);
        // append string from buffer.
        if ( bytesReceived == -1 )
        {
            if(pid == 0)
            {
                child_error("ERROR on socket read.");
            }
            else
            {
                error("ERROR on socket read.");
            }
        }
        else
        {
            rcv.append( buffer.cbegin(), buffer.cend() );
        }
    }
    while ( bytesReceived == MAX_BUF_LENGTH );
    // At this point we have the available data (which may not be a complete
    // application level message).

    return rcv;

    /*
     * //Legacy implementation
     *
     * bzero(buffer,256);
        n = read(newsockfd,buffer,255);

        if (n < 0)
        {
            child_error("ERROR reading from socket");
        }

        if (n == 0)
        {
            child_error("Connection closed by client");
        }

        std::cout << "Message from client " << users.back() << ": " << buffer << std::endl;
     */
}

int client_write(std::string message)
{
    int n;
    n = write(newsockfd, message.c_str(), strlen(message.c_str()));

    if(n < 1)
    {
        if(pid == 0)
        {
            child_error("ERROR on socket write.");
        }
        else
        {
            error("ERROR on socket write.");
        }
    }

    return n;
}

void client_service()
{
    int n;
    std::string response;

    //while connection is open
    while(1)
    {
        std::cout << "Waiting for request from client " << users.back() << std::endl;

        response = read_client();

        std::cout << "Message from client " << users.back() << ": " << response << std::endl;

        n = client_write("I got your message");
    }

    //_exit(0);
}

void check_children()
{
    //holds the state returned by waitpid()
    pid_t state;

    //needed for waitpid()
    int status;

    //pid of the current child being checked
    int current_child;

    //for child in child_pids
    for(int child = 0; child < child_pids.size(); child++)
    {
        current_child =  child_pids[child];

        std::cout << "Checking child process " << current_child << "..." << std::endl;

        //get state of process
        state = waitpid(current_child, &status, WNOHANG);

        //if state is 0, child is alive
        if(state == 0)
        {
            std::cout << "Child process " << current_child << " is alive." << std::endl;
        }
        //else if state is -1, error has occurred
        else if(state == -1)
        {
            //print error
            std::cout << "ERROR in child process " << current_child << std::endl;
            error("Terminating.");
        }
        //else child exited
        else
        {
            std::cout << "Child process " << current_child << " is dead.  Removing..." << std::endl;
            //remove child from child_pids
            child_pids.erase(child_pids.begin() + child);

            std::cout << "Child process " << current_child << " removed." << std::endl;

            //decrement active_children
            active_children--;

            std::cout << current_child << " active child processes." << std::endl;
        }
    }


}

int main(int argc, char *argv[])
{
    //if too few/many arguments are provided
    if(argc != 2)
    {
        error("ERROR: No port number specified.  Correct command usage is: server <port>.");
    }

    //if argv[1] is not an integer
    if (strtol(argv[1], NULL, 0) == 0)
    {
        error("ERROR: Invalid port number specified.  Port must be an integer.  Correct command usage is: server <port>.");
    }

    //if argv[1] is <= 1024
    if(atoi(argv[1]) <= 1024)
    {
        error("ERROR: Ports numbered 1024 and below are reserved by the OS.");
    }

    //clilen stores the size of the address of the client. This is required for the accept system call.
    socklen_t clilen;

    //sockaddr_in is a structure containing an internet address
    /*
    struct sockaddr_in
    {
      short   sin_family; //must be AF_INET
      u_short sin_port;
      struct  in_addr sin_addr;
      char    sin_zero[8]; // Not used, must be zero
    };
    */
    //in_addr structure, contains only one field, a unsigned long called s_addr.

    //serv_addr will contain the address of the server, and cli_addr will contain the address of the client which connects to the server.
    struct sockaddr_in serv_addr, cli_addr;


    //create socket
    //it take three arguments - address domain, type of socket, protocol (zero allows the OS to choose thye appropriate protocols based on type of socket)
    std::cout << "Creating socket..." << std::endl;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }
    std::cout << "Socket created." << std::endl;

    //set server address buffer with zeros using bzero or memset
    //two arguments - pointer to buffer and sizeof buffer
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //atoi() function can be used to convert port number from a string of digits to an integer, if your input is in the form of a string.
    std::cout << "Assigning port " << atoi(argv[1]) << "..." << std::endl;
    portno = atoi(argv[1]);
    std::cout << "Assigned port: " << portno << std::endl;

    //contains a code for the address family
    std::cout << "Assigning address family..." << std::endl;
    serv_addr.sin_family = AF_INET;
    std::cout << "Address family assigned." << std::endl;

    //contains the IP address of the host
    std::cout << "Storing IP address..." << std::endl;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    std::cout << "IP address stored." << std::endl;

    //contain the port number
    std::cout << "Storing port number..." << std::endl;
    serv_addr.sin_port = htons(portno);
    std::cout << "Port number stored." << std::endl;

    //bind() system call binds a socket to an address, in this case the address of the current host and port number on which the server will run.
    //three arguments, the socket file descriptor, the address to which is bound, and the size of the address to which it is bound.
    std::cout << "Binding socket..." << std::endl;
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }
    std::cout << "Socket bound." << std::endl;

    while(1)
    {
        //get state of child processes
        std::cout << "Checking state of child processes..." << std::endl;
        check_children();

        //listen system call allows the process to listen on the socket for connections.
        //The first argument is the socket file descriptor, and second is number of connections that can be waiting while the process is handling a particular connection.
        std::cout << "Listening for connections..." << std::endl;
        listen(sockfd,5);

        std::cout << "Connection detected." << std::endl;
        clilen = sizeof(cli_addr);

        //accept() system call causes the process to block until a client connects to the server.
        std::cout << "Accepting connection..." << std::endl;
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);

        if (newsockfd < 0)
        {
            error("ERROR on accept");
        }

        std::cout << "Connection accepted." << std::endl;


        std::cout << "Acquiring client number..." << std::endl;

        //READ/WRITE 1
        //read client number from client
        std::string client_data = read_client();

        std::cout << "Incoming client: " << client_data << std::endl;


        std::cout << "Checking client count..." << std::endl;

        //Check child count
        //if child count is >= 5, reject connection
        if(active_children >= 5)
        {
            std::cout << "Maximum clients reached.  Rejecting new client." << std::endl;

            /*
             * //Writes removed due to unexplained zerg-rush of socket writes
            //READ/WRITE 2
            //reject connection with close(client_socket)
            n = write(newsockfd,"Max active clients reached.",18);

            if (n < 0)
            {
                error("ERROR writing to socket");
            }
            */
            //READ/WRITE 3
            close(newsockfd);

            std::cout << "Incoming socket closed." << std::endl;

        }
        //else notify and continue
        else
        {
            std::cout << "Maximum clients not reached.  Continuing." << std::endl;

            //READ/WRITE 2
            /*
             * //Wites removed due to unexplained zerg-rush
            n = write(newsockfd,"There's room for you",18);

            if (n < 0)
            {
                error("ERROR writing to socket");
            }

            //READ/WRITE 3
            n = write(newsockfd,"Checking your client ID...",18);

            if (n < 0)
            {
                error("ERROR writing to socket");
            }
             */

            std::cout << "Checking if client " << client_data << " is registered." << std::endl;

            //If client is in client_id list, reject them and close the connection
            if(std::find(users.begin(), users.end(), client_data) != users.end())
            {
                std::cout << "Client " << client_data << " is already registered." << std::endl;

                /*
                //READ/WRITE 4
                //Writes removed due to unexplained zerg-rush
                //reject connection with close(client_socket)
                n = write(newsockfd,"Client ID already online",18);

                if (n < 0)
                {
                    error("ERROR writing to socket");
                }
                 */

                //READ/WRITE 5
                close(newsockfd);

                std::cout << "Incoming socket closed." << std::endl;
            }
            //else, add them to the list and fork
            else
            {
                //add user to clients
                users.push_back(client_data);
                std::cout << "Client " << users.back() << " now registered." << std::endl;

                /*
                 * //Writes removed due to unexplained zerg-rush
                //READ/WRITE 4
                n = write(newsockfd,"You have been registered.",18);

                if (n < 0)
                {
                    error("ERROR writing to socket");
                }
                 */


                pid = fork(); //Program spawns child process here
                //fork() returns the process id (pid) of the child process to the parent and 0 to the child.

                if (pid == 0) //if we are in the child process <-- child execution starts here
                {
                    //Handle requests from client
                    std::cout << "Forked to handle client " << users.back() << "." << std::endl;

                    //READ/WRITE 5
                    n = client_write("Ready for requests.");

                    client_service();
                }
                //else add pid to child_pids
                else
                {
                    child_pids.push_back(pid);
                    //increment number of child processes
                    active_children++;
                }
            }
        }
    }

    return 0;
}


