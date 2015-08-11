# include "robots.h"

/*
 * user.c: user oriented things
 */

void command(void)	/* whats the user trying to tell us */
{
retry:
	move(my_y,my_x);
	refresh();
	if(last_stand) return;
	bad_move = FALSE;
	if(!running) {
		cmd_ch = read_com();
		switch(cmd_ch) {
		case ctrl('W'):
			waiting = TRUE;
		case ctrl('H'):
		case ctrl('J'):
		case ctrl('K'):
		case ctrl('L'):
		case ctrl('Y'):
		case ctrl('U'):
		case ctrl('B'):
		case ctrl('N'):
			cmd_ch |= 0100;
			adjacent = TRUE;
		case 'H':
		case 'J':
		case 'K':
		case 'L':
		case 'Y':
		case 'U':
		case 'B':
		case 'N':
			cmd_ch |= 040;
			running = TRUE;
			first_move = TRUE;
		case 't':
		case 'T':
		case 's':
		case 'S':
		case 'W':
		case 'm':
		case 'M':
		case '?':
		case 'd':
		case 'D':
		case ctrl('R'):
			count = 0;
		}
	}
	switch(cmd_ch) {
	case '.':
	case 'h':
	case 'j':
	case 'k':
	case 'l':
	case 'y':
	case 'u':
	case 'b':
	case 'n':
	case 'w':
		do_move(cmd_ch);
		break;
	case 't':
	case 'r':
	case 'T':
	case 'R':
	teleport:
		new_x = rndx();
		new_y = rndy();
		move(new_y,new_x);
		switch(inch()) {
		case FROBOT:
		case ROBOT:
		case SCRAP:
		case ME:
			goto teleport;
		}
		if( (free_teleports > 0)
		   && ((cmd_ch == 't')||(cmd_ch =='T')) ) {
			if( !isgood(new_y, new_x))
				goto teleport;
			free_teleports--;
		}
		break;
	case 's':
	case 'S':
	case 'W':
		last_stand = TRUE;
		leaveok(stdscr,TRUE);
		return;
	case 'm':
	case 'M':
	case '?':
		good_moves();
		goto retry;
	case 'd':
	case 'D':
		if(dots < 2) {
			dots++;
			put_dots();
		} else {
			erase_dots();
			dots = 0;
		}
		goto retry;
	case 'q':
	case 'Q':
		quit(FALSE);
	case 'a':
	case 'A':	/* Antimatter - sonic screwdriver */
		if (free_teleports) { new_x = my_x;
				  new_y = my_y;
				  screwdriver();
				}
		else goto retry;
		break;
	case ctrl('R'):
		clearok(curscr,TRUE);
		wrefresh(curscr);
		goto retry;
	default:
		bad_move = TRUE;
	}
	if(bad_move) {
		if(running) {
			if(first_move) putchar(BEL);
			running = FALSE;
			adjacent = FALSE;
			waiting = FALSE;
			first_move = FALSE;
		} else {
			putchar(BEL);
		}
		refresh();
		count = 0;
		goto retry;
	}
	first_move = FALSE;
	if(dots) erase_dots();
	mvaddch(my_y,my_x,' ');
	my_x = new_x;
	my_y = new_y;
	move(my_y,my_x);
	if(inch() == ROBOT) munch();
	if(inch() == FROBOT) munch();
	if(dots) put_dots();
	mvaddch(my_y,my_x,ME);
	refresh();
}

int read_com(void)
{
	static int     com;

	if(count == 0) {
		if(isdigit(com = readchar())) {
			count = com-'0';
			while(isdigit(com = readchar()))
				count = count*10+com-'0';
		}
	}
	if(count > 0) count--;
	return(com);
}

void do_move(char dir)	/* implement the users move */
{
	register int x, y;
	new_x = my_x+xinc(dir);
	new_y = my_y+yinc(dir);
	if(adjacent && !first_move) {
		for(x = -2; x <= 2; x++) {
			for(y = -2; y <= 2; y++) {
				move(new_y+y ,new_x+x);
				switch(inch()) {
				case SCRAP:
					if( waiting )
						break;
				case ROBOT:
					if(abs(x) < 2 && abs(y) < 2) {
						bad_move = TRUE;
						return;
					}
					else break;
				case FROBOT:
					if (waiting &&
						blocked(new_y, new_x, y, x) )
						break;
					bad_move = TRUE;
					return;
				}
			}
		}
	}
	move(new_y,new_x);
	switch(inch()) {
	case SCRAP:
		if(moveable_heaps && move_heap(dir))
			break;
	case VERT:
	case HORIZ:
		bad_move = TRUE;
		return;
	}
}

int move_heap(char dir)	/* push a scrap heap */
{
	register int	x, y;

	x = new_x + xinc(dir);
	y = new_y + yinc(dir);
	move(y, x);
	switch(inch()) {
		case VERT:
		case HORIZ:
		case SCRAP:
		case ROBOT:
		case FROBOT:
			return FALSE;
	}
	addch(SCRAP);
	mvaddch(new_y,new_x,' ');
	return TRUE;
}
