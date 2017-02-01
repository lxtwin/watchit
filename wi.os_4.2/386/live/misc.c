/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */


#include <curses.h>

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

void an_option()
{

extern WINDOW *bottom_win;

	mvwaddstr(bottom_win, 1, 8, ": An option or Quit:                              ");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 2, "Select");
	wattroff(bottom_win, A_BOLD);
	wattron(bottom_win, A_REVERSE);
	mvwaddstr(bottom_win, 1, 23, "Q");
	wattroff(bottom_win, A_REVERSE);
	Draw_A_Box(bottom_win, DEFAULT_BOX);

	wattron(bottom_win, A_BLINK);
	mvwaddstr(bottom_win, 1, 29, "_");
	wattroff(bottom_win, A_BLINK);
	wmove(bottom_win, 1, 29);
	wrefresh(bottom_win);

}

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

void any_key()
{

extern WINDOW *bottom_win;

	mvwaddstr(bottom_win, 1, 2, " : Any key to continue:   ");
	wclrtoeol(bottom_win);
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 2, "Select");
	wattroff(bottom_win, A_BOLD);
	wattron(bottom_win, A_REVERSE);
	mvwaddstr(bottom_win, 1, 10, "Any key");
	wattroff(bottom_win, A_REVERSE);
	Draw_A_Box(bottom_win, DEFAULT_BOX);
	wattron(bottom_win, A_BLINK);
	mvwaddstr(bottom_win, 1, 30, "_");
	wattroff(bottom_win, A_BLINK);
	wmove(bottom_win, 1, 30);
	wnoutrefresh(bottom_win);
}

