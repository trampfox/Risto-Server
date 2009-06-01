#include	"lib/basic.h"
#include 	"users.c"
#include	"engine/engine.c"
#include	"lib/protocol.h"

/* funzioni principali per il gioco del tris
 * Parte di autenticazione
 * Parte del parsing del messaggio (comune client e server)
 */
int my_id;

char* fgetsn (char * str){
  char* p;
  //Devo eliminare lo \n, è importantissimo, altrimenti si creerebbe \n\r\n 
  //la readline lato server, si fermerebbe al 1° \n e si metterebbe in attesa 
  //per \r\n, crando confusione...

  p=fgets(str,MAXLINE,stdin);
  str[strlen(str)-1]='\0';
  fflush (stdout);
  fflush(stdin);
  return p;
}

/*Splitta una stringa in n parti e ne ritorna il numero*/
int split_string(char* input,char sep,char **out)
{
  int i=0;

  char* p=NULL;
  char* ss=NULL;
	// Bisogna passare i parametri senza /0 altrimenti ci sono problemi nel parsing
  for(p = strtok_r(input,&sep,&ss),i=0;p!=NULL;p=strtok_r(NULL,&sep,&ss),i++)
  {
    out[i]=(char*)malloc(MAXLINE);
    memset (out[i],'\0',MAXLINE);
    sprintf(out[i],"%s",p);
  }
  return i;
}

/* ritorna il codice del messaggio passato come argomento 
 * il tipo del messaggio e' dato dal primo carattere (numerico) */
messageType(char *message)
{
	return(atoi(&message[0]));
}

int parseMessage(char *buff, char **param)
{
	
	if(split_string(buff, 32, param) <= 4){
		//printf("split_string return value: %s\n", param[0]);
		if (strcmp(param[0], "HELO") == 0) return HELO;
		if (strcmp(param[0], "SROK") == 0) return SROK;
		if (strcmp(param[0], "ERRR") == 0) return ERRR; 
		if (strcmp(param[0], "LIST") == 0) return LIST; 
		if (strcmp(param[0], "RLIS") == 0) return RLIS; 
		if (strcmp(param[0], "RQOK") == 0) return RQOK;
		if (strcmp(param[0], "RUSR") == 0) return RUSR;
		if (strcmp(param[0], "USOK") == 0) return USOK;
		if (strcmp(param[0], "QUIT") == 0) return QUIT;
		if (strcmp(param[0], "PUSH") == 0) return PUSH;
		if (strcmp(param[0], "MVOK") == 0) return MVOK;
		if (strcmp(param[0], "MVER") == 0) return MVER;
	}
	else 
		return -1;
}

/***************************** 
			Server function
******************************/


int serverAuth( int sockfd, int ret )
{
	char message[MAXLINE], type[5];
	
	/* type identifica il tipo di messaggio da inviare. In questo caso
	 * e' uguale a zero -> autenticazione */
	if (ret == SROK)
 		sprintf(type,"SROK\0");
	else
		sprintf(type,"ERRR\0");
	
	memset(message, '\0', MAXLINE);
	snprintf(message, sizeof(message), "%s 440 NULL NULL\r\n", type);

	Write(sockfd, message, strlen(message));
	return 0;
}

/* initialize a copy of game matrix on server used to control the player's shoots
** matrix: game matrix */
int initGameMatrix(int matrix[3][3])
{
	int i, j;
	for (i=0;i<3;i++)
		for (j=0;j<3;j++)
			matrix[i][j] = 0;
 	printf("Game matrix\n");
	t_status (matrix);
	return 1;
}

int controlMove(int matrix[3][3], t_user* players, int y, int x, int player_id)
{
	char message[MAXLINE];
	memset(message, '\0', MAXLINE);
	printf("control move: y:%d, x:%d, player_id:%d\n", y, x, player_id);
	if ((matrix[x][y] != 0)) {
		/* wrong move */
		snprintf(message, sizeof(message), "MVER %d %d NULL\r\n", y, x);
		Write(player_id, message, strlen(message));
	}
	else /* good move */
		/* send MVOK to the the that send a PUSH message */
		snprintf(message, sizeof(message), "MVOK %d %d NULL\r\n", y, x);
		Write(player_id, message, strlen(message));
		/* send a PUSH message with the new move */
		memset(message, '\0', MAXLINE);
		snprintf(message, sizeof(message), "PUSH %d %d %d\r\n", y, x, player_id);
		Write(players[player_id].opp, message, strlen(message));
}

/* Server message management
 * sockfd: client socket descriptor that has sent the message */
int messagemng(char* buff, t_user* players, int matrix[3][3], int sockfd)
{
	int n,k;
	char* param [10];
	char* list[FD_SETSIZE];
	memset (list, '\0', FD_SETSIZE);
	if( (n = parseMessage(buff, param)) >= 0 ){
		printf("\nmessage type: %d\n", n);
		switch (n){
			case HELO:
				if  ((k = addUser(param[1],players,sockfd)) == 0) 
					serverAuth( sockfd, SROK );
				else serverAuth( sockfd, ERRR );
			break;
			case LIST:
				/* prepare and send the list of all available users */
				prepareList(players, list, sockfd);
			break;
			case RQOK: /* l'avversario ha accettato */																				
				userConfirm( players, atoi(param[1]), sockfd );
				initGameMatrix(matrix);
			break;
			case RUSR:
				printf("RUSR request received\n");
				/* control the user request, param[1] contains the user ID */
				controlUser(players, sockfd, param[1]);
			break;
			case PUSH:
				printf("PUSH case\n");
				/* control if the move is correct */
				controlMove(matrix, players, atoi(param[1]), atoi(param[2]), sockfd);
					//sendCoord(sockfd, players[sockfd].opp, atoi(param[1]), atoi(param[2]));
				t_status(matrix);
			break;
			case QUIT:
			break;	
		}		

	}
	return (0);
}

/***************************** 
			Client function
******************************/

void setMyid(int id)
{
	my_id = id;
	return;
}

/* funzione per l'autenticazione del giocatore 
** sockfd: server socket file descriptor */
int auth(int sockfd)
{
	char username[MAXLINE], message[MAXLINE];
	/* type identifica il tipo di messaggio da inviare. In questo caso
	 * e' uguale a zero -> autenticazione */
	int type = HELO; 
	memset(username, '\0', MAXLINE);
	memset(message, '\0', MAXLINE);
	printf("\nPlease insert your username: ");
	fgetsn(username);
	snprintf(message, sizeof(message), "HELO %s NULL NULL\r\n", username);
	/* spedisce la stringa al server */
	Write(sockfd, message, strlen(message));
	return 0;
}

/* funzione che serve al client per richiedere la lista degli utenti collegati
** sockfd: server socket file descriptor */
int askList(int sockfd)
{
	char message[MAXLINE];
	memset(message, '\0', MAXLINE);
	snprintf(message, sizeof(message), "LIST NULL NULL NULL\r\n");
	/* spedisce la stringa al server */
	Write(sockfd, message, strlen(message));
	printf("\nList of available users |id - nickname|\n\n");
	return 0;	

}

/* Gestisce messaggi di tipo RUSR.
 * Un giocatore riceve una richiesta di gioco da parte di un altro giocatore
 * sockfd: socket descriptor del server
 * opponent: id dell'avversario */
int agreeRqst (int sockfd, int opponent )
{
	char message [MAXLINE];
	memset(message, '\0', MAXLINE);
	char decision[MAXLINE];
	printf("Player %d would like to play with you, do you agree? (y/n)\n", opponent);
	fgetsn(decision);
	while ( (decision[0]!='y') && (decision[0]!='n') )
	{
		printf("Insert y or n\n");
		fgetsn(decision);
	}
	if ( decision[0] == 'y' )
		snprintf(message, sizeof(message), "RQOK %d NULL NULL\r\n", opponent);
	else
		snprintf(message, sizeof(message), "USNO NULL NULL NULL\r\n");
	/* send the decision to server*/
	Write(sockfd, message, strlen(message));
	
}

/* Begin the game after a USOK message. It prepares the game matrix and calls 
** pushCoord function
** code: message code (110 or 220)
** matrix: game matrix
** sockfd: server socket descriptor */ 
void beginGame(int code, int matrix[3][3], int sockfd){
 	/*
 	int row1[]={0,0,0};
 	int row2[]={0,0,0};
 	int row3[]={0,0,0};
	matrix[0]=row1;
 	matrix[1]=row2;
 	matrix[2]=row3;
 	*/
 	int i, j;
	for (i=0;i<3;i++)
		for (j=0;j<3;j++)
			matrix[i][j] = 0;
	int k;
	if (code == 220)
	{	
		printf("\nWaiting for the first opponent shoot!\n");
		return;
	}
	else if (code == 110)
	{	
		/*show the first screen game  */
		t_status (matrix);
		k = pushCoord(matrix, my_id, sockfd);
		while (k < 0)
			k = pushCoord(matrix, my_id, sockfd);
		return;
	}
}

/* Control and update the game matrix with the specified coords 
** sockfd: server socket descriptor */
int pushCoord(int matrix[3][3], int my_id, int sockfd)
{
	char temp[MAXLINE];
	memset(temp, '\0', MAXLINE);
	char message [MAXLINE];
	memset(message, '\0', MAXLINE);
	char * n_coord[3];
	int x,y;
	printf("\nPlease insert coordinates: ");
	fgetsn(temp);
	while ((split_string(temp,32,n_coord))!=2)
	{
		printf("\nPlease insert coordinates: ");
		fgetsn(temp);
	} 
	if ( ( (y=control_coord(n_coord[0]))>=0 ) && ( (x=control_coord(n_coord[1]))>=0 ) )
	{
	/*
		if ( t_mossa(matrix, y, x, my_id) == MOSSA_NON_VALIDA)
		{	
			printf("exception!!");
			return (-1);	
		}
		else
		{ */
			/* Send the coordinates */
			snprintf(message, sizeof(message), "PUSH %d %d NULL\r\n",y, x);
			Write(sockfd, message, strlen(message));
			return (1);
	}
	else 
		return (-1);
	
}


/* Client message management. Find the type of messagge with parseMessage
** and then call the relative function 
** sockfd: server socket descriptor 
** buff: string received by client 
** game: game matrix */
int clientMessagemng (char* buff, int matrix[3][3], int sockfd)
{
	int n,k;
	char * param [10];
	int x,y,opp;
	
	//printf("call parse message\n");
	if( (n = parseMessage(buff, param)) >= 0 ){
		//printf("parseMessage return value: %d\n", n);
		switch (n)
		{
			case SROK:
				/* request list of available users */
				askList(sockfd);
			break;
			case ERRR:
				printf("\nError: %s\n", param[1]);
				if (atoi(param[1]) == 440)
					auth( sockfd );
				else
					askList(sockfd);
			break;
			case RLIS:
				if ((strcmp(param[1], "NULL")) == 0)
					printf("No available user\n");
				else
					printf("%s\n",param[1]);
				chooseOption(sockfd);
				//selectPlayer(sockfd);
			break;
			case USOK:
				printf("USOK reply received\n");
				setMyid (atoi (param[2]));
				printf("my_id: %d\n", my_id);
				beginGame(atoi(param[1]), matrix, sockfd);
				//t_status (game);
			break;
			case RUSR:
				printf("RUSR received\n");
				agreeRqst( sockfd, atoi(param[1]) );
			break;
			case PUSH:
				y   =  atoi(param[1]);
				x   =  atoi(param[2]);
				opp =  atoi(param[3]);
				printf("y: %d x: %d opp:%d\n",y,x ,opp);
				updateMatrix(matrix, x, y, opp);
				t_status (matrix);
				k = pushCoord (matrix, my_id, sockfd);
				while (k < 0)
					k = pushCoord (matrix, my_id, sockfd);
			break;
			case MVOK:
				printf("MVOK received\n");
				y = atoi(param[1]);
				x = atoi(param[2]);
				updateMatrix(matrix, y, x, -1);
			break;
			case MVER:
				printf("MVER received\n");
				printf("\nBad move, retry another one\n");
				k = pushCoord (matrix, my_id, sockfd);
				while (k < 0)
					k=pushCoord (matrix, my_id, sockfd);
			break;
			default: 
				printf("Unknown message\n");
			break;
		}
	}
	return 0;
}

/* select player by ID 
** sockfd: server socket file descriptor */
int selectPlayer(int sockfd)
{
	char message[MAXLINE];
	char id[FD_SETSIZE];
	memset(id, '\0', FD_SETSIZE);
	printf("Please select player to play the match.\n");
	printf("Insert player's ID: ");
	/* inserire controllo input */
	fgetsn(id);
	memset(message, '\0', MAXLINE);
	snprintf(message, sizeof(message), "RUSR %s NULL NULL\r\n", id);
	Write(sockfd, message, strlen(message));
}

/* user menu */
int chooseOption(int sockfd)
{
	char choice[3];
	memset(choice, '\0', 3);
	printf("What would you like to do?\n");
	printf("1 - Wait a player\n2 - Choose player\n3 - Reload List\n\n");
	printf("Insert your choice: ");
	fgetsn(choice);
	switch (atoi(choice))
	{
		case 1:
		break;
		case 2:
			selectPlayer(sockfd);
		break;
		case 3:
			system("clear");
			askList(sockfd);
		break;
		default:
		break;
	}	
}

/* Sending coord to server 
** me: client socket descriptor
** sockfd: server socket descriptor
** x, y: coordinates 
int sendCoord (int me, int sockfd, int y, int x)
{
	char message[MAXLINE];
	memset(message, '\0', MAXLINE);
	printf("sendCoord: y:%d, x:%d, sockfd:%d\n", y, x, sockfd);
	snprintf(message, sizeof(message), "PUSH %d %d %d\r\n",y, x);
	Write(sockfd, message, strlen(message));
}
*/
/* update game matrix after a good move */
int updateMatrix(int matrix[3][3], int y, int x, int opp)
{
	printf("update game matrix -> y:%d x:%d\n, opp:%d\n", y, x, opp);
	if (opp == -1) /* sent move case */
		matrix[x][y] = my_id;
	else /* received move case */
		matrix[x][y] = opp;
	printf("Game matrix updated\n");
	t_status(matrix);
}
