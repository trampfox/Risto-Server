#include "basic.h"
#include "semfun.h"
#include "semfun.c"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>


#define MAXPLAYERS 2
#define MAX_USERS_IN_SERVER 150 //esempio

#define AWAY 16
#define BUSY 32
#define FREE 64

typedef struct shoot{
  char packet[MAXLINE]; // pacchetto contenente o \0 oppure il pacchetto standad per un colpo.
  int state;            // WAIT | SHOOT defined in protocollo.h
};

typedef struct u{
  char nickname[MAXLINE];
  int id;                          //id del giocatore
  int id_opp;                      //id dell'avversario
  int state;                       //stato dell'avversario: AWAY | BUSY |FREE
  int to_do;                       //azione da intraprendere: WAIT | WAKE_UP
  struct sockaddr_in myaddr;       //proprio indirizzo
  struct shoot action;
}t_player;

