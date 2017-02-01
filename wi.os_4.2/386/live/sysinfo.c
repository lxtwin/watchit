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
 *		See cpu.c for CPU stats.
 *
 *    Bugs:
 *    v2.3	Move the Cur(3) header one space <-.		
 *					Sat Jul 17 18:48:14 EDT 1993 - PKR. / Done.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
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

void watch_sysinfo(void);

void monitor_info(void);
void monitor_long_info(void);

void display_info(void);
void display_long_info(void);

void wi_sysinfo(int mon_flag);

#define COL_1 2
#define COL_2 28
#define COL_3 56

#define LONG_Y 18

#define T_QUITE	0
#define T_NORMAL	1

extern short demo_pid;
extern int avg_total;

extern int compressed_flag;

extern int The_Current_Display;

int avg_total;

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

int sys_menu(void)
{

int IPC_Test_Flag = 0;

	/*
		Reset all the flags.
	*/
	compressed_flag = 0;

	if (The_Current_Avg != CURRENT_AVG)
		The_Current_Avg = CURRENT_AVG;

	wmove(main_win, 1, 1);
	wclrtobot(main_win);
	Draw_A_Box(main_win, DEFAULT_BOX);
	drawdisp(14);

	mvwaddstr(main_win, 2, 4, "Reading /unix..........");
	wrefresh(main_win);
	mvwaddstr(main_win, 2, 4, "                         ");
	wi_sysinfo(1);
	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		mvwaddstr(bottom_win, 1, 74, "   ");
		mvwaddstr(bottom_win, 1, 8, ": Watchit or Quit:");
		wclrtoeol(bottom_win);
		mvwprintw(bottom_win, 1, 72, ":   ");
		wattron(bottom_win, A_BOLD);
		mvwaddstr(bottom_win, 1, 2, "Select");
		mvwprintw(bottom_win, 1, 61, "Sample time");
		wattroff(bottom_win, A_BOLD);
		wattron(bottom_win, A_REVERSE);
		mvwaddstr(bottom_win, 1, 10, "W");
		mvwaddstr(bottom_win, 1, 21, "Q");
		mvwprintw(bottom_win, 1, 74, "%d", Sleep_Time);
		wattroff(bottom_win, A_REVERSE);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wattron(bottom_win, A_BLINK);
		mvwaddstr(bottom_win, 1, 27, "_");
		wattroff(bottom_win, A_BLINK);
		wmove(bottom_win, 1, 27);
		wnoutrefresh(bottom_win);
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

		case 'G' :
		case 'g' :
			Show_Graphics();
			continue;
#ifdef _REL4

		case 'A' :
		case 'a' :
		case 'T' :
		case 't' :
			Get_The_Avg();
			wrefresh(bottom_win);
			mvwaddstr(bottom_win, 1, 2, "                                    ");
			wi_sysinfo(1);
			break;
#endif

		case 'U' :
		case 'u' :
			mvwaddstr(main_win, 2, 4, "                         ");
			mvwaddstr(main_win, 2, 4, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 2, 4, "                         ");
			wi_sysinfo(1);
			continue;

		case 'W' :
		case 'w' :
			watch_sysinfo();
			return(0);
			
		case 'Q' :
		case 'q' :
			return(0);

		default :
			beep();
			continue;
		}
	}
}


/*
 * +--------------------------------------------------------------------+
 * | Function: X                                         Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        X                                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int Tmp_Sleep_Time = 0;

void wi_sysinfo(int mon_flag)
{

	/*
		get current sysinfo stats.
		I might use nap insted of sleep.
	*/
	kmem_read(&last_minfo, namelist[NM_MINFO].n_value, sizeof(struct minfo));
	kmem_read(&last_sysinfo, namelist[NM_SYSINFO].n_value,
	    sizeof(struct sysinfo));

	if (mon_flag) {
		Tmp_Sleep_Time = Sleep_Time;
		Sleep_Time = 1;
	}

	nap(Sleep_Time * 1000);
	kmem_read(&minfo, namelist[NM_MINFO].n_value, sizeof(struct minfo));
	kmem_read(&sysinfo, namelist[NM_SYSINFO].n_value, sizeof(struct sysinfo));

	avg_total++;

	/*
		Now display the buggers.
	*/
	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 1, COL_2-1, "System Activity / Second");
	wattroff(main_win, A_BOLD);

	if (mon_flag) {
		wattron(main_win, A_BOLD);

		switch(The_Current_Avg) {
			case CURRENT_AVG:
				mvwaddstr(main_win, 3, COL_1+10, "Current ");
				mvwaddstr(main_win, 3, COL_2+11, "Current ");
				mvwaddstr(main_win, 3, COL_3+ 9, "Current ");
				break;

			case BOOT_AVG:
				mvwaddstr(main_win, 3, COL_1+10, "sys boot");
				mvwaddstr(main_win, 3, COL_2+11, "sys boot");
				mvwaddstr(main_win, 3, COL_3+ 9, "sys boot");
				break;

			case WATCHIT_AVG:
				mvwaddstr(main_win, 3, COL_1+10, "wi start");
				mvwaddstr(main_win, 3, COL_2+11, "wi start");
				mvwaddstr(main_win, 3, COL_3+ 9, "wi start");
				break;
		}

		wattroff(main_win, A_BOLD);
		display_info();
	} else {
		mvwaddstr(main_win, 3, COL_1+10, "       ");
		mvwaddstr(main_win, 3, COL_2+11, "       ");
		mvwaddstr(main_win, 3, COL_3+ 9, "       ");
		wattron(main_win, A_BOLD);
		switch(The_Current_Avg) {
			case CURRENT_AVG:
				mvwaddstr(main_win, 3, COL_1+12, "Cur");
				mvwaddstr(main_win, 3, COL_2+13, "Cur");
				mvwaddstr(main_win, 3, COL_3+11, "Cur");
				break;

			case BOOT_AVG:
				mvwaddstr(main_win, 3, COL_1+12, "Sys");
				mvwaddstr(main_win, 3, COL_2+13, "Sys");
				mvwaddstr(main_win, 3, COL_3+11, "Sys");
				break;

			case WATCHIT_AVG:
				mvwaddstr(main_win, 3, COL_1+12, " WI");
				mvwaddstr(main_win, 3, COL_2+13, " WI");
				mvwaddstr(main_win, 3, COL_3+11, " WI");
				break;
		}

		mvwaddstr(main_win, 3, COL_1+19, "Max");
		mvwaddstr(main_win, 3, COL_2+22, "Max");
		mvwaddstr(main_win, 3, COL_3+17, "Max");
		wattroff(main_win, A_BOLD);
		monitor_info();
	}

	if (size_flag)
		do_large_stats(0);
		
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function: X                                         Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        X                                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void monitor_info(void)
{

	/*
		Number 1
	*/
	mvwprintw(main_win, 4, COL_1, "lread:    %5d, %5d",
	    sys_value(lread), max_val(lread));
	mvwprintw(main_win, 5, COL_1, "lwrite:   %5d, %5d",
	    sys_value(lwrite), max_val(lwrite));
	mvwprintw(main_win, 6, COL_1, "bread:    %5d, %5d",
	    sys_value(bread), max_val(bread));
	mvwprintw(main_win, 7, COL_1, "bwrite:   %5d, %5d",
	    sys_value(bwrite), max_val(bwrite));
	mvwprintw(main_win, 8, COL_1, "swapin:   %5d, %5d",
	    sys_value(swapin), max_val(swapin));
	mvwprintw(main_win, 9, COL_1, "swapout:  %5d, %5d",
	    sys_value(swapout), max_val(swapout));
	mvwprintw(main_win, 10, COL_1, "bswapin:  %5d, %5d",
	    sys_value(bswapin), max_val(bswapin));
	mvwprintw(main_win, 11, COL_1, "bswapout: %5d, %5d",
	    sys_value(bswapout), max_val(bswapout));

	mvwprintw(main_win, 13, COL_1, "syscall:  %5d, %5d",
	    sys_value(syscall), max_val(syscall));
	mvwprintw(main_win, 14, COL_1, "sysread:  %5d, %5d",
	    sys_value(sysread), max_val(sysread));
	mvwprintw(main_win, 15, COL_1, "syswrit:  %5d, %5d",
	    sys_value(syswrite), max_val(syswrite));
	mvwprintw(main_win, 16, COL_1, "sysfork:  %5d, %5d",
	    sys_value(sysfork), max_val(sysfork));
	mvwprintw(main_win, 17, COL_1, "sysexec:  %5d, %5d",
	    sys_value(sysexec), max_val(sysexec));

	/*
		Number 2
	*/
	mvwprintw(main_win, 4, COL_2, "readch:  %7d, %7d",
	    sys_value(readch), max_val(readch));
	mvwprintw(main_win, 5, COL_2, "writech: %7d, %7d",
	    sys_value(writech), max_val(writech));
	mvwprintw(main_win, 6, COL_2, "rawch:   %7d, %7d",
	    sys_value(rawch), max_val(rawch));
	mvwprintw(main_win, 7, COL_2, "canch:   %7d, %7d",
	    sys_value(canch), max_val(canch));
	mvwprintw(main_win, 8, COL_2, "outch:   %7d, %7d",
	    sys_value(outch), max_val(outch));
	mvwprintw(main_win, 9, COL_2, "iget:    %7d, %7d",
	    sys_value(iget), max_val(iget));
	mvwprintw(main_win, 10, COL_2, "namei:   %7d, %7d",
	    sys_value(namei), max_val(namei));
	mvwprintw(main_win, 11, COL_2, "dirblk:  %7d, %7d",
	    sys_value(dirblk), max_val(dirblk));

	mvwprintw(main_win, 13, COL_2, "pswitch: %7d, %7d",
	    sys_value(pswitch), max_val(pswitch));
	mvwprintw(main_win, 14, COL_2, "run que: %7d, %7d",
	    sys_value(runque), max_val(runque));
	mvwprintw(main_win, 15, COL_2, "rcvint:  %7d, %7d",
	    sys_value(rcvint), max_val(rcvint));
	mvwprintw(main_win, 16, COL_2, "xmtint:  %7d, %7d",
	    sys_value(xmtint) , max_val(xmtint));
	mvwprintw(main_win, 17, COL_2, "mdmint:  %7d, %7d",
	    sys_value(mdmint), max_val(mdmint));

	/*
		Number 3
	*/

	mvwprintw(main_win, 4, COL_3, "vfault :   %4d, %4d",
	    minfo_value(vfault), max_nval(vfault));
	mvwprintw(main_win, 5, COL_3, "demand:   %4d, %4d",
	    minfo_value(demand), max_nval(demand));
	mvwprintw(main_win, 6, COL_3, "pfault :   %4d, %4d",
	    minfo_value(pfault), max_nval(pfault));
	mvwprintw(main_win, 7, COL_3, "copy/w:   %4d, %4d",
	    minfo_value(cw), max_nval(cw));
	mvwprintw(main_win, 8, COL_3, "vf page:  %4d, %4d",
	    minfo_value(vfpg), max_nval(vfpg));
	mvwprintw(main_win, 9, COL_3, "sf page:  %4d, %4d",
	    minfo_value(sfpg), max_nval(sfpg));
	mvwprintw(main_win, 10, COL_3, "vs page:  %4d, %4d",
	    minfo_value(vspg), max_nval(vspg));
	mvwprintw(main_win, 11, COL_3, "ss page:  %4d, %4d",
	    minfo_value(sspg), max_nval(sspg));

	mvwprintw(main_win, 13, COL_3, "pg freed: %4d, %4d",
	    minfo_value(freedpgs), max_nval(freedpgs));
	mvwprintw(main_win, 14, COL_3, "pg steal: %4d, %4d",
	    minfo_value(steal), max_nval(steal));
	mvwprintw(main_win, 15, COL_3, "pg swap:  %4d, %4d",
	    minfo_value(swap), max_nval(swap));
	mvwprintw(main_win, 16, COL_3, "pg file:  %4d, %4d",
	    minfo_value(file), max_nval(file));
	mvwprintw(main_win, 17, COL_3, "pg cache: %4d, %4d",
	    minfo_value(cache), max_nval(cache));

	if (size_flag)
		monitor_long_info();

	wrefresh(main_win);
	return;
}



/*
 * +--------------------------------------------------------------------+
 * | Function: X                                         Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        X                                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void monitor_long_info(void)
{

	/*
		Number 1
	*/
	mvwprintw(main_win,LONG_Y + 1, COL_1, "rswpin:   %5d, %5d",
	    minfo_value(rsin), max_nval(rsin));
	mvwprintw(main_win,LONG_Y + 2, COL_1, "rswpout:  %5d, %5d",
	    minfo_value(rsout), max_nval(rsout));
	mvwprintw(main_win,LONG_Y + 3, COL_1, "unmodsw:  %5d, %5d",
	    minfo_value(unmodsw), max_nval(unmodsw));
	mvwprintw(main_win,LONG_Y + 4, COL_1, "unmodfl:  %5d, %5d",
	    minfo_value(unmodfl), max_nval(unmodfl));
	   
	/*
		Number 2
	*/
	mvwprintw(main_win,LONG_Y + 1, COL_2, "phread:  %7d, %7d",
	    sys_value(phread), max_val(phread));
	mvwprintw(main_win,LONG_Y + 2, COL_2, "phwrite: %7d, %7d",
	    sys_value(phwrite), max_val(phwrite));
	mvwprintw(main_win,LONG_Y + 3, COL_2, "swapque: %7d, %7d",
	    sys_value(swpque), max_val(swpque));
	mvwprintw(main_win,LONG_Y + 4, COL_2, "sema:    %7d, %7d",
	    sys_value(sema), max_val(sema));
	    
	/*
		Number 3
	*/
	mvwprintw(main_win,LONG_Y + 1, COL_3, "message:  %4d, %4d",
	    sys_value(msg), max_val(msg));
	mvwprintw(main_win,LONG_Y + 2, COL_3, "psoutok:  %4d, %4d",
	    minfo_value(psoutok), max_nval(psoutok));
	mvwprintw(main_win,LONG_Y + 3, COL_3, "psinfail: %4d, %4d",
	    minfo_value(psinfail), max_nval(psinfail));
	mvwprintw(main_win,LONG_Y + 4, COL_3, "psinok:   %4d, %4d",
	    minfo_value(psinok), max_nval(psinok));
	    
	return;
}
/*
 * +--------------------------------------------------------------------+
 * | Function: X                                         Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        X                                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void display_info(void)
{

	/*
		Number 1
	*/
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

	/*
		Number 2
	*/

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

	/*
		Number 3
	*/

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

	if (size_flag)
		display_long_info();

	wrefresh(main_win);
	Sleep_Time = Tmp_Sleep_Time;
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: X                                         Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        X                                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void display_long_info(void)
{

	/*
		Number 1
	*/
	mvwprintw(main_win,LONG_Y + 1, COL_1, "rswpin:   %5d",
		minfo_value(rsin));
	mvwprintw(main_win,LONG_Y + 2, COL_1, "rswpout:  %5d",
		minfo_value(rsout));
	mvwprintw(main_win,LONG_Y + 3, COL_1, "unmodsw:  %5d",
		minfo_value(unmodsw));
	mvwprintw(main_win,LONG_Y + 4, COL_1, "unmodfl:  %5d",
		minfo_value(unmodfl));
	   
	/*
		Number 2
	*/
	mvwprintw(main_win,LONG_Y + 1, COL_2, "phread:  %7d",
		sys_value(phread));
	mvwprintw(main_win,LONG_Y + 2, COL_2, "phwrite: %7d",
		sys_value(phwrite));
	mvwprintw(main_win,LONG_Y + 3, COL_2, "swapque: %7d",
		sys_value(swpque));
	mvwprintw(main_win,LONG_Y + 4, COL_2, "sema:    %7d",
		sys_value(sema));
	    
	/*
		Number 3
	*/
	mvwprintw(main_win,LONG_Y + 1, COL_3, "message:  %4d",
		sys_value(msg));
	mvwprintw(main_win,LONG_Y + 2, COL_3, "psoutok:  %4d",
		minfo_value(psoutok));
	mvwprintw(main_win,LONG_Y + 3, COL_3, "psinfail: %4d",
		minfo_value(psinfail));
	mvwprintw(main_win,LONG_Y + 4, COL_3, "psinok:   %4d",
		minfo_value(psinok));
	
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/03/21 |
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

void watch_sysinfo(void)
{

int cmd = 0;
int Old_time = 0;

	Old_time = Sleep_Time;
	Sleep_Time = 2;
	drawdisp(32);

	wattron(main_win, A_REVERSE | A_BLINK);
	mvwaddstr(main_win, 0, 64, "Medium");
	wattroff(main_win, A_REVERSE | A_BLINK);
	wi_sysinfo(1);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		an_option();
		wi_sysinfo(0);
		while (rdchk(0)) {
			switch(cmd = wgetch(bottom_win)) {
			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				break;
	
			case 'P' :
			case 'p' :
				screen_dump();
				break;

			case 'G' :
			case 'g' :
				Show_Graphics();
				break;

			case 'S' :
			case 's' :
				drawdisp(32);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 58, "Slow");
				wattroff(main_win, A_REVERSE | A_BLINK);
				Sleep_Time = 4;
				break;

			case 'M' :
			case 'm' :
				drawdisp(32);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 64, "Medium");
				wattroff(main_win, A_REVERSE | A_BLINK);
				Sleep_Time = 2;
				break;

			case 'F' :
			case 'f' :
				drawdisp(32);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 72, "Fast");
				wattroff(main_win, A_REVERSE | A_BLINK);
				Sleep_Time = 1;
				break;

			case 'Q' :
			case 'q' :
				Sleep_Time = Old_time;
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
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
