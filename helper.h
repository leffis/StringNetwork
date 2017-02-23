#ifndef _COMMON_H_
#define _COMMON_H_
#include <string>

/* generic methods for pushing length to 4 bytes buff */
void pushTo(char *msg, int i);

/* generic methods for pulling length from 4 bytes buff */
int pullFrom(char *msg);

/* receive massage */
bool receive(int socket, std::string &msg);

/* send message */
void sendTo(int socket, char *msg);

#endif
