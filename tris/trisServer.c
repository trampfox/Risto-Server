#include "lib/basic.h"
#include "users.h"
void sigHandler(int segnale)
{
	int status;
	int pid;
	while((pid = waitpid(-1,&status,WNOHANG)) > 0)
		printf("figlio %d terminato\n", pid);
	return;
}

void close_prog(int status)
{
	printf("Programma Terminato\n");
	exit(EXIT_SUCCESS);
}



main(int argc, char **argv)
{
  int sockfd, connfd, listenfd, i,r, maxi, maxd; /*confd memorizza descrittore della accept */
  int ready, client[FD_SETSIZE];
  char buff[MAXLINE];
  int* matrix[3];
  fd_set allset, rset;
  ssize_t n;
  socklen_t clilen;

  t_user utenti[FD_SETSIZE];
  /*inizializza il vettore degli utenti*/
  for (i=0;i<FD_SETSIZE;i++) {
  	memset(utenti[i].nickname, '\0', MAXLINE);
  	utenti[i].state = 0; /* state 0 -> user not present */
		//strcpy(utenti[i].nickname,"NULL\0");
	}

  struct sockaddr_in servaddr, cliaddr;      
  
  if (signal(SIGCHLD, sigHandler) == SIG_ERR)
  	err_sys("signal SIGCHLD error: ");

  if(signal(SIGINT, close_prog) == SIG_ERR) {
  	err_sys("signal SIGINT failed: ");
  	exit(1);
  }

  if ( (listenfd = socket(AF_INET, SOCK_STREAM,0)) < 0 ) /* apro socket TCP */
    err_sys("socket error");
  
  if (argc != 2)
        err_quit("Usage: %s <port>\n",argv[0]); 
  /* Dati del mio server */
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));  
  
  /* aspetto sulla porta impostata */  
  if (bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) <0 ) 
   	  err_sys("bind error");
  
  /* non accetta piu' di LISTENQ connessioni */
  if ( listen(listenfd,LISTENQ) <0)
    err_sys("lister error");

	maxd = listenfd;
	client[0] = listenfd;
	maxi = -1;
	/* imposto valori di default nella tabella dei client */
	for(i=1;i < FD_SETSIZE;i++)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
  while(1){
  	rset = allset;
  	if ((ready = select(maxd+1,&rset,NULL,NULL,NULL)) < 0)
  		err_sys("select error: \n");
  	/* controllo su quale socket si e' verificata un'attivita' */
  	if (FD_ISSET(listenfd, &rset)) { // caso: nuova connessione da parte di un client
  		clilen=sizeof(struct sockaddr_in);
  		if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
  			err_sys("accept error: ");
  			exit(0);
  		}
  		for (i=0;i<FD_SETSIZE;i++) {
  			if(client[i] < 0) {
  				client[i] = connfd;
  				break;
  			}
  		}
  		if(i == FD_SETSIZE)
  			err_quit("\ntoo many client\n");	
  		FD_SET(connfd, &allset);
  		/* aggiorno range file descriptor */
  		if(connfd > maxd) maxd = connfd; 
  		if(i > maxi) maxi = i;
  		if(--ready <= 0) continue; // non ci sono descrittori pronti
  	}
  		for(i=0;i<=maxi;i++) 
  		{
  			/* controllo se ci sono descrittori da leggere */
  			if((sockfd = client[i]) < 0) continue;
  			if(FD_ISSET(sockfd,&rset)) {
  				if((n = readline(sockfd, buff, MAXLINE-1)) == 0) {
  					/*chiudo il socket, non c'e' piu' niente da leggere */
  					if (close(sockfd) < 0)
							err_sys("close error: ");
						FD_CLR(sockfd, &allset);
						client[i] = -1;
						// gestione uscita
  				}
  				else {
  					printf("===\nricevuto %s da %s %d\n===\n",buff,inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
						printf("\ncall messagemng buff:%s\n", buff);
  					r = messagemng(buff, utenti, matrix, client[i]);
						memset(buff, 0, strlen(buff));
					
  					//n = Write(sockfd, buff, strlen(buff));
  				}
  			}
  		}		
  		memset(buff, 0, strlen(buff));
  	}
  	if (close(sockfd) < 0)
			err_sys("close error: ");
		if (close(listenfd) < 0)
			err_sys("close error: ");
	}
