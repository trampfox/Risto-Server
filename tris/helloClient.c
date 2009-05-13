#include "basic.h"
#include "protocollo.h"
#include "users.h"
#include "core.c"


#define  START   128
#define    END   256

void write_log(int status)
{
	printf("Arrivato SIGPIPE\n");
	exit(1);
}


main(int argc, char *argv[])
{
  int sockfd,n, maxfd, ntout;
  int log=0;
  struct sockaddr_in servaddr;      
  char buff[MAXLINE], rcv[MAXLINE];
  struct hostent *he;
  fd_set rset;
  struct timeval timeout;
  
  t_proto message;
  t_proto message_in;
  t_player personal;
  int coord=0;
  char temp[MAXLINE];

  char * param[MAXPARAM];

  int set=0;

  int * matrice[3]; 
  int row1[]={0,0,0};
  int row2[]={0,0,0};
  int row3[]={0,0,0};

  int v=0;

  matrice[0]=row1;
  matrice[1]=row2;
  matrice[2]=row3;
  
  ntout = 0;
  timeout.tv_sec = 20;
  timeout.tv_usec = 0;
  
  /* controllo se la signal va a buon fine */
  if(signal(SIGPIPE, write_log) == SIG_ERR) {
  	err_sys("signal SIGPIPE failed: ");
  	exit(1);
  }
  
  if(argc <= 2)
	err_quit("Usage: %s <IP address> <port>\n",argv[0]);

  if ( (sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0 ) 
   err_sys("socket error");
   
  he = gethostbyname(argv[1]);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=inet_addr(inet_ntoa(*(struct in_addr*) he->h_addr));
  servaddr.sin_port = htons(atoi(argv[2]));

  if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
  	if (errno == EHOSTUNREACH) {
				printf("host unreachable\n");
				exit(1);
		}
		else if (errno == ENETUNREACH) {
				printf("net unreachable\n");
				exit(1);
		}
	  else
	  	err_sys("connect error: ");
  }
  //mia mod
      printf("Inserisci il tuo nickname[ > 6 char]: "); 
      fflush(stdout);
      
   //end-mia-mod
      FD_ZERO(&rset);
  while (1) { 
  	/* le eseguo ogni volta perche' la select imposta a 0 i fd che non hanno avuto attivita' */
  	FD_SET(fileno(stdin), &rset);
  	FD_SET(sockfd, &rset);
  	maxfd = MAX(fileno(stdin), sockfd) +1;
		/* gestisce il primo fd su cui c'e' attivita' 
		 * la select e' bloccante, quindi rimane in attesa fino a quando non arriva traffico */
  	if ((n = (select(maxfd, &rset, NULL, NULL, &timeout))) < 0) {
	  
	  /*if (n == 0) {
  			ntout++;
  			if ((ntout >= 0) && (ntout <= 2)) {
  				printf("\nrichiamo %d\n", ntout);
  				timeout.tv_sec = 3;
  				timeout.tv_usec = 0; 
				}
  			if (ntout == 3) {
  				printf("passa il turno\n");
  				//close(sockfd);
  				//exit(0);
  			}
  		}
  		else*/ 
  			err_sys("client select error: ");
			fflush(stdout);
		}
		
		/* == gestione fd == */
		if (FD_ISSET(sockfd, &rset)) {
			n = read(sockfd, rcv, MAXLINE-1);
		/* ricevo stringa nulla, quindi il server non e' piu' online */
			if (n <= 0) {
				if (errno == ECONNRESET) {
					printf("error connect reset: \n");
					exit(-1);
				}
				else {
					printf("server not present \n");
					exit(-1);
				}
			}
			fflush(stdout);
			printf("\n\n");
			printf("pacchetto dal server: %s", rcv);
			/*Qui si decide tutto il giuoco: recupero il tipo pacchetto
			  estrapolo informazioni e agisco di conseguenza
			 */
			if(split_string(rcv,32,param)==4){
			  if (atoi(param[0])!=0)
			    message_in.type=atoi(param[0]);
			  switch (message_in.type){
			  case WUP:
			    if (atoi(param[3])!=SHOOT){
			      personal.id=atoi(param[2]);
			      personal.id_opp=atoi(param[3]);
			      //set_matrix(matrice);
			      set=1;
			    }
			    
			    //fflush(stdout);
			    else{
			      printf("Ho ricevuto un colpo!: %d %d\n",atoi(param[1]), atoi(param[2]));
			      printf("Il mio avversario è:%d\n",personal.id_opp);
			      if (t_mossa(matrice,atoi(param[1]),atoi(param[2]),personal.id_opp)==MOSSA_NON_VALIDA)
				printf("Eccezione gioco\n");
			      //In questo ramo gestisco l'invio al core!!!
			      t_status(matrice);
			      v=t_victory(matrice);
			      printf("*%d*\n",v);
			      fflush(stdout);
			      if (v>0){
				if (v==personal.id){
					printf("Hai vinto, complimenti!!\n");
					exit(0);
				}
				else{
					printf("Non vali niente, non sai vincere nemmeno a tris!\n");	
					exit(0);
				}
			      }			      
	
			    }
			    //partita=1;
			    memset(buff,0,MAXLINE);
			    memset(rcv,0,MAXLINE);
			    //ora creo pacchetto per la mossa
			    message.type=SHOOT;
			    message.id_opp=personal.id_opp;
			    coord=1;
			    log=1;
			    printf("Inserisci coord x y: ");
			    fflush(stdout);
			    break;
			    case WAIT:
			    //coord=0;
			    if ((strcmp("no",param[3])!=0)&&(set==0)){
			      personal.id=atoi(param[2]);
			      personal.id_opp=atoi(param[3]);
			      set=1;
			      //set_matrix(matrice);
			    }
			    printf("Attendo una mossa dall' avversario\n");
			    fflush(stdout);
			    break;
			    case RST:
				fflush(stdout);
				printf("Ciao, resetto il gioco!\n\n");
			    break;

			    //case WUP:
			    //coord=1;
			    //printf("Inserisci coord x y: \n");
			    
			  /*case SHOOT:
			    if(param[3]==WUP)
			      coord=1;
			    printf("x: %s\n", param[1]);
			    printf("y: %s\n", param[2]);
			    
			    // ho ricevuto un colpo
			    printf("stampa griglia, posiziona elemento\n");
			    printf("Inserisci coord x y: \n");
			    fflush(stdout);
			    break;*/
			  }
			}

			/*e poi immissione delle coordinate*/
		}

		fflush(stdin);
		fflush(stdout);
		
		if (FD_ISSET(fileno(stdin), &rset)) {
		              if (coord==1){
				//printf("Inserisci coordinate:\n");
				fflush(stdout);
				//do {
				fgetsn(temp);
				printf("%s\n\n",temp);
				coord=0;
				char* n_coord[2];
				if ((split_string(temp,32,n_coord))==2){
				  if(((message.x_coord=control_coord(n_coord[0]))>=0)&&(message.y_coord=control_coord(n_coord[1]))>=0){
				    snprintf(buff,sizeof(buff),"%d %d %d %d\r\n",message.type,message.x_coord,message.y_coord, message.id_opp);
				    if (t_mossa(matrice,message.x_coord,message.y_coord,personal.id)==MOSSA_NON_VALIDA)
				      printf("Eccezione gioco\n");
				    t_status(matrice);
				    v=0;//test per sancire il vincitore!
				    v=t_victory(matrice);
				    printf("*%d*\n",v);

				      
				    //n = write(sockfd, buff, strlen(buff));
				  }
				  else {
				    coord=1;
				    fflush(stdout);
				    printf("Inserisci coordinate x y: ");
				    fflush(stdout);
				  }
				}
				else
				  //fflush(stdout);
				  coord=1;
				//** wrap per fgets
				  //}while((message.x_coord=control_coord(temp))<0);
				
				//snprintf(temp1,sizeof(temp1),"%d %d",SHOOT,message.x_coord);
				
				//snprintf(buff,sizeof(buff),"%s %d %d\r\n",temp1,message.y_coord,message.id_opp);
			      }
			      
			      if (log==0) {
				
				fgetsn(message.nick);
				//message.nick[strlen(message.nick)-1]='\0';
				while (strlen(message.nick)<5){
					printf("Inserisci il tuo nickname[ > 6 char]: ");
					fflush(stdout);
					fgetsn(message.nick);
				}
				
				log=1;
				strcpy(personal.nickname,message.nick);
				printf("Buon divertimento %s !\n",personal.nickname);
				message.type=HELLO;
				snprintf(buff,sizeof(buff),"%d %s no no\r\n",message.type, message.nick);
			      }
			      //printf("var_log: %d | var_coord: %d\n\n",log,coord);
			      n = write(sockfd, buff, strlen(buff));

			      if (v>0){
				if (v==personal.id){
					printf("Hai vinto, complimenti!!\n");
					exit(0);
				}
				else{
					printf("Non vali niente, non sai vincere nemmeno a tris!\n");	
					exit(0);
				}
			      }

			      if (n <= 0) {
				if (errno == EPIPE) {
				  printf("EPIPE: chiusura anomala\n");
				  exit(1);
				}
			/* se ricevo un errore di tipo EINTR non lo considero */
				else if (errno == EINTR)
				  continue;
				else if (errno == ETIMEDOUT) {
				  printf("timeout\n");
				  close(sockfd);
				  exit(1);
				}
			}
		}
		memset(rcv, 0, sizeof(rcv));
  }     	memset (buff,0, sizeof(buff));
	close(sockfd);
	exit(0); 				
}
