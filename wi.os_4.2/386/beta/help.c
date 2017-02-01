/*
 * +--------------------------------------------------------------------+
 * | Function: help()                                    Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Do all the help stuff.                                      |
 * +--------------------------------------------------------------------+
 *
 * Updates:
 *          None.
 *    Bugs:
 *          None yet.
 */

#include "wi.h"

#define ESC '\033'
#define MIN_MENU 0
#define MAX_MENU 14
#define RETURN 0x0a

WINDOW *top_win, *main_win, *bottom_win, *menu_win, *help_win, *cpu_win;
char *arrow_name[12];

int size_flag;
int Doc_Flag = FALSE;

int menu_id;
int demo_ver = FALSE;

void help(void);
void disp_menu(void);
void use_display(int menu_id);
void use_arrow(int menu_id);
void helpbox(char *help_id, int help_screen);

void an_option(void);
void any_key(void);

void help_gen(void);
void help_buf(void);
void help_cup(void);
void help_file(void);
void help_inode(void);
void help_mount(void);
void help_procs(void);
void help_reg(void);
void help_stats(void);
void help_user(void);
void help_var(void);
void help_sys(void);
void help_disk(void);
void help_stream(void);
void help_tty(void);

void (*help_screen[])(void) = {
	help_gen,
	help_buf,
	help_cup,
	help_file,
	help_inode,
	help_mount,
	help_procs,
	help_reg,
	help_stats,
	help_user,
	help_var,
	help_sys,
	help_disk,
	help_stream,
	help_tty
};

/*
 * +--------------------------------------------------------------------+
 * | Function: int main()                                Date: 93/02/27 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	Where it all starts.                                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int main(int argc, char **argv)
{


	size_flag = atoi(argv[1]);

	initscr();
	typeahead(-1); /*  Without this it is too slow */

	mkwindow(1);
	drawwindow(1);
	drawdisp(0);
	wnoutrefresh(top_win);

	mkwindow(2);
	drawwindow(2);
	drawdisp(2);
	wnoutrefresh(main_win);

	mkwindow(3);
	drawwindow(3);
	wnoutrefresh(bottom_win);

	help();

	if (argc NOT_MATCH) {
		clear();
		refresh();
	}
	endwin();
	exit(0);
}

void help()
{

int  option;

static char Sccsid[] = "%Z% %M%         Version %I% %D% - Dapix.";

	menu_id = 0;

	/* open a tmp window */
	menu_win = newwin(17, 14, 5, 5);
	help_win = newwin(14, 65, 6, 10);

	keypad(bottom_win, TRUE);

	*arrow_name = "General";
	disp_menu();
	drawdisp(2);
	wnoutrefresh(main_win);
	wattron(menu_win, A_BOLD);
	mvwaddstr(menu_win, 1, 2, "General   ");
	wattroff(menu_win, A_BOLD);
	wrefresh(menu_win);

	while (TRUE) {
		drawdisp(2);
		touchwin(menu_win);
		an_option();
		wnoutrefresh(main_win);
		wnoutrefresh(menu_win);
		wnoutrefresh(bottom_win);
		doupdate();
		switch (wgetch(bottom_win)) {
		case 'G' :
		case 'g' :
			drawdisp(2);
			wnoutrefresh(main_win);
			menu_id = 0;
			*arrow_name = "General";
			disp_menu();
			wrefresh(menu_win);
			helpbox("General Watchit", menu_id);
			continue;

		case 'B' :
		case 'b' :
			drawdisp(4);
			wnoutrefresh(main_win);
			menu_id = 1;
			*arrow_name = "Buffer";
			disp_menu();
			wnoutrefresh(menu_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'C' :
		case 'c' :
			drawdisp(5);
			wnoutrefresh(main_win);
			menu_id = 2;
			*arrow_name = "CPU";
			disp_menu();
			wnoutrefresh(menu_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'F' :
		case 'f' :
			drawdisp(6);
			wnoutrefresh(main_win);
			menu_id = 3;
			*arrow_name = "File";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'I' :
		case 'i' :
			drawdisp(7);
			wnoutrefresh(main_win);
			menu_id = 4;
			*arrow_name = "Inode";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'M' :
		case 'm' :
			drawdisp(8);
			wnoutrefresh(main_win);
			menu_id = 5;
			*arrow_name = "Mount";
			disp_menu();
			wnoutrefresh(menu_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'P' :
		case 'p' :
			drawdisp(9);
			wnoutrefresh(main_win);
			menu_id = 6;
			*arrow_name = "Processes";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'R' :
		case 'r' :
			drawdisp(10);
			wnoutrefresh(main_win);
			menu_id = 7;
			*arrow_name = "Region";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'S' :
		case 's' :
			drawdisp(11);
			wnoutrefresh(main_win);
			menu_id = 8;
			*arrow_name = "Statistics";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'U' :
		case 'u' :
			drawdisp(12);
			wnoutrefresh(main_win);
			menu_id = 9;
			*arrow_name = "User";
			disp_menu();
			wnoutrefresh(menu_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'V' : /* Display and monitor sys performance */
		case 'v' :
			drawdisp(13);
			wnoutrefresh(main_win);
			menu_id = 10;
			*arrow_name = "Variables";
			disp_menu();
			wnoutrefresh(main_win);
			wrefresh(menu_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'Y' : /* Display and monitor sys performance */
		case 'y' :
			drawdisp(14);
			wnoutrefresh(main_win);
			menu_id = 11;
			*arrow_name = "sYsinfo";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'D' : /* Display and monitor sys performance */
		case 'd' :
			drawdisp(20);
			wnoutrefresh(main_win);
			menu_id = 12;
			*arrow_name = "Disk I/O";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'E' : /* Display and monitor sys performance */
		case 'e' :
			drawdisp(23);
			wnoutrefresh(main_win);
			menu_id = 13;
			*arrow_name = "strEam I/O";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case 'T' : /* Display and monitor sys performance */
		case 't' :
			drawdisp(18);
			wnoutrefresh(main_win);
			menu_id = 14;
			*arrow_name = "Term I/O";
			disp_menu();
			wnoutrefresh(main_win);
			helpbox(*arrow_name, menu_id);
			continue;

		case '+' :
		case KEY_UP:
		case KEY_RIGHT:
			if (menu_id <= MIN_MENU)
				menu_id = MAX_MENU;
			else
				menu_id--;
			disp_menu();
			use_arrow(menu_id);
			continue;

		case '-' :
		case KEY_DOWN:
		case KEY_LEFT:
			if (menu_id >= MAX_MENU)
				menu_id = MIN_MENU;
			else
				menu_id++;
			disp_menu();
			use_arrow(menu_id);
			continue;

		case RETURN:
			disp_menu();
			use_display(menu_id);
			wnoutrefresh(main_win);
			wnoutrefresh(menu_win);
			doupdate();
			helpbox(*arrow_name, menu_id);
			continue;

		case 'Q' :                      /* Quit and exit */
		case 'q' :
		case ESC:
			any_key();
			doupdate();
			wgetch(bottom_win);
			delwin(menu_win);
			delwin(help_win);
			return;
			break;
		}
	}
}


void use_arrow(int menu_id)
{

static char Sccsid[] = "@(#) use_arrow.c	Version 2.1 93/02/27 - Dapix.";

	*arrow_name = "          ";
	wattron(menu_win, A_BOLD);
	switch (menu_id) {
	case 0:
		mvwaddstr(menu_win, 1, 2, "General   ");
		*arrow_name = "General";
		break;

	case 1:
		mvwaddstr(menu_win, 2, 2, "Buffer    ");
		*arrow_name = "Buffer";
		break;

	case 2:
		mvwaddstr(menu_win, 3, 2, "CPU       ");
		*arrow_name = "CPU";
		break;

	case 3:
		mvwaddstr(menu_win, 4, 2, "File      ");
		*arrow_name = "File";
		break;

	case 4:
		mvwaddstr(menu_win, 5, 2, "Inode     ");
		*arrow_name = "Inode";
		break;

	case 5:
		mvwaddstr(menu_win, 6, 2, "Mount     ");
		*arrow_name = "Mount";
		break;

	case 6:
		mvwaddstr(menu_win, 7, 2, "Processes ");
		*arrow_name = "Processes";
		break;

	case 7:
		mvwaddstr(menu_win, 8, 2, "Region    ");
		*arrow_name = "Region";
		break;

	case 8:
		mvwaddstr(menu_win, 9, 2, "Statistics");
		*arrow_name = "Statistics";
		break;

	case 9:
		mvwaddstr(menu_win, 10, 2, "User      ");
		*arrow_name = "User";
		break;

	case 10:
		mvwaddstr(menu_win, 11, 2, "Variables ");
		*arrow_name = "Variables";
		break;

	case 11:
		mvwaddstr(menu_win, 12, 2, "sYsinfo   ");
		*arrow_name = "sYsinfo";
		break;

	case 12:
		mvwaddstr(menu_win, 13, 2, "Disk I/O  ");
		*arrow_name = "Disk I/O";
		break;

	case 13:
		mvwaddstr(menu_win, 14, 2, "strEam I/O");
		*arrow_name = "strEam I/O";
		break;

	case 14:
		mvwaddstr(menu_win, 15, 2, "Term I/O  ");
		*arrow_name = "Term I/O";
		break;

	}
	wattroff(menu_win, A_BOLD);
	wrefresh(menu_win);

}

void use_display(int menu_id)
{

static char Sccsid[] = "@(#) use_dislplay.c	Version 2.1 93/02/27 - Dapix.";

	wattron(menu_win, A_BOLD);
	switch (menu_id) {
	case 0:
		mvwaddstr(menu_win, 1, 2, "General");
		*arrow_name = "General";
		break;

	case 1:
		drawdisp(4);
		mvwaddstr(menu_win, 2, 2, "Buffer");
		*arrow_name = "Buffer";
		break;

	case 2:
		drawdisp(5);
		mvwaddstr(menu_win, 3, 2, "CPU");
		*arrow_name = "CPU";
		break;

	case 3:
		drawdisp(6);
		mvwaddstr(menu_win, 4, 2, "File");
		*arrow_name = "File";
		break;

	case 4:
		drawdisp(7);
		mvwaddstr(menu_win, 5, 2, "Inode");
		*arrow_name = "Inode";
		break;

	case 5:
		drawdisp(8);
		mvwaddstr(menu_win, 6, 2, "Mount");
		*arrow_name = "Mount";
		break;

	case 6:
		drawdisp(9);
		mvwaddstr(menu_win, 7, 2, "Processes");
		*arrow_name = "Processes";
		break;

	case 7:
		drawdisp(10);
		mvwaddstr(menu_win, 8, 2, "Region");
		*arrow_name = "Region";
		break;

	case 8:
		drawdisp(11);
		mvwaddstr(menu_win, 9, 2, "Statistics");
		*arrow_name = "Statistics";
		break;

	case 9:
		drawdisp(12);
		mvwaddstr(menu_win, 10, 2, "User");
		*arrow_name = "User";
		break;

	case 10:
		drawdisp(13);
		mvwaddstr(menu_win, 11, 2, "Variables");
		*arrow_name = "Variables";
		break;

	case 11:
		drawdisp(14);
		mvwaddstr(menu_win, 12, 2, "sYsinfo");
		*arrow_name = "sYsinfo";
		break;

	case 12:
		drawdisp(18);
		mvwaddstr(menu_win, 13, 2, "Disk I/O");
		*arrow_name = "Disk I/O";
		break;

	case 13:
		drawdisp(19);
		mvwaddstr(menu_win, 14, 2, "strEam I/O");
		*arrow_name = "strEam I/O";
		break;

	case 14:
		drawdisp(20);
		mvwaddstr(menu_win, 15, 2, "Term I/O");
		*arrow_name = "Term I/O";
		break;

	}
	wattroff(menu_win, A_BOLD);

}

void disp_menu(void)
{

static char Sccsid[] = "@(#) disp_menu.c	Version 2.1 93/02/27 - Dapix.";

	wattron(menu_win, A_NORMAL);
	mvwaddstr(menu_win, 1, 2, "General   ");
	mvwaddstr(menu_win, 2, 2, "Buffer    ");
	mvwaddstr(menu_win, 3, 2, "CPU       ");
	mvwaddstr(menu_win, 4, 2, "File      ");
	mvwaddstr(menu_win, 5, 2, "Inode     ");
	mvwaddstr(menu_win, 6, 2, "Mount     ");
	mvwaddstr(menu_win, 7, 2, "Processes ");
	mvwaddstr(menu_win, 8, 2, "Region    ");
	mvwaddstr(menu_win, 9, 2, "Statistics");
	mvwaddstr(menu_win, 10, 2, "User      ");
	mvwaddstr(menu_win, 11, 2, "Variables ");
	mvwaddstr(menu_win, 12, 2, "sYsinfo   ");
	mvwaddstr(menu_win, 13, 2, "Disk I/O  ");
	mvwaddstr(menu_win, 14, 2, "strEam I/O");
	mvwaddstr(menu_win, 15, 2, "Term I/O  ");
	box(menu_win, 0, 0);
	wattron(menu_win, A_REVERSE);
	mvwaddstr(menu_win, 0, 2, " WI help ");
	wattroff(menu_win, A_REVERSE);

	wattron(menu_win, A_BOLD);
	mvwprintw(menu_win,menu_id + 1, 2, "%s", *arrow_name);
	wattroff(menu_win, A_BOLD);
}

void helpbox(char *help_id, int screen_num)
{

static char Sccsid[] = "@(#) helpbox.c	Version 2.1 93/02/27 - Dapix.";

	touchwin(menu_win);
	wnoutrefresh(menu_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);
	wattron(help_win, A_REVERSE);
	mvwprintw(help_win, 0, 45, " %s help ",help_id);
	wattroff(help_win, A_REVERSE);
	wnoutrefresh(help_win);
	(*help_screen[screen_num])();
}

/*
	The help screens are from here down.
*/
void  help_gen(void)
{

static char Sccsid[] = "@(#) help_gen.c	Version 2.1 93/02/27 - Dapix.";

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "Watchit is a UNIX system/performance monitor, which allows");
	mvwaddstr(help_win, 3, 2, "the user of wi to see what is happening on their UNIX system.");
	mvwaddstr(help_win, 5, 2, "From the MAIN MENU any of the highlighted selections can be");
	mvwaddstr(help_win, 6, 2, "selected, along with H for help, A for the admin menu, ");
	mvwaddstr(help_win, 7, 2, "^L or ^R to redraw the screen and the arrow keys.");
	mvwaddstr(help_win, 8, 2, "(The arrow keys perform the same function as + and -).");
	mvwaddstr(help_win, 10, 2, "See the appropriate section for more information about each");
	mvwaddstr(help_win, 11, 2, "heading.");

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 6, 23, "H");
	mvwaddstr(help_win, 6, 35, "A");
	mvwaddstr(help_win, 7, 2, "^L");
	mvwaddstr(help_win, 7, 8, "^R");
	mvwaddstr(help_win, 8, 47, "+");
	mvwaddstr(help_win, 8, 53, "-");
	wattroff(help_win, A_REVERSE);

	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);
}

void    help_buf(void)
{

static char Sccsid[] = "@(#) help_buf.c	Version 2.1 93/02/27 - Dapix.";

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 1, 2, "The buffer option displays the system buffer pool along with");
	mvwaddstr(help_win, 2, 2, "their device position and state. A buffer is available for");
	mvwaddstr(help_win, 3, 2, "use only when it is not flagged busy. You can use Page up, ");
	mvwaddstr(help_win, 4, 2, "Page Down or the arrow keys to move around the screen.");
	mvwaddstr(help_win, 5, 2, "The D key will display device numbers and not the names, on");
	mvwaddstr(help_win, 6, 2, "some systems this will be quicker. C will display the cache.");
	mvwaddstr(help_win, 8, 14, "- slot in buffer pool");
	mvwaddstr(help_win, 9, 14, "- major device number");
	mvwaddstr(help_win, 10, 14, "- minor device number");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 5, 6, "D");
	mvwaddstr(help_win, 6, 37, "C");
	mvwaddstr(help_win, 8, 2, "Slot");
	mvwaddstr(help_win, 9, 2, "Maj");
	mvwaddstr(help_win, 10, 2, "Min");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 32, " ");
	wnoutrefresh(bottom_win);
	doupdate();
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Buffer help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 1, 14, "- the entry in /dev for major and minor");
	mvwaddstr(help_win, 2, 14, "- block number on device");
	mvwaddstr(help_win, 3, 14, "- address of disk block");
	mvwaddstr(help_win, 4, 14, "- block transfer count");
	mvwaddstr(help_win, 5, 14, "- errors returned on I/O");
	mvwaddstr(help_win, 8, 16, "- flag for AIO");
	mvwaddstr(help_win, 9, 16, "- ffs buffer on raw I/O");
	mvwaddstr(help_win, 10, 16, "- remote RFS data");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 1, 2, "Device name");
	mvwaddstr(help_win, 2, 2, "Block");
	mvwaddstr(help_win, 3, 2, "Address");
	mvwaddstr(help_win, 4, 2, "Bcnt");
	mvwaddstr(help_win, 5, 2, "E");
	mvwaddstr(help_win, 7, 2, "Flags:");
	mvwaddstr(help_win, 8, 8, "a_i/o");
	mvwaddstr(help_win, 9, 8, "raw_i/o");
	mvwaddstr(help_win, 10, 8, "remote");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 32, " ");
	wnoutrefresh(bottom_win);
	doupdate();
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Buffer help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 1, 2, "Buffer pool flag descriptions");
	mvwaddstr(help_win, 3, 16, "- non-read pseudo-flag");
	mvwaddstr(help_win, 4, 16, "- read when I/O occurs");
	mvwaddstr(help_win, 5, 16, "- transaction finished");
	mvwaddstr(help_win, 6, 16, "- transaction aborted");
	mvwaddstr(help_win, 7, 16, "- not on av_forw/back list");
	mvwaddstr(help_win, 8, 16, "- physical I/O potentially using UNIBUS map");
	mvwaddstr(help_win, 9, 16, "- block on UNIBUS map");
	mvwaddstr(help_win, 10, 16, "- issue wakeup when busy goes off");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 3, 8, "write");
	mvwaddstr(help_win, 4, 8, "read");
	mvwaddstr(help_win, 5, 8, "done");
	mvwaddstr(help_win, 6, 8, "error");
	mvwaddstr(help_win, 7, 8, "busy");
	mvwaddstr(help_win, 8, 8, "phys");
	mvwaddstr(help_win, 9, 8, "map");
	mvwaddstr(help_win, 10, 8, "wanted");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 32, " ");
	wnoutrefresh(bottom_win);
	doupdate();
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Buffer help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 1, 2, "Buffer pool flag descriptions");
	mvwaddstr(help_win, 3, 16, "- delayed write for correct aging");
	mvwaddstr(help_win, 4, 16, "- don't wait for I/O completion");
	mvwaddstr(help_win, 5, 16, "- don't write until blk leaves available list");
	mvwaddstr(help_win, 6, 16, "- open routine called");
	mvwaddstr(help_win, 7, 16, "- old block");
	mvwaddstr(help_win, 8, 16, "- transfer cannot cross 64k boundary");
	mvwaddstr(help_win, 9, 16, "- write queued by bflush");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 3, 6, "age");
	mvwaddstr(help_win, 4, 6, "async");
	mvwaddstr(help_win, 5, 6, "del/write");
	mvwaddstr(help_win, 6, 6, "open");
	mvwaddstr(help_win, 7, 6, "stale");
	mvwaddstr(help_win, 8, 6, "nocross");
	mvwaddstr(help_win, 9, 6, "flush");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);
}

void    help_cup(void)
{

static char Sccsid[] = "@(#) help_screens.c	Version 2.1 93/02/27 - Dapix.";

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 1, 2, "This screen displays the CPU, I/0, wait and memory statistics");
	mvwaddstr(help_win, 2, 2, "for a second, and if the W key is pressed the system activity");
	mvwaddstr(help_win, 3, 2, "will be monitored. When in \"Watchit\" mode the arrow and");
	mvwaddstr(help_win, 4, 2, "+ and - keys can be used to change the number of seconds");
	mvwaddstr(help_win, 5, 2, "between samples.");
	mvwaddstr(help_win, 7, 2, "The CPU statistics are shown in bar chart form and in");
	mvwaddstr(help_win, 8, 2, "percentage form, the column titled \"used\" is the current");
	mvwaddstr(help_win, 9, 2, "values and the column titled \"avg\" is the average since the");
	mvwaddstr(help_win, 10, 2, "monitoring began. If the screen is in long mode then");
	mvwaddstr(help_win, 11, 2, "the cpu is displayed including the wait states.    ");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 4, 2, "+");
	mvwaddstr(help_win, 4, 8, "-");
	mvwaddstr(help_win, 2, 27, "W");
	wattroff(help_win, A_REVERSE);

	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 3, 30, "Watchit");
	mvwaddstr(help_win, 8, 38, "used");
	mvwaddstr(help_win, 9, 32, "avg");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);
}

void    help_file(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The file option displays the system file list along with");
	mvwaddstr(help_win, 3, 2, "the current byte being pointed to.");
	mvwaddstr(help_win, 4, 2, "This screen can also be put into \"watchit\" mode, in");
	mvwaddstr(help_win, 5, 2, "this mode you can watch files grow, and get updated");
	mvwaddstr(help_win, 6, 2, "Page up, Page Down, and the arrow keys will let you");
	mvwaddstr(help_win, 7, 2, "move around the file list.");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " File help ");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "Below is a list of the the field along with their");
	mvwaddstr(help_win, 3, 2, "description.                                      ");
	mvwaddstr(help_win, 5, 14, "- position in f-list");
	mvwaddstr(help_win, 6, 14, "- number of current opens to the file");
	mvwaddstr(help_win, 7, 14, "- entry in i-list");
	mvwaddstr(help_win, 8, 14, "- current file pointer position in file");
	mvwaddstr(help_win, 9, 14, "- next entry in free the list");
	mvwaddstr(help_win, 10, 14, "- umask in octal");
	mvwaddstr(help_win, 11, 14, "- file state flags");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 5, 2, "Slot");
	mvwaddstr(help_win, 6, 2, "Count");
	mvwaddstr(help_win, 7, 2, "Inode");
	mvwaddstr(help_win, 8, 2, "Byte offset");
	mvwaddstr(help_win, 9, 2, "Freelist");
	mvwaddstr(help_win, 10, 2, "umask");
	mvwaddstr(help_win, 11, 2, "Flags");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_inode(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The inode option displays an extended system inode list");
	mvwaddstr(help_win, 3, 2, "showing the devices and associated files.");
	mvwaddstr(help_win, 5, 12, "- position in i-list");
	mvwaddstr(help_win, 6, 12, "- device where inode resides");
	mvwaddstr(help_win, 7, 12, "- inode number");
	mvwaddstr(help_win, 8, 12, "- reference count");
	mvwaddstr(help_win, 9, 12, "- directory entries for file");
	mvwaddstr(help_win, 10, 12, "- owner of file ");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 5, 2, "Slot");
	mvwaddstr(help_win, 6, 2, "M_Device");
	mvwaddstr(help_win, 7, 2, "Inumb");
	mvwaddstr(help_win, 8, 2, "Ref");
	mvwaddstr(help_win, 9, 2, "Lnk");
	mvwaddstr(help_win, 10, 2, "Uid");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Inode help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 10, "- group of owner of file ");
	mvwaddstr(help_win, 3, 10, "- size of file in bytes");
	mvwaddstr(help_win, 4, 10, "- file type and mode");
	mvwaddstr(help_win, 5, 10, "- file name if in /dev or Major & Minor Number");
	mvwaddstr(help_win, 6, 10, "- file type, p=pipe, f=file, d=dir, c/b=device");
	mvwaddstr(help_win, 8, 10, "lck=inode locked, upd=file updated, txt=pure text");
	mvwaddstr(help_win, 9, 10, "acc=time to update, mnt=is mounted, ir=is root device");
	mvwaddstr(help_win, 10, 10, "wnt=a processes waiting on lock, acc=time to update");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "Gid");
	mvwaddstr(help_win, 3, 2, "Size");
	mvwaddstr(help_win, 4, 2, "Mode");
	mvwaddstr(help_win, 5, 2, "Device");
	mvwaddstr(help_win, 6, 2, "T");
	mvwaddstr(help_win, 8, 2, "Flags:");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_mount(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The Mount option displays the mounted file systems if");
	mvwaddstr(help_win, 3, 2, "\"Watchit\" is  selected by  pressing the W key then the file");
	mvwaddstr(help_win, 4, 2, "system activity can be watched. (The + and - keys along with");
	mvwaddstr(help_win, 5, 2, "This screen can be in one of three formats depending on the");
	mvwaddstr(help_win, 6, 2, "number of filesystems mounted. 1 filesystem bar charts are");
	mvwaddstr(help_win, 7, 2, "displayed for blocks and inodes, 2 -> 3 filesystems a bar");
	mvwaddstr(help_win, 8, 2, "chart for blocks is displayed. and 4+ only text is displayed");
	mvwaddstr(help_win, 10, 10, "- the mount table entry number");
	mvwaddstr(help_win, 11, 10, "- major number of device");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 3, 42, "W");
	mvwaddstr(help_win, 4, 39, "+");
	mvwaddstr(help_win, 4, 45, "-");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 3, 3, "Watchit");
	mvwaddstr(help_win, 10, 2, "Slot");
	mvwaddstr(help_win, 11, 2, "Maj");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Mount help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 10, "- minor number of device");
	mvwaddstr(help_win, 3, 10, "- file system type flag");
	mvwaddstr(help_win, 4, 10, "- block size of file system");
	mvwaddstr(help_win, 5, 10, "- file system flags");
	mvwaddstr(help_win, 6, 10, "- mount pointer");
	mvwaddstr(help_win, 7, 10, "- the /dev device name.");
	mvwaddstr(help_win, 8, 10, "- total number of 1K disk blocks");
	mvwaddstr(help_win, 9, 10, "- free number of 1K disk blocks");
	mvwaddstr(help_win, 10, 10, "- total number of inodes");
	mvwaddstr(help_win, 11, 10, "- free number of inodes");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "Min");
	mvwaddstr(help_win, 3, 2, "FS");
	mvwaddstr(help_win, 4, 2, "BSIZ");
	mvwaddstr(help_win, 5, 2, "Flags");
	mvwaddstr(help_win, 6, 2, "Mptr");
	mvwaddstr(help_win, 7, 2, "Device");
	mvwaddstr(help_win, 8, 2, "Blocks");
	mvwaddstr(help_win, 9, 2, "Bfree");
	mvwaddstr(help_win, 10, 2, "Inodes");
	mvwaddstr(help_win, 11, 2, "Ifree");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);
}

void    help_procs(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 1, 2, "The Proc option displays the current process table. This    ");
	mvwaddstr(help_win, 2, 2, "table can be put into \"watchit\" mode in one of two ways");
	mvwaddstr(help_win, 3, 2, "W will watch all procs, including the free slots, and w will");
	mvwaddstr(help_win, 4, 2, "exclude free slots. Use the D key to detail the slot.");
	mvwaddstr(help_win, 6, 14, "- the processes slot id");
	mvwaddstr(help_win, 7, 14, "- process state, s=sleeping, r=running, z=zombie");
	mvwaddstr(help_win, 8, 14, "- cpu usage for scheduling");
	mvwaddstr(help_win, 9, 14, "- system priority");
	mvwaddstr(help_win, 10, 14, "- nice value");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 3, 2, "W");
	mvwaddstr(help_win, 3, 56, "w");
	mvwaddstr(help_win, 4, 30, "D");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 6, 2, "Slot");
	mvwaddstr(help_win, 7, 2, "State");
	mvwaddstr(help_win, 8, 2, "CPU");
	mvwaddstr(help_win, 9, 2, "Priority");
	mvwaddstr(help_win, 10, 2, "Nice");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Processes help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 14, "- process id");
	mvwaddstr(help_win, 3, 14, "- parent process id");
	mvwaddstr(help_win, 4, 14, "- name of process group leader");
	mvwaddstr(help_win, 5, 14, "- real user id");
	mvwaddstr(help_win, 6, 14, "- effective user id");
	mvwaddstr(help_win, 7, 14, "- effective group id");
	mvwaddstr(help_win, 8, 14, "- control tty of process");
	mvwaddstr(help_win, 9, 14, "- the event on which the process is waiting");
	mvwaddstr(help_win, 10, 14, "- time to alarm clock signal");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "Pid");
	mvwaddstr(help_win, 3, 2, "Ppid");
	mvwaddstr(help_win, 4, 2, "Grp leader");
	mvwaddstr(help_win, 5, 2, "Uid");
	mvwaddstr(help_win, 6, 2, "Suid");
	mvwaddstr(help_win, 7, 2, "Sgid");
	mvwaddstr(help_win, 8, 2, "Ctrl tty");
	mvwaddstr(help_win, 9, 2, "Wchan");
	mvwaddstr(help_win, 10, 2, "Sec/alarm");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Processes help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 14, "- resident time to schedule");
	mvwaddstr(help_win, 3, 14, "- current state of the processes");
	mvwaddstr(help_win, 4, 14, "- the command running");
	mvwaddstr(help_win, 6, 2, "Extended screen description.");
	mvwaddstr(help_win, 7, 14, "- major and minor numbers of ctrl tty name");
	mvwaddstr(help_win, 8, 14, "- text, data and stack sizes");
	mvwaddstr(help_win, 9, 14, "- start time of command");
	mvwaddstr(help_win, 10, 14, "- the command running");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "Sec/sched");
	mvwaddstr(help_win, 3, 2, "Flags");
	mvwaddstr(help_win, 4, 2, "Command");
	mvwaddstr(help_win, 7, 2, "tty number");
	mvwaddstr(help_win, 8, 2, "Block size");
	mvwaddstr(help_win, 9, 2, "Start time");
	mvwaddstr(help_win, 10, 2, "Command");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_reg(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The Reg option displays the system region table. Like most");
	mvwaddstr(help_win, 3, 2, "other screens this table cam be put into the \"Watchit\"");
	mvwaddstr(help_win, 4, 2, "mode. Page up, Page down and the arrow keys function.");
	mvwaddstr(help_win, 6, 14, "- the region table slot id");
	mvwaddstr(help_win, 7, 14, "- size in pages of region");
	mvwaddstr(help_win, 8, 14, "- Nbr of valid pages in region");
	mvwaddstr(help_win, 9, 14, "- Nbr of users pointing at region");
	mvwaddstr(help_win, 10, 14, "- Nbr of procs sent hold request");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 6, 2, "Slot");
	mvwaddstr(help_win, 7, 2, "Psz");
	mvwaddstr(help_win, 8, 2, "#vl");
	mvwaddstr(help_win, 9, 2, "Rct");
	mvwaddstr(help_win, 10, 2, "Swp");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Region help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 14, "- size in bytes of section of file");
	mvwaddstr(help_win, 3, 14, "- forward link");
	mvwaddstr(help_win, 4, 14, "- backward link");
	mvwaddstr(help_win, 5, 14, "- pointer to inode");
	mvwaddstr(help_win, 6, 14, "- priv=private, stxt=shared text, shmm=shared mem");
	mvwaddstr(help_win, 7, 14, "- address of list of pointers");
	/* mvwaddstr(help_win, 8, 14, " "); */
 	mvwaddstr(help_win, 9, 14, "nofr=don't free region, done=region initialised, ");
	mvwaddstr(help_win, 10, 14, "stack=stack type, age=old region, want=reg wanted");
	mvwaddstr(help_win, 11, 14, "swap=swapped to disk, lock=locked reg");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "Fsize");
	mvwaddstr(help_win, 3, 2, "Forw");
	mvwaddstr(help_win, 4, 2, "Back");
	mvwaddstr(help_win, 5, 2, "Ino");
	mvwaddstr(help_win, 6, 2, "Type");
	mvwaddstr(help_win, 7, 2, "Listaddr");
	mvwaddstr(help_win, 8, 2, "Flags:");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);
}

void    help_stats(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 1, 2, "The stats option displays the system statistics");
	mvwaddstr(help_win, 3, 16, "- the name of the O/S running");
	mvwaddstr(help_win, 4, 16, "- the release of the O/S running");
	mvwaddstr(help_win, 5, 16, "- the BUS type");
	mvwaddstr(help_win, 6, 16, "- the processor type");
	mvwaddstr(help_win, 7, 16, "- the kernel node name");
	mvwaddstr(help_win, 8, 16, "- the SCO serial number");
	mvwaddstr(help_win, 9, 16, "- the number of user licence");
	mvwaddstr(help_win, 10, 16, "- current system time");
	mvwaddstr(help_win, 11, 16, "- time since last boot");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 3, 2, "Sysname");
	mvwaddstr(help_win, 4, 2, "Release");
	mvwaddstr(help_win, 5, 2, "BUS Type");
	mvwaddstr(help_win, 6, 2, "Machine");
	mvwaddstr(help_win, 7, 2, "Nodename");
	mvwaddstr(help_win, 8, 2, "Serial");
	mvwaddstr(help_win, 9, 2, "# users");
	mvwaddstr(help_win, 10, 2, "Current time");
	mvwaddstr(help_win, 11, 2, "Age of system");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_user(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 1, 2, "The user option displays the system user list information");
	mvwaddstr(help_win, 2, 2, "one item at a time. This screen like namy others will run");
	mvwaddstr(help_win, 3, 2, "in watchit mode by using the \"W\" key.                  ");
	mvwaddstr(help_win, 5, 15, "- user, group, real user and real group id's");
	mvwaddstr(help_win, 6, 15, "- process time and control tty name");
	mvwaddstr(help_win, 7, 15, "- the state of process locks");
	mvwaddstr(help_win, 8, 15, "- command and process information");
	mvwaddstr(help_win, 9, 15, "- file and file I/O information");
	mvwaddstr(help_win, 10, 15, "- the Nbr of open files and file slot entry");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 5, 2, "User/Grp ID");
	mvwaddstr(help_win, 6, 2, "Process Info");
	mvwaddstr(help_win, 7, 2, "Misc Info");
	mvwaddstr(help_win, 8, 2, "Locks");
	mvwaddstr(help_win, 9, 2, "File Info");
	mvwaddstr(help_win, 10, 2, "Open Files");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_var(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The variable option displays the configured and current");
	mvwaddstr(help_win, 3, 2, "system parameters for UNIX. The variable menu is split into");
	mvwaddstr(help_win, 4, 2, "seven sub-menus, each displaying different parameters.");
	mvwaddstr(help_win, 5, 2, "The system can monitor the number of inodes, files, processes");
	mvwaddstr(help_win, 6, 2, "and file systems if the M key is pressed. Like the other");
	mvwaddstr(help_win, 7, 2, "screens the + and - keys along with the arrow keys");
	mvwaddstr(help_win, 8, 2, "change the sample time from 1 to 99 seconds.");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 6, 26, "W");
	mvwaddstr(help_win, 7, 14, "+");
	mvwaddstr(help_win, 7, 20, "-");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_sys(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The sysinfo option displays the systems statistics for a");
	mvwaddstr(help_win, 3, 2, "variety of system variables. When the M key is pressed");
	mvwaddstr(help_win, 4, 2, "the programme starts to monitor the system and the highest");
	mvwaddstr(help_win, 5, 2, "value of each parameter is displayed over the sample time.");
	mvwaddstr(help_win, 7, 14, "- the logical number of buffers read");
	mvwaddstr(help_win, 8, 14, "- the logical number of buffers written");
	mvwaddstr(help_win, 9, 14, "- the actual number of buffers read");
	mvwaddstr(help_win, 10, 14, "- the actual number of buffers written");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 3, 40, "W");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 7, 2, "lread");
	mvwaddstr(help_win, 8, 2, "bwrite");
	mvwaddstr(help_win, 9, 2, "bread");
	mvwaddstr(help_win, 10, 2, "bwrite");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Sysinfo help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 14, "- Nbr of logical blocks swapped in");
	mvwaddstr(help_win, 3, 14, "- Nbr of logical blocks swapped out");
	mvwaddstr(help_win, 4, 14, "- Nbr of actual blocks swapped in");
	mvwaddstr(help_win, 5, 14, "- Nbr of actual blocks swapped out");
	mvwaddstr(help_win, 6, 14, "- system calls per second");
	mvwaddstr(help_win, 7, 14, "- chars read by the read() system call");
	mvwaddstr(help_win, 8, 14, "- chars written by the write() system call");
	mvwaddstr(help_win, 9, 14, "- Nbr of fork() system calls");
	mvwaddstr(help_win, 10, 14, "- Nbr of exec's per second");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "swapin");
	mvwaddstr(help_win, 3, 2, "swapout");
	mvwaddstr(help_win, 4, 2, "bswapin");
	mvwaddstr(help_win, 5, 2, "bswapout");
	mvwaddstr(help_win, 6, 2, "syscall");
	mvwaddstr(help_win, 7, 2, "sysread");
	mvwaddstr(help_win, 8, 2, "syswrit");
	mvwaddstr(help_win, 9, 2, "sysfork");
	mvwaddstr(help_win, 10, 2, "sysexec");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for the next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 36, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Sysinfo help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 14, "- chars read by the read() system call");
	mvwaddstr(help_win, 3, 14, "- chars written by the write() system call");
	mvwaddstr(help_win, 4, 14, "- Nbr of raw tty chars read");
	mvwaddstr(help_win, 5, 14, "- Nbr of raw tty chars put to a canonical queue");
	mvwaddstr(help_win, 6, 14, "- Nbr of tty chars written");
	mvwaddstr(help_win, 7, 14, "- Nbr of get inodes");
	mvwaddstr(help_win, 8, 14, "- Nbr of times looked up the path from inode");
	mvwaddstr(help_win, 9, 14, "- Nbr of directory blocks read");
	mvwaddstr(help_win, 10, 14, "- Nbr of process context switch");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "readch");
	mvwaddstr(help_win, 3, 2, "writech");
	mvwaddstr(help_win, 4, 2, "rawch");
	mvwaddstr(help_win, 5, 2, "canch");
	mvwaddstr(help_win, 6, 2, "outch");
	mvwaddstr(help_win, 7, 2, "iget");
	mvwaddstr(help_win, 8, 2, "namei");
	mvwaddstr(help_win, 9, 2, "dirblk");
	mvwaddstr(help_win, 10, 2, "pswitch");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 32, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Sysinfo help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 14, "- the number of processes in the run queue");
	mvwaddstr(help_win, 3, 14, "- tty interrupts received");
	mvwaddstr(help_win, 4, 14, "- tty interrupts transmitted");
	mvwaddstr(help_win, 5, 14, "- other line interrupts (modem)");
	mvwaddstr(help_win, 6, 14, "- Nbr of translation faults");
	mvwaddstr(help_win, 7, 14, "- Nbr of demand zero & demand fill pages");
	mvwaddstr(help_win, 8, 14, "- Nbr of protection faults");
	mvwaddstr(help_win, 9, 14, "- Nbr of copy on writes");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "run que");
	mvwaddstr(help_win, 3, 2, "rcvint");
	mvwaddstr(help_win, 4, 2, "xmtint");
	mvwaddstr(help_win, 5, 2, "mdmint");
	mvwaddstr(help_win, 6, 2, "vfault");
	mvwaddstr(help_win, 7, 2, "demand");
	mvwaddstr(help_win, 8, 2, "pfault");
	mvwaddstr(help_win, 9, 2, "copy/w");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wrefresh(help_win);

	mvwaddstr(bottom_win, 1, 20, "                                             ");
	mvwaddstr(bottom_win, 1, 2, " Press any key for next page:");
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 9, "any key");
	wattroff(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 32, " ");
	wrefresh(bottom_win);
	wgetch(bottom_win);
	wmove(help_win, 1, 1);
	wclrtobot(help_win);
	box(help_win, 0, 0);

	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 0, 45, " Sysinfo help ");
	wattroff(help_win, A_REVERSE);
	mvwaddstr(help_win, 2, 14, "- Nbr of pages freed by vhand");
	mvwaddstr(help_win, 3, 14, "- Nbr of pages freed by sched");
	mvwaddstr(help_win, 4, 14, "- Nbr of pages swapped by vhand");
	mvwaddstr(help_win, 5, 14, "- Nbr of pages swapped by vhand");
	mvwaddstr(help_win, 6, 14, "- Nbr of pages freed");
	mvwaddstr(help_win, 7, 14, "- Nbr of pages stolen");
	mvwaddstr(help_win, 8, 14, "- Nbr of pages on swap");
	mvwaddstr(help_win, 9, 14, "- Nbr of pages on file");
	mvwaddstr(help_win, 10, 14, "- Nbr of pages in cache");
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 2, 2, "vf page");
	mvwaddstr(help_win, 3, 2, "sf page");
	mvwaddstr(help_win, 4, 2, "vs page");
	mvwaddstr(help_win, 5, 2, "ss page");
	mvwaddstr(help_win, 6, 2, "pg freed");
	mvwaddstr(help_win, 7, 2, "pg steal");
	mvwaddstr(help_win, 8, 2, "pg swap");
	mvwaddstr(help_win, 9, 2, "pg file");
	mvwaddstr(help_win, 10, 2, "pg cache");
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);
}

void    help_disk(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "This screen displayes the disk, tape and floppy drive");
	mvwaddstr(help_win, 3, 2, "stats. If the screen is in long mode then the system cache");
	mvwaddstr(help_win, 4, 2, "stats. are also dispayed. You can either take a snap shot");
	mvwaddstr(help_win, 5, 2, "of the system by using the U key of use W to run in ");
	mvwaddstr(help_win, 6, 2, "Watchit mode.");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 5, 29, "U");
	mvwaddstr(help_win, 5, 42, "W");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_stream(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The stream option displays the configured and current");
	mvwaddstr(help_win, 3, 2, "system stream parameters. This table can be eitherupdated");
	mvwaddstr(help_win, 4, 2, "or put into \"Watchit\" mode by using the U or W keys.");
	mvwaddstr(help_win, 5, 2, "The screen will display all the current totals along with");
	mvwaddstr(help_win, 6, 2, "the number of fails and the maximum number used showed in");
	mvwaddstr(help_win, 7, 2, "a percentage format. If you are using the long screen then");
	mvwaddstr(help_win, 8, 2, "more stream info is displayed.                     ");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 4, 42, "U");
	mvwaddstr(help_win, 4, 47, "W");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

void    help_tty(void)
{

	wattron(help_win, A_NORMAL);
	mvwaddstr(help_win, 2, 2, "The tty option displays the configured and current serial");
	mvwaddstr(help_win, 3, 2, "I/O devices, these devices are displayed only whenthey are ");
	mvwaddstr(help_win, 4, 2, "enabled. The upper case tty name will show up the same");
	mvwaddstr(help_win, 5, 2, "as the lower case tty. ");
	mvwaddstr(help_win, 6, 2, "This screen will also run in \"Watchit\" mode if the \"W\"");
	mvwaddstr(help_win, 7, 2, "key is used. ");
	wattron(help_win, A_REVERSE);
	mvwaddstr(help_win, 6, 54, "W");
	wattroff(help_win, A_REVERSE);
	wattron(help_win, A_BOLD);
	mvwaddstr(help_win, 12, 36, "(C)opyright  - Dapix 1993.");
	wattroff(help_win, A_BOLD);
	wnoutrefresh(help_win);
	any_key();
	doupdate();
	wgetch(bottom_win);

}

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
	drawwindow(3);

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

	mvwaddstr(bottom_win, 1, 8, ": Any key to continue:   ");
	wclrtoeol(bottom_win);
	wattron(bottom_win, A_BOLD);
	mvwaddstr(bottom_win, 1, 2, "Select");
	wattroff(bottom_win, A_BOLD);
	wattron(bottom_win, A_REVERSE);
	mvwaddstr(bottom_win, 1, 10, "Any key");
	wattroff(bottom_win, A_REVERSE);
	drawwindow(3);
	wattron(bottom_win, A_BLINK);
	mvwaddstr(bottom_win, 1, 30, "_");
	wattroff(bottom_win, A_BLINK);
	wmove(bottom_win, 1, 30);
	wnoutrefresh(bottom_win);
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
