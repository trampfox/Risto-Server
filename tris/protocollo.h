#include "basic.h"

#define PACK_LEN    5
#define MAXPARAM   10


#define HELP        -1
#define SHOOT        7
#define VICTORY      2
#define LOSE         3
#define HELLO        4
#define WAIT         5
#define WUP          6
#define RST	     8

typedef struct mex{
  char message[MAXLINE];
  int type;
}msg;

typedef struct _PROTO_{
  int type;
  int x_coord;
  int y_coord;
  char nick[MAXLINE];
  int id_opp;

  msg message;
}t_proto;
