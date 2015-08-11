# include "robots.h"

/*
 * opt.c: do a psuedo ROGUEOPTS sort of thing
 */

typedef struct Opt {
	char	*name;
	int	type;
} Opt;

Opt	options[] = {
# define	OPT_NAME	1
	{ "name",		OPT_NAME, },	/* Who am i boss ? */
# define	OPT_MOVE_HEAP	2
	{ "moveheaps",	OPT_MOVE_HEAP, },	/* can push heaps */
# define	OPT_NOMOVE_HEAP	3
	{ "nomoveheaps",	OPT_NOMOVE_HEAP, },/* can't push heaps */
# define	OPT_SHOW_HSCORE 4
	{ "showhscore",	OPT_SHOW_HSCORE, },
# define	OPT_NOSHOW_HSCORE 5
	{ "noshowhscore", OPT_NOSHOW_HSCORE, },
	{ 0,		0 },
};

/* get_robot_opts: Personalise robots to the users tastes. Model after
 * the rogue/urogue type environment stuff. 
 */

void get_robot_opts(char *str)
{
	register char	 *p;
	Opt	*op;
	int	len, len2;

	p = str;
	while(*p)
	{
		while(*p && !isalpha(*p)) p++;  /* skip non-alphas */
		str = p;

		while(isalpha(*p)) p ++;	/* match longest word */
		len = p - str;

		for(op = options; op->name; op ++)	/* see if defined */
			if( strncmp(str, op->name, len) == 0)
				break;
		if( op->name == NULL)
			continue;
		switch(op->type)	/* OK, now do something */
		{
			case OPT_NAME:
				while(*p == '=') p++;	/* skip ='s */
				str = p;	/* OK, now look for name */
				while(*p && *p != ',') p++;
				len2 = (MAXSTR - 1) -
					(strlen(my_user_name) + 4);
				len = p - str;
				len = len < len2 ? len : len2;
				(void) sprintf(whoami, "%.*s (%s)", len,
					str, my_user_name);
				break;
			case	OPT_MOVE_HEAP:
				moveable_heaps = TRUE;
				break;
			case	OPT_NOMOVE_HEAP:
				moveable_heaps = FALSE;
				break;
			case	OPT_SHOW_HSCORE:
				show_highscore = TRUE;
				break;
			case	OPT_NOSHOW_HSCORE:
				show_highscore = FALSE;
				break;
		}
	}
}

