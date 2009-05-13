#include	"basic.h"

#include	<stdarg.h>		/* ANSI C header file */
#include	<syslog.h>		/* per syslog() */

int		daemon_proc;		/* set nonzero by daemon_init() */

/* tutte le funzioni utilizzano err_doit per stampare un messaggio di errore.
   Differiscono per come viene gestito il controllo del programma. */

static void	err_doit(int, int, const char *, va_list);

/* err_ret gestisce errori non fatali attinenti una chiamata di sistema.
   Stampa un messaggio e restituisce il controllo al programma principale. */

void
err_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* err_sys gestisce errori fatali attinenti una chiamata di sistema.
   Stampa un messaggio e termina l'esecuzione del programma. */

void
err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* err_sys gestisce errori fatali attinenti una chiamata di sistema.
   Stampa un messaggio, effettua un dump del core e termina l'esecuzione 
   del programma. */

void
err_dump(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
	exit(1);		/* shouldn't get here */
}

/* err_msg gestisce errori non fatali non attinenti una chiamata di sistema.
   Stampa un messaggio e restituisce il controllo al programma principale. */

void
err_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* err_sys gestisce errori fatali non attinenti una chiamata di sistema.
   Stampa un messaggio e termina l'esecuzione del programma. */

void
err_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Stampa un messaggio e restituisce il controllo alla funzione chiamante. 
   Per invocare la funzione bisogna specificare "errnoflag" per indicare il
   codice dell'errore che si è verificato e "level" per indicare il livello
   di syslog. */

static void
err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int		errno_save, n;
	char	buf[MAXLINE];

	errno_save = errno;    /* codice dell'errore che deve essere stampato */
#ifdef	HAVE_VSNPRINTF
	vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
#else
	vsprintf(buf, fmt, ap);			/* this is not safe */
#endif
	n = strlen(buf);
	if (errnoflag)
		snprintf(buf+n, sizeof(buf)-n, ": %s", strerror(errno_save));
	strcat(buf, "\n");

	if (daemon_proc) {
		syslog(level, buf);
	} else {
		fflush(stdout);		/* se stdout and stderr coincidono */
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}
