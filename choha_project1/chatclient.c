/*
programmer name: Haewon Cho

program name: chat client

program description: It starts on host B with host A's hostname and port number.
		     It gets user handle from user then send initial message with prompt.
		     By it, it connects with server. 
		     It can chat with server.
		     When it gets \quit by user, it ends connection.

course name: CS 372 Intro to Computer Networks

last modified: Feb 9, 2020
*/










#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int isQuit = 0;



/*
 * codes from beej's guide
 *
 * parameter: result struct addrinfo
 * return: socket descriptor
 *
 * This function calls all the socket related functions...making socket, connecting to remote host 
 */

int socketFuncs(struct addrinfo *servinfo)
{
	int sock;
	
	if((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol))<0)		//making socket and check if socket descriptor is greater than 0
	{
		fprintf(stderr, "socket error\n");

		exit(1);
	}

	if(connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) <0)						//connecting with remost host and check if it is connected
	{
		fprintf(stderr, "connect error\n");
		exit(1);
	}
	
	return sock;												//return socket descriptor
		
}






/*
 * parameter: socket descriptor, client handle
 * 
 * this function initiates the connection by sending its client handle saying hi to server.
 * then prints the line on client side either. 
 */

void initContact(int sock, char* clientHandle)
{
	
	char arrow[10];
	char sayHi[30];
	char output[50];
	
	strcpy(arrow, "> ");											//prompt line
	strcpy(sayHi, "Hi! The handle name is ");
	strcat(output, clientHandle);
	strcat(output, arrow);
	strcat(output, sayHi);											//saying hi
	strcat(output, clientHandle);

	int sendStatus = send(sock, output, strlen(output), 0);							//send the result line to server
	printf("%s\n", output);											//print the result line

}









/*
 * some codes from cs344 OPT HW
 *
 * parameter: socket descriptor, client handle, message to receive
 *
 * this function receive messages from server side
 * if the received message's length is 0, then isQuit variable is 1 to break the loop
 */

void recvMessage(int sock, char* clientHandle, char* message)
{
	
	recv(sock, message, 508, 0);										//receive message
	if(strlen(message) == 0)										//if the length of strng is 0 then let user know
	{													//the server ended connection
		printf("server ended connection\n");
		isQuit = 1;
	}
	else
	{
		printf("%s\n", message);									//else print the received message
	}
}







/*
 * some codes from cs 344 OPT HW
 *
 * parameter: socket descriptor, client handle, message to send
 *
 * the function send message to server side
 * if the user types \quit, then isQuit is 1 to break the loop
 */
void sendMessage(int sock, char* clientHandle, char* message)
{

	
	printf("%s> ", clientHandle);										//print prompt line
	fflush(stdout);
	scanf("%[^\n]%*c", message);										//get message from user
	if(strcmp(message, "\\quit")==0)									//compare input message from user to \quit
	{
		isQuit = 1;											//if its \quit, then isQuit to 1
		printf("closing the connection\n");
	}
	else
	{
		send(sock, message, strlen(message), 0);							//else send the message to server
	}
		
	
}







/*
 * parameter: socket descriptor, client handle
 *
 * this keep sending and reciveing message back and forth using while loop
 * if the while loop is broken, close the connection
 */

void chat(int sock, char* clientHandle)
{
	char sending[503];
	char recving[508];
	fgets(sending, 500, stdin);
	while(1)
	{
		memset(sending, '\0', sizeof(sending));								//make sure the sending buffer is empty
		memset(recving, '\0', sizeof(recving));								//make sure the receiving buffer is empty
		recvMessage(sock, clientHandle, recving);							//receive message from server
		if(isQuit == 1)											//if isQuit is 1 then break the loop 
		{
			break;
		}
		sendMessage(sock, clientHandle, sending);							//send message to server
		if(isQuit == 1)											//if isQuit is 1 then break the loop
		{
			break;
		}
	}

	close(sock);												//then close connection
		
}





int main(int argc, char *argv[])
{
	if(argc != 3)												//if the arguments are not 3 then exit
	{
		fprintf(stderr,"You need two arguments\n");
		exit(0);
	}

	
	char clientHandle[10];
	
	printf("Enter your handle: ");										//get client handle from user
	//fflush(stdout);
	scanf("%s", clientHandle);

	
	//from beej's guide	
	int sock;
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;										//result addrinfo
	char serverHandle[10];	

	memset(&hints, 0, sizeof(hints));									//make sure the struct is empty
	hints.ai_family = AF_INET;										//IP v4
	hints.ai_socktype = SOCK_STREAM;									//TCP stream socket
	//hints.ai_flags= AI_PASSIVE;

	if((status= getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)					//argv[1]= hostname, argv[2] = port number
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));				//servinfo points to linked list of hints addrinfo
		exit(1); 											//check if the function worked if not, exit
	}
	
	
	sock = socketFuncs(servinfo);										//get socket desctiptor by calling socketFunc
	initContact(sock, clientHandle);									//initiate connection by calling initContact
	chat(sock, clientHandle);										//chat until the while is broken
	
	//from beej's guide
	freeaddrinfo(servinfo);											//free the linked list

	return 0;

}
