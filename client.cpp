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
#include <queue>
#include <netdb.h>
#include <pthread.h>
#include "helper.h"

using namespace std;

queue<string> msgs;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *inThread(void *args) {
	string input;
	while (!cin.eof() && getline(cin, input)) {
		pthread_mutex_lock(&mutex);
		msgs.push(input);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void *outThread(void *socket) {
	string msg;
	long clientSocket = (long) socket;
	while (!cin.eof()) {
		while (msgs.size() > 0) {
			msg = msgs.front();
			pthread_mutex_lock(&mutex);
			msgs.pop();
			pthread_mutex_unlock(&mutex);
			sendTo(clientSocket, (char *) msg.c_str());
			receive(clientSocket, msg);
			cout << "Server: " << msg << endl;
			sleep(2);
		}
	}
	close(clientSocket);
	return NULL;
}

int main(int argc, char *argv[]) {
	long clientSocket;
	int port, retval;
	struct hostent *server;
	struct sockaddr_in client;
	pthread_t in_thread, out_thread;

	/* make a client socket */
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	assert(clientSocket >= 0);

	/* get server info */
	char *serverAddr = getenv("SERVER_ADDRESS");
	char *portStr = getenv("SERVER_PORT");
	assert(serverAddr != NULL);
	assert(portStr != NULL);
	port = atoi(portStr);
	server = gethostbyname(serverAddr);
	assert(server != NULL);

	/* init socket and connect */
    memset(&client, 0, sizeof(client)); // zero out
    client.sin_family = AF_INET; // the address family
    memcpy(&client.sin_addr.s_addr, server->h_addr, server->h_length);
    client.sin_port = htons(port);
    retval = connect(clientSocket, (struct sockaddr *) &client, sizeof(client));
    assert(retval >= 0);

    /* thread to get input */
    pthread_create(&in_thread, NULL, inThread, NULL);

    /* thread to send message */
    pthread_create(&out_thread, NULL, outThread, (void *)(long) clientSocket);

    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    return 1;
}
