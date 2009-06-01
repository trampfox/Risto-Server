#include "lib/basic.h"
#define TIMEOUT 15

void write_log(int status)
{
	printf("Arrivato SIGPIPE\n");
	exit(1);
}


void close_prog(int status)
{
	printf("Programma Terminato\n");
	exit(EXIT_SUCCESS);
}



main(int argc, char *argv[])
{
  int sockfd, n, num, maxfd, r, ntout;
<<<<<<< HEAD:tris/trisClient.c
=======
  int *matrice[3];
>>>>>>> 448bc7b29924ba90a86cb5cbff3efceb93bbe415:tris/trisClient.c
  struct sockaddr_in servaddr;
  char buff[MAXLINE], str[MAXLINE], rcv[MAXLINE];
  struct hostent *he;
  fd_set rset;
  struct timeval timeout;
  
  ntout = 0;
  timeout.tv_sec = TIMEOUT;
  timeout.tv_usec = 0;
  
  /* controllo se la signal va a buon fine */
  if(signal(SIGPIPE, write_log) == SIG_ERR) {
  	err_sys("signal SIGPIPE failed: ");
  	exit(1);
  }

  if(signal(SIGINT, close_prog) == SIG_ERR) {
  	err_sys("signal SIGINT failed: ");
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
	
  /* tenta la connessione */
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
  	printf("Welcome\n\n");
  	/* autenticazione sul socket del server */
  	auth(sockfd);

	fflush(stdout);
	FD_ZERO(&rset);
  while (1) { 
  	/* le eseguo ogni volta perche' la select imposta a 0 i fd che non hanno avuto attivita' */
  	FD_SET(fileno(stdin), &rset);
  	FD_SET(sockfd, &rset);
  	maxfd = MAX(fileno(stdin), sockfd) + 1;
		/* gestisce il primo fd su cui c'e' attivita' 
		 * la select e' bloccante, quindi rimane in attesa fino a quando non arriva traffico */
  	if ((n = (select(maxfd, &rset, NULL, NULL, NULL))) <= 0) {
		/* parte gestione timeout (rimettere &timeout come ultimo parametro)
  		if (n == 0) {
  			ntout++;
  			if ((ntout >= 0) && (ntout <= 2)) {
  				printf("\nrichiamo %d\n", ntout);
  				timeout.tv_sec = TIMEOUT;
  				timeout.tv_usec = 0; 
				}
  			if (ntout == 3) {
  				printf("\ninput timeout\n");
  				close(sockfd);
  				exit(0);
  			}
  		}
  		else */
  			err_sys("client select error: ");
		}
		
		/* == gestione fd == */
		if (FD_ISSET(sockfd, &rset)) {
			n = Read(sockfd, rcv, MAXLINE-1);
			/* gestione del messaggio */
<<<<<<< HEAD:tris/trisClient.c
			r = clientMessagemng(rcv, sockfd);						
=======
			r = clientMessagemng(rcv, matrice, sockfd);						
>>>>>>> 448bc7b29924ba90a86cb5cbff3efceb93bbe415:tris/trisClient.c
			//printf("echo: %s\n", rcv);
			//printf("Inserisci la stringa: ");
			fflush(stdin);
			fflush(stdout);

		}
		if (FD_ISSET(fileno(stdin), &rset)) {
			if (fgets(str, MAXLINE, stdin) == NULL)
				return;
			if ( strlen(str) > 0 )
			{
				str[strlen(str)-1] = '\0';
				snprintf(buff, sizeof(buff), "%s\r\n", str);
				n = Write(sockfd, buff, strlen(buff));
			}
		}
		memset(rcv, 0, sizeof(rcv));
  }     	
	close(sockfd);
	exit(0); 				
}
