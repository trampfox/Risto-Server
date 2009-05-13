#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int down(int semid, int semnum)	
	/* operazione DOWN sulla componente semnum di semid */
{
  int r;
  struct sembuf sb;

  sb.sem_num=semnum;
  sb.sem_op=-1;
  sb.sem_flg=0;
  r=semop(semid,&sb,1);
  if (r==-1) perror("semop in down");
  return r;
}

int up(int semid, int semnum)	
	/* operazione UP sulla componente semnum di semid */
{
  int r;
  struct sembuf sb;

  sb.sem_num=semnum;
  sb.sem_op=1;
  sb.sem_flg=0;
  r=semop(semid,&sb,1);
  if (r==-1) perror("semop in up");
  return r;
}

int seminit(int semid, int semnum, int initval)
	/* inizializzazione con initval della componente semnum di semid */
{
  int r;
 
  #if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
  /* union semun is defined by including <sys/sem.h> */
  #else
  /* according to X/OPEN we have to define it ourselves */
  union semun {
        int val;                  /* value for SETVAL */
        struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
        unsigned short *array;    /* array for GETALL, SETALL */
                                  /* Linux specific part: */
        struct seminfo *__buf;    /* buffer for IPC_INFO */
  };
  #endif

  union semun arg;

  arg.val=initval;
  r=semctl(semid,semnum,SETVAL,arg);
  if (r==-1) perror("semop in up");
  return r;
}

