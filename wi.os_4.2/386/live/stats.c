/*
 * +--------------------------------------------------------------------+
 * | Function: stats().                                  Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the stat struct.                                       |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */


#include "wi.h"
#include <sys/utsname.h>

#define	MINUTE	(60L)
#define	HOUR	(MINUTE*60L)
#define	DAY	(HOUR*24L)

void wi_stats(void);

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |                                                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void wi_stats(void)
{

WINDOW *win;
int  i = 0;


	kmem_read(&utsname, namelist[NM_UTSNAME].n_value, sizeof(utsname));
	kmem_read(&ktime,   namelist[NM_TIME].n_value,    sizeof(ktime));
	kmem_read(&klbolt,  namelist[NM_LBOLT].n_value,   sizeof(klbolt));

	/* open a tmp window */
	if (size_flag)
		win = newwin(13, 53, 25, 25);
	else
		win = newwin(13, 53, 8, 25);
	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);

	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 1, 2, "Sysname");
	mvwaddstr(win, 2, 2, "Release");
	mvwaddstr(win, 3, 2, "BUS Type");
	mvwaddstr(win, 4, 2, "Machine");
	mvwaddstr(win, 5, 2, "Nodename");
	mvwaddstr(win, 6, 2, "Serial");
	mvwaddstr(win, 7, 2, "# Users");
	mvwaddstr(win, 8, 2, "Watchit type");
	mvwaddstr(win, 9, 2, "Current time");
	mvwaddstr(win, 11, 2, "Age of System");
	Set_Colour(win, Colour_Black);

	mvwprintw(win, 1, 9, ":  %.*s", sizeof(utsname.sysname), utsname.sysname);
	mvwprintw(win, 2, 9, ":  %s", scoutsname.release);
	mvwprintw(win, 3, 10, ": %s", scoutsname.bustype);
	mvwprintw(win, 4, 9, ":  %s", scoutsname.machine);
	mvwprintw(win, 5, 10, ": %.*s", sizeof(utsname.nodename), utsname.nodename);
	mvwprintw(win, 6, 8, ":   %s", scoutsname.sysserial);
	mvwprintw(win, 7, 9, ":  %s", scoutsname.numuser);

#ifdef _OLD_OS
#	ifdef _486
		mvwaddstr(win, 8, 14, ": UNIX 3.2v4.1, ODT 2.0 - 486 Only");
#	else
#		ifdef _586
			mvwaddstr(win, 8, 14, ": UNIX 3.2v4.1, ODT 2.0 - Pentium Only");
#		else
			mvwaddstr(win, 8, 14, ": SCO UNIX 3.2v4.1, ODT 2.0 or earlier");
#		endif
#	endif
#else
#	ifdef _486
		mvwaddstr(win, 8, 14, ": UNIX 3.2v4.2, ODT 3.0 - 486 Only");
#	else
#		ifdef _586
			mvwaddstr(win, 8, 14, ": UNIX 3.2v4.2, ODT 3.0 - Pentium Only");
#		else
			mvwaddstr(win, 8, 14, ": SCO UNIX 3.2v4.2, ODT 3.0");
#		endif
#	endif
#endif
	mvwprintw(win, 9, 14, ": %.24s", ctime(&ktime));
	mvwaddstr(win, 11, 15, ":");

	klbolt /= HZ;			/* convert to seconds */
	if (klbolt >= DAY)
		wprintw(win, " %d %s", klbolt / DAY,
		    klbolt >= (2 * DAY) ? "Days, ": "Day, ");
	else
		wprintw(win, " 0 Days, ");

	klbolt %= DAY;
	if (klbolt >= HOUR)
		wprintw(win, " %d %s", klbolt / HOUR,
		    klbolt >= (2 * HOUR) ? "Hours, ": "Hour, ");

	klbolt %= HOUR;
	klbolt /= MINUTE;
	wprintw(win, " %d %s", klbolt,
	    klbolt MATCHES || klbolt >= 2 ? "Minutes.": "Minute.");

	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 0, 32, " System Statistics ");
	Set_Colour(win, Normal);
	wnoutrefresh(win);
	doupdate();

	for (i = 0; ;) {
		an_option();
		doupdate();

		switch(wgetch(bottom_win)){
		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			break;

		case 'P':
		case 'p':
			screen_dump();
			break;

		case 'G':
		case 'g':
			Show_Graphics();
			break;

		case 'q':
		case 'Q':
			delwin(win);
			return;

		default:
			beep();
			break;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

