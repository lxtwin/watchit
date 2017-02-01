/*
 * +--------------------------------------------------------------------+
 * | Function: procs.c                                   Date: 92/04/04 |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the proc info.                                         |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *(1) 2.3: All the graphic shit and the detial screen can now be
 *		watched.
 *							Sun Aug 22 13:28:04 EDT 1993 - PKR.
 * Bugs:
 *          None yet.
 */

#include "wi.h"

#include <pwd.h>
#include <grp.h>
#include <sys/dir.h>
#include <sys/var.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/user.h>
#include <sys/immu.h>
#include <sys/region.h>
#include <sys/proc.h>

#define START_OF_NICE 4

extern int size_flag;
int    pg_ctr;
int max_procs_to_display;
extern int nprocs;

char dev_name[32];

void wi_procs(void);
void getproc(int i, int pg_ctr);
void proc_header(void);
char *get_cpu_time(time_t ticks);
void detial_proc_menu(int i);
void detial_proc(int i, WINDOW *tmp_window);
void detial_proc_head(WINDOW *tmp_window);
void watch_procs(void);
void proc_mon_header(void);

void draw_process_screen(int slot, int max_val);
void watch_processes(int *main_slot, int max_val);

void nice_proc(register int slot);
void get_new_nice_value(int *new_nice);
void num_of_open_procs(void);

extern proc_t *current_procs;
extern proc_t **ptr_procs;

int procs_per_pstat[SXBRK + 1];
int procs_in_core;
int procs_alive;

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
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


void wi_procs(void)
{

int	i;
int	get_value = 0;
register max_val;

	procs = (proc_t *) malloc(v.v_proc * sizeof(proc_t));
	kmem_read(procs, namelist[NM_PROC].n_value, sizeof(proc_t) * v.v_proc);

	proc_header();
	max_val = v.v_proc;
	pg_ctr = 4;
	for (i = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			an_option();
			num_of_open_procs();
			doupdate();

			switch(wgetch(bottom_win)) {
			case 'w' :
				if (size_flag)
					if (i <= 31)
						i = 0;
					else
						i -= 31;
				else
					if (i <= 13)
						i = 0;
					else
						i -= 13;
				drawdisp(27);
				watch_processes(&i, max_val);
				drawdisp(9);
				break;

			case 'b' :
			case 'B' :
			case KEY_PPAGE:
				if (size_flag) {
					if (i <= 61) {
						beep();
						i = 0;
					} else
						i -= 62;
				} else {
					if (i <= 25) {
						beep();
						i = 0;
					} else
						i -= 26;
				}
				break;

			case 'f' :
			case 'F' :
			case KEY_NPAGE:
				if (size_flag) {
					if (i >= (max_val - 30)) {
						i = (max_val - 31);
						beep();
					}
				} else {
					if (i >= (max_val - 12)) {
						i = (max_val - 13);
						beep();
					}
				}
				break;

			case KEY_UP:
			case '-' :
				if (size_flag) {
					if (i <= 32) {
						beep();
						i = 0;
					} else
						i -= 32;
				} else {
					if (i <= 13) {
						beep();
						i = 0;
					} else
						i -= 14;
				}
				break;

			case KEY_DOWN:
			case '+' :
				if (i == (max_val)) {
					if (size_flag)
						i = (max_val - 31);
					else
						i = (max_val - 13);
					beep();
				} else {
					if (size_flag) {
						if (i <= 30)
							i = 0;
						else
							i -= 30;
					} else {
						if (i <= 12)
							i = 0;
						else
							i -= 12;
					}
				}
				break;

			case 'd' :
			case 'D' :
				drawdisp(9);
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 46, "Detail");
				wattroff(main_win, A_REVERSE);
				wrefresh(main_win);
				get_value = 0;
				get_detial(&get_value, 0, (max_val - 1));
				(void) detial_proc_menu(get_value);
				drawdisp(9);
				wrefresh(main_win);
				if (size_flag)
					if (i <= 31)
						i = 0;
					else
						i -= 31;
				else
					if (i <= 13)
						i = 0;
					else
						i -= 13;
				break;

#ifdef CHANGE_NICE
			case 'n' :
			case 'N' :
				if (nice_enabled) {
					get_value = 0;
					if (!(get_item(&get_value, START_OF_NICE, (max_val - 1)))) {
						(void) nice_proc(get_value);
						kmem_read(procs, namelist[NM_PROC].n_value,
				    			sizeof(proc_t) * v.v_proc);
					}
				} else
					beep();
				if (size_flag)
					if (i <= 31)
						i = 0;
					else
						i -= 31;
				else
					if (i <= 13)
						i = 0;
					else
						i -= 13;
				break;
#endif
			case 's' :
			case 'S' :
				drawdisp(9);
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 70, "Search");
				wattroff(main_win, A_REVERSE);
				wrefresh(main_win);
				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				i = get_value;
				drawdisp(9);
				wrefresh(main_win);
				break;

			case 'q' :
			case 'Q' :
				free((char *) procs);
				return;

			case 'W' :
				drawdisp(15);
				if (size_flag)
					max_procs_to_display = 31;
				else
					max_procs_to_display = 13;
				watch_procs();
				drawdisp(9);
				break;

			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				if (size_flag)
					if (i <= 31)
						i = 0;
					else
						i -= 31;
				else
					if (i <= 13)
						i = 0;
					else
						i -= 13;
				break;

			case 'P' :
			case 'p' :
				screen_dump();
				if (size_flag)
					if (i <= 31)
						i = 0;
					else
						i -= 31;
				else
					if (i <= 13)
						i = 0;
					else
						i -= 13;
				break;

			case 'G' :
			case 'g' :
				Show_Graphics();

			case 'u' :
			case 'U' :
				kmem_read(procs, namelist[NM_PROC].n_value,
				    sizeof(proc_t) * v.v_proc);

			default :

				if (size_flag)
					if (i <= 31)
						i = 0;
					else
						i -= 31;
				else
					if (i <= 13)
						i = 0;
					else
						i -= 13;
				break;

			}
			proc_header();
			pg_ctr = 4;
		} else {
			getproc(i, pg_ctr);
			if (i == (max_val)) {
				pg_ctr++;
				i = 0;
			} else
				i++;
			pg_ctr++;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: getproc()                                 Date: 92/04/04 |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the proc info.                                     |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void getproc(int i, int pg_ctr)
{

proc_t   *pp;

	mvwaddstr(main_win,pg_ctr, 4, "                                                                         ");
	if (procs[i].p_stat MATCHES) {
		mvwprintw(main_win,pg_ctr, 2, "%4d - Free", i);
		return;
	}

	pp = &procs[i];

	mvwprintw(main_win, pg_ctr, 3, "%3d", i);

	if ((pp->p_stat == SRUN) || (pp->p_stat == SONPROC))
		wattron(main_win, A_BOLD);


	mvwprintw(main_win, pg_ctr, 7, "%c %3d %3d %5d %5d %3d %0.2d",
	    " sRzdipx"[pp->p_stat], pp->p_cpu & 0377, pp->p_uid,
	    pp->p_pid, pp->p_ppid, pp->p_pri & 0377, pp->p_nice);

	if (pp->p_stat == SZOMB) {
		mvwaddstr(main_win, pg_ctr, 60, "<zombie>");
	} else if (pp->p_stat != 0) {
		if (finduser(pp, i, &user)) {
			mvwprintw(main_win, pg_ctr, 36, "%s",
			    get_cpu_time(user.u_utime));
			mvwprintw(main_win, pg_ctr, 43, "%s",
			    get_cpu_time(user.u_stime));
			mvwprintw(main_win, pg_ctr, 51 - 1, "%5d %3s",
			    (ctob((u_long)user.u_tsize + user.u_dsize + user.u_ssize))/ 1024, print_tty(pp->p_pid, 0));
			mvwprintw(main_win, pg_ctr, 60, "%.17s", user.u_psargs);
		} else {
			switch(pp->p_stat) {
			case SZOMB:
				mvwaddstr(main_win, pg_ctr, 60, "<zombie>");
				break;
			case SXBRK:
				mvwaddstr(main_win, pg_ctr, 60, "<xbreak>");
				break;
			case SIDL:
				mvwaddstr(main_win, pg_ctr, 60, "<in creation>");
				break;
			default :
				mvwaddstr(main_win, pg_ctr, 60, "<swapping>");
			}
		}
	}
	if ((pp->p_stat == SRUN) || (pp->p_stat == SONPROC))
		wattroff(main_win, A_BOLD);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: getproc(int i, int pg_ctr);               Date: %D% |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displayes the proc info.                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void proc_header(void)
{

	mvwaddstr(main_win, 2, 2, "                                                                        ");
	mvwaddstr(main_win, 3, 2, "                                                                        ");
	mvwaddstr(main_win, 16, 4, "                                                                       ");
	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 2, "Slot");
	mvwaddstr(main_win, 2, 7, "S");
	mvwaddstr(main_win, 2, 9, "CPU");
	mvwaddstr(main_win, 2, 13, "UID");
	mvwaddstr(main_win, 2, 19, "PID");
	mvwaddstr(main_win, 2, 24, "PPID");
	mvwaddstr(main_win, 2, 29, "PRI");
	mvwaddstr(main_win, 2, 33, "NI");
	mvwaddstr(main_win, 2, 36 + 2, "UCPU");
	mvwaddstr(main_win, 2, 43 + 2, "SCPU");
	mvwaddstr(main_win, 2, 51, "SIZE");
	mvwaddstr(main_win, 2, 56, "TTY");
	mvwaddstr(main_win, 2, 60, "CMD");
	wattroff(main_win, A_BOLD);
	wnoutrefresh(main_win);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: get_cpu_time(time_t ticks);               Date: %D% |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the CPU time.                                          |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

char *get_cpu_time(time_t ticks)
{

static char time[10];
time_t	mm, ss;

	if (ticks < 0)
		return("------");

	ticks /= HZ;
	mm = ticks / 60L;
	ticks -= mm * 60L;
	ss = ticks;

	if (mm > 9999)
		(void)strcpy(time, ">9999m");
	else if (mm > 999)
		(void)sprintf(time, "%5ldm",mm);
	else
		(void)sprintf(time, "%3lu:%0.2lu",mm,ss);

	return(time);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: detial_proc_head();                       Date: %D% |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displayes the proc info detial header.                      |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */


void detial_proc_head(WINDOW *tmp_window)
{

	wattron(tmp_window, A_REVERSE);
	mvwaddstr(tmp_window, 0, 2, " Detail Processes ");
	wattroff(tmp_window, A_REVERSE);

	mvwaddstr(tmp_window, 0, 44, "Slow");
	mvwaddstr(tmp_window, 0, 51, "Medium");
	mvwaddstr(tmp_window, 0, 59, "Fast");

	wattron(tmp_window, A_BOLD);

	mvwaddstr(tmp_window, 2, 2, "Slot");
	mvwaddstr(tmp_window, 2, 8, "State");
	mvwaddstr(tmp_window, 2, 15, "Cpu");
	mvwaddstr(tmp_window, 2, 20, "Priority");
	mvwaddstr(tmp_window, 2, 30, "Nice");
	mvwaddstr(tmp_window, 2, 38, "Pid");
	mvwaddstr(tmp_window, 2, 44, "Ppid");
	mvwaddstr(tmp_window, 2, 51, "Group leader");

	mvwaddstr(tmp_window, 5, 2, "ID's");

	mvwaddstr(tmp_window, 7, 5, "Control tty");
	mvwaddstr(tmp_window, 7, 21, "Wchan");
	mvwaddstr(tmp_window, 7, 29, "Sec to Alarm");
	mvwaddstr(tmp_window, 7, 44, "Sec to Scheduel");

	mvwaddstr(tmp_window, 10, 2, "Start Time");
	mvwaddstr(tmp_window, 10, 32, "Text, Data and Stack Size");

	mvwaddstr(tmp_window, 12, 2, "Flags");
	mvwaddstr(tmp_window, 14, 2, "Command");
	wattroff(tmp_window, A_BOLD);
	mvwaddstr(tmp_window, 10, 12, ":");
	mvwaddstr(tmp_window, 5, 6, ":");
	mvwaddstr(tmp_window, 12, 7, ":");
	mvwaddstr(tmp_window, 14, 9, ":");
	wnoutrefresh(tmp_window);

}

/*
 * +--------------------------------------------------------------------+
 * | Function: detial_proc_menu(int proc)                Date: 92/04/04 |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the proc info in detial                            |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void detial_proc_menu(int i)
{

WINDOW *tmp_window;
int cmd = 0, naptime = 0;

	if (size_flag)
		tmp_window = newwin(17, 65, 20, 10);
	else
		tmp_window = newwin(17, 65, 5, 10);

	Draw_A_Box(tmp_window, DEFAULT_BOX);
	detial_proc_head(tmp_window);
	detial_proc(i, tmp_window);

	wattron(tmp_window, A_REVERSE | A_BLINK);
	mvwaddstr(tmp_window, 0, 51, "Medium");
	wattroff(tmp_window, A_REVERSE | A_BLINK);
	naptime = 1000;
	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		wnoutrefresh(tmp_window);
		an_option();
		num_of_open_procs();
		doupdate();

		while (rdchk(0)) {
			switch(cmd = wgetch(bottom_win)) {
			case KEY_PPAGE:
			case KEY_UP:
			case '-' :
				if (i <= 0) {
					i = 0;
					beep();
				} else
					i--;
				break;
	
			case KEY_NPAGE:
			case KEY_DOWN:
			case '+' :
				if (i >= (v.v_proc - 1)) {
					i = (v.v_proc - 1);
					beep();
				} else
					i++;
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
				if (Graphics_Mode == SCREEN_D_MODE) {
					Graphics_Mode = GRAPHICS_MODE;
					mvwaddstr(bottom_win, 1, 2, " Console Capture graphics mode disabled.");
		 		} else {
					Graphics_Mode = SCREEN_D_MODE;
					mvwaddstr(bottom_win, 1, 2, " Console Capture graphics mode enabled.");
				}
				wclrtoeol(bottom_win);
				Draw_A_Box(bottom_win, DEFAULT_BOX);
				wrefresh(bottom_win);
				sleep(2);
				break;

			case 'S' :
			case 's' :
				detial_proc_head(tmp_window);
				wattron(tmp_window, A_REVERSE | A_BLINK);
				mvwaddstr(tmp_window, 0, 44, "Slow");
				wattroff(tmp_window, A_REVERSE | A_BLINK);
				naptime = 2000;
				break;

			case 'M' :
			case 'm' :
				detial_proc_head(tmp_window);
				wattron(tmp_window, A_REVERSE | A_BLINK);
				mvwaddstr(tmp_window, 0, 51, "Medium");
				wattroff(tmp_window, A_REVERSE | A_BLINK);
				naptime = 1000;
				break;

			case 'F' :
			case 'f' :
				detial_proc_head(tmp_window);
				wattron(tmp_window, A_REVERSE | A_BLINK);
				mvwaddstr(tmp_window, 0, 59, "Fast");
				wattroff(tmp_window, A_REVERSE | A_BLINK);
				naptime = 500;
				break;

			case 'Q' :
			case 'q' :
				delwin(tmp_window);
				touchwin(main_win);
				wrefresh(main_win);
				return;

			default :
				break;
			}
		}
		detial_proc(i, tmp_window);
		nap(naptime);
		kmem_read(procs, namelist[NM_PROC].n_value,
			sizeof(proc_t) * v.v_proc);
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: detial_proc(int proc)                     Date: 92/04/04 |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the proc info in detial                            |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void detial_proc(int i, WINDOW *tmp_window)
{

proc_t   *pp;
struct passwd *pw, *getpwuid();
struct group  *gr, *getgrgid();

char na[] = "N/A";

		mvwaddstr(tmp_window, 8, 2, "                                                          ");
		mvwaddstr(tmp_window, 11, 2, "                                                          ");
	if (procs[i].p_stat MATCHES) {

		mvwprintw(tmp_window, 3, 2, "%3d    %s   %s    %s      %s    %s    %s       %s", i, na, na, na, na, na, na, na);


		mvwprintw(tmp_window, 5, 8, "uid = N/A, suid = N/A, sgid = N/A                             ");
		mvwaddstr(tmp_window, 8, 2, "       N/A          N/A        N/A             N/A        ");
		mvwprintw(tmp_window, 11, 2, " N/A                            T: N/A, D: N/A, S: N/A     ");
		mvwaddstr(tmp_window, 13, 2, " free()                                                   ");

		mvwaddstr(tmp_window, 15, 2, " Slot free - ready for use.      ");
		wnoutrefresh(tmp_window);
		return;
	}

	pp = &procs[i];

	mvwprintw(tmp_window, 3, 2, "%3d     %c   %3d     %3d     %3d   %5.1d  %5.1d     %5.1d",
	    i, " sRzdipx"[pp->p_stat], pp->p_cpu & 0377, pp->p_pri & 0377,
	    pp->p_nice, pp->p_pid, pp->p_ppid, pp->p_pgrp);

	mvwprintw(tmp_window, 5, 8, "uid = %d(%s), suid = %d(%s), sgid = %d(%s)",
	    pp->p_uid,
	    (((pw = getpwuid(pp->p_uid)) == NULL) ? "unknown": pw->pw_name),
	    pp->p_suid,
	    (((pw = getpwuid(pp->p_suid)) == NULL) ? "unknown": pw->pw_name),
	    pp->p_sgid,
	    (((gr = getgrgid(pp->p_sgid)) == NULL) ? "unknown": gr->gr_name));

	mvwprintw(tmp_window, 8, 32, "% 4.1d            % 4.1d", pp->p_time, pp->p_clktim);
	mvwaddstr(tmp_window, 8, 4, "                ");

	mvwprintw(tmp_window, 11, 3, "%.24s", ctime(&user.u_start));

	if (pp->p_wchan)
		mvwprintw(tmp_window, 8, 18, "%8x", pp->p_wchan);
	else if (pp->p_stat == SZOMB) {
		mvwaddstr(tmp_window, 3, 10, "z");
		mvwaddstr(tmp_window, 8, 18, "  zombie");
	} else
		mvwaddstr(tmp_window, 8, 18, " running");

	mvwaddstr(tmp_window, 13, 2, "                                                          ");
	wmove(tmp_window, 13, 2);

	if (pp->p_stat == SSLEEP)
		waddstr(tmp_window, " awaiting an event");
	if (pp->p_stat == SRUN)
		waddstr(tmp_window, " running");
	if (pp->p_stat == SZOMB)
		waddstr(tmp_window, " zombie");
	if (pp->p_stat == SSTOP)
		waddstr(tmp_window, " stop");
	if (pp->p_stat == SIDL)
		waddstr(tmp_window, " idle");
	if (pp->p_stat == SXBRK)
		waddstr(tmp_window, " xswapped");

	if (pp->p_flag & SSYS)
		waddstr(tmp_window, " sys_res");
	if (pp->p_flag & STRC)
		waddstr(tmp_window, " traced");
	if (pp->p_flag & SWTED)
		waddstr(tmp_window, " waited");
	if (pp->p_flag & SNWAKE)
		waddstr(tmp_window, " nwake");
	if (pp->p_flag & SLOAD)
		waddstr(tmp_window, " incore");
	if (pp->p_flag & SLOCK)
		waddstr(tmp_window, " locked");
	if (pp->p_flag & SPOLL)
		waddstr(tmp_window, " str poll");
	if (pp->p_flag & SSLEEP)
		waddstr(tmp_window, " sleep()");
	if (pp->p_flag & SEXIT)
		waddstr(tmp_window, " exit()");

	if (pp->p_stat == SZOMB) {
		mvwaddstr(tmp_window, 15, 2, "                                 ");
		mvwaddstr(tmp_window, 15, 2, " zombie");
		mvwaddstr(tmp_window, 8, 6, "/dev/??");
	} else if (pp->p_stat != 0) {
		if (finduser(pp, i, &user)) {
			mvwprintw(tmp_window, 11, 34, "T: %d, D: %d, S: %d  ",
			    (ctob(user.u_tsize))/ BSIZE,
			    (ctob(user.u_dsize))/ BSIZE,
			    (ctob(user.u_ssize))/ BSIZE);
			mvwaddstr(tmp_window, 15, 2, "                                                            ");
			mvwprintw(tmp_window, 15, 2, " %.59s", user.u_psargs);
			if (pp->p_ppid  > 1) {
				print_dev(0, major(user.u_ttyd), minor(user.u_ttyd), dev_name, 0);
				mvwprintw(tmp_window, 8, 6, "/dev/%s", dev_name);
			} else
				mvwprintw(tmp_window, 8, 6, "/dev/tty%s", print_tty(pp->p_pid, 0));
		} else {
			waddstr(tmp_window, " swapped");
			mvwaddstr(tmp_window, 8, 6, "/dev/??");
		}
	}

	wnoutrefresh(tmp_window);
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
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

void watch_procs(void)
{

int cmd, counter, naptime;
register int proc_ctr;

	proc_mon_header();
	for (counter = 4; counter <= page_len; counter++)
		mvwaddstr(main_win, counter, 1, "                                                                           ");

	wattron(main_win, A_REVERSE | A_BLINK);
	mvwaddstr(main_win, 0, 64, "Medium");
	wattroff(main_win, A_REVERSE | A_BLINK);
	naptime = 1000;
	if (size_flag)
		max_procs_to_display = 31;
	else
		max_procs_to_display = 12;

	get_sorted_procs();
	max_procs_to_display = min(nprocs,max_procs_to_display);
	for (proc_ctr = 0; proc_ctr < max_procs_to_display; proc_ctr++)
		watch_the_procs(proc_ctr);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		for (counter = 4; counter <= page_len; counter++)
			mvwaddstr(main_win, counter, 1, "                                                                            ");
		if (size_flag)
			max_procs_to_display = 31;
		else
			max_procs_to_display = 12;

		get_sorted_procs();
		max_procs_to_display = min(nprocs,max_procs_to_display);
		for (proc_ctr = 0; proc_ctr < max_procs_to_display; proc_ctr++)
			watch_the_procs(proc_ctr);
		wnoutrefresh(main_win);
		an_option();
		num_of_open_procs();
		doupdate();
		nap(naptime);

		while (rdchk(0)) {
			switch(cmd = wgetch(bottom_win)) {
			case 'S' :
			case 's' :
				drawdisp(15);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 57, "Slow");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 2000;
				break;

			case 'M' :
			case 'm' :
				drawdisp(15);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 64, "Medium");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 1000;
				break;

			case 'F' :
			case 'f' :
				drawdisp(15);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 72, "Fast");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 500;
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

void proc_mon_header(void)
{

	mvwaddstr(main_win, 2, 2, "                                                                        ");
	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 3, "S");
	mvwaddstr(main_win, 2, 5, "CPU");
	mvwaddstr(main_win, 2, 11, "PID");
	mvwaddstr(main_win, 2, 16, "PPID");
	mvwaddstr(main_win, 2, 21, "PRI");
	mvwaddstr(main_win, 2, 25, "NI");
	mvwaddstr(main_win, 2, 30, "UCPU");
	mvwaddstr(main_win, 2, 37, "SCPU");
	mvwaddstr(main_win, 2, 43, "SIZE");
	mvwaddstr(main_win, 2, 48, "TTY");
	mvwaddstr(main_win, 2, 52, "COMMAND");
	wattroff(main_win, A_BOLD);
	wnoutrefresh(main_win);
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
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

void watch_processes(int *main_slot, int max_val)
{

int get_value = 0;
int cmd = 0, naptime = 0;
int slot = 0;

	proc_header();

	slot = *main_slot;

	wattron(main_win, A_REVERSE | A_BLINK);
	mvwaddstr(main_win, 0, 64, "Medium");
	wattroff(main_win, A_REVERSE | A_BLINK);
	naptime = 1000;

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		draw_process_screen(slot, max_val);
		wnoutrefresh(main_win);
		an_option();
		num_of_open_procs();
		doupdate();
		nap(naptime);

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

			case 'l' :
			case 'L' :
				drawdisp(27);
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 50, "Lookup");
				wattroff(main_win, A_REVERSE);
				wrefresh(main_win);
				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				slot = get_value;
				mvwaddstr(main_win, 0, 50, "Lookup");
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 50, "L");
				wattroff(main_win, A_REVERSE);
				wattron(main_win, A_REVERSE | A_BLINK);
				if (naptime == 2000)
					mvwaddstr(main_win, 0, 58, "Slow");
				else if (naptime == 1000)
					mvwaddstr(main_win, 0, 64, "Medium");
				else
					mvwaddstr(main_win, 0, 72, "Fast");
				wattroff(main_win, A_REVERSE | A_BLINK);
				break;

#ifdef CHANGE_NICE
			case 'n' :
			case 'N' :
				if (nice_enabled) {
					get_value = 0;
					if (!(get_item(&get_value, START_OF_NICE, (max_val - 1)))) {
						(void) nice_proc(get_value);
						kmem_read(procs, namelist[NM_PROC].n_value,
				    			sizeof(proc_t) * v.v_proc);
					}
				} else
					beep();
				break;
#endif
			case 'S' :
			case 's' :
				drawdisp(27);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 58, "Slow");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 2000;
				break;

			case 'M' :
			case 'm' :
				drawdisp(27);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 64, "Medium");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 1000;
				break;

			case 'F' :
			case 'f' :
				drawdisp(27);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 72, "Fast");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 500;
				break;

			case KEY_PPAGE:
				if (size_flag) {
					if (slot <= 31) {
						beep();
						slot = 0;
					} else
						slot -= 31;
				} else {
					if (slot <= 25) {
						beep();
						slot = 0;
					} else
						slot -= 13;
				}
				break;

			case KEY_NPAGE:
				if (size_flag) {
					if (slot >= (max_val - 61)) {
						slot = (max_val - 31);
						beep();
					} else
						slot += 31;
				} else {
					if (slot >= (max_val - 25)) {
						slot = (max_val - 13);
						beep();
					} else
						slot += 13;
				}
				break;

			case KEY_UP:
			case '-' :
				if (slot MATCHES) {
					beep();
					slot = 0;
				} else
					slot--;
				break;

			case KEY_DOWN:
			case '+' :
				if (size_flag) {
					if (slot >= (max_val - 31)) {
						slot = (max_val - 31);
						beep();
					} else
						slot++;
				} else {
					if (slot >= (max_val - 13)) {
						slot = (max_val - 13);
						beep();
					} else
						slot++;
				}
				break;

			case 'Q' :
			case 'q' :
				*main_slot = slot;
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

void draw_process_screen(int slot, int max_val)
{

register counter;
int pg_ctr;

	kmem_read(procs, namelist[NM_PROC].n_value, sizeof(proc_t) * v.v_proc);

	pg_ctr = 4;
	for (counter = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			return;
		} else {
			getproc(slot, pg_ctr);
			if (slot == (max_val)) {
				pg_ctr++;
				mvwaddstr(main_win, pg_ctr, 2, "                                                                      ");
				slot = 0;
			} else
				slot++;
			pg_ctr++;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: nice_proc(int proc)                       Date: 92/04/04 |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the proc info in detial                            |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void nice_proc(register int slot)
{

extern int no_kmem_write;
proc_t tmp_proc;
int new_nice;
int my_uid;

	if (procs[slot].p_stat MATCHES) {

		mvwaddstr(bottom_win, 1, 2, " : Slot is currently Free, Any key to continue:");
		wclrtoeol(bottom_win);
		wattron(bottom_win, A_BOLD);
		mvwaddstr(bottom_win, 1, 2, "Select");
		mvwaddstr(bottom_win, 1, 28, "Free");
		mvwaddstr(bottom_win, 1, 34, "Any key");
		wattroff(bottom_win, A_BOLD);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		mvwaddstr(bottom_win, 1, 54, " ");
		wrefresh(bottom_win);
		wgetch(bottom_win);
		touchwin(main_win);
		wrefresh(main_win);
		return;
	}

	my_uid = -1;
	my_uid = getuid();

	kmem_read((caddr_t)&tmp_proc,
		(daddr_t)((proc_t *)namelist[NM_PROC].n_value + slot),
		sizeof(proc_t));

	if (no_kmem_write)
		return;

	if ((my_uid MATCHES) || (my_uid == tmp_proc.p_uid)) {
		new_nice = 20;
		get_new_nice_value(&new_nice);

		tmp_proc.p_nice = new_nice;

		kmem_write((daddr_t)&((proc_t *)namelist[NM_PROC].n_value)[slot] +
			(((caddr_t)&tmp_proc.p_nice) - (caddr_t)&tmp_proc),
			(caddr_t)&tmp_proc.p_nice, sizeof(tmp_proc.p_nice));
	} else {
		beep();
		mvwprintw(bottom_win, 1, 2, " Sorry you don't have permission to renice pid: %d - Press <Enter>", tmp_proc.p_pid);
		wclrtoeol(bottom_win);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		mvwaddstr(bottom_win, 1, 69, " ");
		wrefresh(bottom_win);
		wgetch(bottom_win);
		touchwin(main_win);
		wrefresh(main_win);
		return;
	}
	any_key();
	doupdate();
	touchwin(main_win);
	wrefresh(main_win);
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/03/20 |
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

void get_new_nice_value(int *new_nice)
{

WINDOW *tmp1;
char tmp_val[16];

	tmp_val[0] = '\0';

	tmp1 = newwin(3, 46, 14, 12);
	Draw_A_Box(tmp1, DEFAULT_BOX);
	wattron(tmp1, A_REVERSE);
	mvwaddstr(tmp1, 0, 1, " nice select ");
	wattroff(tmp1, A_REVERSE);
	while (TRUE) {
		mvwprintw(tmp1, 1, 25, "                   ");
		mvwprintw(tmp1, 1, 2, "Select a value between %d and %d: ",
			MIN_NICE_VALUE, MAX_NICE_VALUE);
		wrefresh(tmp1);
		wgetstr(tmp1, tmp_val);
		if (tmp_val[0] == 'q') {
			delwin(tmp1);
			touchwin(main_win);
			wrefresh(main_win);
			return;
		}
		*new_nice = atoi(tmp_val);
		if (atoi(tmp_val) >= MIN_NICE_VALUE && atoi(tmp_val) <= MAX_NICE_VALUE) {
			delwin(tmp1);
			touchwin(main_win);
			wrefresh(main_win);
			return;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void num_of_open_procs(void)                   Date: %Z% |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Draw the header for file                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */


void num_of_open_procs()
{

int proc_ctr  = 0;
proc_t *tmp_procs;
register i;

	/* get proc count */
	tmp_procs = (proc_t *) malloc(v.v_proc * sizeof(proc_t));
	kmem_read(tmp_procs, namelist[NM_PROC].n_value,
	    sizeof(proc_t) * v.v_proc);

	for (i = 0; i < v.v_proc; i++) {
		if (tmp_procs[i].p_stat MATCHES)
			continue;
		proc_ctr++;
	}
	free((char *) tmp_procs);

	wattron(bottom_win, A_BOLD);
	mvwprintw(bottom_win, 1, 58, "Procs Running");
	wattroff(bottom_win, A_BOLD);
	mvwprintw(bottom_win, 1, 71, ": %3d", proc_ctr);
	mvwaddstr(bottom_win, 1, 28, " ");
	wnoutrefresh(bottom_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

