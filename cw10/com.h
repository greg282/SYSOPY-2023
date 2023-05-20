#ifndef COM
#define COM
#include <string.h>
#define LIST 2
#define INIT 5
#define _2ONE 3
#define _2ALL 4
#define STOP 1
#define PING 6
#define MAX_MESSAGE_LEN 512
#define MAX_CLIENTS 10
#define SERVER_CONST 2137
struct Message
{
    int idTo;
    int idFrom;
    int client_id;
    int command_type;
    char username[256];
    char msg[MAX_MESSAGE_LEN];
};
#endif