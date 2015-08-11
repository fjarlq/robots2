/*
 * robots.h: include file for the robots game
 */

# include <ncurses.h>
# include <signal.h>
# include <pwd.h>
# include <ctype.h>
# include <sys/types.h>
# include <errno.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <time.h>
# ifdef	BSD42
# include <sys/file.h>
# endif

# define MIN_ROBOTS	10	/* no. of robots you start with */
# define MAX_ROBOTS	500	/* maximum robots on a screen	*/
# define MIN_VALUE	10	/* what each robot is worth to begin */
# define MAX_FREE	3	/* max free teleports per level	*/
# define FASTEST	2	/* the fastest robot (dont fiddle) */

# define VERT	'|'		/* vertical wall	*/
# define HORIZ	'-'		/* horizontal wall	*/
# define ROBOT	'='		/* normal robot		*/
# define FROBOT '#'		/* fast robot		*/
# define SCRAP  '@'
# define ME	'I'
# define MUNCH	'*'
# define DOT	'.'

# define LEVEL		(level+1)

# define MSGPOS		35	/* where messages appear on bottom line */
# define RVPOS		47

/* These you may want to fiddle with. Position of the two high score files */
# define HOF_FILE	"/var/games/robots_hof"
# define TMP_FILE	"/var/games/robots_tmp"

# define NUMSCORES	20		/* number of people to record */
# define NUMNAME	"Twenty"	/* above spelt out */

# define TEMP_DAYS	7		/* how long temp scores last */
# define TEMP_NAME	"Week"

# define ROBOTOPTS	"ROBOTOPTS"	/* environment tailoring */

# define MAXSTR		100

# define MULT		1.5		/* multiplier for fast robots */

/* if ALLSCORES Undefined - record top n players */
# define ALLSCORES			/* record top n scores */

# define SECSPERDAY	86400

# define ctrl(x)	((x)&037)
# define BEL	ctrl('G')

# define	abs(X)  ((X) < 0 ? -(X) : (X))
# define	sign(X) ((X) < 0 ? -1 : (X) > 0)

struct robot {
	bool	alive;	/* is this suker still kicking */
	int	x;
	int	y;
	int	speed;  /* speed of robot 1 or 2 for now */
} rbt_list[MAX_ROBOTS+1];

extern	char	whoami[];
extern	char	my_user_name[];
extern	char	cmd_ch;

extern	bool	moveable_heaps;
extern	bool	show_highscore;
extern	bool	last_stand;
extern	bool	bad_move;
extern	bool	running;
extern	bool	waiting;
extern	bool	first_move;
extern	bool	adjacent;
extern	bool	dead;

extern	int	my_x, my_y;
extern	int	new_x, new_y;
extern	int	count;
extern	int	free_teleports;
extern	int 	dots;
extern	int	robot_value;
extern	int	level;
extern	int	max_robots;
extern	int	scrap_heaps;
extern	int	nrobots_alive;
extern	int	free_per_level;
extern	int	old_free;

extern	long	score;

/* good.c */
void good_moves(void);
int isgood(int ty, int tx);
int scan(int y, int x, int yi, int xi);
int blocked(int my, int mx, int y, int x);

/* main.c */
void draw_screen(void);
char readchar(void);
void put_dots(void);
void erase_dots(void);
int xinc(char dir);
int yinc(char dir);
void munch(void);
void quit(bool eaten);
int rndx(void);
int rndy(void);
int rnd(int mod);
void msg(char *message, ...);
int lk_open(char *file, int mode);
int lk_close(int fd, char *file);

/* opt.c */
void get_robot_opts(char *str);

/* robot.c */
void put_robots(void);
void robots(int speed);
void collision(struct robot *r, struct robot *end);
void screwdriver(void);

/* score.c */
void scoring(bool eaten);
int record_score(bool eaten, char *fname, int max_days, char *type_str);
int do_score(bool eaten, int fd, int max_days, char *type_str);
void scorer(void);

/* user.c */
void command(void);
int read_com(void);
void do_move(char dir);
int move_heap(char dir);
