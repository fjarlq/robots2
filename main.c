/*
 *	R O B O T S
 *
 *	The game of robots.
 *	History:
 *	Original Implementation
 *		Allan Black <allan@cs.strath.ac.uk>
 *	Updated for fast robots, moveable heaps, ROBOTOPTS
 *	antimatter, v7/sys5/4.2 portability etc.
 *		Graeme Lunt <gal@cs.nott.ac.uk>
 *		Julian Onions <jpo@cs.nott.ac.uk>
 *
 * Provided free as long as you don't make money from it!
 */

# include "robots.h"

char	whoami[MAXSTR];
char	my_user_name[MAXSTR];
char	cmd_ch;

bool	dead = FALSE;
bool	last_stand;
bool	show_highscore = TRUE;
bool	running, adjacent, first_move, bad_move, waiting;
bool	moveable_heaps = TRUE;

int	my_x, my_y;
int	new_x, new_y;
int	level = 0;
int	free_teleports = 0;
int	old_free;
int	free_per_level = 1;
int	count;
int	dots = 0;
int	robot_value = MIN_VALUE;
int	max_robots = MIN_ROBOTS;
int	nrobots_alive;
int	scrap_heaps = 1;  /* to allow for first level */

long	score = 0;
long	seed;

# ifdef TIOCSLTC
struct ltchars	ltc;
char	dsusp;
# endif TIOCSLTC

int	interrupt();

# define	TERM_UNINIT	00
# define	TERM_CURSES	01
# define	TERM_LTC	02

int	term_state = TERM_UNINIT;	/* cuts out some race conditions */
char	*getenv();
struct	passwd	*getpwuid();
char	_obuf[BUFSIZ];

main(argc,argv)
	int argc;
	char *argv[];
{
	register struct passwd *pass;
	register char *x;
	int i;

	setbuf(stdout, _obuf);
	if(argc > 1) {
		if(argv[1][0] == '-') {
			switch(argv[1][1]) {
			case 's':
				show_highscore = TRUE;
				scoring(FALSE);
				exit(0);
			}
		}
	}
	if((pass = getpwuid(getuid())) == 0) {
		x = "ANON";
	} else {
		x = pass->pw_name;
	}
	(void) strcpy(my_user_name, x);
	(void) strcpy(whoami,x);
	if((x = getenv(ROBOTOPTS)) != NULL && *x != '\0')
		get_robot_opts(x);
	seed = time((time_t *)0)+getuid();
	(void) signal(SIGQUIT,interrupt);
	(void) signal(SIGINT,interrupt);
	if( initscr() == ERR) {
		fprintf(stderr, "Curses won't initialise - seek a guru\n");
		quit(FALSE);
	}
	term_state |= TERM_CURSES;
	crmode();
	noecho();
# ifdef TIOCSLTC
	(void) ioctl(1,TIOCGLTC,&ltc);
	dsusp = ltc.t_dsuspc;
	ltc.t_dsuspc = ltc.t_suspc;
	(void) ioctl(1,TIOCSLTC,&ltc);
	term_state |= TERM_LTC;
# endif TIOCSLTC
	for(;;) {
		count = 0;
		running = FALSE;
		adjacent = FALSE;
		waiting = FALSE;
		last_stand = FALSE;
		old_free = -1;
		if(rnd(free_per_level) < free_teleports) {
			free_per_level++;
			if(free_per_level > MAX_FREE) free_per_level = MAX_FREE;
		}
		free_teleports += free_per_level;
		leaveok(stdscr,FALSE);
		draw_screen();
		put_robots();
		do {
			my_x = rndx();
			my_y = rndy();
			move(my_y,my_x);
		} while(inch() != ' ');
		addch(ME);
		for(;;) {
			scorer();
			if(nrobots_alive == 0) break;
			command();
			for(i=1;i<=FASTEST;i++)
				robots(i);
			if(dead) munch();
		}
		msg("%d robots are now %d scrap heaps",max_robots, scrap_heaps);
		leaveok(stdscr,FALSE);
		move(my_y,my_x);
		refresh();
		(void) readchar();
		level++;
	}
}

draw_screen()
{
	register int x, y;
	clear();
	for(y = 1; y < LINES-2; y++) {
		mvaddch(y,0,VERT);
		mvaddch(y,COLS-1,VERT);
	}
	for(x = 0; x < COLS; x++) {
		mvaddch(0,x,HORIZ);
		mvaddch(LINES-2,x,HORIZ);
	}
}

readchar()
{
	static char buf[1];
	extern int errno;

	while(read(0,buf,1) != 1)
		if( errno != EINTR)
			quit(TRUE);
	return(buf[0]);
}

put_dots()
{
	register int x, y;
	for(x = my_x-dots; x <= my_x+dots; x++) {
		for(y = my_y-dots; y <= my_y+dots; y++) {
			move(y,x);
			if(inch() == ' ') addch(DOT);
		}
	}
}

erase_dots()
{
	register int x, y;
	for(x = my_x-dots; x <= my_x+dots; x++) {
		for(y = my_y-dots; y <= my_y+dots; y++) {
			move(y,x);
			if(inch() == DOT) addch(' ');
		}
	}
}

xinc(dir)
	char dir;
{
	switch(dir) {
	case 'h':
	case 'y':
	case 'b':
		return(-1);
	case 'l':
	case 'u':
	case 'n':
		return(1);
	case 'j':
	case 'k':
	default:
		return(0);
	}
}

yinc(dir)
	char dir;
{
	switch(dir) {
	case 'k':
	case 'y':
	case 'u':
		return(-1);
	case 'j':
	case 'b':
	case 'n':
		return(1);
	case 'h':
	case 'l':
	default:
		return(0);
	}
}

munch()
{
	scorer();
	msg("MUNCH! You're robot food");
	leaveok(stdscr,FALSE);
	mvaddch(my_y,my_x,MUNCH);
	move(my_y,my_x);
	refresh();
	(void) readchar();
	quit(TRUE);
}

quit(eaten)
	bool eaten;
{
	if( term_state & TERM_CURSES ) {
		move(LINES-1,0);
		refresh();
		endwin();
		term_state &= ~ TERM_CURSES;
	}
	putchar('\n');
# ifdef TIOCSLTC
	if( term_state & TERM_LTC ) {
		ltc.t_dsuspc = dsusp;
		(void) ioctl(1,TIOCSLTC,&ltc);
		term_state &= ~ TERM_LTC;
	}
# endif TIOCSLTC
	(void) signal(SIGINT, SIG_DFL);
	scoring(eaten);
	exit(0);
}

rndx()
{
	return(rnd(COLS-2)+1);
}

rndy()
{
	return(rnd(LINES-3)+1);
}

rnd(mod)
	int mod;
{
	if(mod <= 0) return(0);
	return((((seed = seed*11109L+13849L) >> 16) & 0xffffL) % mod);
}

/* VARARGS 1 */
msg(message,a1, a2, a3, a4, a5, a6, a7)
	char *message;
	unsigned int a1, a2, a3, a4, a5, a6, a7;
{
	static char msgbuf[1000];

	(void) sprintf(msgbuf, message, a1, a2, a3, a4, a5, a6, a7);
	mvaddstr(LINES-1,MSGPOS,msgbuf);
	clrtoeol();
	refresh();
}

interrupt()
{
	quit(FALSE);
}

/*
 * file locking routines - much nicer under BSD ...
 */

# ifdef BSD42
lk_open(file, mode)	/* lock a file using the flock sys call */
char	*file;
int	mode;
{
	int	fd;

	if( (fd = open(file, mode)) < 0)
		return -1;
	if( flock(fd, LOCK_EX) < 0)
	{
		(void) close(fd);
		return -1;
	}
	return fd;
}

lk_close( fd, file)
int	fd;
char	*file;
{
# ifdef lint
	file = file;	/* now will you shut up lint???? */
# endif
	return close(fd);
}
# else

# define	LOCKTIME	(60)	/* 1 minute */
# include	<sys/stat.h>

lk_open(file, mode)	/* lock a file by crude means */
char	*file;
int	mode;
{
	char	tfile[128], lfile[128];
	struct	stat stbuf;
	time_t	now;
	int	fd;

	(void) sprintf(tfile, "%s.t", file);	/* temp file */
	(void) sprintf(lfile, "%s.l", file);	/* lock file */

	if( close(creat(tfile, 0)) < 0)	/* make temp file */
		return -1;
	while( link(tfile, lfile) == -1) /* now attempt the lock file */
	{
		if( stat(lfile, &stbuf) < 0)
			continue;	/* uhh? -- try again */
		time(&now);
		/* OK - is this file old? */
		if( stbuf.st_mtime + LOCKTIME < now)
			unlink(lfile);	/* ok its old enough - junk it */
		else	sleep(1);	/* snooze ... */
	}
	unlink(tfile);	/* tmp files done its job */
	if((fd = open(file, mode)) < 0) {
		unlink(lfile);
		return -1;
	}
	return fd;
}

lk_close(fd, fname)
int	fd;
char	*fname;
{
	char	lfile[128];

	(void) sprintf(lfile, "%s.l", fname);	/* recreate the lock file name */
	if( unlink(lfile) == -1)	/* blow it away */
		perror(lfile);
	return close(fd);
}
# endif

