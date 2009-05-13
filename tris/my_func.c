#include "basic.h"
#include <ctype.h>
/*Prende in input una stringa standard, restituisce il numero delle 
  parole oggetto di split
  out è un vettore di puntatori a stringhe
*/
//int split_string(char* input,char separator,char** output);


int split_string(char* input,char sep,char **out){
  int i=0;

  char*p=NULL;
  char* ss=NULL;


  for(p=strtok_r(input,&sep,&ss),i=0;p!=NULL;p=strtok_r(NULL,&sep,&ss),i++){
    out[i]=(char*)malloc(MAXLINE);
    sprintf(out[i],"%s",p);
  }
  return i;
}

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


int control_coord(char* el){
  int k;
  if (isdigit(el[0])){
    k=atoi(el);
    if ((k<0)||(k>2)){
      printf("inserisci coord tra 0 e 2!\n");
      return -1;
    }
    else
      return k;
  }
  else return -1;
}


/*
main(){

  char* stringhe[15];
  char line[MAXLINE]="2 3 4 \r\n   ";
  int i=0;
  int j=0;
  j=split_string(line,32,stringhe);
  printf("%d\n",j);
  for(i=0;i<j;i++)
    printf("%s\n",stringhe[i]);

}

*/







