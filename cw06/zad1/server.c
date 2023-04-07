#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#define LIST 1
#define INIT 8
#define MAX_MESSAGE_LEN 512
#define MAX_CLIENTS 10 
#define _2ONE 2
#define _2ALL 3


key_t server_key;
int qidServer;
int clients[MAX_CLIENTS];

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

void sigint_handler(int signo){
    //TODO:
}
void clean()
{
    msgctl(qidServer, IPC_RMID, NULL);
}

void init_handler(struct MsgBuf received){
    int idClient=-1;
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i]==-1){
            idClient=i;
            break;
        }
    }

    if(idClient==-1){
        printf("Server is full\n");
        return;
    }
    else{
        clients[idClient]=received.mtext.idFrom;
        printf("Client %d connected,generated ID: %d\n",received.mtext.idFrom,idClient);
        struct MsgBuf response;
        response.mtype=INIT;
        response.mtext.idFrom=qidServer;
        response.mtext.client_id=idClient;
        if(msgsnd(received.mtext.idFrom,&response,MSG_BUF_SIZE,0)==-1){
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }
}

void list_handler(struct MsgBuf received){
    char msg[MAX_MESSAGE_LEN]="";
    // put all registered client id in msg
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i]!=-1){
            char tmp[32];
            sprintf(tmp,"ClientID: %d \n",i);
            strcat(msg,tmp);
        }
    }

    struct MsgBuf response;
    response.mtype=LIST;
    response.mtext.idFrom=qidServer;
    response.mtext.client_id=received.mtext.client_id;
    strcpy(response.mtext.msg,msg);
    if(msgsnd(received.mtext.idFrom,&response,MSG_BUF_SIZE,0)==-1){
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }else{
        printf("Sent online client list to client %d \n",received.mtext.client_id);
    }
}

void to_one_handler(struct MsgBuf received){
    //check if client is registered
    if(clients[received.mtext.idTo]==-1){
        printf("Client %d is not registered\n",received.mtext.client_id);
        return;
    }
 
    //send message to client
    struct MsgBuf response;
    response.mtype=_2ONE;
    response.mtext.idFrom=received.mtext.client_id;
    response.mtext.client_id=received.mtext.client_id;
    strcpy(response.mtext.msg,received.mtext.msg);
    if(msgsnd(clients[received.mtext.idTo],&response,MSG_BUF_SIZE,0)==-1){
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }else{
        printf("Sent message to client %d from client %d\n",received.mtext.idTo,received.mtext.client_id);
    }
}

void to_all_handler(struct MsgBuf received){
    //send message to all clients
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i]!=-1 && i!=received.mtext.client_id){
            struct MsgBuf response;
            response.mtype=_2ALL;
            response.mtext.idFrom=received.mtext.client_id;
            response.mtext.client_id=received.mtext.client_id;
            strcpy(response.mtext.msg,received.mtext.msg);
            if(msgsnd(clients[i],&response,MSG_BUF_SIZE,0)==-1){
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }else{
                printf("Sent message to client %d from client %d\n",i,received.mtext.client_id);
            }
        }
    }
}

int main (int argc, char **argv)
{   
    //atexit(clean);
    //set clients array vals to -1
    for(int i=0;i<MAX_CLIENTS;i++){
        clients[i]=-1;
    }

    setbuf(stdout, NULL);
    //setting up handler for SIGINT
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);

    server_key=ftok(getenv("HOME"),'s');
    if(server_key==-1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    qidServer=msgget(server_key,IPC_CREAT|0600);
    if(qidServer==-1){
        perror("Could not create server queue");
        exit(EXIT_FAILURE);
    }
    struct MsgBuf received;
    printf("%d\n",qidServer);

    while (1)
    {
        if (msgrcv(qidServer, &received, MSG_BUF_SIZE, 0, IPC_NOWAIT) == -1)
        {
           //print errno value
           if(errno!=ENOMSG){
            printf("CHUJ");
           }
           
        }else{
            printf("Received message from %d: %s\n", received.mtext.idFrom, received.mtext.msg);
            switch(received.mtype){
                case INIT:
                init_handler(received);
                break;
                case LIST:
                list_handler(received);
                break;
                case _2ONE:
                to_one_handler(received);
                break;
                case _2ALL:
                to_all_handler(received);
                break;
                default:
                break;
            }
        }
       
    }   
    
        
    
}