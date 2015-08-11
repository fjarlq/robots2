# include "robots.h"

/*
 * good.c: Figure out all possible good moves.
 * This piece of code use to be trivial - then fast robots came
 * and moveable heaps and its now an AI expert system in its own right!
 */

void good_moves(void)
{
	register int test_x, test_y;
	register char *m, *a;
	static char moves[] = "hjklyubn.a";
	static char ans[sizeof (moves)];
	char savebuf[9], savechar;
	int i,j, notsilly;

	a = ans;
	for(m = moves; *m != 'a'; m++) {
		test_x = my_x+xinc(*m);
		test_y = my_y+yinc(*m);
		move(test_y,test_x);
		switch(inch()) {
			case ' ':
			case DOT:
			case ME:
				if( isgood(test_y, test_x) )
					*a++ = *m;
				break;
			case SCRAP:
				if (moveable_heaps) {
				int nscrap_x,nscrap_y;
				nscrap_x = test_x +xinc(*m);
				nscrap_y = test_y +yinc(*m);
				move(nscrap_y,nscrap_x);
				if (inch() == ' ') {
					addch(SCRAP);
					if (isgood(test_y,test_x))
						*a++ = *m;
					move(nscrap_y,nscrap_x);
					addch(' ');
					}
				}
				break;
		}
	}
	/* now lets do the antimatter case */
	if (free_teleports) {
	notsilly = 0;	/* silly to do it if not get robots */
	for(i= -1;i <= 1; i++)
		for(j= -1; j<= 1; j++) {
			move(my_y+i,my_x+j);
			savechar = inch();
			if (savechar == FROBOT || savechar == ROBOT) {
				savebuf[ (i+1)*3 +j +1] = savechar;
				addch(' ');
				notsilly = 1;
				}
			else
				savebuf[ (i+1)*3 +j +1] = ' ';
			}
	if( notsilly && isgood(my_y,my_x) )
		*a++ = *m;

	for(i= -1;i <= 1; i++)
		for(j= -1; j<= 1; j++) {
			move(my_y+i,my_x+j);
			savechar = savebuf[ (i+1)*3 +j +1];
			if (savechar == FROBOT || savechar == ROBOT)
				addch(savechar);
			}
	}
	*a = 0;
	if(ans[0]) {
		a = ans;
	} else {
		a = "Forget it!";
	}
	mvprintw(LINES-1,MSGPOS,"%-*.*s",RVPOS-MSGPOS,RVPOS-MSGPOS,a);
}

int isgood(int ty, int tx)
{
	register int x, y;

	for(x = -2; x <= 2; x++) {
		for(y = -2; y <= 2; y++) {
			if ( abs(x) > 1 || abs(y) > 1 ) { /* we are 2 steps out */
				if( blocked(ty, tx, y, x))
					continue;
				move(ty+y,tx+x);
				if ( abs(x) == 2 && abs(y) == 2
							&& inch() == FROBOT){
					return FALSE;	}
				if ( x == -1 && scan(ty+y, tx+x, 0, 1) )
					return FALSE;
				if ( y == -1 && scan(ty+y, tx+x, 1, 0) )
					return FALSE;
			} /* outer perimeter checked */
			else {
				move(ty+y,tx+x);
				switch(inch()) {
					case FROBOT:
					case ROBOT: /* robot next to you */
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/* scan along this line looking for collision conditions */
int scan(int y, int x, int yi, int xi)
{
	int rcount = 0;
	register int ctr;

	for(ctr = 0;ctr<=2;ctr++) {
		move(y + (ctr*yi), x + (ctr*xi));
		switch(inch()) {
			case FROBOT:
				rcount += 4;
				break;
			case ROBOT:
				rcount ++;
				break;
		}
	}
	return rcount == 4;
}

int blocked(int my, int mx, int y, int x)
{
	if ( x < 0 ) x++;
	if ( y < 0 ) y++;
	if ( x > 0 ) x--;
	if ( y > 0 ) y--;
	move(my+y, mx+x);
	return inch() == SCRAP;
}
