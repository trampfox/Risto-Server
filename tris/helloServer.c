#include "basic.h"
#include "protocollo.c"
#include "users.c"
#include "my_func.c"
#include "my_signal.h"

void busy_waiting(t_player l[], int id, int tot){
	int k;
	while((k=select_opponent(l,id,tot))==0){
		if (l[id].state==BUSY){
			printf("avversario di %d: %d\n",id,l[id].id_opp);	
			return;
		}
	      	printf("nessun utente disponibile per %d | stato %d\n",id,l[id].state);
	      	sleep(2);
	}
	printf("avversario di %d: %d\n",id,l[id].id_opp);		
}



int main(int argc, char **argv)
{

  struct hostent *he;
  
  int sockfd, connfd, clilen,n; //semid serve per creare un semaforo per la memoria condivisa
  char* buff;
  char send[MAXLINE];
  pid_t pid;

  t_player* user_q;

  int semid0 = semget(IPC_PRIVATE,1,0600);
  if (semid0 == -1) perror("Creazione semaforo");
  seminit(semid0,0,1);

  int shmid=shmget(IPC_PRIVATE,MAX_USERS_IN_SERVER*sizeof(t_player),0600);
  user_q=(t_player*)shmat(shmid,0,0); 

  int sh_tot_id=shmget(IPC_PRIVATE,sizeof(int),0600);
  //int *tot =(int*)shmat(sh_tot_id,0,0);

  int id=0;

  struct sockaddr_in servaddr, cliaddr;

  if(signal(SIGCHLD,gestisci_zombie)==SIG_ERR)
    err_sys("errore nella signal");

  if(signal(SIGINT,gestisci_ctrlc)==SIG_ERR)
    err_sys("errore nella signal");



  if(argc!=2)
    err_quit("Usage: %s <Port>\n",argv[0]);  
  else if ((atoi(argv[1])<11000)||(atoi(argv[1])>12000))
    err_quit("Inserisci una porta compresa tra 11000 e 12000");
  
  if ( (sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0 )
    err_sys("socket error");
  
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons( atoi(argv[1]) );
    
  if (bind( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) <0 ) 
   	  err_sys("bind error");
  
  if ( listen(sockfd,LISTENQ) <0)
    err_sys("lister error");
  
  buff=(char*)malloc(MAXLINE);

  while(1){

      clilen=sizeof(struct sockaddr_in);  
      if ( (connfd=accept( sockfd, (struct sockaddr *) &cliaddr, &clilen)) <0 ) 
        err_sys("accept error\n");
  
    id++;
    if ((pid=fork())==0){
      char* param[MAXPARAM];
      t_proto message_in;
      
      user_q=(t_player*)shmat(shmid,0,0);
      int *tot=(int*)shmat(sh_tot_id,0,0);

      close(sockfd);
      he=gethostbyaddr(&(cliaddr.sin_addr), sizeof(cliaddr.sin_addr),AF_INET);
      printf("Dati del client: %s %d\n",he->h_name,ntohs(cliaddr.sin_port));
      

      while(((n=readline(connfd,buff,MAXLINE))>=0)){
	if (n==0){
	  printf("ther's no peer for enjoy\n");
 
	}
	/*
	if ((user_q[id].to_do)==WUP){
	  printf("Devo svegliare %d\n",id);
	  snprintf(send,sizeof(send),"%s",user_q[id].action.packet);
	  fflush(stdout);
	  user_q[id].to_do=WUP;
	  user_q[id].action.state=WUP;
	  printf("%s\n",send);
	}
	*/
	if(split_string(buff,32,param)==4){
	  if (atoi(param[0])!=0)
	    message_in.type=atoi(param[0]);
	  fflush(stdout);
	  
	  //printf("Tipo messaggio: %d Payload: %s\n\n",message_in.type,param[1]);
	  fflush(stdout);
	  
	  switch (message_in.type){
	  case HELLO: 
	    strcpy(message_in.nick,param[1]);		      
	    //aggiungo un utente in coda con nickname id e indirizzo/porta
            
	    id=preset_user(user_q,id,*tot);
	    
	    set_user(user_q,message_in.nick,id,cliaddr,FREE);
            
	    down(semid0,0);
	    *tot=id;
	    up(semid0,0);
            
	    printf("Ho stretto la mano al client %d\n",id);
	    printf("totale utenti collegati: %d\n\n",*tot);

	    /*busy waiting per attendere l' avversario*/
	    busy_waiting(user_q,id,*tot);

	    print_users(user_q,*tot);
	    memset(buff,0,MAXLINE);
	    //snprintf(send,sizeof(send),"%d %s %d %d\r\n",message_in.type,message_in.nick,id,user_q[id].id_opp);
	    //importante per sincronizzare ...
	    sleep(1);
	    snprintf(send,sizeof(send),"%d %s %d %d\r\n",user_q[id].to_do,message_in.nick,id,user_q[id].id_opp);
	    if((writen(connfd,send,strlen(send)))!=strlen(send))
	      err_sys("write error");
	   // sleep(1);
	    memset(&message_in,0,sizeof(message_in));
	    break;
	  case SHOOT:	    
	    if ((user_q[id].to_do)!=WAIT){
	      
	      printf("%d\n",id);	      
	     // down(semid0,0);
	      int id_opp = user_q[id].id_opp;
	      // ... something for shooting
	      user_q[id_opp].action.state=SHOOT;
	      user_q[id_opp].to_do=WUP;
	      snprintf(user_q[id_opp].action.packet,sizeof(user_q[id_opp].action.packet),"%d %d %d %d\r\n",WUP,atoi(param[1]),atoi(param[2]),SHOOT);
	      user_q[id].to_do=WAIT;
	      user_q[id].action.state=WAIT;
	      //up(semid0,0);
	      fflush(stdout);
	      snprintf(send,sizeof(send),"%d no no no\r\n",WAIT);
	      if((writen(connfd,send,strlen(send)))!=strlen(send))
		err_sys("write error");
	    }
	    
	  }
	    buff= (char*)malloc(MAXLINE);
	    if ((user_q[id].to_do)==WAIT){	      
		while((user_q[id].action.state)==WAIT);
		snprintf(send,sizeof(send),"%s",user_q[id].action.packet);
		//user_q[id].to_do=WUP;
		//user_q[id].action.state=WUP;
		//printf("%s\n",user_q[id].action.packet);	
		if((writen(connfd,send,strlen(send)))!=strlen(send))
		  err_sys("write error");
	    }


	}

      }  
    }
  }
  if (semctl(semid0,0,IPC_RMID)==-1) perror("Rimozione semaforo");
  if (shmctl(shmid,IPC_RMID,0)==-1) perror("Rimozione shm");
  if (shmctl(sh_tot_id,IPC_RMID,0)==-1) perror("Rimozione shm");
  close(sockfd);
  return (0);
}
