#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "helper.h"

using namespace std;

/* make string title case */
void titleCase(string &str) {
    if (str.empty()) return;
    //cout << str.length() << endl;
    for (unsigned int i = 0; i < str.length(); ++i)
    {
        str[i] = tolower(str[i]);
    }
    //cout << "here" << endl;
    for (unsigned int i = 0; i < str.length(); ++i)
    {
        if (isalpha(str[i])) str[i] = toupper(str[i]);
        //cout << "debug " << str[i] << endl;
        while(i < str.length() && !isspace(str[i])) i++;
    }
    //cout << "finish" << endl;
}

/* Handle requests from clients */
void handle_requests(int serverSocket, int &highestFds, fd_set &masterFds) {
	int clientSocket;
    struct sockaddr_in clientAddr;
	socklen_t clientSize = sizeof(clientAddr);
    fd_set activeFds;
	memcpy(&activeFds, &masterFds, sizeof(masterFds));
	int retval = select(highestFds + 1, &activeFds, NULL, NULL, NULL);
	assert(retval > 0);
	string msg;

    //cout << "/* entering select process */" << endl;
	for (int i = 0; i <= highestFds; ++i)
	{
        //cout << serverSocket << " " << i << endl;
		if (!FD_ISSET(i, &activeFds)) {
            //cout << i << endl;
            continue;
        }
        else if (i == serverSocket) {
            //cout << "here" << endl;
        	clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, &clientSize);
        	assert(clientSocket >= 0);
            //cout << "client: " << clientSocket << endl;
        	FD_SET(clientSocket, &masterFds);
        	if (clientSocket > highestFds) highestFds = clientSocket;
        }
        else {
            /* client fds */
            //cout << "receive" << endl;
            clientSocket = i;
        	if (receive(clientSocket, msg)) {
                cout << msg << endl;
                titleCase(msg);
            } else {
                FD_CLR(clientSocket, &masterFds);
                close(clientSocket);
                continue;
            }
            //cout << "send" << endl;
            sendTo(clientSocket, (char *)msg.c_str());
        }
	}
}

int main(int argc, char **argv) {
	int serverSocket, retval;
	struct sockaddr_in serverAddr;

	/* make a server socket */
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	assert(serverSocket >= 0);

	/* init socket */
    memset(&serverAddr, 0, sizeof(serverAddr)); // zero out
    serverAddr.sin_family = AF_INET; // the address family
    serverAddr.sin_addr.s_addr = INADDR_ANY; // set host IP address
    serverAddr.sin_port = 0; // random available port

    /* bind to host */
    retval = bind(serverSocket, (struct sockaddr *) & (serverAddr), sizeof(serverAddr));
	assert(retval >= 0);
	
	/* listen to the socket using listen(int, int) from sys/socket.h, maximum 5 connections */
	listen(serverSocket, 5);

	/* give out server info */
	char hostname[256];
	gethostname(hostname, 256);
	socklen_t serverSize = sizeof(serverAddr);
	retval = getsockname(serverSocket, (struct sockaddr*)&serverAddr, &serverSize);
	assert(retval != -1);
    cout << "SERVER_ADDRESS " << hostname << endl;
    cout << "SERVER_PORT " << ntohs(serverAddr.sin_port) << endl;

    /* accept clients and selecting */
    int highestFds = serverSocket;
    fd_set masterFds;
    FD_ZERO(&masterFds);
    FD_SET(serverSocket, &masterFds);
	while (true) {
		handle_requests(serverSocket, highestFds, masterFds);
	}

	/* should not reach */
	cerr << "server unexpected return." << endl;
	return -1;
}
