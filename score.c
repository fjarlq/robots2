# include "robots.h"

/*
 * score.c: All the scoring code is in here.
 */

struct scorefile {
	int	s_uid;
	long	s_score;
	char	s_name[MAXSTR];
	bool	s_eaten;
	int	s_level;
	int	s_days;
};

# define FILE_SIZE	(NUMSCORES*sizeof(struct scorefile))

void scoring(bool eaten)
{
	static char buf[MAXSTR];

	(void) sprintf(buf,"for this %s",TEMP_NAME);
	if( record_score(eaten,TMP_FILE,TEMP_DAYS,buf) || show_highscore) {
		printf("[Press return to continue]");
		fflush(stdout);
		getchar();
	}
	record_score(eaten,HOF_FILE,0,"of All Time");
}

int record_score(bool eaten, char *fname, int max_days, char *type_str)
{
	int value;
	int fd;

	/* block signals while recording the score 
	 * hope this routine doesn't get stuck! 
	 */
	sig_t	oint, oterm, ohup, otstp;

	oint = signal(SIGINT, SIG_IGN);
	oterm = signal(SIGTERM, SIG_IGN);
	ohup = signal(SIGHUP, SIG_IGN);
	otstp = signal(SIGTSTP, SIG_IGN);
	
	if((fd = lk_open(fname,2)) < 0) {
		perror(fname);
		value = FALSE;
	} else {
		value = do_score(eaten,fd,max_days,type_str);
		lk_close(fd, fname);
	}
	(void) signal(SIGINT, oint);
	(void) signal(SIGTERM, oterm);
	(void) signal(SIGHUP, ohup);
	(void) signal(SIGTSTP, otstp);
	return value;
}

int do_score(bool eaten, int fd, int max_days, char *type_str)
{
	register struct scorefile *position;
	register int x;
	register struct scorefile *remove, *sfile, *eof;
	struct scorefile *oldest, *this;
	int uid, this_day, limit;
	ssize_t nread;

	this_day = max_days ? time((time_t *)0)/SECSPERDAY : 0;
	limit = this_day-max_days;
	sfile = (struct scorefile *)(malloc(FILE_SIZE));
	if( sfile == NULL)
	{
		fprintf( stderr, "Out of memmory so no scoring");
		return FALSE;
	}
	eof = &sfile[NUMSCORES];
	this = 0;
	for(position = sfile; position < eof; position++) {
		position->s_score = 0;
		position->s_days = 0;
	}
	nread = read(fd, (char *)sfile, FILE_SIZE);
	if (nread == -1) {
		perror("Error: do_score: read");
		return FALSE;
	}
	if (nread > 0 && nread != FILE_SIZE) {
		fprintf(stderr, "Error: do_score: score file truncated (%zd != %zd)\n", nread, FILE_SIZE);
		return FALSE;
	}
	remove = 0;
	if(score > 0) {
		uid = getuid();
		oldest = 0;
		x = limit;
		for(position = eof-1; position >= sfile; position--) {
			if(position->s_days < x) {
				x = position->s_days;
				oldest = position;
			}
		}
		position = 0;
		for(remove = sfile; remove < eof; remove++) {
			if(position == 0 && score > remove->s_score) position = remove;
# ifndef ALLSCORES
			if(remove->s_uid == uid) break;
# endif
		}
		if(remove < eof) {
			if(position == 0 && remove->s_days < limit) position = remove;
		} else if(oldest) {
			remove = oldest;
			if(position == 0) {
				position = eof-1;
			} else if(remove < position) {
				position--;
			}
		} else if(position) {
			remove = eof-1;
		}
		if(position) {
			if(remove < position) {
				while(remove < position) {
					*remove = *(remove+1);
					remove++;
				}
			} else {
				while(remove > position) {
					*remove = *(remove-1);
					remove--;
				}
			}
			position->s_score = score;
			(void) strncpy(position->s_name,whoami,MAXSTR);
			position->s_eaten = eaten;
			position->s_level = LEVEL;
			position->s_uid = uid;
			position->s_days = this_day;
			this = position;

			if(lseek(fd,0L,0) == -1L ||
				write(fd,(char *)sfile,FILE_SIZE) != FILE_SIZE)
				perror("scorefile");
			close(fd);
		}
	}
	if( show_highscore || this )
	{
		printf(
# ifdef ALLSCORES
			"\nTop %s Scores %s:\n",
# else
			"\nTop %s Robotists %s:\n",
# endif
			NUMNAME,
			type_str
		);
		printf("Rank   Score	Name\n");
		for(position = sfile; position < eof; position++) {
			if(position->s_score == 0) break;
			if(position == this)
				putchar('>');
			else  putchar(' ');
			printf(
				"%c%2zd %9ld   %s: %s on level %d.",
				position->s_days < limit ? '*' : ' ',
				position-sfile+1,
				position->s_score,
				position->s_name,
				position->s_eaten ? "eaten" : "chickened out",
				position->s_level
			);
			if(position == this)
				putchar('<');
			putchar('\n');
		}
	}
	return (this != 0);
}

void scorer(void)
{
	static char tels[6];
	if(free_teleports != old_free) {
		if(free_teleports > free_per_level) {
			(void) sprintf(tels,"%d+%d",
				       free_per_level,
				       free_teleports-free_per_level);
		} else {
			(void) sprintf(tels,"%d",free_teleports);
		}
		old_free = free_teleports;
	}
	move(LINES-1,0);
	clrtoeol();
	printw("<%s> level: %d    score: %ld",tels,LEVEL,score);
        mvprintw(LINES-1,RVPOS,"heaps:%3d robots:%3d value: %d",
					scrap_heaps,nrobots_alive,robot_value);
	clrtoeol();
}

