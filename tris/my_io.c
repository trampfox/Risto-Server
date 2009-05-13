#include	"basic.h"

/* writen scrive n byte sul descrittore fd usando la system call write. 
   Riprova fino a quando i dati non sono disponibili o si verifica un errore. 
   Se, però, la write viene interrotta da un segnale (errore EINTR) la funzione 
   continua ad iterare. */

ssize_t		
writen(int fd, const void *vptr, size_t n)
{

   size_t		nleft;    /* byte ancora da scrivere */
   ssize_t		nwritten; /* byte scritti dall'ultima write */
   const char	*ptr;

   ptr = vptr;
   nleft = n;
   while (nleft > 0) {
	if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
	   if (errno == EINTR)
	      /* se la write è stata interrotta da un segnale
		 prosegue ed assume che non ha letto nulla */
              nwritten = 0;		
	   else
	      return(-1);	
	}

	nleft -= nwritten;  /* aggiorna il numero di byte da scrivere */
	ptr   += nwritten;  
       /* aggiorna la posizione nel buffer dove devono essere letti 
          i rimanenti byte */
   }
   return(n);   /* restituisce il numero di byte scritti. */
}


/* readn legge n byte dal descrittore fd usando la system call read. 
   Riprova fino a quando i dati non sono disponibili o si verifica un errore. 
   Se la read viene interrotta da un segnale (errore EINTR) la funzione 
   continua ad iterare. Se la read legge l'EOF termina avendo letto meno di n
   byte. */

ssize_t		
readn(int fd, void *vptr, size_t n)
{

   size_t	nleft;	/* byte che rimangono da leggere */
   ssize_t	nread;	/* byte letti nell'ultima read */
   char	*ptr;

   ptr = vptr;
   nleft = n;
   while (nleft > 0) {
	if ( (nread = read(fd, ptr, nleft)) < 0) {
           if (errno == EINTR)
           /* se la read è stata interrotta da un segnale
	      prosegue ed assume che non ha letto nulla */
	      nread = 0;		
	   else
	      return(-1);
	} 
        else 
	   if (nread == 0) /* la read restituisce 0 quando legge l'EOF */
	      break;			

	nleft -= nread;    /* aggiorna il numero di byte da leggere */
	ptr   += nread;
       /* aggiorna la posizione nel buffer dove devono essere scritti 
          i rimanenti byte */
   }
   return(n - nleft);	/* restituisce il numero di byte letti ( >= 0 ) */
}

/* my_read legge tutti i dati disponibili nel buffer in una sola volta e poi 
   li esamina un byte per volta. Restituisce in ptr il prossimo byte da
   esaminare. 
   Il valore di ritorno è 
	> 0 se l'operazione è riuscita
	= 0 se ha letto l'EOF
        < 0 se c'è stato un errore.
   La read, invece, deve leggere un byte per volta perchè non sa quanti 
   dati sono stati effettivamente inviati. */

ssize_t
my_read(int fd, char *ptr)
{

   static int	read_cnt = 0;      /* numero di byte letti */
   static char	read_buf[MAXLINE]; /* buffer per mantenere i dati letti */
   static char	*read_ptr;         /* puntatore al prossimo byte da leggere */

   if (read_cnt <= 0) {
   /* se non ci sono byte disponibili nel buffer chiama la read */
	while ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) 
        /* la read itera finché non ci sono dati disponibili */
	      if (errno != EINTR) 
		  return(-1);     /* esce perchè si è verificato un errore */
	      if (read_cnt == 0)  
		      return(0);  /* esce perchè ha letto l'EOF */
	      read_ptr = read_buf;/* si posizione sul prossimo byte da passare */
	}

   read_cnt--;
   *ptr = *read_ptr++;
   /* legge il prossimo byte dal buffer e decrementa il numero dei byte 
      disponibili */
   return(1);
}

/* la readline legge una sequenza di byte di lunghezza <= maxlen e fino al
   primo '\n'. La funzione termina solo dopo aver letto il '\n' oppure 
   dopo aver riempito tutto il buffer di lettura.
   La readline utilizza la my_read invece della read per motivi di 
   effiecienza. */

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{

   int	n, rc;
   char	c, *ptr;

   ptr = vptr;
   for (n = 1; n < maxlen; n++) {
    	if ( (rc = my_read(fd, &c)) == 1) {  /* my_read è OK */
	   *ptr++ = c;
	   if (c == '\n')
	      break;	/* esce dal ciclo perchè ha letto il newline */
        } 
        else 
	   if (rc == 0) {	/* my_read ha letto l'EOF */
	      if (n == 1)
		 return(0);	/* esce e non ha letto nulla */
	      else
		 break;		/* esce solo dal ciclo perchè 
                                   ha letto qualcosa prima dell'EOF */
	   } 
           else			/* my_read è uscita per un errore */
	      return(-1);	/* termina; errno settato dalla read() */
   }

   *ptr = 0;	
   /* inserisce uno 0 per indicare la fine dell'input, coma la fgets() */
   return(n);                  /* restituisce il numero di byte letti */
}


