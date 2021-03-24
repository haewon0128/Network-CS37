"""
Programmer Name: Haewon Cho

Program Name: chat server

Program Description: It starts on host A then waits on a port that user gave for client.
		     It connects with host B which is on client side.
		     Then it sends and receives messages with client.
		     When the connection between client and server ends, 
		     it goes back to status to get new connection.
		     It can end the connection by typing \quit.

Course Name: CS 372 Intro to Computer Networks

Last Modified: Feb 09, 2020

** all codes are from the lecture note 15 on canvas 
"""







#!/bin/python


from socket import *
import sys



"""
parameters: connection socket, client handle
return: int isQuit
 
This fuction recieve messages from client.
isQuit variable checks if the client ended the connection.
"""
def recvMessage(connectionSocket, clientHandle):
	isQuit = 0					   		#to check if client side ended connection			
	message = ""							
	message = connectionSocket.recv(500)				#receive message 
	if message == "":						#if the received message is nothing
		print 'client ended connection'				#isQuit is 1 and print to let user know that client ended connection
		isQuit = 1
	else:
		print '{}> {}'.format(clientHandle, message)		#print received message
	return isQuit










"""
parameters: connection socket, server handle
return: string message 

This function send messages to client
If the message to send is '\quit' then it just return the message not to send the message 
"""
def sendMessage(connectionSocket, serverHandle):
	message = ""
	message = raw_input("{}> ".format(serverHandle))[:500]
	arrow = "Haewon> "						#user handle
	result = ""
	if message == "\quit":						#if user type \quit then close connection
		print 'closing the connection'

	else:
		result = arrow + message				#sending message is handle + typed message
		connectionSocket.send(result)				#send message
	return message








"""
parameters: connection socket, server handle, client handle

This function calls recvMessage and sendMessage functions using connection socket and handles.
If the sending message is '\quit', then break the while loop.
If the recieved message is nothing, then break the while loop.
"""
def chat(connectionSocket, serverHandle, clientHandle):
	while 1:
		message = ""
		message = sendMessage(connectionSocket, serverHandle)		#call sendMessage to send message
		
		if message == "\quit":						#if the sending message is \quit then break the loop
			break

		isQuit = recvMessage(connectionSocket, clientHandle)		#call recvMessage to recieve message
		if isQuit == 1:							#if the received message was nothing
			break							#then break the loop







if __name__ == "__main__":

	serverPort = sys.argv[1]
	serverSocket = socket(AF_INET, SOCK_STREAM)				#create TCP socket
	serverSocket.bind(('', int(serverPort)))
	serverSocket.listen(1)							#server begins listening for incoming TCP request

	serverHandle = "Haewon"
	clientHandle = ""
	while 1:								#until get the SIGINT, keeps listening
		isquit = 0
		print 'The Server is ready to receive'				
		connectionSocket, addr = serverSocket.accept()			#server waits on accept() for incoming requests, new socket created on return
		
		clientHandle = connectionSocket.recv(1024)			#get initial contact
		print clientHandle
		clientHandle = clientHandle[0: clientHandle.find(">")]		#get client handle
		chat(connectionSocket, serverHandle, clientHandle)		#call chat function to chat with client back and forth
		connectionSocket.close()					#close connection to this client


