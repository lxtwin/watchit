/*
 * +--------------------------------------------------------------------+
 * | Function: admmenu().                                Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the stat struct.                                       |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *(1) v2.3 Now shows an error if the screen can not change to the long
 *		screen format.
 *						Sun Jul 18 21:40:39 EDT 1993 - PKR / Done.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */

#include "wi.h"

extern	int  colour_flag;

int  admmenu(void);
void adm_defaults(void);

int admmenu(void)
{

int option;
int err_id;
extern int ansi_flag;

	switch(disptype) {
	case (CGA):
	case (EGA):
	case (VGA):

	default :
		break;
	}

	while (TRUE) {
		Draw_A_Box(main_win, DEFAULT_BOX);
		drawdisp(3);
		wnoutrefresh(main_win);
		drawdisp(1);
		wnoutrefresh(top_win);
		an_option();
		doupdate();
		option = wgetch(bottom_win);
		switch (option) {
		case '+' : /* increase sleep time */
		case KEY_UP:
		case KEY_RIGHT:
			change_time(TRUE);
			continue;

		case '-' : /* decrease sleep time */
		case KEY_DOWN:
		case KEY_LEFT:
			change_time(TRUE);
			continue;

		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			continue;

		case 'p' :
			screen_dump();
			continue;

		case 'S' :
		case 's' :
			wmove(main_win, 1, 1);
			wclrtobot(main_win);
			Draw_A_Box(main_win, DEFAULT_BOX);
			drawdisp(3);
			wnoutrefresh(main_win);
			drawdisp(1);
			wnoutrefresh(top_win);
			an_option();
			mvwaddstr(main_win, 0, 2, "Colours");
			mvwaddstr(main_win, 0, 11, "Defaults");
			wattron(main_win, A_REVERSE);
			mvwaddstr(main_win, 0, 21, "Screen_Size");
			wattroff(main_win, A_REVERSE);
			mvwaddstr(main_win, 9, 16, "Changing the screen size..........");
			wnoutrefresh(main_win);
			doupdate();
			mvwaddstr(main_win, 9, 16, "                                    ");
			nap(500);

			mvwaddstr(bottom_win, 1, 2, "                                           ");
			mvwaddstr(bottom_win, 1, 2, " ");
			wmove(bottom_win, 1, 1);
			wrefresh(bottom_win);
			switch(disptype) {
			case (CGA):
			case (EGA):
			case (VGA):
				delwin(main_win);
				delwin(bottom_win);
				if (size_flag) {
					switch(ioctl(0, CONS_CURRENT)) {
					case (CGA):
						ioctl(0, SW_ENHC80x25, 0L);
						break;

					case (EGA):
						ioctl(0, SW_ENHC80x25, 0L);
						break;

					case (VGA):
						ioctl(0, SW_VGA80x25, 0L);
						break;

					default :
						break;
					}
					size_flag = FALSE;
					page_len = PAGE_LEN;
				} else {
					if ((err_id = ioctl(0, SW_ENHC80x43, 0L)) == -1) {
						size_flag = FALSE;
						page_len = PAGE_LEN;
						fprintf(stderr, "Error: unable to change to 43 lines.\n");
						fflush(stderr);
						sleep(2);
					} else {
						size_flag = TRUE;
						page_len = L_PAGE_LEN;
					}
				}
				Draw_A_Box(top_win, DEFAULT_BOX);
				drawdisp(1);
				wnoutrefresh(top_win);

				if (size_flag)
					main_win = LONG_MAIN_WIN;
				else
					main_win = LONG_MAIN_WIN;
				Fill_A_Box(main_win);
				Draw_A_Box(main_win, DEFAULT_BOX);
				drawdisp(2);
				wnoutrefresh(main_win);

				if (size_flag)
					bottom_win = LONG_BOTTOM_WIN;
				else
					bottom_win = LONG_BOTTOM_WIN;
				Fill_A_Box(bottom_win);
				Draw_A_Box(bottom_win, DEFAULT_BOX);

				Draw_A_Box(cache_win, DEFAULT_BOX);
				wnoutrefresh(bottom_win);
				touchwin(top_win);
				touchwin(main_win);
				touchwin(bottom_win);
				Draw_A_Box(main_win, DEFAULT_BOX);
				drawdisp(2);
				doupdate();
				return(0);

			default :
				mvwaddstr(main_win, 10, 10, "Sorry this option is not supported on this terminal.");
				wrefresh(main_win);
				nap(1000);
				mvwaddstr(main_win, 10, 10, "                                                        ");
			}
			continue;

		case 'D' :
		case 'd' :
			adm_defaults();
			continue;

		case 'Q' :
		case 'q' :
			wmove(main_win, 1, 1);
			wclrtobot(main_win);
			drawdisp(4);
			Draw_A_Box(main_win, DEFAULT_BOX);
			drawdisp(2);
			if (size_flag)
				mvwprintw(main_win, 34, 2, "                                        ");
			else
				mvwprintw(main_win, 17, 2, "                                        ");
			return(0);

		default :
			continue;
		}
	}
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

void adm_defaults(void)
{

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 2, "Watchit Default Settings.");
	mvwaddstr(main_win, 11, 2, "CPU Table.");
	mvwaddstr(main_win, 15, 2, "Mount Table.");
	mvwaddstr(main_win, 4, 40, "Misc Settings.");
	mvwaddstr(main_win, 9, 40, "Variable Table.");
	wattroff(main_win, A_BOLD);

	mvwprintw(main_win, 3, 2, "min sample time:     %d second(s).   ", 1);
	mvwprintw(main_win, 4, 2, "max sample time:     %d second(s).   ", 99);
	mvwprintw(main_win, 5, 2, "current sample time: %d second(s).   ",
		Sleep_Time);
	mvwaddstr(main_win, 7, 2, "\"Fast\" time:   1/2 a second.");
	mvwaddstr(main_win, 8, 2, "\"Medium\" time: 1 second.");
	mvwaddstr(main_win, 9, 2, "\"Slow\" time:   2 seconds.");

	mvwprintw(main_win, 12, 2, "CPU_MIN_LIMIT: %d %%   ", CPU_MIN_LIMIT);
	mvwprintw(main_win, 13, 2, "CPU_MAX_LIMIT: %d %%   ", CPU_MAX_LIMIT);

	mvwprintw(main_win, 16, 2, "MNT_MAX_LIMIT: %d %%   ", MNT_MAX_LIMIT);

	mvwprintw(main_win, 5, 40, "number of colours: %d   ", TOTAL_COLOURS);
	mvwprintw(main_win, 6, 40, "page length:       %d    ", PAGE_LEN);
	mvwprintw(main_win, 7, 40, "long page length:  %d   ", L_PAGE_LEN);

	mvwprintw(main_win, 10, 40, "PER_MAX_NINODE:  %d %%   ", PER_MAX_NINODE);
	mvwprintw(main_win, 11, 40, "PER_MAX_NFILE:   %d %%   ", PER_MAX_NFILE);
	mvwprintw(main_win, 12, 40, "PER_MAX_NMOUNT:  %d %%   ", PER_MAX_NMOUNT);
	mvwprintw(main_win, 13, 40, "PER_MAX_NPROC:   %d %%   ", PER_MAX_NPROC);
	mvwprintw(main_win, 14, 40, "PER_MAX_NREGION: %d %%   ", PER_MAX_NREGION);
	mvwprintw(main_win, 15, 40, "PER_MAX_NCALL:   %d %%   ", PER_MAX_NCALL);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
