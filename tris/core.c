#include "core.h"
//#include "users.c"
#include "my_func.c"

   


  int t_mossa(int*game[],int x, int y,int player){
    if ((x>=0)&&(y<=2)){
      if (game[x][y]==0){
	game[x][y]=player;
	return MOSSA_VALIDA;
      }
    }
    return MOSSA_NON_VALIDA;
  }
  
  
  char func(int i){
    char seme[]="XO"; 
    if (i==0)
      return 43;
    else{
      return seme[i%2]; /* usa 0 e 1 per identificare l'utente che ha fatto la mossa'
    }
  }

  void t_status(int *game[]){
      printf("%c %c %c\n%c %c %c\n%c %c %c\n",func(game[0][0]),func(game[0][1]),func(game[0][2]),func(game[1][0]),func(game[1][1]),func(game[1][2]),func(game[2][0]),func(game[2][1]),func(game[2][2]));
  }
    
  int t_victory(int *game[]){
    /*
      Testa la matrice per decretare un vincitore:
      Restituisce l' id del vincitore, -1 in caso contrario.
     */
    //Test per orizzontali
    if      ((game[0][0]== game[0][1])&& (game[0][0]==game[0][2])) return game[0][0];
    else if ((game[1][0]== game[1][1])&& (game[1][0]==game[1][2])) return game[1][0];
    else if ((game[2][0]== game[2][1])&& (game[2][0]==game[2][2])) return game[2][0];
    //Test per le verticali
    else if ((game[0][0]== game[1][0])&& (game[0][0]==game[2][0])) return game[0][0];
    else if ((game[0][1]== game[1][1])&& (game[0][1]==game[2][1])) return game[0][1];
    else if ((game[0][2]== game[1][2])&& (game[0][2]==game[2][2])) return game[0][2];
    //Test per le diagonali
    else if ((game[0][0]== game[1][1])&& (game[2][2]==game[0][0])) return game[0][0];
    else if ((game[0][2]== game[1][1])&& (game[2][0]==game[0][2])) return game[0][2];
    else return -1;
  }

/*
   main(){

     int * matrice[3]; 
     int row1[]={0,0,0};
     int row2[]={0,0,0};
     int row3[]={0,0,0};
     int players[]={0,1,2};
     int x,y;
     t_player* utenti=NULL;
     matrice[0]=row1;
     matrice[1]=row2;
     matrice[2]=row3;
     int k;
     int id=0;
     int m=9;
     char nick[MAXLINE];
     char nick1[MAXLINE];
     char vincente[MAXLINE];


     printf("Inserisci nickname 1: ");
     fgets(nick,MAXLINE,stdin);
     nick[strlen(nick)-1]='\0';
     id++;
     set_user(&utenti,nick,id);
     printf("Inserisci nickname 2: ");
     fgets(nick1,MAXLINE,stdin);
     nick1[strlen(nick1)-1]='\0';
     id++;
     set_user(&utenti,nick1,id);

     char parola[MAXLINE]="Io sono leggenda\0";
     char * out[20];
     
     k=split_string(parola,' ',out);
     printf("%s\n",out[0]);


     while(1){
       printf("Tocca a %s\n", nick);
       printf("Inserisci coord x: ");
       scanf("%d",&x);
       printf("Inserisci coord y: ");
       scanf("%d",&y);
       if (t_mossa(matrice,x,y,players,nick2id(utenti,nick))==MOSSA_NON_VALIDA){
	 printf("Eccezione gioco\n");
	 m++;
       }
       m--;
       if ((k=t_victory(matrice))>0){
	 id2nick(utenti,k,vincente);
	 printf("La vittoria va a %s\n",vincente);
	 t_status(matrice);
	 exit(0);
       }
       t_status(matrice);
       if (m<1){
	 printf("Patta!");
	 exit(0);
       }
       
       printf("Tocca a %s\n", nick1);
       printf("Inserisci coord x: ");
       scanf("%d",&x);
       printf("Inserisci coord y: ");
       scanf("%d",&y);
       if (t_mossa(matrice,x,y,players,nick2id(utenti,nick1))==MOSSA_NON_VALIDA){
	 printf("Eccezione gioco\n");
	 m++;
       }
       m--;
       if ((k=t_victory(matrice))>0){
	 id2nick(utenti,k,vincente);
	 printf("La vittoria va a %s\n",vincente);
	   t_status(matrice);
	   exit(0);
       }
       t_status(matrice);
       if (m<1){
	 printf("Patta!\n");
	 exit(0);
       }
     }
     exit(0);
   }

*/
