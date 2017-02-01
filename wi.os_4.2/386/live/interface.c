/*
 * +--------------------------------------------------------------------+
 * | Function: interface.c                               Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Starts menu cusres etc.                                     |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *     1.2:	Will get console type and set up the screen.
 *
 *	Motes:
 *		This version uses shm so I don't know if I should continue or
 *		not.
 *
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

#include   <sys/unistd.h>

#define KEY_PROGRAM	"/etc/watchit/bin/keys.hlp"
#define TOTAL_KEY_PROGRAM	"/etc/watchit/bin/keys.hlp -watchit"

#define HELP_PROGRAM	"/etc/watchit/bin/wi.hlp"
#define TOTAL_HELP_PROGRAM	"/etc/watchit/bin/wi.hlp -watchit"

#define ESC '\033'

void	clear_the_screen(void);

extern	int  errno;
extern	int  demo_ver;

extern	int  disptype;
extern	int  size_flag;
extern	int  page_len;

extern	long demo_time;
extern 	short demo_pid;

int interface(void);

/*
 * +--------------------------------------------------------------------+
 * | Function: interface();                              Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        The main loop of every thing                                |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int interface()
{

extern int lines;
extern int unmask;
extern int Sleep_Time;
extern int num_of_hash_disks;

int	cmd, err_id;
register int ctr = 0;
int option = 0;

/*
 +-----------------------------------------------------------------------+
 |               Run this for ever. The main program loop.               |
 +-----------------------------------------------------------------------+
 */

	keypad(bottom_win,TRUE);
	drawdisp(2);
	wnoutrefresh(main_win);
	while (TRUE) {
		demo_pid = 9999;
		Fill_A_Box(main_win, 0, 0);
		drawdisp(1);
		drawdisp(2);
		wnoutrefresh(top_win);
		wnoutrefresh(main_win);
		mvwaddstr(bottom_win, 1, 1, "       : An option, Help, Admin, Key or Quit:");
		w_clrtoeol(bottom_win, 0, 0);
		Set_Colour(bottom_win, Colour_White);
		mvwaddstr(bottom_win, 1, 2, "Select");
		Set_Colour(bottom_win, Colour_Banner);
		mvwaddstr(bottom_win, 1, 21, "H");
		mvwaddstr(bottom_win, 1, 27, "A");
		mvwaddstr(bottom_win, 1, 34, "K");
		mvwaddstr(bottom_win, 1, 41, "Q");
		Set_Colour(bottom_win, Normal);
		wmove(bottom_win, 1, 47);
		wnoutrefresh(bottom_win);
		doupdate();
		option = wgetch(bottom_win);
		switch (option) {
		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			return(0);

		case 'A' :
		case 'a' :
			werase(main_win);
			admmenu();
			return(0);

		case 'B' :
		case 'b' :
		case  KEY_F(1):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(4);
			wi_bufs();
			return(0);

		case 'C' :
		case 'c' :
		case  KEY_F(2):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(5);
			cpu_menu();
			return(0);

		case 'D' :
			Fill_A_Box(main_win, 0, 0);
			num_of_hash_disks = NUM_OF_HASH_DISKS;
			mvwaddstr(bottom_win, 1, 2, " Watchit reading device entries from /dev - Please wait.......");
			w_clrtoeol(bottom_win, 0, 0);
			Draw_A_Box(bottom_win, DEFAULT_BOX);
			wrefresh(bottom_win);

			/*	
				As my main device table is setup I should get the devices.
			*/
			for (ctr = 0; ctr <= num_of_hash_disks; ctr++)
				print_dev(1, nodelist[ctr].major_node
				    , nodelist[ctr].minor_node, nodelist[ctr].bdev_name, 0);


			return(0);

		case 'd' :
			if (major_flag) {
				major_flag = 0;
				mvwaddstr(bottom_win, 1, 2, " Short device name flag enabled.");
			} else {
				major_flag = 1;
				mvwaddstr(bottom_win, 1, 2, " Short device name flag disabled.");
			}
			w_clrtoeol(bottom_win, 0, 0);
			Draw_A_Box(bottom_win, DEFAULT_BOX);
			wrefresh(bottom_win);
			sleep(2);
			return(0);

		case 'F' :
		case 'f' :
		case  KEY_F(3):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(6);
			wi_files();
			return(0);

		case 'I' :
		case 'i' :
		case  KEY_F(4):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(7);
			wi_inodes();
			return(0);

		case 'L' :
		case 'l' :
			Fill_A_Box(main_win, 0, 0);
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
					size_flag = 0;
					page_len = PAGE_LEN;
				} else {
					if ((err_id = ioctl(0, SW_ENHC80x43, 0L)) == -1) {
						size_flag = 0;
						page_len = PAGE_LEN;
						fprintf(stderr, "Error: unable to change to 43 lines.\n");
						fflush(stderr);
						sleep(2);
					} else {
						size_flag = 1;
						page_len = L_PAGE_LEN;
					}
				}

				if (size_flag) {
					main_win = LONG_MAIN_WIN;
					bottom_win = LONG_BOTTOM_WIN;
				} else {
					main_win = MAIN_WIN;
					bottom_win = BOTTOM_WIN;
				}

				Fill_A_Box(top_win, 0, 0);
				Fill_A_Box(bottom_win, 0, 0);
				Fill_A_Box(main_win, 0, 0);

				Draw_A_Box(top_win, DEFAULT_BOX);
				Draw_A_Box(bottom_win, DEFAULT_BOX);
				Draw_A_Box(main_win, DEFAULT_BOX);

				drawdisp(1);
				drawdisp(2);

				wnoutrefresh(top_win);
				wnoutrefresh(bottom_win);
				wnoutrefresh(main_win);
				wclear(main_win);
				doupdate();
				return(0);

			default :
				return(0);
			}

		case 'M' :
		case 'm' :
		case  KEY_F(5):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(8);
			mount_menu();
			return(0);

		case 'O' :
		case 'o' :
		case  KEY_F(6):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(17);
			wrefresh(main_win);
			cmd = 1;
			while (cmd) {
				an_option();
				option = wgetch(bottom_win);
				switch (option) {
				case 'L' & 0x1F:
				case 'R' & 0x1F:
					clear_the_screen();
					break;

				case 'P' :
				case 'p' :
					screen_dump();
					break;

				case 'O' :
				case 'o' :
					if ((access(ONEAC_UPS, F_OK)) == 0) {
						if (size_flag)
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
						savetty();
						reset_shell_mode();
						system(ONEAC_UPS);
						if (! ((disptype = ioctl(0, CONS_CURRENT))FAILS)) {
							switch(disptype) {
							case (CGA):
							case (EGA):
							case (VGA):
								/* printf("%s", colours[colour_number]); */
								break;

							default :
								/* I don't know who I am .... */
								break;
							}
						}
						resetty();
						if (size_flag)
							ioctl(0, SW_ENHC80x43, 0L);
						touchwin(curscr);
						wrefresh(curscr);
					} else
						beep();
					break;

				case 'D' :
				case 'd' :
					drawdisp(20);
					wrefresh(main_win);
					disk_stats();
					Fill_A_Box(main_win, 0, 0);
					drawdisp(17);
					wrefresh(main_win);
					break;

				case 'S' :
				case 's' :
					drawdisp(23);
					wrefresh(main_win);
					stream_stats();
					Fill_A_Box(main_win, 0, 0);
					drawdisp(17);
					wrefresh(main_win);
					break;

				case 'T' :
				case 't' :
					drawdisp(18);
					wrefresh(main_win);
					tty_io();
					Fill_A_Box(main_win, 0, 0);
					drawdisp(17);
					wrefresh(main_win);
					break;

				case 'Q' :
				case 'q' :
					cmd = 0;
					break;

				default :
					beep();
					break;
				}
			}
			return(0);

		case 'P' :
			screen_dump();
			return(0);

		case 'p' :
		case  KEY_F(7):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(9);
			wi_procs();
			return(0);

		case 'R' :
		case 'r' :
		case  KEY_F(8):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(10);
			wi_regions();
			return(0);

		case 'S' :
		case 's' :
		case  KEY_F(9):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(11);
			wrefresh(main_win);
			wi_stats();
			touchwin(main_win);
			drawdisp(2);
			wrefresh(main_win);
			continue;

		case 'U' :
		case 'u' :
		case  KEY_F(10):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(12);
			wi_users();
			return(0);

		case 'V' :
		case 'v' :
		case  KEY_F(11):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(11);
			var_menu();
			return(0);

		case 'Y' :
		case 'y' :
		case  KEY_F(12):
			Fill_A_Box(main_win, 0, 0);
			drawdisp(12);
			sys_menu();
			return(0);

		case 'K' :
		case 'k' :
			if (!(access(KEY_PROGRAM, X_OK))) {
				if (size_flag)
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
				savetty();
				reset_shell_mode();
				system(TOTAL_KEY_PROGRAM);
				if (! ((disptype = ioctl(0, CONS_CURRENT))FAILS)) {
					switch(disptype) {
					case (CGA):
					case (EGA):
					case (VGA):
						/* printf("%s", colours[colour_number]); */
						break;

					default :
						/* I don't know who I am .... */
						break;
					}
				}
				resetty();
				if (size_flag)
					ioctl(0, SW_ENHC80x43, 0L);
				touchwin(curscr);
				wrefresh(curscr);
			} else {
				beep();
				Draw_A_Box(bottom_win, DEFAULT_BOX);
				wattron(bottom_win, A_BOLD);
				mvwaddstr(bottom_win, 1, 2, "Select");
				wattroff(bottom_win, A_BOLD);
				mvwprintw(bottom_win, 1, 8, ": Sorry I can't find %s ",
				    KEY_PROGRAM);
				wrefresh(bottom_win);
				nap(1500);
			}
			return(0);

		case '?' :	/* Help menu	*/
		case 'H' :
		case 'h' :
			if (!(access(HELP_PROGRAM, X_OK))) {
				if (size_flag)
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
				savetty();
				reset_shell_mode();
				system(TOTAL_HELP_PROGRAM);
				if (! ((disptype = ioctl(0, CONS_CURRENT))FAILS)) {
					switch(disptype) {
					case (CGA):
					case (EGA):
					case (VGA):
						/* printf("%s", colours[colour_number]); */
						break;

					default :
						/* I don't know who I am .... */
						break;
					}
				}
				resetty();
				if (size_flag)
					ioctl(0, SW_ENHC80x43, 0L);
				touchwin(curscr);
				wrefresh(curscr);
			} else {
				beep();
				Draw_A_Box(bottom_win, DEFAULT_BOX);
				wattron(bottom_win, A_BOLD);
				mvwaddstr(bottom_win, 1, 2, "Select");
				wattroff(bottom_win, A_BOLD);
				mvwprintw(bottom_win, 1, 8, ": Sorry I can't find %s ",
				    HELP_PROGRAM);
				wrefresh(bottom_win);
				nap(1500);
			}
			return(0);

		case '+' : /* increase sleep time */
		case KEY_UP:
		case KEY_RIGHT:
			wrefresh(main_win);
			change_time(TRUE);
			return(0);

		case '-' : /* decrease sleep time */
		case KEY_DOWN:
		case KEY_LEFT:
			wrefresh(main_win);
			change_time(TRUE);
			return(0);

		case 'Q' :	/* Quit and exit */
		case 'q' :
		case ESC:
			return(1);

		default :
			beep();
			return(0);
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
