#ifndef COM
#define COM
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>

#define LIST 2
#define INIT 5
#define _2ONE 3
#define _2ALL 4
#define STOP 1
#define MAX_MESSAGE_LEN 512
#define MAX_CLIENTS 10
#define SERVER_CONST 2137
struct Mtext
{
    int idTo;
    int idFrom;
    int client_id;
    char msg[MAX_MESSAGE_LEN];
};

struct MsgBuf
{
    long mtype;
    struct Mtext mtext;
};
const size_t MSG_BUF_SIZE = sizeof(struct MsgBuf) - sizeof(long);
#endif