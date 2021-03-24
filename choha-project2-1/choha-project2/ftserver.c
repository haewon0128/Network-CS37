/*
programmer name: Haewon Cho

program name: file transfer server

program description: It starts on host A then waits on a port that user gave for client.
		     It connects with host B which is on client side.
		     It gets request for file list or files from client.
		     When client asks for list or a file that exists on server side, 
	   	     Server sends the list or file through a connection that host B started.
		     After sending the data, server close the new data transfer connection,
	 	     but it gets back to status to get new connection. 

course name: CS 372 Intro to Computer Networks

last modified: Mar 8, 2020
*/









#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>








/*
 * codes from beej's guide... reused project 1 codes
 *
 * parameter: hostname, port
 *
 * return: socket decriptor
 *
 * this function creates new socket to creates new connect to client for data transfer connection
 * 
 */

int getClientSock(char* hostname, char* port)
{
	int sock;
        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo;
        memset(&hints, 0, sizeof(hints));                                                                       //make sure the struct is empty
        hints.ai_family = AF_INET;                                                                              //IP v4
        hints.ai_socktype = SOCK_STREAM;


        if((status= getaddrinfo(hostname, port, &hints, &servinfo)) != 0)                                       //argv[1]= hostname, argv[2] = port number
        {
                fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));                               //servinfo points to linked list of hints addrinfo
                exit(1);                                                                                        //check if the function worked if not, exit
        }


        if((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol))<0)                //making socket and check if socket descriptor is greater than 0
        {
                fprintf(stderr, "socket error\n");

                exit(1);
        }
        if(connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) <0)                                           //connecting with remost host and check if it is connected
        {
                fprintf(stderr, "connect error\n");
                exit(1);
        }

	return sock;	
}










/*
 * parameter: sock
 *
 * this function opens current directory then send names of regular files to client through new connection.
 * when it finishes sending, it sends @@@@@ to show all the names are sent to client.
 * then close the socket.
 *
 * referenct: https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 */

void sendDir(int sock)
{
	char fileName[500];
	DIR *d;
    	struct dirent *dir;
    	d = opendir(".");							//open cureent directory
	

    	if (d)
	{
		while ((dir = readdir(d)) != NULL)				//until there is something to read
        	{
			if (dir->d_type == DT_REG)				//if the file type is regurlar
			{
         			memset(fileName, '\0', sizeof(fileName));
				strcpy(fileName, dir->d_name);
				send(sock, fileName, sizeof(fileName), 0);	// send filename to client			  		
        		}
		}
        	closedir(d);
	}
	char * done = "@@@@@";
	send(sock, done, strlen(done), 0);					//last filename
	close(sock);
}












/*
 * parameter: connSock
 * 
 * this function gets new new port number and hostname of client to connect to client
 * when the connection is successful, then it sends the second conection is opened
 * then it calls sendDir, then let the client knows the list is sent. 
 */

void showList(int connSock)
{
	char OK[20] = "";
	char port[20] = "";
	char message[50] = "I got it";
	char hostname[100] = "";
	char secondconn[50] = "second connection is opened";
	int sock;

	send(connSock, message, sizeof(message), 0);
	recv(connSock, port, 20, 0);						//gets port number				
	send(connSock, message, sizeof(message), 0);
	recv(connSock, hostname, 100, 0);					//gets hostname like flip2.flip3
	send(connSock, message, sizeof(message), 0);
	
	sock = getClientSock(hostname, port);					//new connection
	send(sock, secondconn, sizeof(secondconn), 0);				//initiate contact

	fflush(stdout);
	recv(sock, OK, 20, 0);
	

	sendDir(sock);								//send list
	printf("Requested list is sent\n\n");					

}










/*
 * parameter: sock, filename
 *
 * this function sends buffer of text to client through data transfer connection
 * when the entire file sent, it sends @@@@@ to show the sending is done to client
 */

void sendLines(int sock, char * filename)
{
	int sent=0; 
	size_t textsize = 0;	
	int fd = open(filename,O_RDONLY); 
	char buffer[1000];
	int charsSent, charsRead;
	

	//memset(buffer, '\0', sizeof(buffer));
	printf("%s is requested\n", filename);
	 
	while(1)           	 
        {
		memset(buffer, '\0', sizeof(buffer));							//make sure empty the buffer
		charsRead = read(fd, buffer, sizeof(buffer));						//read file to buffer 
		if(charsRead ==0)									//if it reads 0 size then break the loop because reading is done
		{
			break;
		}
		else if (charsRead <0)									//if it reads less than 0, then exit because something wrong
		{
			printf("error: sending file1\n");
			close(sock);			
			exit(1);
		}
		charsSent = send(sock, buffer, strlen(buffer),0); 					//if sent char is less than 0, then error
		if(charsSent <0)
		{
			printf("error: sending file2\n");
                        close(sock);
                        exit(1);
		}


	}
	char * done = "@@@@@";										//send @@@@@ to let client know sending done
        send(sock, done, strlen(done), 0);
}









/*
 * parameter: connSock
 *
 * this function checks if the requested file exists in the current directory
 * if there is no file, then wait for new connection
 * if there is, it gets host name and port for new data connection
 * if client does not want to overwrite the file then wait for new connection
 */
void sendFile(int connSock)
{
	char * noFile = "nofile";
	char OK[20] = "";
        char port[20] = "";
        char message[50] = "I got it";
        char hostname[100] = "";
	char filename[100] = "";
        char secondconn[50] = "second connection is opened";
        int sock;

	send(connSock, message, sizeof(message), 0);	
	recv(connSock, filename, 100, 0);								//get the requested filename

	if(strcmp(filename, "N")==0)									//if the filename is N then it means client not overwriting the file
	{
		printf("Duplicated file name.. client does not want to overwrite\n\n");
		return;	
	}


	FILE *fptr = fopen(filename, "r");
        if (fptr == NULL)										//if there is no such file
        {
                printf("There is no such file %s\n\n", filename);
		send(connSock, noFile, strlen(noFile), 0); 
                return;
        }
        fclose(fptr);



	send(connSock, message, sizeof(message), 0);
	recv(connSock, port, 20, 0);									//there is file, then gets port and hostname
        send(connSock, message, sizeof(message), 0);
        recv(connSock, hostname, 100, 0);
        send(connSock, message, sizeof(message), 0);	



	sock = getClientSock(hostname, port);								//new data connection
	send(sock, secondconn, sizeof(secondconn), 0);							//initiate contact
	
	fflush(stdout);
	recv(sock, OK, 20, 0);
	//printf("%s\n", OK);
	
	sendLines(sock, filename);									//call sendLines to send the file
	
	printf("Requested file is sent\n\n");								//let client know sending done
	close(sock);
}




/*
 * parameter: connSock
 *
 * this function get the command,
 * if the command is -l which is list, then calls showlist
 * if the command is -g which is sending file, then call send file
 */

void handleRequest(int connSock)
{
	char cmd[20] = "";
	recv(connSock, cmd, 20, 0);
	printf("Command Received: %s\n", cmd);	
	if(strcmp(cmd, "-l")==0)
	{
		printf("List is requested\n");
		showList(connSock);
	}
	else if(strcmp(cmd, "-g")==0)
	{
		printf("File is requested\n");
		sendFile(connSock);
	}
	else
	{
		printf("what do you want...?\n");
	}
}






/*
 * parameter: sock
 *
 * this function accpet the connection from client
 * calls handleRequest to handle the command
 * then close the socket
 *
 * reference: https://medium.com/@yashitmaheshwary/simple-chat-server-using-sockets-in-c-f72fc8b5b24e
 */
void startUp(int sock)
{
	//char cmd[20] = "";
	int connSock;
	while(1)
	{
		printf("waiting for new connection\n");
		connSock = accept(sock, (struct sockaddr *)NULL, NULL);						//accept new connection with client
		if(connSock <0)
		{
			printf("connSock Error\n");
			exit(1);
		}
		handleRequest(connSock);	
		close(connSock);	
	}
	
}









int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		printf("Wrong number of arguments\n");
	}

	struct sockaddr_in serv; 
	int sock; 
	int connSock; 
	char message[100] = "";
	int serverPort = atoi(argv[1]);

	serv.sin_family = AF_INET;
	
	serv.sin_port = htons(serverPort);								
	serv.sin_addr.s_addr = INADDR_ANY;

	sock = socket(AF_INET, SOCK_STREAM, 0);								//tcp socket
	if(sock < 0)
	{
		printf("socket error\n");
		exit(1);
	}
	if(bind(sock, (struct sockaddr *)&serv, sizeof(serv)) <0)
	{
		printf("bind error: use other ports\n");
		exit(1);
	}
	listen(sock,5);											//begins listening for incoming tcp request 

	startUp(sock);	

	return 0;
}
