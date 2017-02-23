#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "helper.h"
using namespace std;

/* encode length of string into 4 bytes buff */
void pushTo(char *msg, int i) {
    *msg++ = i>>24;
    *msg++ = i>>16;
    *msg++ = i>>8;
    *msg++ = i;
}

/* decode length of string from 4 bytes buff */
int pullFrom(char *msg) {
    unsigned int unsigned_int = ((unsigned int) msg[0]<<24)
                                    | ((unsigned int)msg[1]<<16)
                                    | ((unsigned int)msg[2]<<8)
                                    | msg[3];

    int i;

    if (unsigned_int <= 0x7fffffffu) i = unsigned_int;
    else i = -1 - (int)(0xffffffffu - unsigned_int);
    return i;
}

/* receive massage */
bool receive(int socket, string &msg) {
    char msgSizeBytes[4];
    int retval;

    /* get length of string */
    retval = recv(socket, &msgSizeBytes, 4, 0);
    if (retval <= 0) {
        return false;
    }
    int msgSize = pullFrom(msgSizeBytes);
    //cout << msgSize << endl;
    char context[msgSize];
    memset(context, 0, sizeof(context));

    int accBytes = 0;
    int recvBytes = msgSize;

    /* receiving */
    //cout << "receiving" << endl;
    while (recvBytes > 0) {
        retval = recv(socket, &context[accBytes], recvBytes, 0);
        //cout << retval << endl;
        if (retval <= 0) {
            return false;
        }
        accBytes += retval;
        recvBytes -= retval;
    }
    /* receieved */
    //cout << "receieved" << endl;
    msg.assign(context, msgSize-1);
    return true;
}

/* send message */
void sendTo(int socket, char *msg) {
    /* set up response message */
    int msgSize = strlen(msg) + 1;
    int responseSize = msgSize + 4;
    char *response = new char[responseSize];
    pushTo(response, msgSize);
    strcpy(response + 4, msg);

    /* sending */
    int accBytes = 0;
    int sendBytes = responseSize;
    int retval;
    while (sendBytes > 0) {
        retval = send(socket, &response[accBytes], sendBytes, 0);
        if (retval < 0) break;
        accBytes += retval;
        sendBytes -= retval;
    }
    /* sent */
    delete response;
}
