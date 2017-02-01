/*
 * +--------------------------------------------------------------------+
 * | Function: vars().                                   Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the var info.                                          |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *(1) v2.3	All the graphic stuff.
 *						Sun Aug 22 16:41:05 EDT 1993 - PKR.
 * Bugs:
 *(1) v2.3 Look at the nspttys value, looks funny on a compaq.     
 *						Sat Jul 17 18:34:47 EDT 1993 - PKR.
 *
 *(1) Done	The knspttys value in the kernel was removed, so I removed
 *		it. While I was doing this I thought that I would trim off
 *		some of the fat.
 *						Sun Jul 18 15:03:42 EDT 1993 - PKR.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%			Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%			Version %I% %D% - Dapix "
#endif /* __STDC__ */


#include "wi.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mount.h>
#include <sys/seg.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stream.h>
#include <fcntl.h>
#include <sys/dir.h>
#include <sys/var.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/user.h>
#include <sys/immu.h>
#include <sys/region.h>
#include <sys/page.h>
#include <sys/proc.h>
#include <sys/locking.h>
#include <sys/sysinfo.h>
#include <sys/stream.h>
#include <sys/strstat.h>
#include <sys/callo.h>
#include <sys/page.h>
#include <sys/utsname.h>
#include <values.h>
#include <sys/flock.h>

#define BLKSIZ 1024

#define T_QUITE	0
#define T_NORMAL	1

extern int unmask;
static char  na[] = "---";
extern short demo_pid;

int var_menu(void);
void wi_vars(int screen_id);
void watch_vars(void);
void Get_The_Display(int mode);

void vscreen0(void),
vscreen1(void),
vscreen2(void),
vscreen3(void),
vscreen4(void),
vscreen5(void),
vscreen6(void),
vscreen7(void);

void (*screen[])(void) = {
	vscreen0,
	vscreen1,
	vscreen2,
	vscreen3,
	vscreen4,
	vscreen5,
	vscreen6,
	vscreen7
};

int The_Current_Display = BY_TIME;

/*
 * +--------------------------------------------------------------------+
 * | Function: var_menu()                                Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the var info and pass it on.                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int option;

int var_menu(void)
{

int last_option = 1;

	option = 49;

	wmove(main_win, 1, 1);
	wclrtobot(main_win);
	Draw_A_Box(main_win, DEFAULT_BOX);
	drawdisp(13);
	wattron(bottom_win, A_REVERSE);
	mvwaddstr(bottom_win, 1, 74, "1");
	wattroff(bottom_win, A_REVERSE);
	wi_vars(1);
	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		mvwaddstr(bottom_win, 1, 41, "                          ");
		mvwaddstr(bottom_win, 1, 8, ": Screens 1 -> 7, Watchit or Quit:");
		mvwaddstr(bottom_win, 1, 72, ": ");
		wattron(bottom_win, A_BOLD);
		mvwaddstr(bottom_win, 1, 66, "Screen");
		mvwaddstr(bottom_win, 1, 2, "Select");
		wattroff(bottom_win, A_BOLD);
		wattron(bottom_win, A_REVERSE);
		mvwaddstr(bottom_win, 1, 18, "1");
		mvwaddstr(bottom_win, 1, 23, "7");
		mvwaddstr(bottom_win, 1, 26, "W");
		mvwaddstr(bottom_win, 1, 37, "Q");
		wattroff(bottom_win, A_REVERSE);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wattron(bottom_win, A_BLINK);
		mvwaddstr(bottom_win, 1, 43, "_");
		wattroff(bottom_win, A_BLINK);
		wmove(bottom_win, 1, 43);
		wnoutrefresh(bottom_win);
		doupdate();
		option = wgetch(bottom_win);
		switch (option) {
		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			mvwaddstr(bottom_win, 1, 2, "                                                ");
			wattron(bottom_win, A_REVERSE);
			mvwprintw(bottom_win, 1, 74, "%.1d",last_option);
			wattroff(bottom_win, A_REVERSE);
			break;

		case 'P' :
		case 'p' :
			screen_dump();
			mvwaddstr(bottom_win, 1, 2, "                                                ");
			wattron(bottom_win, A_REVERSE);
			mvwprintw(bottom_win, 1, 74, "%.1d", last_option);
			wattroff(bottom_win, A_REVERSE);
			break;

		case 'A' :
		case 'a' :
		case 'T' :
		case 't' :
			Get_The_Display(T_NORMAL);
			wrefresh(bottom_win);
			nap(1500);
			mvwaddstr(bottom_win, 1, 2, "                                    ");
			wattron(bottom_win, A_REVERSE);
			mvwprintw(bottom_win, 1, 74, "%.1d", last_option);
			wattroff(bottom_win, A_REVERSE);
			if (last_option == 2)
				wi_vars(2);
			break;

		case 'G' :
		case 'g' :
			Show_Graphics();
			wattron(bottom_win, A_REVERSE);
			mvwprintw(bottom_win, 1, 74, "%.1d", last_option);
			wattroff(bottom_win, A_REVERSE);
			break;

		case KEY_UP:
		case KEY_PPAGE:
		case KEY_RIGHT:
			if (last_option >= 7)
				last_option = 0;
			option = ++last_option;
			mvwaddstr(main_win, 17, 2, "                                                                           ");
			wattron(bottom_win, A_NORMAL);
			mvwaddstr(bottom_win, 1, 76, " ");
			wattron(bottom_win, A_REVERSE);
			mvwprintw(bottom_win, 1, 74, "%.1d",option);
			wattroff(bottom_win, A_REVERSE);
			wnoutrefresh(bottom_win);
			wmove(main_win, 1, 1);
			wclrtobot(main_win);
			drawdisp(13);
			wi_vars(option);
			break;

		case KEY_DOWN:
		case KEY_NPAGE:
		case KEY_LEFT:
			if (last_option <= 1)
				last_option = 8;
			option = --last_option;
			mvwaddstr(main_win, 17, 2, "                                                                           ");
			wattron(bottom_win, A_NORMAL);
			mvwaddstr(bottom_win, 1, 76, " ");
			wattron(bottom_win, A_REVERSE);
			mvwprintw(bottom_win, 1, 74, "%.1d",option);
			wattroff(bottom_win, A_REVERSE);
			wnoutrefresh(bottom_win);
			wmove(main_win, 1, 1);
			wclrtobot(main_win);
			drawdisp(13);
			wi_vars(option);
			break;

		case 'U' :
		case 'u' :
			option = 50;

		case '1' :
		case '2' :
		case '3' :
		case '4' :
		case '5' :
		case '6' :
		case '7' :

			mvwaddstr(main_win, 17, 2, "                                                                           ");
			option -= 48; /* subtract the ASCII constant. */
			wattron(bottom_win, A_NORMAL);
			mvwaddstr(bottom_win, 1, 76, " ");
			wattron(bottom_win, A_REVERSE);
			mvwprintw(bottom_win, 1, 74, "%.1d",option);
			wattroff(bottom_win, A_REVERSE);
			wnoutrefresh(bottom_win);
			wmove(main_win, 1, 1);
			wclrtobot(main_win);
			drawdisp(13);
			wi_vars(option);
			last_option = option;
			break;

		case 'W' :
		case 'w' :
			wmove(main_win, 1, 1);
			wclrtobot(main_win);
			wnoutrefresh(main_win);
			watch_vars();
			return(0);
		
		case 'Q' :
		case 'q' :
			drawdisp(1);
			wrefresh(top_win);
			return(0);

		default :
			beep();
			continue;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: vars()                                    Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the var info and pass it on.                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void wi_vars(int screen_id)
{

/*
	display the selected screen.
*/
	(*screen[screen_id])();
	wnoutrefresh(main_win);
	doupdate();
}

/*
		OK I have to make all the different screens now.
	dummy entry
*/

void vscreen0()
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/06/03 |
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

void vscreen1(void)
{

char	conf_value[64];
int	conf_default = -1;
int	conf_current = -1;
int	conf_min     = -1;
int	conf_max     = -1;
int	extra_bit    = 0;

	conf_value[0] = '\0';

	if (size_flag)
		extra_bit = 10;

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 10, "Parameter");
	mvwaddstr(main_win, 2, 28, "Kernel Name");
	mvwaddstr(main_win, 2, 41, "Minimum");
	mvwaddstr(main_win, 2, 50, "Maximum");
	mvwaddstr(main_win, 2, 59, "Default");
	mvwaddstr(main_win, 2, 68, "Current");
	mvwaddstr(main_win, 4, 3, "Disks and Buffers");
	mvwaddstr(main_win, 12 + extra_bit, 3, "Character Buffers");
	wattroff(main_win, A_BOLD);


	strcpy(conf_value, "NDISK");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 5, 1, "  disk drives              %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_disk);
	else
		mvwprintw(main_win, 5, 1, "  Disk drives              %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 50, na, v.v_disk);


	strcpy(conf_value, "NBUF");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 6, 1, "  system cache buffers     %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_buf);
	else
		mvwprintw(main_win, 6, 1, "  System cache buffers     %11s  %7d  %7d  %7s  %7d",
		    conf_value, 0, 32000, na, v.v_buf);


	strcpy(conf_value, "NPBUF");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 7, 1, "  read/write I/O bufs.     %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_pbuf);
	else
		mvwprintw(main_win, 7, 1, "  Read/Write I/O bufs.     %11s  %7d  %7d  %7s  %7d",
		    conf_value, 6, 60, na, v.v_pbuf);


	strcpy(conf_value, "NHBUF");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 8, 1, "  hash queues for NBUF     %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_hbuf);
	else
		mvwprintw(main_win, 8, 1, "  Hash queues for NBUF     %11s  %7d  %7d  %7s  %7d",
		    conf_value, 32, 16384, na, v.v_hbuf);


	strcpy(conf_value, "MAXBUF");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 9, 1, "  max bufs in buf cache    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_maxbuf);
	else
		mvwprintw(main_win, 9, 1, "  Max bufs in buf cache    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 0, 32000, na, v.v_maxbuf);


	strcpy(conf_value, "CTBUFSIZE");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 10, 1, "  ct buf tape size         %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 10, 1, "  CT buf tape size         %11s  %7d  %7d  %7d  %7d",
		    conf_value, 0, 256, na, na);

	/*
		CLIST and other stuff
	*/

	strcpy(conf_value, "NCLIST");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 13 + extra_bit, 1, "  serial char. list bufs   %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_clist);
	else
		mvwprintw(main_win, 13 + extra_bit, 1, "  Serial char. list bufs   %11s  %7d  %7d  %7s  %7d",
		    conf_value, 120, 16640, na, v.v_clist);


	strcpy(conf_value, "NEMAP");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 14 + extra_bit, 1, "  max I/O translation maps %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_emap);
	else
		mvwprintw(main_win, 14 + extra_bit, 1, "  Max I/O translation maps %11s  %7d  %7d  %7s  %7d",
		    conf_value, 10, 10, na, v.v_emap);


	strcpy(conf_value, "NUMSXT");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 15 + extra_bit, 1, "  shell layer sessions     %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_sxt);
	else
		mvwprintw(main_win, 15 + extra_bit, 1, "  shell layer sessions     %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 32, na, v.v_sxt);


	strcpy(conf_value, "SPTMAP");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 16 + extra_bit, 1, "  virtual space alloc map  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_sptmap);
	else
		mvwprintw(main_win, 16 + extra_bit, 1, "  Virtual space alloc map  %11s  %7d  %7d  %7s  %7d",
		    conf_value, 50, 500, na, v.v_sptmap);


	if (size_flag) {
		/*
			extra disk stufF
		*/
		strcpy(conf_value, "DMAABLEBUF");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 11, 1, "  trans. bufs for DMA 16Mb %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 11, 1, "  Trans. bufs for DMA 16Mb %11s  %7d  %7d  %7s  %7s",
			    conf_value, 4, 128, na, na);


		strcpy(conf_value, "PLOWBUFS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 12, 1, "  %% of bufs below 16Mb     %11s  %7d%% %7d%% %7d%% %7d%%",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 12, 1, "  %% of bufs below 16Mb     %11s  %7d%% %7d%% %7s%% %7s%%",
			    conf_value, 5, 100, na, na);


		strcpy(conf_value, "NCOPYBUF");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 13, 1, "  num copy bufs below 16Mb %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 13, 1, "  Num copy bufs below 16Mb %11s  %7d  %7d  %7s  %7s",
			    conf_value, 4, 1000, na, na);


		strcpy(conf_value, "NAUTOUP");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 14, 1, "  sec before bdflush writes%11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 14, 1, "  sec before bdflush writes%11s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 60, na, na);


		strcpy(conf_value, "BDFLUSHR");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 15, 1, "  sec to run bdflush       %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 15, 1, "  sec to run bdflush       %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 300, na, na);


		strcpy(conf_value, "PUTBUFSZ");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 16, 1, "  size of buffer putbuf    %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 16, 1, "  size of buffer putbuf    %11s  %7d  %7d  %7s  %7s",
			    conf_value, 2000, 10000, na, na);


		strcpy(conf_value, "PIOMAP");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 18, 1, "  size of PIO breakup map  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 18, 1, "  size of PIO breakup map  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 50, 50, na, na);


		strcpy(conf_value, "PIOMAXSZ");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 19, 1, "  max pages for Prog. I/O  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 19, 1, "  max pages for Prog. I/O  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 4, 64, na, na);

		/*
			extra clist stuff
		*/
		strcpy(conf_value, "NUMXT");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 17 + extra_bit, 1, "  layers of bitmapped devs %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 17 + extra_bit, 1, "  layers of bitmapped devs %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 32, na, na);


		strcpy(conf_value, "NKDVTTY");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 18 + extra_bit, 1, "  virtual cons kbd tty's   %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 18 + extra_bit, 1, "  virtual cons kbd tty's   %11s  %7d  %7d  %7s  %7s",
			    conf_value, 8, 8, na, na);


		strcpy(conf_value, "NCPYRIGHT");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 19 + extra_bit, 1, "  num of copyright strings %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 19 + extra_bit, 1, "  num of copyright strings %11s  %7d  %7d  %7s  %7s",
			    conf_value, 10, 10, na, na);


		strcpy(conf_value, "MAX_CFGSIZE");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 21 + extra_bit, 1, "  info saved by str driver %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 21 + extra_bit, 1, "  info saved by str driver %11s  %7d  %7d  %7s  %7s",
			    conf_value, 256, 32768, na, na);


		strcpy(conf_value, "PRFMAX");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 22 + extra_bit, 1, "  # symbols for profiler   %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 22 + extra_bit, 1, "  # symbols for profiler   %11s  %7d  %7d  %7s  %7s",
			    conf_value, 2048, 8192, na, na);


		strcpy(conf_value, "KDBSYMSIZE");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 23 + extra_bit, 1, "  kernel debug symbol size %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 23 + extra_bit, 1, "  kernel debug symbol size %11s  %7d  %7d  %7s  %7s",
			    conf_value, 50000, 500000, na, na);
	}
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/06/03 |
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

void vscreen2(void)
{

struct callo *call_outs;

extern int The_Current_Display;
extern unsigned kmaxmem;
register	i;

int mnt_ctr   = 0;
int reg_ctr   = 0;
int file_ctr  = 0;
int proc_ctr  = 0;
int call_ctr  = 0;
int inode_ctr = 0;
int flock_ctr = 0;

int percent_mnt      = 0;
int percent_regions  = 0;
int percent_files    = 0;
int percent_procs    = 0;
int percent_callouts = 0;
int percent_inodes   = 0;
int percent_flocks   = 0;
int percent_mem      = 0;

int file_ovf  = 0;
int proc_ovf  = 0;
int inode_ovf = 0;
int flock_ovf = 0;

char	conf_value[64];
int	conf_current = -1;
int	conf_default = -1;
int	conf_min = -1;
int	conf_max = -1;
int	extra_bit = 0;


	/*
		Work out the values of the tables
	*/
	conf_value[0] = '\0';

	/* the system overflow table */
	kmem_read(&syserr, namelist[NM_SYSERR].n_value, sizeof(struct syserr));

	/* file lock info */
	kmem_read(&flckinfo, namelist[NM_FLCKINFO].n_value, sizeof(struct flckinfo));

	/* get file locks */
	flock_ctr = flckinfo.reccnt;

	/* get icount */
	inodes = (inode_t *) malloc(v.v_inode * sizeof(inode_t));
	kmem_read(inodes, namelist[NM_INODE].n_value,
	    sizeof(inode_t) * v.v_inode);

	for (i = 0; i < v.v_inode; i++) {
		if (inodes[i].i_count MATCHES)
			continue;
		inode_ctr++;
	}
	free((char *) inodes);

	/* get mount count */
	mounts = (struct mount *) malloc(v.v_mount * sizeof(struct mount));
	kmem_read(mounts, namelist[NM_MOUNT].n_value,
	    sizeof(struct mount) * v.v_mount);

	for (i = 0; i < v.v_mount; i++) {
		if (mounts[i].m_flags MATCHES)
			continue;
		mnt_ctr++;
	}
	free((char *) mounts);

	/* get file count */
	files = (file_t *) malloc(v.v_file * sizeof(file_t));
	kmem_read(files, namelist[NM_FILE].n_value,
	    sizeof(file_t) * v.v_file);

	for (i = 0; i < v.v_file; i++) {
		if (files[i].f_count MATCHES || files[i].f_flag MATCHES)
			continue;
		file_ctr++;
	}
	free((char *) files);

	/* get proc count */
	procs = (proc_t *) malloc(v.v_proc * sizeof(proc_t));
	kmem_read(procs, namelist[NM_PROC].n_value,
	    sizeof(proc_t) * v.v_proc);

	for (i = 0; i < v.v_proc; i++) {
		if (procs[i].p_stat MATCHES)
			continue;
		proc_ctr++;
	}
	free((char *) procs);

	/* get regions */
	regions = (struct region *) malloc(v.v_region * sizeof(struct region));
	kmem_read(regions, namelist[NM_REGION].n_value,
	    sizeof(struct region) * v.v_region);

	for (i = 0; i < v.v_region; i++) {
		if (regions[i].r_pgsz MATCHES)
			continue;
		reg_ctr++;
	}
	free((char *) regions);

	/* get callout count */
	call_outs = (struct callo *) malloc(v.v_call * sizeof(struct callo));
	kmem_read(call_outs, namelist[NM_CALLOUT].n_value,
	    sizeof(struct callo) * v.v_call);

	for (i = 0; i < v.v_call; i++) {
		if (call_outs[i].c_id MATCHES)
			continue;
		call_ctr++;
	}
	free((char *) call_outs);

	kmem_read(&kavailmem, namelist[NM_KAVAILMEM].n_value, sizeof(kavailmem));

	/*
		work out the percentages
	*/
	percent_mnt      = (mnt_ctr * 100 / v.v_mount);
	percent_regions  = (reg_ctr * 100 / v.v_region);
	percent_files    = (file_ctr * 100 / v.v_file);
	percent_procs    = (proc_ctr * 100 / v.v_proc);
	percent_callouts = (call_ctr * 100 / v.v_call);
	percent_inodes   = (inode_ctr * 100 / v.v_inode);
	percent_flocks   = flckinfo.recs ? (flckinfo.reccnt * 100 / flckinfo.recs): 0;
	percent_mem      = (((kavailmem * NBPC) / 1024) * 100 / ((kmaxmem * NBPC) / BLKSIZ));

	/*
		work out the overflows.
	*/
	file_ovf  = syserr.fileovf;
	proc_ovf  = syserr.procovf;
	inode_ovf = flckinfo.recovf;
	flock_ovf = syserr.fileovf;

	if (size_flag)
		extra_bit = 11;

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 10, "Parameter");
	mvwaddstr(main_win, 2, 28, "Kernel Name");
	mvwaddstr(main_win, 2, 41, "Minimum");
	mvwaddstr(main_win, 2, 50, "Maximum");
	mvwaddstr(main_win, 2, 59, "Default");

	switch(The_Current_Display) {
		case BY_TIME:
			mvwaddstr(main_win, 2, 68, "Current");
			break;

		case BY_PERCNT:
			mvwaddstr(main_win, 2, 68, "Percent");
			break;

		case BY_ERRORS:
			mvwaddstr(main_win, 2, 68, "Ovflows");
			break;
	}
	
	if (size_flag)
		mvwaddstr(main_win, 9 + extra_bit, 3, "Clocks");
	mvwaddstr(main_win, 3, 3, "Files");
	mvwaddstr(main_win, 3, 10, "Inodes and Filesystems");
	mvwaddstr(main_win, 12 + extra_bit, 3, "Processes");
	mvwaddstr(main_win, 12 + extra_bit, 14, "Memory and Swapping");
	wattroff(main_win, A_BOLD);

	strcpy(conf_value, "NINODE");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 4, 1, "  max size of inode table  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, v.v_inode, inode_ctr);
	else
		mvwprintw(main_win, 4, 1, "  max size of inode table  %11s  %7d  %7d  %7d  %7d",
		    conf_value, 100, 6500, v.v_inode, inode_ctr);


	strcpy(conf_value, "NHINODE");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 5, 1, "  inode hash table size    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_hinode);
	else
		mvwprintw(main_win, 5, 1, "  inode hash table size    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 64, 4096, na, v.v_hinode);


	strcpy(conf_value, "NMOUNT");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 6, 1, "  max size of mount table  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, v.v_mount, mnt_ctr);
	else
		mvwprintw(main_win, 6, 1, "  max size of mount table  %11s  %7d  %7d  %7d  %7d",
		    conf_value, 4, 50, v.v_mount, mnt_ctr);


	strcpy(conf_value, "NFILE");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 7, 1, "  max size of file table   %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, v.v_file, file_ctr);
	else
		mvwprintw(main_win, 7, 1, "  max size of file table   %11s  %7d  %7d  %7d  %7d",
		    conf_value, 100, 8000, v.v_file, file_ctr);


	strcpy(conf_value, "NOFILES");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 8, 1, "  num of files / process   %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_nofiles);
	else
		mvwprintw(main_win, 8, 1, "  num of files / process   %11s  %7d  %7d  %7s  %7d",
		    conf_value, 60, 150, na, v.v_nofiles);

		strcpy(conf_value, "FLCKREC");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 9, 1, "  num of record locks      %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, 0);
		else
			mvwprintw(main_win, 9, 1, "  num of record locks      %11s  %7d  %7d  %7s  %7s",
			    conf_value, 50, 500, na, na);

	strcpy(conf_value, "NCALL");
	if (size_flag)
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 10 + extra_bit, 1, "  number of call outs      %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, v.v_call, call_ctr);
		else
			mvwprintw(main_win, 10 + extra_bit, 1, "  number of call outs      %11s  %7d  %7d  %7d  %7d",
			    conf_value, 30, 500, v.v_call, call_ctr);
	else
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 10, 1, "  number of call outs      %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, v.v_call, call_ctr);
		else
			mvwprintw(main_win, 10, 1, "  number of call outs      %11s  %7d  %7d  %7d  %7d",
			    conf_value, 30, 500, v.v_call, call_ctr);


	/*
		MEM and other stuff
	*/
	strcpy(conf_value, "NPROC");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 13 + extra_bit, 1, "  max size of proc table   %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, v.v_proc, proc_ctr);
	else
		mvwprintw(main_win, 13 + extra_bit, 1, "  max size of proc table   %11s  %7d  %7d  %7d  %7d",
		    conf_value, 30, 3000, v.v_proc, proc_ctr);


	strcpy(conf_value, "MAXUP");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 14 + extra_bit, 1, "  max processeses per user %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_maxup);
	else
		mvwprintw(main_win, 14 + extra_bit, 1, "  max processeses per user %11s  %7d  %7d  %7s  %7d",
		    conf_value, 25, 200, na, v.v_maxup);


	strcpy(conf_value, "MAXUMEM");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 15 + extra_bit, 1, "  max user system memory   %11s  %7dk %7dk %7uk %7uk",
		    conf_value, conf_min, conf_max, conf_default, (kmaxmem * NBPC) / BLKSIZ, (kmaxmem * NBPC) / BLKSIZ);
	else
		mvwprintw(main_win, 15 + extra_bit, 1, "  max user system memory   %11s  %7sk %7dk %7dk %7uk",
		    conf_value, 2560, 131072, na, (kmaxmem * NBPC) / BLKSIZ);


	strcpy(conf_value, "NREGION");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 16 + extra_bit, 1, "  number of regions        %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, v.v_region, reg_ctr);
	else
		mvwprintw(main_win, 16 + extra_bit, 1, "  number of regions        %11s  %7d  %7d  %7d  %7d",
		    conf_value, 100, 10000, v.v_region, reg_ctr);


	strcpy(conf_value, "MAXPMEM");
	mvwprintw(main_win, 17 + extra_bit, 1, "  max amount of mem to use %11s        0k %7dk %7s  %7dk ",
	    conf_value, v.v_maxpmem MATCHES ? ((kmaxmem * NBPC) / BLKSIZ): ((v.v_maxpmem * NBPC) / BLKSIZ), na, (kavailmem
	    * NBPC) / 1024);

	if (size_flag) {
		/*
			EXTRA STUFF
		*/


		strcpy(conf_value, "SHLBMAX");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 11, 1, "  shared libs attached/proc%11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 11, 1, "  shared libs attached/proc%11s  %7d  %7d  %7s  %7s",
			    conf_value, 2, 16, na, na);



		strcpy(conf_value, "NMPBUF");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 13, 1, "  EAFS cluster buffers     %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 13, 1, "  EAFS cluster buffers     %11s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 100, na, na);


		strcpy(conf_value, "NMPHEADBUF");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 14, 1, "  EAFS standalone buffers  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 14, 1, "  EAFS standalone buffers  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 10000, na, na);


		strcpy(conf_value, "BFREEMIN");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 15, 1, "  min # bufs on freelist   %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 15, 1, "  min # bufs on freelist   %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 100, na, na);


		strcpy(conf_value, "S5CACHEENTS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 16, 1, "  # comp. in fileame cache %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 16, 1, "  # comp. in fileame cache %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 4096, na, na);


		strcpy(conf_value, "S5HASHQS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 17, 1, "  hash queues in name cache%11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 17, 1, "  hash queues in name cache%11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 8191, na, na);


		strcpy(conf_value, "S5OFBIAS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 18, 1, "  files kept in cache      %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 18, 1, "  files kept in cache      %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 256, na, na);


		strcpy(conf_value, "NGROUPS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 10, 1, "  max supplemental groups  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 10, 1, "  max supplemental groups  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 16, na, na);

		/*
			extra proc, mem and swapping stuff
		*/

		strcpy(conf_value, "GPGSLO");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 18 + extra_bit, 1, "  low # in pages for vhand %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 18 + extra_bit, 1, "  low # in pages for vhand %11s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 200, na, na);


		strcpy(conf_value, "GPGSHI");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 19 + extra_bit, 1, "  hi # in pages for vhand  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 19 + extra_bit, 1, "  hi # in pages for vhand  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 300, na, na);


		strcpy(conf_value, "MAXSC");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 20 + extra_bit, 1, "  max pages swapped at once%11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 20 + extra_bit, 1, "  max pages swapped at once%11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 1, na, na);


		strcpy(conf_value, "MAXFC");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 21 + extra_bit, 1, "  max pages put to freelist%11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 21 + extra_bit, 1, "  max pages put to freelist%11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 1, na, na);


		strcpy(conf_value, "AGEINTERVAL");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 22 + extra_bit, 1, "  # ticks until page aged  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 22 + extra_bit, 1, "  # ticks until page aged  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 2, 100, na, na);


		strcpy(conf_value, "ULIMIT");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 23 + extra_bit, 1, "  file size in 512k blocks %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 23 + extra_bit, 1, "  file size in 512k blocks %11s  %7d  %7d  %7s  %7s",
			    conf_value, 2048, 2097152, na, na);

	}

	/*
		Display the table
	*/

	/*
		Print the items bold and flashing if they above the defined in wi.h
	*/

	switch(The_Current_Display) {

	case BY_TIME:
		wattron(main_win, A_BOLD);
		mvwprintw(main_win, 4, 68, "%7d ", inode_ctr);
		mvwprintw(main_win, 6, 68, "%7d ", mnt_ctr);
		mvwprintw(main_win, 7, 68, "%7d ", file_ctr);
		mvwprintw(main_win, 9, 68, "%7d ", flock_ctr);
	
		mvwprintw(main_win, 10 + extra_bit, 68, "%7d ", call_ctr);
	
		mvwprintw(main_win, 13 + extra_bit, 68, "%7d ", proc_ctr);
		mvwprintw(main_win, 16 + extra_bit, 68, "%7d ", reg_ctr);
		mvwprintw(main_win, 17 + extra_bit, 68, "%7d",
	    	(kavailmem * NBPC) / 1024);
		wattroff(main_win, A_BOLD);
	
		wattron(main_win, A_BOLD | A_BLINK);
		if (percent_inodes >= per_max_ninode)
			mvwprintw(main_win, 4, 68, "%7d", inode_ctr);
		if (percent_mnt >= per_max_nmount)
			mvwprintw(main_win, 6, 68, "%7d", mnt_ctr);
		if (percent_files >= per_max_nfile)
			mvwprintw(main_win, 7, 68, "%7d", file_ctr);
		if (percent_flocks >= per_max_nfile)
			mvwprintw(main_win, 9, 68, "%7d ", flock_ctr);
	
		if (percent_callouts >= per_max_ncall)
			mvwprintw(main_win, 10 + extra_bit, 68, "%7d", call_ctr);
	
		if (percent_procs >= per_max_nproc)
			mvwprintw(main_win, 13 + extra_bit, 68, "%7d", proc_ctr);
		if (percent_regions >= per_max_nregion)
			mvwprintw(main_win, 16 + extra_bit, 68, "%7d", reg_ctr);
		wattroff(main_win, A_BOLD | A_BLINK);
		break;

	case BY_PERCNT:
		mvwaddstr(main_win, 4, 75, "%");
		mvwaddstr(main_win, 6, 75, "%");
		mvwaddstr(main_win, 7, 75, "%");
		mvwaddstr(main_win, 9, 75, "%");
	
		mvwaddstr(main_win, 10 + extra_bit, 75, "%");
	
		mvwaddstr(main_win, 13 + extra_bit, 75, "%");
		mvwaddstr(main_win, 16 + extra_bit, 75, "%");
		mvwaddstr(main_win, 17 + extra_bit, 75, "%");

		wattron(main_win, A_BOLD);
		mvwprintw(main_win, 4, 72, "%3d", percent_inodes);
		mvwprintw(main_win, 6, 72, "%3d", percent_mnt);
		mvwprintw(main_win, 7, 72, "%3d", percent_files);
		mvwprintw(main_win, 9, 72, "%3d", percent_flocks);
	
		mvwprintw(main_win, 10 + extra_bit, 72, "%3d", percent_callouts);
	
		mvwprintw(main_win, 13 + extra_bit, 72, "%3d", percent_procs);
		mvwprintw(main_win, 16 + extra_bit, 72, "%3d", percent_regions);
		mvwprintw(main_win, 17 + extra_bit, 68, "    %3d", percent_mem);
			wattroff(main_win, A_BOLD);
	
		wattron(main_win, A_BOLD | A_BLINK);
		if (percent_inodes >= per_max_ninode)
			mvwprintw(main_win, 4, 72, "%3d", percent_inodes);
		if (percent_mnt >= per_max_nmount)
			mvwprintw(main_win, 6, 72, "%3d", percent_mnt);
		if (percent_files >= per_max_nfile)
			mvwprintw(main_win, 7, 72, "%3d", percent_files);
		if (percent_flocks >= per_max_nfile)
			mvwprintw(main_win, 9, 72, "%3d", percent_flocks);
	
		if (percent_callouts >= per_max_ncall)
			mvwprintw(main_win, 10 + extra_bit, 72, "%3d", percent_callouts);
	
		if (percent_procs >= per_max_nproc)
			mvwprintw(main_win, 13 + extra_bit, 72, "%3d", percent_procs);
		if (percent_regions >= per_max_nregion)
			mvwprintw(main_win, 16 + extra_bit, 72, "%3d", percent_regions);
		wattroff(main_win, A_BOLD | A_BLINK);
		break;

	case BY_ERRORS:
		mvwaddstr(main_win, 4, 75, " ");
		mvwaddstr(main_win, 6, 75, " ");
		mvwaddstr(main_win, 7, 75, " ");
		mvwaddstr(main_win, 9, 75, " ");
	
		mvwaddstr(main_win, 10 + extra_bit, 75, " ");
	
		mvwaddstr(main_win, 13 + extra_bit, 75, " ");
		mvwaddstr(main_win, 16 + extra_bit, 75, " ");
		mvwaddstr(main_win, 17 + extra_bit, 75, " ");

		if (inode_ovf)
			wattron(main_win, A_REVERSE);
		else
			wattron(main_win, A_BOLD);
		mvwprintw(main_win, 4, 72, "%3d", inode_ovf);

		if (inode_ovf)
			wattroff(main_win, A_REVERSE);
		else
			wattroff(main_win, A_BOLD);

		if (file_ovf)
			wattron(main_win, A_REVERSE);
		else
			wattron(main_win, A_BOLD);
		mvwprintw(main_win, 7, 72, "%3d", file_ovf);

		if (file_ovf)
			wattroff(main_win, A_REVERSE);
		else
			wattroff(main_win, A_BOLD);

		if (flock_ovf)
			wattron(main_win, A_REVERSE);
		else
			wattron(main_win, A_BOLD);
		mvwprintw(main_win, 9, 72, "%3d", flock_ovf);

		if (flock_ovf)
			wattroff(main_win, A_REVERSE);
		else
			wattroff(main_win, A_BOLD);

		if (proc_ovf)
			wattron(main_win, A_REVERSE);
		else
			wattron(main_win, A_BOLD);
		mvwprintw(main_win, 13 + extra_bit, 72, "%3d", proc_ovf);

		if (proc_ovf)
			wattroff(main_win, A_REVERSE);
		else
			wattroff(main_win, A_BOLD);

		wattron(main_win, A_BOLD);
		mvwprintw(main_win, 6, 72, "N/A");
	
		mvwprintw(main_win, 10 + extra_bit, 72, "N/A");
	
		mvwprintw(main_win, 16 + extra_bit, 72, "N/A");
		mvwprintw(main_win, 17 + extra_bit, 68, "    N/A");
		wattroff(main_win, A_BOLD);
		break;
	}

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/06/03 |
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

void vscreen3(void)
{

char	conf_value[64];
int	conf_current = -1;
int	conf_default = -1;
int	conf_min = -1;
int	conf_max = -1;
int	extra_bit1 = 0;
int	extra_bit2 = 0;

	conf_value[0] = '\0';

	if (size_flag) {
		extra_bit1 = 1;
		extra_bit2 = 2;
	}

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 10, "Parameter");
	mvwaddstr(main_win, 2, 28, "Kernel Name");
	mvwaddstr(main_win, 2, 41, "Minimum");
	mvwaddstr(main_win, 2, 50, "Maximum");
	mvwaddstr(main_win, 2, 59, "Default");
	mvwaddstr(main_win, 2, 68, "Current");

	mvwaddstr(main_win, 4, 3, "MultiScreens");
	mvwaddstr(main_win, 11 + extra_bit1, 3, "Hardware parameters");
	if (size_flag)
		mvwaddstr(main_win, 22, 3, "SCSI disk parameters");
	wattroff(main_win, A_BOLD);


	strcpy(conf_value, "NSCRN");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 5, 1, "  number of MultiScreens   %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_scrn);
	else
		mvwprintw(main_win, 5, 1, "  number of MultiScreens   %11s  %7d  %7d  %7s  %7d",
		    conf_value, 0, 12, na, v.v_scrn);

	strcpy(conf_value, "NSPTTYS");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 6, 1, "  number of pseudo-ttys    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);

	else
		mvwprintw(main_win, 6, 1, "  number of pseudo-ttys    %11s  %7d  %7d  %7s  %7s",
		    conf_value, 1, 256, na, na);


	strcpy(conf_value, "TBLNK");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 8, 1, "  seconds for screensaver  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 8, 1, "  seconds for screensaver  %11s  %7d  %7d  %7s  %7s",
		    conf_value, 0, 32767, na, na);


	strcpy(conf_value, "SCRNMEM");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 9, 1, "  # 1024k blks for scrnsaver%10s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, (conf_current MATCHES ? v.v_scrnmem: conf_current));
	else
		mvwprintw(main_win, 9, 1, "  # 1024k blks for scrnsaver%10s  %7dk %7dk %7s  %7s",
		    conf_value, 9, 128, na, na);


	strcpy(conf_value, "KBTYPE");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 12 + extra_bit2, 1, "  keyboard type AT or PC/XT%11s  %7s  %7s  %7s  %7s",
		    conf_value, "PC", "AT", (conf_default MATCHES ? "PC": "AT"), (v.v_kbtype MATCHES ? "PC": "AT"));
	else
		mvwprintw(main_win, 12 + extra_bit2, 1, "  keyboard type AT or PC/XT%11s  %7s  %7s  %7s  %7s",
		    conf_value, "PC", "AT", na, (v.v_kbtype MATCHES ? "PC": "AT"));


	strcpy(conf_value, "DO387CR3");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 13 + extra_bit2, 1, "  High order bits of CR3   %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 13 + extra_bit2, 1, "  High order bits of CR3   %11s  %7d  %7d  %7d  %7s",
		    conf_value, 0, 1, 0, na);


	strcpy(conf_value, "DMAEXCL");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 14 + extra_bit2, 1, "  multi DMA requests       %11s  %7s  %7s  %7s  %7s",
		    conf_value, "No", "Yes", (conf_default MATCHES ? "Yes": "No"), (conf_current MATCHES ? "Yes": "No"));
	else
		mvwprintw(main_win, 14 + extra_bit2, 1, "  multi DMA requests       %11s  %7s  %7s  %7s  %7s",
		    conf_value, "No", "Yes", na, na);


	strcpy(conf_value, "NSHINTR");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 15 + extra_bit2, 1, "  Num of devs on same intr %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 15 + extra_bit2, 1, "  Num of devs on same intr %11s  %7d  %7d  %7d  %7s",
		    conf_value, 2, 20, 8, na);

	strcpy(conf_value, "MODE_SELECT");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 16 + extra_bit2, 1, "  Does printer check mode  %11s  %7s  %7s  %7s  %7s",
		    conf_value, "No", "Yes", (conf_default MATCHES ? "No": "Yes"), (conf_current MATCHES ? "No": "Yes"));
	else
		mvwprintw(main_win, 16 + extra_bit2, 1, "  Does printer check mode  %11s  %7s  %7s  %7s  %7s",
		    conf_value, "No", "Yes", na, na);


	strcpy(conf_value, "VGA_PLASMA");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 17 + extra_bit2, 1, "  VGA gas plasma present   %11s  %7s  %7s  %7s  %7s",
		    conf_value, "No", "Yes", (conf_default MATCHES ? "No": "Yes"), (conf_current MATCHES ? "No": "Yes"));
	else
		mvwprintw(main_win, 17 + extra_bit2, 1, "  VGA Gas plasma present   %11s  %7s  %7s  %7s  %7s",
		    conf_value, "No", "Yes", na, na);

	if (size_flag) {

		strcpy(conf_value, "NAHACCB");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 24, 1, "  num of Adaptec mailboxes %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 24, 1, "  num of Adaptec mailboxes %11s  %7d  %7d  %7s  %7s",
			    conf_value, 4, 255, na, na);


		strcpy(conf_value, "SDSKOUT");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 25, 1, "  outstanding SCSI requests%11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 25, 1, "  outstanding SCSI requests%11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 256, na, na);
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/06/03 |
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

void vscreen4(void)
{

char	conf_value[64];
int	conf_current = -1;
int	conf_default = -1;
int	conf_min     = -1;
int	conf_max     = -1;
int	extra_bit1   = 0;
int	extra_bit2   = 0;
int	extra_bit3   = 0;

	conf_value[0] = '\0';

	if (size_flag) {
		extra_bit1 = 1;
		extra_bit2 = 2;
		extra_bit3 = 3;
	}

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 10, "Parameter");
	mvwaddstr(main_win, 2, 28, "Kernel Name");
	mvwaddstr(main_win, 2, 41, "Minimum");
	mvwaddstr(main_win, 2, 50, "Maximum");
	mvwaddstr(main_win, 2, 59, "Default");
	mvwaddstr(main_win, 2, 68, "Current");

	mvwaddstr(main_win, 4, 3, "Message Queues");
	mvwaddstr(main_win, 13 + extra_bit2, 3, "Event Queues and Devices");

	if (size_flag)
		mvwaddstr(main_win, 22, 3, "Asynchronous I/O");
	wattroff(main_win, A_BOLD);


	strcpy(conf_value, "MSGMAP");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 5 + extra_bit1, 1, "  size of control map      %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, msginfo.msgmap);
	else
		mvwprintw(main_win, 5 + extra_bit1, 1, "  size of control map      %11s  %7d  %7d  %7s  %7d",
		    conf_value, 10, 1024, na, msginfo.msgmap);


	strcpy(conf_value, "MSGMAX");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 6 + extra_bit1, 1, "  max size of a message    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, msginfo.msgmax);
	else
		mvwprintw(main_win, 6 + extra_bit1, 1, "  max size of a message    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 512, 8192, na, msginfo.msgmax);


	strcpy(conf_value, "MSGMNB");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 7 + extra_bit1, 1, "  max char in a message    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, msginfo.msgmnb);
	else
		mvwprintw(main_win, 7 + extra_bit1, 1, "  max char in a message    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 4096, 16384, na, msginfo.msgmnb);


	strcpy(conf_value, "MSGMNI");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 8 + extra_bit1, 1, "  message que identifiers  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, msginfo.msgmni);
	else
		mvwprintw(main_win, 8 + extra_bit1, 1, "  message que identifiers  %11s  %7d  %7d  %7s  %7d",
		    conf_value, 50, 50, na, msginfo.msgmni);


	strcpy(conf_value, "MSGTQL");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 9 + extra_bit1, 1, "  msg headers - systemwide %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, msginfo.msgtql);
	else
		mvwprintw(main_win, 9 + extra_bit1, 1, "  msg headers - systemwide %11s  %7d  %7d  %7s  %7d",
		    conf_value, 40, 40, na, msginfo.msgtql);


	strcpy(conf_value, "MSGSSZ");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 10 + extra_bit1, 1, "  msg byte segment size    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, msginfo.msgssz);
	else
		mvwprintw(main_win, 10 + extra_bit1, 1, "  msg byte segment size    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 8, 8, na, msginfo.msgssz);


	strcpy(conf_value, "MSGSEG");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 11 + extra_bit1, 1, "  # of message segments    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, msginfo.msgseg);
	else
		mvwprintw(main_win, 11 + extra_bit1, 1, "  # of message segments    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1024, 1024, na, msginfo.msgseg);

	/*
		Event Queues.
	*/
	strcpy(conf_value, "EVQUEUES");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 14 + extra_bit3, 1, "  max # of event queues    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_evqueues);
	else
		mvwprintw(main_win, 14 + extra_bit3, 1, "  max # of event queues    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 256, na, v.v_evqueues);


	strcpy(conf_value, "EVDEVS");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 15 + extra_bit3, 1, "  num of devices per queue %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_evdevs);
	else
		mvwprintw(main_win, 15 + extra_bit3, 1, "  num of devices per queue %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 256, na, v.v_evdevs);


	strcpy(conf_value, "EVDEVSPERQ");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 16 + extra_bit3, 1, "  max # of event queues    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_evdevsperq);
	else
		mvwprintw(main_win, 16 + extra_bit3, 1, "  max # of event queues    %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 16, na, v.v_evdevsperq);


	/*
		Extra stuff for long screen
	*/
	if (size_flag) {
		strcpy(conf_value, "NAIOPROC");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 24, 1, "  # procs. running together%11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 24, 1, "  # procs. running together%11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 16, na, na);


		strcpy(conf_value, "NAIOREQ");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 25, 1, "  max # pending requests   %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 25, 1, "  max # pending requests   %11s  %7d  %7d  %7s  %7s",
			    conf_value, 5, 200, na, na);


		strcpy(conf_value, "NAIOBUF");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 27, 1, "  max # of AIO buffers     %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 27, 1, "  max # of AIO buffers     %11s  %7d  %7d  %7s  %7s",
			    conf_value, 5, 200, na, na);


		strcpy(conf_value, "NAIOHBUF");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 28, 1, "  max # of AIO hash queues %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 28, 1, "  max # of AIO hash queues %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 16, na, na);


		strcpy(conf_value, "NAIOREQPP");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 30, 1, "  # requests pending/proc  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 30, 1, "  # requests pending/proc  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 30, 200, na, na);


		strcpy(conf_value, "NAIOLOCKTBL");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 31, 1, "  max # of AIO locks       %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 31, 1, "  max # of AIO locks       %11s  %7d  %7d  %7s  %7s",
			    conf_value, 5, 20, na, na);
	}
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/06/03 |
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

void vscreen5(void)
{

char	conf_value[64];
int	conf_current = -1;
int	conf_default = -1;
int	conf_min     = -1;
int	conf_max     = -1;
int	extra_bit    = 0;

	conf_value[0] = '\0';

	if (size_flag)
		extra_bit = 1;

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 10, "Parameter");
	mvwaddstr(main_win, 2, 28, "Kernel Name");
	mvwaddstr(main_win, 2, 41, "Minimum");
	mvwaddstr(main_win, 2, 50, "Maximum");
	mvwaddstr(main_win, 2, 59, "Default");
	mvwaddstr(main_win, 2, 68, "Current");

	mvwaddstr(main_win, 4, 3, "Semaphores");

	if (size_flag) {
		mvwaddstr(main_win, 18, 3, "Boot load extension parameters (BTLD)");
		mvwaddstr(main_win, 28, 3, "Multiprocessing parameters");
	}
	wattroff(main_win, A_BOLD);

	strcpy(conf_value, "SEMMAP");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 6, 1, "  size of control map      %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semmap);
	else
		mvwprintw(main_win, 6, 1, "  size of control map      %11s  %7d  %7d  %7d  %7d",
		    conf_value, 10, 100, na, seminfo.semmap);


	strcpy(conf_value, "SEMMNI");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 7, 1, "  num of active semaphores %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semmni);
	else
		mvwprintw(main_win, 7, 1, "  num of active semaphores %11s  %7d  %7d  %7s  %7d",
		    conf_value, 10, 300, na, seminfo.semmni);


	strcpy(conf_value, "SEMMNS");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 8, 1, "  system semaphores        %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semmns);
	else
		mvwprintw(main_win, 8, 1, "  system semaphores        %11s  %7d  %7d  %7s  %7d",
		    conf_value, 60, 60, na, seminfo.semmns);


	strcpy(conf_value, "SEMMNU");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 9, 1, "  undo structures          %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semmnu);
	else
		mvwprintw(main_win, 9, 1, "  undo structures          %11s  %7d  %7d  %7s  %7d",
		    conf_value, 10, 100, na, seminfo.semmnu);


	strcpy(conf_value, "SEMMSL");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 10, 1, "  semaphores / identifier  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semmsl);
	else
		mvwprintw(main_win, 10, 1, "  semaphores / identifier  %11s  %7d  %7d  %7s  %7d",
		    conf_value, 25, 25, na, seminfo.semmsl);


	strcpy(conf_value, "SEMOPM");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 11, 1, "  semaphores per semop()   %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semopm);
	else
		mvwprintw(main_win, 11, 1, "  semaphores per semop()   %11s  %7d  %7d  %7s  %7d",
		    conf_value, 10, 10, na, seminfo.semopm);


	strcpy(conf_value, "SEMUME");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 12, 1, "  undo's per structure     %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semume);
	else
		mvwprintw(main_win, 12, 1, "  undo's per structure     %11s  %7d  %7d  %7s  %7d",
		    conf_value, 10, 10, na, seminfo.semume);


	strcpy(conf_value, "SEMVMX");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 13, 1, "  max value for a semaphore%11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semvmx);
	else
		mvwprintw(main_win, 13, 1, "  max value for a semaphore%11s  %7d  %7d  %7s  %7d",
		    conf_value, 32767, 32767, na, seminfo.semvmx);


	strcpy(conf_value, "SEMAEM");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 14, 1, "  semaphore exit adjustment%11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semaem);
	else
		mvwprintw(main_win, 14, 1, "  semaphore exit adjustment%11s  %7d  %7d  %7s  %7d",
		    conf_value, 16384, 16384, na, seminfo.semaem);


	strcpy(conf_value, "SEMUSZ");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 15, 1, "  semaphore user size      %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, seminfo.semusz);
	else
		mvwprintw(main_win, 15, 1, "  semaphore user size      %11s  %7s  %7s  %7s  %7d",
		    conf_value, na, na, na, seminfo.semusz);


	if (size_flag) {
		strcpy(conf_value, "EXTRA_NDEV");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 20, 1, "  reserved driver slots    %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 20, 1, "  reserved driver slots    %11s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 30, na, na);


		strcpy(conf_value, "EXTRA_NEVENT");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 21, 1, "  extra event slots       %12s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 21, 1, "  extra event slots       %12s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 30, na, na);


		strcpy(conf_value, "EXTRA_NFILSYS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 22, 1, "  fsys mounted at boot   %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 22, 1, "  fsys mounted at boot   %13s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 30, na, na);


		strcpy(conf_value, "MAX_BDEV");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 24, 1, "  max num of block devices %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 24, 1, "  max num of block devices %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 127, na, na);


		strcpy(conf_value, "MAX_CDEV");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 25, 1, "  max num of char devices  %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 25, 1, "  max num of char devices  %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 127, na, na);

		strcpy(conf_value, "MAXACPUS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 30, 1, "  maximum number of CPUs   %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 30, 1, "  maximum number of CPUs   %11s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 29, na, na);

	}
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/06/03 |
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

void vscreen6(void)
{

char	conf_value[64];
int	conf_current = -1;
int	conf_default = -1;
int	conf_min     = -1;
int	conf_max     = -1;
int	extra_bit    = 0;

	conf_value[0] = '\0';

	if (size_flag)
		extra_bit = 1;

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 10, "Parameter");
	mvwaddstr(main_win, 2, 28, "Kernel Name");
	mvwaddstr(main_win, 2, 41, "Minimum");
	mvwaddstr(main_win, 2, 50, "Maximum");
	mvwaddstr(main_win, 2, 59, "Default");
	mvwaddstr(main_win, 2, 68, "Current");

	mvwaddstr(main_win, 4, 3, "Shared Memory");

	if (size_flag) {
		mvwaddstr(main_win, 16, 3, "Security parameters");
		mvwaddstr(main_win, 23, 3, "TCP/IP parameters");
		mvwaddstr(main_win, 29, 3, "NFS parameters");
	}

	wattroff(main_win, A_BOLD);


	strcpy(conf_value, "XSDSEGS");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 6, 1, "  system wide shared segs  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_xsdsegs);
	else
		mvwprintw(main_win, 6, 1, "  system wide shared segs  %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 150, na, v.v_xsdsegs);


	strcpy(conf_value, "XSDSLOTS");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 7, 1, "  shared data slots        %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_xsdslots);
	else
		mvwprintw(main_win, 7, 1, "  shared data slots        %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 10, na, v.v_xsdslots);




	strcpy(conf_value, "SHMMAX");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 9, 1, "  min shm segment byte size%11s  %7dk %7dk %7dk %7dk",
		    conf_value, conf_min/NBPC, conf_max/NBPC, conf_default/NBPC, shminfo.shmmax/NBPC);
	else
		mvwprintw(main_win, 9, 1, "  min shm segment byte size%11s  %7dk %7dk %7s  %7dk",
		    conf_value, 32, 2048, na, shminfo.shmmax/NBPC);


	strcpy(conf_value, "SHMMNI");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 10, 1, "  shared mem. identifiers  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, shminfo.shmmni);
	else
		mvwprintw(main_win, 10, 1, "  shared mem. identifiers  %11s  %7d  %7d  %7s  %7d",
		    conf_value, 100, 500, na, shminfo.shmmni);


	strcpy(conf_value, "SHMMIN");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 11, 1, "  max shm segments/process %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, shminfo.shmmin);
	else
		mvwprintw(main_win, 11, 1, "  max shm segments/process %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 1, na, shminfo.shmmin);


	strcpy(conf_value, "SHMSEG");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 12, 1, "  total sys shm segs in 4K %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, shminfo.shmseg);
	else
		mvwprintw(main_win, 12, 1, "  shared data slots        %11s  %7d  %7d  %7s  %7d",
		    conf_value, 6, 15, na, shminfo.shmseg);


	strcpy(conf_value, "SHMALL");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 13, 1, "  total clicks shm in sys  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, shminfo.shmall);
	else
		mvwprintw(main_win, 13, 1, "  total clicks shm in sys  %11s  %7d  %7d  %7s  %7d",
		    conf_value, 256, 512, na, shminfo.shmall);


	if (size_flag) {
		/*
		Security
	*/
		strcpy(conf_value, "SECLUID");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 18, 1, "  login user id            %11s  %7s  %7s  %7s  %7s",
			    conf_value, "Off", "On", (conf_default MATCHES ? "Off": "On"), (conf_current MATCHES ? "Off":
			    "On"));
		else
			mvwprintw(main_win, 18, 1, "  login user id            %11s  %7s  %7s  %7s  %7s",
			    conf_value, "No", "Yes", na, na);


		strcpy(conf_value, "SECSTOPIO");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 19, 1, "  keep a device open       %11s  %7s  %7s  %7s  %7s",
			    conf_value, "Off", "On", (conf_default MATCHES ? "Off": "On"), (conf_current MATCHES ? "Off":
			    "On"));
		else
			mvwprintw(main_win, 19, 1, "  keep a device open       %11s  %7s  %7s  %7s  %7s",
			    conf_value, "No", "Yes", na, na);


		strcpy(conf_value, "SECCLEARID");
		if (get_tune(conf_value, &conf_default, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 20, 1, "  clearing of SUID/SGID    %11s  %7s  %7s  %7s  %7s",
			    conf_value, "Off", "On", (conf_default MATCHES ? "Off": "On"), (conf_current MATCHES ? "Off":
			    "On"));
		else
			mvwprintw(main_win, 20, 1, "  clearing of SUID/SGID    %11s  %7s  %7s  %7s  %7s",
			    conf_value, "No", "Yes", na, na);

		/*
		TCP/IP
	*/
		strcpy(conf_value, "IPFORWARDING");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 25, 1, "  forward datagrams    %15s  %7s  %7s  %7s  %7s",
			    conf_value, "No", "Yes", (conf_default NOT_MATCH ? "Yes": "No"), (conf_current NOT_MATCH ? "Yes":
			    "No"));
		else
			mvwprintw(main_win, 25, 1, "  forward datagrams    %15s  %7s  %7s  %7s  %7s",
			    conf_value, "No", "Yes", na, na);


		strcpy(conf_value, "IPSENDREDIRECTS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 26, 1, "  send ICMP redirects  %15s  %7s  %7s  %7s  %7s",
			    conf_value, "No", "Yes", (conf_default NOT_MATCH ? "Yes": "No"), (conf_current NOT_MATCH ? "Yes":
			    "No"));
		else
			mvwprintw(main_win, 26, 1, "  send ICMP redirects  %15s  %7s  %7s  %7s  %7s",
			    conf_value, "No", "Yes", na, na);


		/*
		NFS
	*/
		strcpy(conf_value, "NNFSRNODE");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 31, 1, "  mnt point NFS inode ext. %11s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 31, 1, "  mnt point NFS inode ext. %11s  %7d  %7d  %7s  %7s",
			    conf_value, 100, 600, na, na);

	}

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/06/03 |
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

void vscreen7(void)
{

extern int knmuxlink;

char	conf_value[64];
int	conf_current = -1;
int	conf_default = -1;
int	conf_min     = -1;
int	conf_max     = -1;
int	extra_bit    = 0;

	conf_value[0] = '\0';

	if (size_flag)
		extra_bit = 1;

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 10, "Parameter");
	mvwaddstr(main_win, 2, 28, "Kernel Name");
	mvwaddstr(main_win, 2, 41, "Minimum");
	mvwaddstr(main_win, 2, 50, "Maximum");
	mvwaddstr(main_win, 2, 59, "Default");
	mvwaddstr(main_win, 2, 68, "Current");

	mvwaddstr(main_win, 4, 3, "Streams Data");

	if (size_flag)
		mvwaddstr(main_win, 21, 3, "LAN Manager & netBEUI parameters");
	wattroff(main_win, A_BOLD);

	kmem_read(&knmuxlink, namelist[NM_KNMUXLINK].n_value, sizeof(knmuxlink));

	/* Test to see if we have streams or not. 
	if (!namelist[NM_KNMUXLINK].n_sclass) {
		knmuxlink = 0;
		knstrpush = 0;
		knstrevent = 0;
		kmaxsepgcnt = 0;
		kstrmsgsz = 0;
		kstrctlsz = 0;
		kstrlofrac = 0;
		kstrmedfrac = 0;
	} else {
		kmem_read(&knstrpush,   namelist[NM_KNSTRPUSH].n_value, 
		    sizeof(knstrpush));
		kmem_read(&kstrmsgsz,   namelist[NM_KSTRMSGSZ].n_value, 
		    sizeof(kstrmsgsz));
		kmem_read(&kstrctlsz,   namelist[NM_KSTRCTLSZ].n_value, 
		    sizeof(kstrctlsz));
		kmem_read(&kstrlofrac,  namelist[NM_KSTRLOFRAC].n_value,
		    sizeof(kstrlofrac));
		kmem_read(&knstrevent,  namelist[NM_KNSTREVENT].n_value,
		    sizeof(knstrevent));
		kmem_read(&kmaxsepgcnt, namelist[NM_KMAXSEPGCNT].n_value,
		    sizeof(kmaxsepgcnt));
		kmem_read(&kstrmedfrac, namelist[NM_KSTRMEDFRAC].n_value,
		    sizeof(kstrmedfrac));
	} */


	if (!namelist[NM_KNMUXLINK].n_sclass)
		mvwaddstr(main_win, 4, 60, "Not Installed");

	wattroff(main_win, A_BOLD);

	strcpy(conf_value, "NQUEUE");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 5 + extra_bit, 1, "  total stream queues      %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_nqueue);
	else
		mvwprintw(main_win, 5 + extra_bit, 1, "  total stream queues      %11s  %7d  %7d  %7s  %7d",
		    conf_value, 4, 1024, na, v.v_nqueue);


	strcpy(conf_value, "NSTREAM");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 6 + extra_bit, 1, "  total stream headers     %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, v.v_nstream);
	else
		mvwprintw(main_win, 6 + extra_bit, 1, "  total stream headers     %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 512, na, v.v_nstream);


	strcpy(conf_value, "NMUXLINK");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 7 + extra_bit, 1, "  multiplexer links        %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, knmuxlink);
	else
		mvwprintw(main_win, 7 + extra_bit, 1, "  multiplexer links        %11s  %7d  %7d  %7s  %7d",
		    conf_value, 1, 87, na, knmuxlink);


	strcpy(conf_value, "NSTRPUSH");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 8 + extra_bit, 1, "  modules on a stream      %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 8 + extra_bit, 1, "  modules on a stream      %11s  %7d  %7d  %7s  %7s",
		    conf_value, 9, 9, na, na);


	strcpy(conf_value, "NSTREVENT");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 9 + extra_bit, 1, "  stream event structures  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 9 + extra_bit, 1, "  stream event structures  %11s  %7d  %7d  %7s  %7s",
		    conf_value, 256, 512, na, na);


	strcpy(conf_value, "MAXSEPGCNT");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 10 + extra_bit, 1, "  max stream page count    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 10 + extra_bit, 1, "  max stream page count    %11s  %7d  %7d  %7s  %7s",
		    conf_value, 0, 32, na, na);


	strcpy(conf_value, "STRMSGSZ");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 11 + extra_bit, 1, "  max size of data message %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 11 + extra_bit, 1, "  max size of data message %11s  %7d  %7d  %7s  %7s",
		    conf_value, 4096, 4096, na, na);


	strcpy(conf_value, "STRCTLSZ");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 12 + extra_bit, 1, "  max size of ctl message  %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 12 + extra_bit, 1, "  max size of ctl message  %11s  %7d  %7d  %7s  %7s",
		    conf_value, 1024, 1024, na, na);


	strcpy(conf_value, "STRLOFRAC");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 13 + extra_bit, 1, "  low buffers per class    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 13 + extra_bit, 1, "  low buffers per class    %11s  %7d  %7d  %7s  %7s",
		    conf_value, 0, 95, na, na);


	strcpy(conf_value, "STRMEDFRAC");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 14 + extra_bit, 1, "  med buffers per class    %11s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 14 + extra_bit, 1, "  med buffers per class    %11s  %7d  %7d  %7s  %7s",
		    conf_value, 80, 100, na, na);


	if (size_flag) {

		strcpy(conf_value, "NUMTIM");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 16, 1, "  stream mod pushed by TLI%12s  %7d  %7d  %7d  %7d",
		    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 16, 1, "  stream mod pushed by TLI%12s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 512, na, na);

		strcpy(conf_value, "NUMTRW");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 17, 1, "  TLI read/write struct. %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 17, 1, "  TLI read/write struct. %13s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 512, na, na);

		strcpy(conf_value, "NUMSP");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 18, 1, "  Num of /dev/sp devices %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 18, 1, "  Num of /dev/sp devices %13s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 256, na, na);

		strcpy(conf_value, "NLOG");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 19, 1, "  Num of log devices     %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 19, 1, "  Num of log devices     %13s  %7d  %7d  %7s  %7s",
			    conf_value, 3, 3, na, na);

		/*
		Netbeui & lan man
	*/
		strcpy(conf_value, "NFSBUFS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 23, 1, "  max num of network bufs%13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 23, 1, "  max num of network bufs%13s  %7d  %7d  %7s  %7s",
			    conf_value, 4, 1000, na, na);

		strcpy(conf_value, "NLBC");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 24, 1, "  max # streams for co lo0%12s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 24, 1, "  max # streams for co lo0%12s  %7d  %7d  %7s  %7s",
			    conf_value, 8, 128, na, na);

		strcpy(conf_value, "NLBU");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 25, 1, "  max # streams for lo0  %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 25, 1, "  max # streams for lo0  %13s  %7d  %7d  %7s  %7s",
			    conf_value, 8, 128, na, na);

		strcpy(conf_value, "NALIAS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 26, 1, "  # of alias table entries%12s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 26, 1, "  # of alias table entries%12s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 1000, na, na);

		strcpy(conf_value, "NASEVENT");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 27, 1, "  # of async event calls %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 27, 1, "  # of async event calls %13s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 1000, na, na);

		strcpy(conf_value, "NBIDS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 28, 1, "  number of BIND entries %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 28, 1, "  number of BIND entries %13s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 1000, na, na);

		strcpy(conf_value, "NCALLRETRY");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 29, 1, "  # of LAN connect retrys%13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 29, 1, "  # of LAN connect retrys%13s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 10000, na, na);

		strcpy(conf_value, "NNCB_NAMES");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 30, 1, "  length of NCB names    %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 30, 1, "  length of NCB names    %13s  %7d  %7d  %7s  %7s",
			    conf_value, 0, 256, na, na);


		strcpy(conf_value, "KEY_RETRIES");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 32, 1, "  # of netBEUI TX retries%13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 32, 1, "  # of netBEUI TX retries%13s  %7d  %7d  %7s  %7s",
			    conf_value, 1, 50, na, na);

		strcpy(conf_value, "KEY_NTES");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 33, 1, "  num of netBIOS names   %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 33, 1, "  num of netBIOS names   %13s  %7d  %7d  %7s  %7s",
			    conf_value, 2, 254, na, na);

		strcpy(conf_value, "KEY_BUFS");
		if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
			mvwprintw(main_win, 34, 1, "  # of bufs for input    %13s  %7d  %7d  %7d  %7d",
			    conf_value, conf_min, conf_max, conf_default, conf_current);
		else
			mvwprintw(main_win, 34, 1, "  # of bufs for input    %13s  %7d  %7d  %7s  %7s",
			    conf_value, 2, 1000, na, na);

	}
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

void watch_vars(void)
{

int cmd, nap_time;

	drawdisp(31);

	nap_time = 1000;
	wi_vars(2);
	wattron(main_win, A_REVERSE | A_BLINK);
	mvwaddstr(main_win, 0, 64, "Medium");
	wattroff(main_win, A_REVERSE | A_BLINK);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		wi_vars(2);
		an_option();
		nap(nap_time);

		while (rdchk(0)) {
			switch(cmd = wgetch(bottom_win)) {
			case 'T' :
			case 't' :
				Get_The_Display(T_NORMAL);
				wrefresh(bottom_win);
				nap(750);
				break;

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
				drawdisp(31);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 58, "Slow");
				wattroff(main_win, A_REVERSE | A_BLINK);
				nap_time = 4000;
				break;

			case 'M' :
			case 'm' :
				drawdisp(31);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 64, "Medium");
				wattroff(main_win, A_REVERSE | A_BLINK);
				nap_time = 2000;
				break;

			case 'F' :
			case 'f' :
				drawdisp(31);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 72, "Fast");
				wattroff(main_win, A_REVERSE | A_BLINK);
				nap_time = 1000;
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

void Get_The_Display(int mode)
{

extern int The_Current_Display;

	switch(The_Current_Display) {
		case BY_TIME:
			The_Current_Display = BY_PERCNT;
			if (mode == T_NORMAL) {
				mvwaddstr(bottom_win, 1, 2, " Display type is now by percent.");
				wclrtoeol(bottom_win);
				Draw_A_Box(bottom_win, DEFAULT_BOX);
			}
			break;

		case BY_PERCNT:
			The_Current_Display = BY_ERRORS;
			if (mode == T_NORMAL) {
				mvwaddstr(bottom_win, 1, 2, " Display type is now by overflows.");
				wclrtoeol(bottom_win);
				Draw_A_Box(bottom_win, DEFAULT_BOX);
			}
			break;

		case BY_ERRORS:
			The_Current_Display = BY_TIME;
			if (mode == T_NORMAL) {
				mvwaddstr(bottom_win, 1, 2, " Display type is now by time.");
				wclrtoeol(bottom_win);
				Draw_A_Box(bottom_win, DEFAULT_BOX);
			}
			break;
	}
	if (mode == T_NORMAL)
		wnoutrefresh(bottom_win);
	return;
 }

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */


