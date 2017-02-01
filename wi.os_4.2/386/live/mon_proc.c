/*
 * +--------------------------------------------------------------------+
 * | Function: mon_procs.c                               Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the proc info.                                         |
 * +--------------------------------------------------------------------+
 *
 * Notes:
 *		Nathan gave me the idea after showing me the u386mon bit.
 *		I am not sure if I can include this or not ?
 *
 * Updates:   
 *          None.
 * Bugs:
 *    v2.3  Change this to look like xenix when there are not many
 *		Items.
 *						Sat Jul 17 18:45:17 EDT 1993 - PKR.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */

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
#include <sys/sysi86.h>

#define PID		0
#define SCPU	1
#define UCPU	2
#define NICE	3
#define PRI		4
#define SIZE	5

int sort_type;

extern int errno;
extern int nprocs;
extern proc_t *current_procs;
extern proc_t **ptr_procs;
extern int max_procs_to_display;

int	mypid;
int	nprocs = 0;

int ppproc_pid_compare(register proc_t **ppp1, register proc_t **ppp2);
void get_sorted_procs(void);
int get_user(register proc_t *tmp_proc, register user_t *tuser);
void watch_the_procs(int proc_ctr);

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

int ppproc_pid_compare(register proc_t **ppp1, register proc_t **ppp2)
{
	return((*ppp1)->p_pid - (*ppp2)->p_pid);
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

void get_sorted_procs(void)
{

register proc_ctr;
register char *char_ptr;
register int *int_ptr;
register proc_t *tmp_proc;

	kmem_read((caddr_t)current_procs, namelist[NM_PROC].n_value,
	    sizeof(proc_t) * v.v_proc);

	for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++)
		tmp_proc = ptr_procs[proc_ctr] = (current_procs + proc_ctr);
	
	nprocs = 0;
	for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
		tmp_proc = ptr_procs[proc_ctr];
		if ((tmp_proc->p_stat MATCHES) ||
		    (tmp_proc->p_pid NOT_MATCH)  ||
		    (tmp_proc->p_flag & SSYS)) {
			tmp_proc->p_pid = 32767;
			continue;
		}
		nprocs++;
	}

	sort_type = SIZE;

	switch(sort_type) {
		case SCPU:
			if (nprocs > max_procs_to_display) {
				nprocs = 0;
				for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
					tmp_proc = ptr_procs[proc_ctr];
					if (tmp_proc->p_pid == 32767)
						continue;
					else if (tmp_proc->p_stime <= 5) {
						tmp_proc->p_pid = 32767;
						continue;
					}
					nprocs++;
				}
			}
			break;

		case UCPU:
			if (nprocs > max_procs_to_display) {
				nprocs = 0;
				for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
					tmp_proc = ptr_procs[proc_ctr];
					if (tmp_proc->p_pid == 32767)
						continue;
					else if (tmp_proc->p_utime <= 5) {
						tmp_proc->p_pid = 32767;
						continue;
					}
					nprocs++;
				}
			}
			break;

		case NICE:
			if (nprocs > max_procs_to_display) {
				nprocs = 0;
				for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
					tmp_proc = ptr_procs[proc_ctr];
					if (tmp_proc->p_pid == 32767)
						continue;
					else if (tmp_proc->p_nice == 20) {
						tmp_proc->p_pid = 32767;
						continue;
					}
					nprocs++;
				}
			}
			break;

		case PRI:
			if (nprocs > max_procs_to_display) {
				nprocs = 0;
				for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
					tmp_proc = ptr_procs[proc_ctr];
					if (tmp_proc->p_pid == 32767)
						continue;
					else if (tmp_proc->p_pri <= 20) {
						tmp_proc->p_pid = 32767;
						continue;
					}
					nprocs++;
				}
			}
			break;

		case SIZE:
			if (nprocs > max_procs_to_display) {
				nprocs = 0;
				for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
					tmp_proc = ptr_procs[proc_ctr];
					if (tmp_proc->p_pid == 32767)
						continue;
					else if (get_user(tmp_proc, &user)) {
						*int_ptr = (ctob((u_long)user.u_tsize + user.u_dsize + user.u_ssize))/ 1024;
				    		if (*int_ptr <= 50) {
							tmp_proc->p_pid = 32767;
							continue;
						}
					}
					nprocs++;
				}
			}

			if (nprocs > max_procs_to_display) {
				nprocs = 0;
				for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
					tmp_proc = ptr_procs[proc_ctr];
					if (tmp_proc->p_pid == 32767)
						continue;
					else if (get_user(tmp_proc, &user)) {
						*int_ptr = (ctob((u_long)user.u_tsize + user.u_dsize + user.u_ssize))/ 1024;
				    		if (*int_ptr <= 100) {
							tmp_proc->p_pid = 32767;
							continue;
						}
					}
					nprocs++;
				}
			}

			break;

		default :
			break;
	}

	if (nprocs > max_procs_to_display) {
		nprocs = 0;
		for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
			tmp_proc = ptr_procs[proc_ctr];
			if (tmp_proc->p_pid == 32767)
				continue;
			else if (tmp_proc->p_stat == SZOMB) {
				tmp_proc->p_pid = 32767;
				continue;
			}
			nprocs++;
		}
	}

	if (nprocs > max_procs_to_display) {
		nprocs = 0;
		for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
			tmp_proc = ptr_procs[proc_ctr];
			if (tmp_proc->p_pid == 32767)
				continue;
			else if (get_user(tmp_proc, &user)) {
				if (!strcmp(char_ptr = user.u_comm, "sh") ||
				    !strcmp(char_ptr, "csh")		||
				    !strcmp(char_ptr, "ksh")		||
				    !strcmp(char_ptr, "-ksh")		||
				    !strcmp(char_ptr, "rsh")		||
				    !strcmp(char_ptr, "getty")	||
				    !strcmp(char_ptr, "/etc/getty")	||
				    !strcmp(char_ptr, "uugetty")) {
					tmp_proc->p_pid = 32767;
					continue;
				}
			}
			nprocs++;
		}
	}

	if (nprocs > max_procs_to_display) {
		nprocs = 0;
		for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
			tmp_proc = ptr_procs[proc_ctr];
			if (tmp_proc->p_pid == 32767)
				continue;
			else if (get_user(tmp_proc,&user)) {
				if (!strcmp(char_ptr = user.u_comm, "cron") ||
				    !strcmp(char_ptr, "errdemon")	||
				    !strcmp(char_ptr, "lpsched")	||
				    !strcmp(char_ptr, "sched")	||
				    !strcmp(char_ptr, "vhand")	||
				    !strcmp(char_ptr, "logger")	||
				    !strcmp(char_ptr, "biod")		||
				    !strcmp(char_ptr, "rexd")		||
				    !strcmp(char_ptr, "inetd")	||
				    !strcmp(char_ptr, "bdflush")	||
				    !strcmp(char_ptr, "sterr")	||
				    !strcmp(char_ptr, "scologin")) {
					tmp_proc->p_pid = 32767;
					continue;
				}
			}
			nprocs++;
		}
	}

	if (nprocs > max_procs_to_display) {
		nprocs = 0;
		for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
			tmp_proc = ptr_procs[proc_ctr];
			if (tmp_proc->p_pid == 32767)	/* previously eliminated? */
				continue;
			else if (!(tmp_proc->p_flag & SLOAD) && (tmp_proc->p_stat != SRUN)) {
				tmp_proc->p_pid = 32767;
				continue;
			}
			nprocs++;
		}
	}

	if (nprocs > max_procs_to_display) {
		nprocs = 0;
		for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
			tmp_proc = ptr_procs[proc_ctr];
			if (tmp_proc->p_pid == 32767)
				continue;
			else if (!tmp_proc->p_cpu) {
				tmp_proc->p_pid = 32767;
				continue;
			}
			nprocs++;
		}
	}


	if (nprocs > max_procs_to_display) {
		nprocs = 0;
		for (proc_ctr = 0; proc_ctr < v.v_proc; proc_ctr++) {
			tmp_proc = ptr_procs[proc_ctr];
			if (tmp_proc->p_pid == 32767)
				continue;
			else if (tmp_proc->p_stat == SSLEEP) {
				tmp_proc->p_pid = 32767;
				continue;
			}
			nprocs++;
		}
	}

	if (nprocs > max_procs_to_display) {
		nprocs = max_procs_to_display;
	}

	(void)qsort((char *)ptr_procs,(unsigned)v.v_proc,
	    sizeof(proc_t *),ppproc_pid_compare);
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

int get_user(register proc_t *tmp_proc, register user_t *tuser)
{
	return(!!sysi86(RDUBLK, tmp_proc->p_pid, (char *)tuser, sizeof(*tuser)));
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

void watch_the_procs(int proc_ctr)
{

	proc_t *tmp_proc = ptr_procs[proc_ctr];

	if ((tmp_proc->p_stat == SRUN) && !(tmp_proc->p_flag & SLOAD))
		wattron(main_win, A_BOLD | A_BLINK);
	else if (tmp_proc->p_stat == SRUN)
		wattron(main_win, A_BOLD);

	if (size_flag)
		if (tmp_proc->p_stat == SONPROC)
			wattron(main_win, A_BOLD);

	mvwprintw(main_win, proc_ctr + 4, 3, "%c %3d %5d %5d %3d %0.2d",
	    " sRzdipx"[tmp_proc->p_stat], tmp_proc->p_cpu & 0377,
	    tmp_proc->p_pid, tmp_proc->p_ppid, tmp_proc->p_pri & 0377, tmp_proc->p_nice);

	if (tmp_proc->p_stat == SZOMB) {
		mvwaddstr(main_win, proc_ctr + 4, 50, "<zombie>");
	} else if (tmp_proc->p_stat != 0) {
		if (get_user(tmp_proc, &user)) {
			mvwprintw(main_win, proc_ctr + 4, 28, "%s",
			    get_cpu_time(user.u_utime));
			mvwprintw(main_win, proc_ctr + 4, 35, "%s",
			    get_cpu_time(user.u_stime));
			mvwprintw(main_win, proc_ctr + 4, 42, "%5d %3s",
			    (ctob((u_long)user.u_tsize + user.u_dsize + user.u_ssize))/ 1024, print_tty(tmp_proc->p_pid,
			    0));

			mvwprintw(main_win, proc_ctr + 4, 52, "%.25s", user.u_psargs);
		} else {
			switch(tmp_proc->p_stat) {
			case SZOMB:
				mvwaddstr(main_win, proc_ctr + 4, 52, "<zombie>");
				break;
			case SXBRK:
				mvwaddstr(main_win, proc_ctr + 4, 52, "<xbreak>");
				break;
			case SIDL:
				mvwaddstr(main_win, proc_ctr + 4, 52, "<in creation>");
				break;
			default :
				mvwaddstr(main_win, proc_ctr + 4, 52, "<swapping>");
			}
		}
	}
	if ((tmp_proc->p_stat == SRUN) && !(tmp_proc->p_flag & SLOAD))
		wattroff(main_win, A_BOLD | A_BLINK);
	else if (tmp_proc->p_stat == SRUN)
		wattroff(main_win, A_BOLD);

	if (size_flag)
		if (tmp_proc->p_stat == SONPROC)
			wattroff(main_win, A_BOLD);
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
