#define AWAY 16
#define BUSY 32
#define FREE 64


typedef struct u{
  int id;		//id corrisponde al socket descriptor assegnatogli dal server
  int state;		//sato dell'utente: BUSY FREE (AWAY?)
  int opp;
  char nickname[MAXLINE];
}t_user;


int addUser(char *, t_user *, int);
