#include "lib/basic.h"
#include "users.h"

void userToString(t_user user, int id, char *ret)
{
	printf("userToString id: %d, nick: %s", id, user.nickname);
	memset(ret,0,sizeof(ret));
	sprintf(ret,"|%d - %s|", id, user.nickname);
	printf("userToString ret: %s", ret);
}

/* Server function */

int addUser(char *nick, t_user *utenti, int id)
{	
	int i,r;
	i = 0;
	/* control if there's another user with the same nick */
	for (i=0; i < FD_SETSIZE; i++)
	{
		if (strcmp(utenti[i].nickname,nick) == 0)
			return (-1);
	}
	strcpy( utenti[id].nickname, nick );
	utenti[id].state = FREE;
	utenti[id].id = id;	
	return 0;		
}	

/* prepare the list of users that are authenticated on the server 
** utenti: list of users
** id_nick: list of available users */
void prepareList (t_user *utenti, char *list[], int sockfd)
{
	int i,j;
	int flag = 0; /* if 0 there's not a free users */
	j=0;
	char *nick;
	char message[MAXLINE], buff[MAXLINE];
	memset(message, '\0', MAXLINE);
	memset(buff, '\0', MAXLINE);
	snprintf(message, sizeof(message), "RLIS ");
	for (i=0; i < FD_SETSIZE ;i++) 
	{
		/* show only free users */
		if (utenti[i].state == FREE){
			printf("prepareList id: %d, nick: %s, state: %d\n", i, utenti[i].nickname, utenti[i].state);
			sprintf(buff, "|%d-%s|\r\n", i, utenti[i].nickname);
			strcat(message, buff);
			memset(buff, '\0', MAXLINE);
			flag = 1;
		}
	}
	if (flag == 0) {
		sprintf(buff, "NULL NULL NULL\r\n", i, utenti[i].nickname);
		strcat(message, buff);
	}
	printf("prepareList message: %sCall Write\n", message);
	/* send list */
	Write(sockfd, message, strlen(message));
	memset(message, '\0', MAXLINE);
}

/* if the requested user exixts it sends a USOK message, ERRR message if not exixts 
** players: list of registered users
** sockfd: client socket file descriptor
** id: user id */
int controlUser(t_user *players, int sockfd, char *id)
{
	int userId;
	char message[MAXLINE], err_msg[16];
	memset(message, '\0', MAXLINE);
	userId = atoi(id);
	/* user requests play against himself */
	if (userId == sockfd) {
		snprintf(message, sizeof(message), "ERRR 330 NULL NULL\r\n");
		printf("controlUser - invalid user id: %d\n", userId);
		Write(sockfd, message, strlen(message));
	}
	else if (players[userId].state == FREE) {
		/*Chiede all'avversario se vuole giocare con questo utente ...*/
		//snprintf(message, sizeof(message), "USOK NULL NULL NULL\r\n");
		snprintf(message, sizeof(message), "RUSR %d NULL NULL\r\n", sockfd);
		printf("controlUser - valid user id: %d\n", userId);
		/* update user's state */
		//players[userId].state = BUSY;
		//players[sockfd].state = BUSY;
		Write(userId, message, strlen(message));
	}
	/* request user id doesn't exixt */
	else {
		snprintf(message, sizeof(message), "ERRR 550 NULL NULL\r\n");
		printf("controlUser - invalid user id: %d\n", userId);
		Write(sockfd, message, strlen(message));
	}
}

int starterUser(int a, int b)
{
	int v [2];
	int s = (a+b)*17;
	v[0] = a;
	v[1] = b;
	srand(s);
	s=rand()%2;
	return v[s];
}



/* passo ai relativi client il loro avversario */ 
int userConfirm (t_user *players, int sockfd, int me)
{
	char message [MAXLINE];
	char message1 [MAXLINE];
	int k;
	players[me].state = BUSY;
	players[sockfd].state = BUSY;
	players[me].opp = sockfd;
	players[sockfd].opp = me;
	if ( (k = starterUser( sockfd, me ) ) == me )
	{
		
		snprintf(message, sizeof(message),   "USOK 110 %d NULL\r\n",me);
		snprintf(message1, sizeof(message1), "USOK 220 %d NULL\r\n",sockfd);
		Write(me, message, strlen(message));
		Write(sockfd, message1, strlen(message1));
	}
	else if ( k == sockfd )
	{
		
		snprintf(message, sizeof(message),   "USOK 110 %d NULL\r\n",sockfd);
		snprintf(message1, sizeof(message1), "USOK 220 %d NULL\r\n",me);
		Write(sockfd, message, strlen(message));
		Write(me, message1, strlen(message1));
	}
}



/*
int preset_user(t_player l[],int id, int tot){
  int i=1;
  while ((l[i].state!=AWAY)utenti[i].nickname,nick)&&(i<=tot))i++;
  if (l[i].state==AWAY)
    return i;
  else 
    return id;
}

int set_user(t_player l[], char nickname[], int id,struct sockaddr_in addr, int state){
  if (id<MAX_USERS_IN_SERVER){
    strcpy(l[id].nickname,nickname);
    l[id].id=id;
    memcpy(&(l[id].myaddr),&addr,sizeof(addr));
    l[id].state=state;
    l[id].action.state=WAIT;
    return 0;
  }
  else 
    return -1;
}

void delete_user (t_player l[], int id){ 
  l[id].state=AWAY;
}



void set_busy(t_player* u, int avversario){
  u->state=BUSY;
  u->id_opp=avversario;
}


void print_users(t_player l[],int tot){
  int i;
  char utente[MAXLINE];
  for (i=1;i<=tot;i++){
    user_to_string(l[i],utente);
    printf("%s\n",utente);
  }
}

int select_opponent(t_player l[], int id, int tot){
  int i;
  int semid;
  semid = semget(IPC_PRIVATE,1,0600);
  if (semid == -1) perror("Creazione semaforo");
  seminit(semid,0,1);

  //while((l[i].state==BUSY)&&(i<MAX_USERS_IN_SERVER)){i++;}
  for (i=1;i<=tot;i++){
    if ((l[i].state==FREE)&&(i!=id)){
      up(semid,0);
      set_busy(&l[i],id);
      set_busy(&l[id],i);
      l[id].to_do=WUP;
      l[i].to_do=WAIT;
      down(semid,0);
      return i;
   }
  }
  if (semctl(semid,0,IPC_RMID)==-1) perror("Rimozione semaforo");
  return 0;
}


/*
int nick2id(t_player* l, char* nick){
  t_player *t=l;
  while (t){
    if (strcmp(t->nickname,nick)==0)
      return t->id;
    t=t->next;
  }
  return -1;
}
*/
/*
int id2nick(t_player* l, int id, char* out){
  t_player *t=l;
  while (t){
    if (t->id==id){
      strcpy(out,t->nickname);
      return 1;
    }
    t=t->next;
  }
  return -1;
}
*/
