"""
programmer name: Haewon Cho

program name: file transfer client

program description: It starts on host B with host A's hostname and port number and command. 
		     It sends command to get list of file or files. 
		     When the file is available or list is requested, new connections is opened with new port number.
		     Server sends files or list through the new connection. 
		     When the tranfer is done, it closes the connection. 

course name: CS 372 Intro to Computer Networks

last modified: March 8, 2020
"""




#!/bin/python


from socket import *
import sys
import os












"""
Parameter: serverName, serverPort, cmd

return clientSocket

This function creates client socket, and connect to the server with given serverName and serverPort
Then it sends the command by user to server.
Then it returns the created client socket. 
"""
def initiateContact(serverName, serverPort,cmd):
	clientSocket = socket(AF_INET, SOCK_STREAM)				#create tcp socket
	clientSocket.connect((serverName, int(serverPort)))			#connect to the server
	clientSocket.send(cmd)							#initial contact
	return clientSocket














"""
Parameter: clientSocket, host_name, serverPort2

This function sends host_name of the client and new serverport to create new connection
"""
def requestList(clientSocket, host_name, serverPort2):
	clientSocket.recv(1024)							#get answer that server got the command
	clientSocket.send(serverPort2)						#send serverPort
        clientSocket.recv(1024)							#get answer that server got the port numver
        clientSocket.send(host_name)						#send host_name of client
        clientSocket.recv(1024)












"""
Parameter: clientSocket, host_name, serverPort2, reqFile

This function checks if the required file already exists on client side.
If the file exists, it asks user if user wants to overwrite the file
If user does not want to, then just exit 
If user does want to overwrite the function, it sends the file name to server
If server doesnt have the file, then exit
If server has the file, then it sends host_name and serverPort to create new connection
"""
def requestFile(clientSocket, host_name, serverPort2, reqFile):
	clientSocket.recv(1024)							
	isExist = os.path.exists(reqFile)					#check if requested file is on client side
        if isExist:								#if there is,
        	print "There is already " + reqFile
		print "Do you want to overwrite the file?"
		userInput = raw_input("Y/N : ")					#ask user
		userInput = userInput.upper()

		while 1:
			if userInput == "Y" or userInput == "N":		#until the answer is y/n
				break
			else:
				userInput = raw_input("Enter Y or N : ")
				userInput = userInput.upper()
 
		if userInput == "N":						#if user doesnt' want to overwrite the file,
			clientSocket.send("N")
			print "client does not want to overwrite the file"
			exit()							#exit
			 
	


	clientSocket.send(reqFile)	
	if clientSocket.recv(1024) == "nofile":					#if there is no requested file on server side,
		print "there is no such file"
		clientSocket.close()
		exit()								#exit
	clientSocket.send(serverPort2)						#if there is, send server port number and host name of client
	clientSocket.recv(1024)
	clientSocket.send(host_name)
        clientSocket.recv(1024)













"""
Parameter: serverPort2

return: connectionSocket

To create new connection, it creates new socket, then accept the connection from server
Then return the created new connection Socket for data transferring
"""
def getConnSocket(serverPort2):
	serverSocket = socket(AF_INET, SOCK_STREAM)				#create tcp socket
        serverSocket.bind(('', int(serverPort2)))
        serverSocket.listen(1)							#it begins listening for incoming tcp request
        connectionSocket, addr = serverSocket.accept()				#it waits on accept() for incoming requests, new socket created on return
	return connectionSocket












"""
Parameter: connectionSocket

This function get file list from server
When it gets @@@@@, it knows that is the file list transferring is done.
It does not print @@@@@
"""
def recvList(connectionSocket):
	
	print "getting list\n"
	fileName = connectionSocket.recv(500)					#recv first file name
	
	while 1:
		if "@@@@@" in fileName:						#if the file name is @@@@@
			break							#then break the loop
		print fileName							#if the file name is not @@@@@ then print the name
		fileName = connectionSocket.recv(500)				#get next file name










"""
Parameter: connectionSocket, reqFile

This function gets file from server and save the file 
it recv until the text includes @@@@@
if the received text includes @@@@@, it writes the text on the file excpet @@@@@
"""
def recvFile(connectionSocket, reqFile):
	print "getting file\n"

	fd = open(reqFile, "w")							#open the new file named the requested file
	
	bufferText = connectionSocket.recv(1000)				#get first file buffer from server
	while 1:
		if "@@@@@" in bufferText:					#if the buffer includes @@@@@
			bufferText = bufferText[:-5]				#then write the buffer on the file except @@@@@
			fd.write(bufferText)		
			break							#because that was the last buffer, break the loop
		fd.write(bufferText)
		bufferText = connectionSocket.recv(1000)			#if not @@@@@, then recv more buffer












if __name__ == "__main__":
	
	if len(sys.argv) < 5 or len(sys.argv) > 6:				
		print "Wrong Arguments.."
	
	host_name = gethostname() 						#get host_name such as flip1, flip2... of client 
	
	serverName = sys.argv[1]+".engr.oregonstate.edu"			
	serverPort = sys.argv[2]
	cmd = sys.argv[3]		

	clientSocket = initiateContact(serverName, serverPort, cmd)		#get the clientSocket 
	
	if cmd == "-l":								#when user wants to get the list
		print "list requested"
		serverPort2 = sys.argv[4]
		requestList(clientSocket, host_name, serverPort2)
		connectionSocket = getConnSocket(serverPort2)
		
		print connectionSocket.recv(1024)				#get message that saying "second connection is opened"
		connectionSocket.send("OK")
		recvList(connectionSocket)					#get list
		connectionSocket.close()					#close the data transferring connection
		print "\ndone with getting list"
	elif cmd == "-g":							#when user wants to get the file
		print "file requested"
		reqFile = sys.argv[4]

		serverPort2 = sys.argv[5]
		requestFile(clientSocket, host_name, serverPort2, reqFile)
		connectionSocket = getConnSocket(serverPort2)
		
		print connectionSocket.recv(1024)				#get message for the second connectio opened
		connectionSocket.send("OK")
		recvFile(connectionSocket, reqFile)				#get file
		connectionSocket.close()					#close the data transferring connection
		print "done with getting requested file"
	clientSocket.close()							#close clientsocket
