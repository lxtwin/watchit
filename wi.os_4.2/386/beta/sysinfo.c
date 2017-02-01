/*
 * +--------------------------------------------------------------------+
 * | Function: sysinfo.c                                 Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the system activity                                    |
 * +--------------------------------------------------------------------+
 *
 * Notes:
 *	I will have to use minfo for more information on the kernel as 
 *	there is a lot more to it.
 *
 * Updates:   
 *		A total re-write.
 *
 *    Bugs:
 *    v2.3	Move the Cur(3) header one space <-.		
 *					Sat Jul 17 18:48:14 EDT 1993 - PKR. / Done.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix"
#endif /* __STDC__ */


#include "wi.h"

#include <sys/sysinfo.h>
#include <sys/dir.h>
#include <sys/var.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/user.h>
#include <sys/immu.h>
#include <sys/region.h>
#include <sys/proc.h>
#include <signal.h>

void monitor_info(void);
void display_info(void);

int  Change_Sysinfo_Type(int *type_flag);
void watch_Sysinfo(int *sysinfo_type);

void get_sysinfo_stats(void);
void update_sysinfo_stats(int sysinfo_type);
void Draw_Sysinfo(void);

void Do_Sysinfo_Stats(void);
void sysinfo_header(int sysinfo_type);

static void sig_alrm_sysinfo(int signo);

/*
 *	Saved structurs for the totals
 */
struct sysinfo	max_timed_sysinfo;	/* kernel max sysinf struct	*/
struct minfo	max_timed_minfo;		/* kernel minfo max structre	*/

struct sysinfo	max_total_sysinfo;	/* kernel max sysinf struct	*/
struct minfo	max_total_minfo;		/* kernel minfo max structre	*/

#define BY_CURRENT	0
#define BY_TIMED		1
#define BY_TOTAL		2

#define COL_1	 2
#define COL_2	 28
#define COL_3	 56

#define LONG_Y 18

extern short demo_pid;
extern int avg_total;

int avg_total;

extern time_t current_idle;
extern time_t current_user;
extern time_t current_kernel;
extern time_t current_wait;
extern time_t current_total;
extern time_t current_total2;

extern time_t current_sxbrk;
extern time_t current_io;
extern time_t current_wswap;
extern time_t current_pio;
extern time_t current_wait_total;

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %Z% |
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

int sysinfo_menu(void)
{

	extern int compressed_flag;
	int sysinfo_type;

	sysinfo_type = BY_CURRENT;
	compressed_flag = 0;

	drawdisp(14);
	mvwaddstr(main_win, 2, 4, "Reading /unix..........");
	wrefresh(main_win);
	mvwaddstr(main_win, 2, 4, "                         ");

	get_sysinfo_stats();
	sleep(1);
	update_sysinfo_stats(sysinfo_type);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		Set_Colour(bottom_win, Normal);
		mvwaddstr(bottom_win, 1, 1, "       : Watchit or Quit:");
		w_clrtoeol(bottom_win, 0, 0);
		Set_Colour(bottom_win, Colour_White);
		mvwaddstr(bottom_win, 1, 2, "Select");
		Set_Colour(bottom_win, Colour_Banner);
		mvwaddstr(bottom_win, 1, 10, "W");
		mvwaddstr(bottom_win, 1, 21, "Q");
		Set_Colour(bottom_win, Normal);
		Set_Colour(main_win, Normal);

		wmove(bottom_win, 1, 27);
		wnoutrefresh(top_win);
		doupdate();
		switch (wgetch(bottom_win)) {
		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			continue;

		case 'P' :
		case 'p' :
			screen_dump();
			continue;

		case 't' :
		case 'c' :
		case 'a' :
		case 'T' :
		case 'C' :
		case 'A' :
			Change_Sysinfo_Type(&sysinfo_type);
			mvwaddstr(main_win, 1, 7, "Reading /unix..........                              ");
			wmove(main_win, 1, 30);
			wnoutrefresh(main_win);
			doupdate();
			mvwaddstr(main_win, 1, 7, "                                                     ");
			get_sysinfo_stats();
			sleep(1);
			update_sysinfo_stats(sysinfo_type);
			break;

		case 'R' :
		case 'r' :
			cpu_averages[_AVG_CPU_IDLE] 	 = 0;
			cpu_averages[_AVG_CPU_USER] 	 = 0;
			cpu_averages[_AVG_CPU_KERNEL]	 = 0;
			cpu_averages[_AVG_CPU_WAIT]	 = 0;
			cpu_averages[_AVG_CPU_SXBRK]	 = 0;
			cpu_averages[_AVG_CPU_TOTAL]	 = 0;
			cpu_averages[_AVG_CPU_TOTAL_2] = 0;

			cpu_averages[_AVG_W_IO]		 = 0;
			cpu_averages[_AVG_W_SWAP]	 = 0;
			cpu_averages[_AVG_W_PIO]	 = 0;
			cpu_averages[_AVG_W_TOTAL]	 = 0;

			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			continue;

		case 'U' :
		case 'u' :
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			get_sysinfo_stats();
			sleep(1);
			update_sysinfo_stats(sysinfo_type);
			break;

		case 'W' :
		case 'w' :
			drawdisp(32);
			watch_Sysinfo(&sysinfo_type);
			drawdisp(14);
			wrefresh(main_win);
			continue;

		case 'Q' :
		case 'q' :
			return(0);

		default :
			continue;

		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int header(void)                        Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       the main sysinfo loop                                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void sysinfo_header(int sysinfo_type)
{

	int position = 3;
	register int ctr = 0;

	for (ctr = 1; ctr < (page_len + 1); ctr++)
		mvwaddstr(main_win, ctr, 2, "                                                                           ");

	/*
		Now display the buggers.
	*/

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 1, 24, "System Activity by ");

	w_clrtoeol(main_win, 0, 0);

	switch (sysinfo_type) {
	case BY_TIMED :
		mvwaddstr(main_win, 1, 40, "per Period.");
		break;

	case BY_TOTAL :
		mvwaddstr(main_win, 1, 43, "Total.");
		break;

	case BY_CURRENT :
	default :
		mvwaddstr(main_win, 1, 40, "per Second.");
		break;
	}

	Set_Colour(main_win, Normal);
	wnoutrefresh(main_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/03/13 |
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

static void sig_alrm_sysinfo(int signo)
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/03/13 |
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

void watch_Sysinfo(int *sysinfo_type)
{

	int cmd = 0;
	int Sleep_Time_Flag = TRUE;
	int naptime = 0;

	drawdisp(32);

	naptime = Sleep_Time;
	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);
	wnoutrefresh(top_win);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		wnoutrefresh(main_win);
		doupdate();
		an_option();
		get_sysinfo_stats();
		wnoutrefresh(main_win);
		an_option();
		doupdate();

		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_sysinfo) == SIG_ERR)
			printf("\nsignal(SIGALRM) error\n");

		alarm(naptime);	/* set the alarm timer */

		if ((cmd = wgetch(bottom_win)) < 0) {
			alarm(0);			/* stop the alarm timer */
			update_sysinfo_stats(*sysinfo_type);
		} else {
			alarm(0);			/* stop the alarm timer */
			switch(cmd) {
			case 'd' :
			case 'D' :
				if (major_flag) {
					major_flag = 0;
					mvwaddstr(bottom_win, 1, 2, " Short device name flag enabled.");
				} else {
					major_flag = 1;
					mvwaddstr(bottom_win, 1, 2, " Short device name flag disabled.");
				}
				w_clrtoeol(bottom_win, 0, 0);
				wrefresh(bottom_win);
				nap(750);
				break;

			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				break;

			case 'C' :
			case 'c' :
				change_time(FALSE);
				touchwin(main_win);
				drawdisp(32);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				doupdate();
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'P' :
			case 'p' :
				screen_dump();
				break;

			case 'I' :
			case 'i' :
				drawdisp(32);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				drawdisp(32);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				drawdisp(32);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
			case 'f' :
				drawdisp(32);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				break;

			case 't' :
			case 'T' :
				Change_Sysinfo_Type((int *)sysinfo_type);
				mvwaddstr(main_win, 1, 7, "Reading /unix..........");
				wnoutrefresh(main_win);
				doupdate();
				mvwaddstr(main_win, 1, 7, "                         ");
				break;

			case 'Q' :
			case 'q' :
				return;

			default :
				beep();
				break;
			}
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int Change_Sysinfo_Type(void)            Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        change the time when the arrow keys are used.               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int Change_Sysinfo_Type(int *sysinfo_type)
{

	extern int Sleep_Time;

	WINDOW *win;

	win = newwin(5, 23, 11, 10);
	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);
	mvwaddstr(win, 1, 2, "Total per Second");
	mvwaddstr(win, 2, 2, "Total over a Period");
	mvwaddstr(win, 3, 2, "The Running Total");
	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 1, 12, "S");
	mvwaddstr(win, 2, 15, "P");
	mvwaddstr(win, 3,  6, "R");

	Set_Colour(win, Normal);

	wnoutrefresh(win);
	an_option();
	doupdate();
	while (TRUE) {
		switch(wgetch(bottom_win)) {
		case 's' :
		case 'c' :
		case 'S' :
		case 'C' :
			*sysinfo_type = BY_CURRENT;
			mvwaddstr(bottom_win, 1, 2, " Watchit sysinfo average set to current Average.");
			break;

		case 'p' :
		case 'P' :
			*sysinfo_type = BY_TIMED;
			mvwaddstr(bottom_win, 1, 2, " Watchit sysinfo average set by period.");
			break;

		case 'r' :
		case 'R' :
			*sysinfo_type = BY_TOTAL;
			mvwaddstr(bottom_win, 1, 2, " Watchit sysinfo average set to Sys boot total.");
			break;

		case 'q' :
		case 'Q' :	/* Quit and exit */
		case 27  :
			delwin(win);
			touchwin(main_win);
			return(0);

		default :
			an_option();
			beep();
			continue;
		}

		delwin(win); /* added this */
		touchwin(main_win);
		return(0);
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %Z% |
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

void monitor_info(void)
{

	/* Number 1 */
	mvwprintw(main_win, 4, COL_1, "lread:    %5d, %5d",
	    sys_value(lread), Max_Sysinfo_Val(lread));
	mvwprintw(main_win, 5, COL_1, "lwrite:   %5d, %5d",
	    sys_value(lwrite), Max_Sysinfo_Val(lwrite));
	mvwprintw(main_win, 6, COL_1, "bread:    %5d, %5d",
	    sys_value(bread), Max_Sysinfo_Val(bread));
	mvwprintw(main_win, 7, COL_1, "bwrite:   %5d, %5d",
	    sys_value(bwrite), Max_Sysinfo_Val(bwrite));
	mvwprintw(main_win, 8, COL_1, "swapin:   %5d, %5d",
	    sys_value(swapin), Max_Sysinfo_Val(swapin));
	mvwprintw(main_win, 9, COL_1, "swapout:  %5d, %5d",
	    sys_value(swapout), Max_Sysinfo_Val(swapout));
	mvwprintw(main_win, 10, COL_1, "bswapin:  %5d, %5d",
	    sys_value(bswapin), Max_Sysinfo_Val(bswapin));
	mvwprintw(main_win, 11, COL_1, "bswapout: %5d, %5d",
	    sys_value(bswapout), Max_Sysinfo_Val(bswapout));

	mvwprintw(main_win, 13, COL_1, "syscall:  %5d, %5d",
	    sys_value(syscall), Max_Sysinfo_Val(syscall));
	mvwprintw(main_win, 14, COL_1, "sysread:  %5d, %5d",
	    sys_value(sysread), Max_Sysinfo_Val(sysread));
	mvwprintw(main_win, 15, COL_1, "syswrit:  %5d, %5d",
	    sys_value(syswrite), Max_Sysinfo_Val(syswrite));
	mvwprintw(main_win, 16, COL_1, "sysfork:  %5d, %5d",
	    sys_value(sysfork), Max_Sysinfo_Val(sysfork));
	mvwprintw(main_win, 17, COL_1, "sysexec:  %5d, %5d",
	    sys_value(sysexec), Max_Sysinfo_Val(sysexec));

	/* Number 2 */
	mvwprintw(main_win, 4, COL_2, "readch:  %7d, %7d",
	    sys_value(readch), Max_Sysinfo_Val(readch));
	mvwprintw(main_win, 5, COL_2, "writech: %7d, %7d",
	    sys_value(writech), Max_Sysinfo_Val(writech));
	mvwprintw(main_win, 6, COL_2, "rawch:   %7d, %7d",
	    sys_value(rawch), Max_Sysinfo_Val(rawch));
	mvwprintw(main_win, 7, COL_2, "canch:   %7d, %7d",
	    sys_value(canch), Max_Sysinfo_Val(canch));
	mvwprintw(main_win, 8, COL_2, "outch:   %7d, %7d",
	    sys_value(outch), Max_Sysinfo_Val(outch));
	mvwprintw(main_win, 9, COL_2, "iget:    %7d, %7d",
	    sys_value(iget), Max_Sysinfo_Val(iget));
	mvwprintw(main_win, 10, COL_2, "namei:   %7d, %7d",
	    sys_value(namei), Max_Sysinfo_Val(namei));
	mvwprintw(main_win, 11, COL_2, "dirblk:  %7d, %7d",
	    sys_value(dirblk), Max_Sysinfo_Val(dirblk));

	mvwprintw(main_win, 13, COL_2, "pswitch: %7d, %7d",
	    sys_value(pswitch), Max_Sysinfo_Val(pswitch));
	mvwprintw(main_win, 14, COL_2, "run que: %7d, %7d",
	    sys_value(runque), Max_Sysinfo_Val(runque));
	mvwprintw(main_win, 15, COL_2, "rcvint:  %7d, %7d",
	    sys_value(rcvint), Max_Sysinfo_Val(rcvint));
	mvwprintw(main_win, 16, COL_2, "xmtint:  %7d, %7d",
	    sys_value(xmtint) , Max_Sysinfo_Val(xmtint));
	mvwprintw(main_win, 17, COL_2, "mdmint:  %7d, %7d",
	    sys_value(mdmint), Max_Sysinfo_Val(mdmint));

	/* Number 3 */
	mvwprintw(main_win, 4, COL_3, "vfault :   %4d, %4d",
	    minfo_value(vfault), Max_Minfo_Val(vfault));
	mvwprintw(main_win, 5, COL_3, "demand:   %4d, %4d",
	    minfo_value(demand), Max_Minfo_Val(demand));
	mvwprintw(main_win, 6, COL_3, "pfault :   %4d, %4d",
	    minfo_value(pfault), Max_Minfo_Val(pfault));
	mvwprintw(main_win, 7, COL_3, "copy/w:   %4d, %4d",
	    minfo_value(cw), Max_Minfo_Val(cw));
	mvwprintw(main_win, 8, COL_3, "vf page:  %4d, %4d",
	    minfo_value(vfpg), Max_Minfo_Val(vfpg));
	mvwprintw(main_win, 9, COL_3, "sf page:  %4d, %4d",
	    minfo_value(sfpg), Max_Minfo_Val(sfpg));
	mvwprintw(main_win, 10, COL_3, "vs page:  %4d, %4d",
	    minfo_value(vspg), Max_Minfo_Val(vspg));
	mvwprintw(main_win, 11, COL_3, "ss page:  %4d, %4d",
	    minfo_value(sspg), Max_Minfo_Val(sspg));

	mvwprintw(main_win, 13, COL_3, "pg freed: %4d, %4d",
	    minfo_value(freedpgs), Max_Minfo_Val(freedpgs));
	mvwprintw(main_win, 14, COL_3, "pg steal: %4d, %4d",
	    minfo_value(steal), Max_Minfo_Val(steal));
	mvwprintw(main_win, 15, COL_3, "pg swap:  %4d, %4d",
	    minfo_value(swap), Max_Minfo_Val(swap));
	mvwprintw(main_win, 16, COL_3, "pg file:  %4d, %4d",
	    minfo_value(file), Max_Minfo_Val(file));
	mvwprintw(main_win, 17, COL_3, "pg cache: %4d, %4d",
	    minfo_value(cache), Max_Minfo_Val(cache));

	if (size_flag) {
		/* Number 1 */
		mvwprintw(main_win,LONG_Y + 1, COL_1, "rswpin:   %5d, %5d",
		    minfo_value(rsin), Max_Minfo_Val(rsin));
		mvwprintw(main_win,LONG_Y + 2, COL_1, "rswpout:  %5d, %5d",
		    minfo_value(rsout), Max_Minfo_Val(rsout));
		mvwprintw(main_win,LONG_Y + 3, COL_1, "unmodsw:  %5d, %5d",
		    minfo_value(unmodsw), Max_Minfo_Val(unmodsw));
		mvwprintw(main_win,LONG_Y + 4, COL_1, "unmodfl:  %5d, %5d",
		    minfo_value(unmodfl), Max_Minfo_Val(unmodfl));

		/* Number 2 */
		mvwprintw(main_win,LONG_Y + 1, COL_2, "phread:  %7d, %7d",
		    sys_value(phread), Max_Sysinfo_Val(phread));
		mvwprintw(main_win,LONG_Y + 2, COL_2, "phwrite: %7d, %7d",
		    sys_value(phwrite), Max_Sysinfo_Val(phwrite));
		mvwprintw(main_win,LONG_Y + 3, COL_2, "swapque: %7d, %7d",
		    sys_value(swpque), Max_Sysinfo_Val(swpque));
		mvwprintw(main_win,LONG_Y + 4, COL_2, "sema:    %7d, %7d",
		    sys_value(sema), Max_Sysinfo_Val(sema));

		/* Number 3 */
		mvwprintw(main_win,LONG_Y + 1, COL_3, "message:  %4d, %4d",
		    sys_value(msg), Max_Sysinfo_Val(msg));
		mvwprintw(main_win,LONG_Y + 2, COL_3, "psoutok:  %4d, %4d",
		    minfo_value(psoutok), Max_Minfo_Val(psoutok));
		mvwprintw(main_win,LONG_Y + 3, COL_3, "psinfail: %4d, %4d",
		    minfo_value(psinfail), Max_Minfo_Val(psinfail));
		mvwprintw(main_win,LONG_Y + 4, COL_3, "psinok:   %4d, %4d",
		    minfo_value(psinok), Max_Minfo_Val(psinok));

	}
	wnoutrefresh(main_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %Z% |
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

void display_info(void)
{


	/* Number 1 */
	mvwprintw(main_win, 4, COL_1, "lread:    %5d", sys_value(lread));
	mvwprintw(main_win, 5, COL_1, "lwrite:   %5d", sys_value(lwrite));
	mvwprintw(main_win, 6, COL_1, "bread:    %5d", sys_value(bread));
	mvwprintw(main_win, 7, COL_1, "bwrite:   %5d", sys_value(bwrite));
	mvwprintw(main_win, 8, COL_1, "swapin:   %5d", sys_value(swapin));
	mvwprintw(main_win, 9, COL_1, "swapout:  %5d", sys_value(swapout));
	mvwprintw(main_win, 10, COL_1, "bswapin:  %5d", sys_value(bswapin));
	mvwprintw(main_win, 11, COL_1, "bswapout: %5d", sys_value(bswapout));

	mvwprintw(main_win, 13, COL_1, "syscall:  %5d", sys_value(syscall));
	mvwprintw(main_win, 14, COL_1, "sysread:  %5d", sys_value(sysread));
	mvwprintw(main_win, 15, COL_1, "syswrit:  %5d", sys_value(syswrite));
	mvwprintw(main_win, 16, COL_1, "sysfork:  %5d", sys_value(sysfork));
	mvwprintw(main_win, 17, COL_1, "sysexec:  %5d", sys_value(sysexec));

	/* Number 2 */
	mvwprintw(main_win, 4, COL_2, "readch:  %7d", sys_value(readch));
	mvwprintw(main_win, 5, COL_2, "writech: %7d", sys_value(writech));
	mvwprintw(main_win, 6, COL_2, "rawch:   %7d", sys_value(rawch));
	mvwprintw(main_win, 7, COL_2, "canch:   %7d", sys_value(canch));
	mvwprintw(main_win, 8, COL_2, "outch:   %7d", sys_value(outch));
	mvwprintw(main_win, 9, COL_2, "iget:    %7d", sys_value(iget));
	mvwprintw(main_win, 10, COL_2, "namei:   %7d", sys_value(namei));
	mvwprintw(main_win, 11, COL_2, "dirblk:  %7d", sys_value(dirblk));

	mvwprintw(main_win, 13, COL_2, "pswitch: %7d", sys_value(pswitch));
	mvwprintw(main_win, 14, COL_2, "run que: %7d", sys_value(runque));
	mvwprintw(main_win, 15, COL_2, "rcvint:  %7d", sys_value(rcvint));
	mvwprintw(main_win, 16, COL_2, "xmtint:  %7d", sys_value(xmtint));
	mvwprintw(main_win, 17, COL_2, "mdmint:  %7d", sys_value(mdmint));

	/* Number 3 */
	mvwprintw(main_win, 4, COL_3, "vfault :   %4d", minfo_value(vfault));
	mvwprintw(main_win, 5, COL_3, "demand:   %4d", minfo_value(demand));
	mvwprintw(main_win, 6, COL_3, "pfault :   %4d", minfo_value(pfault));
	mvwprintw(main_win, 7, COL_3, "copy/w:   %4d", minfo_value(cw));
	mvwprintw(main_win, 8, COL_3, "vf page:  %4d", minfo_value(vfpg));
	mvwprintw(main_win, 9, COL_3, "sf page:  %4d", minfo_value(sfpg));
	mvwprintw(main_win, 10, COL_3, "vs page:  %4d", minfo_value(vspg));
	mvwprintw(main_win, 11, COL_3, "ss page:  %4d", minfo_value(sspg));

	mvwprintw(main_win, 13, COL_3, "pg freed: %4d", minfo_value(freedpgs));
	mvwprintw(main_win, 14, COL_3, "pg steal: %4d", minfo_value(steal));
	mvwprintw(main_win, 15, COL_3, "pg swap:  %4d", minfo_value(swap));
	mvwprintw(main_win, 16, COL_3, "pg file:  %4d", minfo_value(file));
	mvwprintw(main_win, 17, COL_3, "pg cache: %4d", minfo_value(cache));

	if (size_flag) {
		/* Number 1 */
		mvwprintw(main_win,LONG_Y + 1, COL_1, "rswpin:   %5d",
		    minfo_value(rsin));
		mvwprintw(main_win,LONG_Y + 2, COL_1, "rswpout:  %5d",
		    minfo_value(rsout));
		mvwprintw(main_win,LONG_Y + 3, COL_1, "unmodsw:  %5d",
		    minfo_value(unmodsw));
		mvwprintw(main_win,LONG_Y + 4, COL_1, "unmodfl:  %5d",
		    minfo_value(unmodfl));

		/* Number 2 */
		mvwprintw(main_win,LONG_Y + 1, COL_2, "phread:  %7d",
		    sys_value(phread));
		mvwprintw(main_win,LONG_Y + 2, COL_2, "phwrite: %7d",
		    sys_value(phwrite));
		mvwprintw(main_win,LONG_Y + 3, COL_2, "swapque: %7d",
		    sys_value(swpque));
		mvwprintw(main_win,LONG_Y + 4, COL_2, "sema:    %7d",
		    sys_value(sema));

		/* Number 3 */
		mvwprintw(main_win,LONG_Y + 1, COL_3, "message:  %4d",
		    sys_value(msg));
		mvwprintw(main_win,LONG_Y + 2, COL_3, "psoutok:  %4d",
		    minfo_value(psoutok));
		mvwprintw(main_win,LONG_Y + 3, COL_3, "psinfail: %4d",
		    minfo_value(psinfail));
		mvwprintw(main_win,LONG_Y + 4, COL_3, "psinok:   %4d",
		    minfo_value(psinok));
	}
	wnoutrefresh(main_win);
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function: void get_sysinfo_stats(void)               Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        gets the sysinfo stuff.                                     |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          Moved to allow alarm to work.
 *    Bugs:
 *          None yet.
 */

void get_sysinfo_stats(void)
{

	kmem_read(&last_minfo, namelist[NM_MINFO].n_value, sizeof(struct minfo));
	kmem_read(&last_sysinfo, namelist[NM_SYSINFO].n_value, sizeof(struct sysinfo));
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void update_sysinfo_stats(void)           Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Updates, draws and does lots of stuff.                      |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          Moved to allow alarm to work.
 *    Bugs:
 *          None yet.
 */

void update_sysinfo_stats(int sysinfo_type)
{

static int last_header = 69;

current_idle   = 0L;
current_user   = 0L;
current_kernel = 0L;
current_wait   = 0L;
current_total  = 0L;

current_sxbrk      = 0L;
current_io         = 0L;
current_wswap      = 0L;
current_pio        = 0L;
current_wait_total = 0L;


	kmem_read(&minfo, namelist[NM_MINFO].n_value, sizeof(struct minfo));
	kmem_read(&sysinfo, namelist[NM_SYSINFO].n_value, sizeof(struct sysinfo));

	kmem_read(&nswap, namelist[NM_SWAPBLKS].n_value, sizeof(nswap));
	kmem_read(&kavailmem, namelist[NM_KAVAILMEM].n_value, sizeof(kavailmem));


		current_user   = sys_value(cpu[CPU_USER]);
		current_idle   = sys_value(cpu[CPU_IDLE]);
		current_kernel = sys_value(cpu[CPU_KERNEL]);
		current_wait   = sys_value(cpu[CPU_WAIT]);
		current_sxbrk  = sys_value(cpu[CPU_SXBRK]);
		current_total  = current_user + current_idle + current_kernel +
			current_wait;
		current_total2 = current_user + current_idle + current_kernel;

		current_io         = sys_value(wait[W_IO]);
		current_wswap      = sys_value(wait[W_SWAP]);
		current_pio        = sys_value(wait[W_PIO]);
		current_wait_total = current_io + current_wswap + current_pio;

/*
 * +--------------------------------------------------------------------+
 * |                AVG TO BE WORKED OUT HERE                           |
 * +--------------------------------------------------------------------+
 */

		cpu_averages[_AVG_CPU_IDLE] 		+= current_idle;
		cpu_averages[_AVG_CPU_USER] 		+= current_user;
		cpu_averages[_AVG_CPU_KERNEL]		+= current_kernel;
		cpu_averages[_AVG_CPU_WAIT]		+= current_wait;
		cpu_averages[_AVG_CPU_SXBRK]		+= current_sxbrk;
		cpu_averages[_AVG_CPU_TOTAL]		+= current_total;
		cpu_averages[_AVG_CPU_TOTAL_2]	+= current_total2;

		cpu_averages[_AVG_W_IO]			+= current_io;
		cpu_averages[_AVG_W_SWAP]		+= current_wswap;
		cpu_averages[_AVG_W_PIO]		+= current_pio;
		cpu_averages[_AVG_W_TOTAL]		+= current_wait_total;
	
	/*
		Here is where I will draw all the stuff

	if (sysinfo_type != last_header) {
		sysinfo_header(sysinfo_type);
		last_header = sysinfo_type;
	}

	*/

	sysinfo_header(sysinfo_type);
	Draw_Sysinfo();

	switch (sysinfo_type) {
	case BY_TIMED :
		mvwaddstr(main_win, 1, 40, "per Period.");
		break;

	case BY_TOTAL :
		mvwaddstr(main_win, 1, 43, "Total.");
		break;

	case BY_CURRENT :
	default :
		mvwaddstr(main_win, 1, 40, "per Second.");
		break;
	}
	if (size_flag)
/*
		current_user   = sys_value(cpu[CPU_USER]);
		current_idle   = sys_value(cpu[CPU_IDLE]);
		current_kernel = sys_value(cpu[CPU_KERNEL]);
		current_wait   = sys_value(cpu[CPU_WAIT]);
		current_sxbrk  = sys_value(cpu[CPU_SXBRK]);
		current_total  = current_user + current_idle + current_kernel +
			current_wait;
		current_total2 = current_user + current_idle + current_kernel;

		current_io         = sys_value(wait[W_IO]);
		current_wswap      = sys_value(wait[W_SWAP]);
		current_pio        = sys_value(wait[W_PIO]);
		current_wait_total = current_io + current_wswap + current_pio;


		cpu_averages[_AVG_CPU_IDLE] 		+= current_idle;
		cpu_averages[_AVG_CPU_USER] 		+= current_user;
		cpu_averages[_AVG_CPU_KERNEL]		+= current_kernel;
		cpu_averages[_AVG_CPU_WAIT]		+= current_wait;
		cpu_averages[_AVG_CPU_SXBRK]		+= current_sxbrk;
		cpu_averages[_AVG_CPU_TOTAL]		+= current_total;
		cpu_averages[_AVG_CPU_TOTAL_2]	+= current_total2;

		cpu_averages[_AVG_W_IO]			+= current_io;
		cpu_averages[_AVG_W_SWAP]		+= current_wswap;
		cpu_averages[_AVG_W_PIO]		+= current_pio;
		cpu_averages[_AVG_W_TOTAL]		+= current_wait_total;
	

	*/
		Do_Sysinfo_Stats();

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: Do_Sysinfo_Stats()                        Date: 93/03/21 |
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

void Draw_Sysinfo(void)
{

	/* Number 1 */
	mvwaddstr(main_win,  4, COL_1, "lread:         ");
	mvwaddstr(main_win,  5, COL_1, "lwrite:        ");
	mvwaddstr(main_win,  6, COL_1, "bread:         ");
	mvwaddstr(main_win,  7, COL_1, "bwrite:        ");
	mvwaddstr(main_win,  8, COL_1, "swapin:        ");
	mvwaddstr(main_win,  9, COL_1, "swapout:       ");
	mvwaddstr(main_win, 10, COL_1, "bswapin:       ");
	mvwaddstr(main_win, 11, COL_1, "bswapout:      ");

	mvwaddstr(main_win, 13, COL_1, "syscall:       ");
	mvwaddstr(main_win, 14, COL_1, "sysread:       ");
	mvwaddstr(main_win, 15, COL_1, "syswrit:       ");
	mvwaddstr(main_win, 16, COL_1, "sysfork:       ");
	mvwaddstr(main_win, 17, COL_1, "sysexec:       ");

	/* Number 2 */
	mvwaddstr(main_win,  4, COL_2, "readch:         ");
	mvwaddstr(main_win,  5, COL_2, "writech:        ");
	mvwaddstr(main_win,  6, COL_2, "rawch:          ");
	mvwaddstr(main_win,  7, COL_2, "canch:          ");
	mvwaddstr(main_win,  8, COL_2, "outch:          ");
	mvwaddstr(main_win,  9, COL_2, "iget:           ");
	mvwaddstr(main_win, 10, COL_2, "namei:          ");
	mvwaddstr(main_win, 11, COL_2, "dirblk:         ");

	mvwaddstr(main_win, 13, COL_2, "pswitch:        ");
	mvwaddstr(main_win, 14, COL_2, "run que:        ");
	mvwaddstr(main_win, 15, COL_2, "rcvint:         ");
	mvwaddstr(main_win, 16, COL_2, "xmtint:         ");
	mvwaddstr(main_win, 17, COL_2, "mdmint:         ");

	/* Number 3 */
	mvwaddstr(main_win,  4, COL_3, "vfault :       ");
	mvwaddstr(main_win,  5, COL_3, "demand:       ");
	mvwaddstr(main_win,  6, COL_3, "pfault :       ");
	mvwaddstr(main_win,  7, COL_3, "copy/w:       ");
	mvwaddstr(main_win,  8, COL_3, "vf page:      ");
	mvwaddstr(main_win,  9, COL_3, "sf page:      ");
	mvwaddstr(main_win, 10, COL_3, "vs page:      ");
	mvwaddstr(main_win, 11, COL_3, "ss page:      ");

	mvwaddstr(main_win, 13, COL_3, "pg freed:     ");
	mvwaddstr(main_win, 14, COL_3, "pg steal:     ");
	mvwaddstr(main_win, 15, COL_3, "pg swap:      ");
	mvwaddstr(main_win, 16, COL_3, "pg file:      ");
	mvwaddstr(main_win, 17, COL_3, "pg cache:     ");

	if (size_flag) {
		/* Number 1 */
		mvwaddstr(main_win,LONG_Y + 1, COL_1, "rswpin:        ");
		mvwaddstr(main_win,LONG_Y + 2, COL_1, "rswpout:       ");
		mvwaddstr(main_win,LONG_Y + 3, COL_1, "unmodsw:       ");
		mvwaddstr(main_win,LONG_Y + 4, COL_1, "unmodfl:       ");

		/* Number 2 */
		mvwaddstr(main_win,LONG_Y + 1, COL_2, "phread:         ");
		mvwaddstr(main_win,LONG_Y + 2, COL_2, "phwrite:        ");
		mvwaddstr(main_win,LONG_Y + 3, COL_2, "swapque:        ");
		mvwaddstr(main_win,LONG_Y + 4, COL_2, "sema:           ");

		/* Number 3 */
		mvwaddstr(main_win,LONG_Y + 1, COL_3, "message:      ");
		mvwaddstr(main_win,LONG_Y + 2, COL_3, "psoutok:      ");
		mvwaddstr(main_win,LONG_Y + 3, COL_3, "psinfail:     ");
		mvwaddstr(main_win,LONG_Y + 4, COL_3, "psinok:       ");
		Do_Sysinfo_Stats();
	}
	wnoutrefresh(main_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: Do_Sysinfo_Stats()                        Date: 93/03/21 |
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

void Do_Sysinfo_Stats(void)
{

int position = 0;

	Set_Colour(main_win, Normal);

	position = 24;
	mvwprintw(main_win, position, 10, "%s   used    Avg", SCALE);
	mvwprintw(main_win, position + 3, 10, "%s   used    avg", SCALE);
	mvwprintw(main_win, position + 6, 10, "%s   used    avg", SCALE);
	mvwprintw(main_win, position + 9, 10, "%s   used    avg", SCALE);

	mvwaddstr(main_win, position + 1, 3, "User");
	mvwaddstr(main_win, position + 4, 3, "Kernel");
	mvwaddstr(main_win, position + 7, 3, "Idle");
	mvwaddstr(main_win, position + 10, 3, "Wait");

/*
 * +--------------------------------------------------------------------+
 * |                         USER INFO                                  |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, position + 1, 67, "%      %");
	Draw_The_Text(main_win, ((current_user * 100) / current_total), position + 1, 64, USER_BAR);

	Draw_The_Text(main_win,
	((cpu_averages[_AVG_CPU_USER] * 100) / cpu_averages[_AVG_CPU_TOTAL]),
	position + 1, 71, USER_BAR);

	Draw_A_Bar(main_win, ((current_user * 100) / current_total), position + 1, 10, USER_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         KERNEL INFO                                |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, position + 4, 67, "%      %");
	Draw_The_Text(main_win, ((current_kernel * 100) / current_total), position + 4, 64, KERNEL_BAR);

	Draw_The_Text(main_win,
	((cpu_averages[_AVG_CPU_KERNEL] * 100) / cpu_averages[_AVG_CPU_TOTAL]),
	position + 4, 71, KERNEL_BAR);

	Draw_A_Bar(main_win, ((current_kernel * 100) / current_total), position + 4, 10, KERNEL_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         IDLE INFO                                  |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, position + 7, 67, "%      %");
	Draw_The_Text(main_win, ((current_idle * 100) / current_total), position + 7, 64, IDLE_BAR);

	Draw_The_Text(main_win,
	((cpu_averages[_AVG_CPU_IDLE] * 100) / cpu_averages[_AVG_CPU_TOTAL]),
	position + 7, 71, IDLE_BAR);

	Draw_A_Bar(main_win, ((current_idle * 100) / current_total), position + 7, 10, IDLE_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         WAIT INFO                                  |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, position + 10, 67, "%      %");

	Draw_The_Text(main_win, ((current_wait * 100) / current_total), position + 10, 64, WAIT_BAR);

	Draw_The_Text(main_win,

	    100 - (((cpu_averages[_AVG_CPU_USER] * 100) / cpu_averages[_AVG_CPU_TOTAL]) + 
	    ((cpu_averages[_AVG_CPU_KERNEL] * 100) / cpu_averages[_AVG_CPU_TOTAL]) + 
	    ((cpu_averages[_AVG_CPU_IDLE] * 100) / cpu_averages[_AVG_CPU_TOTAL])),
	position + 10, 71, WAIT_BAR);

	Draw_A_Bar(main_win, ((current_wait * 100) / current_total), position + 10, 10, WAIT_BAR);

	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
