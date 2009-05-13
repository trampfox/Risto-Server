#include "users.h"



/*
t_player *new_user(){
  return malloc(sizeof(t_player));
}


int set_user(t_player** l, char nickname[], int id){
  t_player* user;

  user=new_user();
  user->next=NULL;
  strcpy(user->nickname,nickname);
  user->id=id;
  
  if (*l==NULL){
    *l=user;
  }
  else{
    user->next=*l;
    *l=user;
  }  
}
*/

int preset_user(t_player l[],int id, int tot){
  int i=1;
  while ((l[i].state!=AWAY)&&(i<=tot))i++;
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


int user_to_string (t_player u, char ret[]){
  struct hostent *he;
  memset(ret,0,sizeof(ret));
  he=gethostbyaddr(&(u.myaddr.sin_addr), sizeof(u.myaddr.sin_addr),AF_INET);
  sprintf(ret,"id:%d, nickname: %s, addr: %s, porta: %d, stato:%d",u.id,u.nickname,he->h_name,ntohs(u.myaddr.sin_port),u.state);
  return 1;
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
