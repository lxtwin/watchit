/*
 * +--------------------------------------------------------------------+
 * | Function: user().                                   Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the user info.                                         |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *(1) 2.3: All the graphic, stuff.
 *							Sun Aug 22 16:37:57 EDT 1993 - PKR.
 *Bugs:
 *          None yet.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%			Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%			Version %I% %D% - Dapix "
#endif /* __STDC__ */

#include "wi.h"

#include <sys/lock.h>
#include <sys/sysi86.h>
#include <pwd.h>
#include <grp.h>
#include <sys/dir.h>
#include <sys/var.h>
#include <sys/user.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/page.h>
#include <sys/immu.h>
#include <sys/seg.h>
#include <sys/user.h>
#include <sys/region.h>
#include <sys/proc.h>

extern char *print_tty(int pid, int type_flag);

int	finduser(proc_t *proc, int slot, user_t *user);
void	print_files(proc_t *proc, int slot);
int	getuser(int i);
void	found_zombie(int i);
void	do_free(int i);
void do_swapped(int i);
void	watch_user(int *old_slot, int max_val);
void wi_users(void);
void draw_page(void);

extern int size_flag;

int	monitoring_flag;

/*
 * +--------------------------------------------------------------------+
 * | Function: finduser().                               Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       get the user struct for each proc found.                     |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int finduser(proc_t *proc, int slot, user_t *user)
{

int ipde = 0;
paddr_t mptr;
register caddr_t uptr = (caddr_t) user;
register int ubrdcount = sizeof(user_t);

proc_t *procs = (proc_t *) namelist[NM_PROC].n_value;

#ifdef RDUBLK		/* I will keep it in here just in case. */
	sysi86(RDUBLK, proc->p_pid, (char *) user, sizeof (*user));
#else
	if (proc->p_flag & SULOAD) {
		for (ipde = 0; ipde < proc->p_usize; ipde++) {
			if (!proc->p_ubptbl[ipde].pgm.pg_pres)
				return(0);

			mptr = proc->p_ubptbl[ipde].pgm.pg_pfn * NBPP;
			mem_read(uptr, (daddr_t)mptr, min(ubrdcount, NBPP));
			uptr += NBPP;
			if ((ubrdcount -= NBPP) <= 0)
				break;
		}
	} else
		swap_read((caddr_t)user, (proc->p_ubdbd.dbd_blkno) * NBPSCTR,
		    sizeof(user_t));

#endif
	if (user->u_procp - procs == slot)
		return(1);
	else
		return(0);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: user().                                   Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the user info.                                         |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void wi_users(void)
{

int	i = 0;
int	get_value = 0;
int max_val;

	procs = (proc_t *) malloc(v.v_proc * sizeof(proc_t));
	users = (user_t *) malloc((sizeof(user_t) + ((sizeof(file_t *) + 1) * v.v_nofiles)+ 1) & ~1);

	kmem_read(procs, namelist[NM_PROC].n_value, sizeof(proc_t) * v.v_proc);

	/* usr_header(); */
	max_val = v.v_proc;
	for (i = 0;; i++) {
		if (i == (max_val))
			i = max_val - 1 ;

		getuser(i);
		wnoutrefresh(main_win);
		an_option();
		doupdate();
		switch(wgetch(bottom_win)) {
		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			i--;
			break;

		case 'P' :
		case 'p' :
			screen_dump();
			i--;
			break;

		case 'G' :
		case 'g' :
			Show_Graphics();
			i--;
			break;

		case 'b' :
		case 'B' :
		case KEY_PPAGE:
		case KEY_UP:
		case '-' :
			if (i MATCHES) {
				i = -1;
				beep();
			} else
				i -= 2;
			break;

		case 's' :
		case 'S' :
			drawdisp(12);
			wattron(main_win, A_REVERSE);
			mvwaddstr(main_win, 0, 70, "Search");
			wattroff(main_win, A_REVERSE);
			wrefresh(main_win);
			get_value = 0;
			find_item(&get_value, 0, (max_val - 1));
			i = get_value;
			i--;
			drawdisp(12);
			wrefresh(main_win);
			break;

		case 'w' :
		case 'W' :
			drawdisp(16);
			monitoring_flag = 1;
			watch_user(&i, max_val);
			monitoring_flag = 0;
			drawdisp(12);
			i--;
			break;

		case 'q' :
		case 'Q' :
			free((char *) procs);
			free((char *) users);
			return;

		case 'u' :
		case 'U' :
			kmem_read(procs, namelist[NM_PROC].n_value,
			    sizeof(proc_t) * v.v_proc);
			i--;
			break;

		case 'f' :
		case 'F' :
		case KEY_NPAGE:
		case KEY_DOWN:
		case '+' :
		default :
			if (i >= (max_val - 1)) {
				i = (max_val - 1);
				beep();
			}
			break;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: getuser()                                 Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the user info.                                     |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int getuser(int i)
{

char dev_name[32];
int	counter;
proc_t   *pp;
proc_t   *pp_ptr;
proc_t   proc;
struct passwd *pw, *getpwuid();
struct group  *gr, *getgrgid();
static char	*segments[] = {
	"data",
	"system",
	"user i"
};


	pp_ptr = &procs[i];
	pp = (proc_t *) namelist[NM_PROC].n_value;
	if (procs[i].p_stat MATCHES) {
		do_free(i);
		return(0);
	}

	if (i >= 0) {
		kmem_read(&proc, (long) &pp[i], sizeof(proc));
		if (!finduser(&proc, i, &user)) {
			if (pp_ptr->p_stat == SZOMB)
				found_zombie(i);
			else
				do_swapped(i);
			return(0);
		}
	} else
		kmem_read(&user, namelist[NM_USER].n_value, sizeof(user));

	/*
		Now display all the info.
	*/

	draw_page();

	wattroff(main_win, A_BOLD);
	wattron(main_win, A_REVERSE);
	mvwprintw(main_win, 2, 74, "%d", i);
	wattroff(main_win, A_REVERSE);

	/*
		uid/gid info
	*/
	mvwaddstr(main_win, 3, 40, "                                     ");
	mvwprintw(main_win, 3, 2, "        uid = %d(%s), gid = %d(%s), ruid = %d(%s), rgid = %d(%s)",
	    user.u_uid,
	    (((pw = getpwuid(user.u_uid)) == NULL) ? "unknown": pw->pw_name),
	    user.u_gid,
	    (((gr = getgrgid(user.u_gid)) == NULL) ? "unknown": gr->gr_name),
	    user.u_ruid,
	    (((pw = getpwuid(user.u_ruid)) == NULL) ? "unknown": pw->pw_name),
	    user.u_rgid,
	    (((gr = getgrgid(user.u_rgid)) == NULL) ? "unknown": gr->gr_name));

	/*
		process info
	*/
	print_dev(0, major(user.u_ttyd), minor(user.u_ttyd), dev_name, 1);

	mvwaddstr(main_win, 5, 40, "                                     ");
	mvwprintw(main_win, 5, 2, "        time: usr: %d, sys: %d, c_usr: %d, c_sys: %d",
	    user.u_utime, user.u_stime, user.u_cutime, user.u_cstime);
	mvwaddstr(main_win, 6, 30, "                      ");
	mvwprintw(main_win, 6, 2, "        tty: /dev/%s, maj(%d), min(%d)"
	    , (pp_ptr->p_ppid  > 1) ? dev_name: print_tty(pp_ptr->p_pid, 1)
	    , major(user.u_ttyd), minor(user.u_ttyd));

	mvwprintw(main_win, 6, 61, ":%s%s%s%s",
	    user.u_lock == UNLOCK  ? " unlocked": "",
	    user.u_lock & PROCLOCK ?     " proc": "",
	    user.u_lock & TXTLOCK  ?     " text": "",
	    user.u_lock & DATLOCK  ?     " data": "");

	/*
		misc info
	*/
	mvwprintw(main_win, 8, 2, "        args:                                                             ");
	
	if ((pp_ptr->p_stat == SRUN) || (pp_ptr->p_stat == SONPROC)) {
		wattron(main_win, A_BOLD);
		mvwprintw(main_win, 8, 15, " %.60s", user.u_psargs);
		wattroff(main_win, A_BOLD);
	} else
		mvwprintw(main_win, 8, 15, " %.60s", user.u_psargs);

	mvwaddstr(main_win, 9, 40, "                                     ");
	mvwprintw(main_win, 9, 2, "        pid: %d, ppid: %d, memory: %lx, type: %s, state: %s%s%s%s%s",
	    pp_ptr->p_pid, pp_ptr->p_ppid, user.u_mem
	    , user.u_acflag ? "fork": "exec",
		(pp_ptr->p_stat == SRUN)    ? "<running>": "",
		(pp_ptr->p_stat == SONPROC) ? "<current>": "",
		(pp_ptr->p_stat == SSLEEP)  ? "<sleeping>": "",
		(pp_ptr->p_stat == SSTOP)   ? "<stopped>": "",
		(pp_ptr->p_stat == SIDL)    ? "<idle>" : "",
		(pp_ptr->p_flag == SUSWAP)  ? "<swapping>": "");

	mvwaddstr(main_win, 10, 40, "                                     ");
	mvwprintw(main_win, 10, 2, "        dir inode: %d, start: %.24s    ",
	    user.u_cdir - (inode_t *) namelist[NM_INODE].n_value,
	    ctime(&user.u_start));

	/*
		file info - Here comes some nifty programming for file mode.
	*/
	mvwaddstr(main_win, 12, 40, "                                     ");
	mvwprintw(main_win, 12, 2, "        file mode:%s%s%s%s%s%s%s%s, file offset: %lx, I/O bytes: %ld",
	    (user.u_fmode & FREAD)   ? " read"	: "",
	    (user.u_fmode & FWRITE)  ? " write"	: "",
	    (user.u_fmode & FNDELAY) ? " ndel"	: "",
	    (user.u_fmode & FAPPEND) ? " app"	: "",
	    (user.u_fmode & FSYNC)   ? " sync"	: "",
	    (user.u_fmode & FCREAT)  ? " cr"	: "",
	    (user.u_fmode & FTRUNC)  ? " trunc"	: "",
	    (user.u_fmode & FEXCL)   ? " excl"	: "",
	    user.u_offset, user.u_count);

	mvwaddstr(main_win, 13, 40, "                                     ");
	mvwprintw(main_win, 13, 2, "        usr addr: %lx, seg: %s, umask: %03o, ulimit: %ld",
	    user.u_base, segments[user.u_segflg], user.u_cmask, user.u_limit);

	if (size_flag) {

		for (counter = 16; counter <= 25; counter++)
			mvwaddstr(main_win,counter, 2, "                                                                          ");
		mvwprintw(main_win, 15, 2, "        ux_uid: %d, ux_gid: %d, ux_mode: %d",
	    	user.u_exfile.ux_uid, user.u_exfile.ux_gid, user.u_exfile.ux_mode);
		mvwprintw(main_win, 16, 2, "        comp: %x, nextcomp: %x, reg.r_val1: %d",
	    		user.u_comp, user.u_nextcp, user.u_r.r_reg.r_val1);
		mvwprintw(main_win, 17, 2, "        bsize: %ld, pgproc: %d, ap: %x, error: %d",
			user.u_bsize, user.u_pgproc, user.u_ap, user.u_error);
		mvwprintw(main_win, 18, 2, "        pboff: %d, pbsize: %d, pbdev: (%d,%d)",
			user.u_pboff, user.u_pbsize, major(user.u_pbdev), minor(user.u_pbdev));
		mvwprintw(main_win, 19, 2, "        rablock: %x, errcnt: %d, pdir: %x",
			user.u_rablock, user.u_errcnt, user.u_pdir);
		mvwprintw(main_win, 20, 2, "        cdir: %x, rdir: %x, ticks: %lx",
			user.u_cdir, user.u_rdir, user.u_ticks);
		mvwprintw(main_win, 21, 2, "        dent.d_name: %s, dent.d_ino: %d",
			user.u_dent.d_name, user.u_dent.d_ino);

		mvwprintw(main_win, 22, 2, "        pr_base: %x, pr_size: %d, pr_off: %d, pr_scale: %d",
	    		user.u_prof.pr_base, user.u_prof.pr_size, user.u_prof.pr_off, user.u_prof.pr_scale);
		mvwprintw(main_win, 23, 2, "        ior: %lx, iow: %lx, iosw: %lx, ioch: %lx",
	    		user.u_ior, user.u_iow, user.u_iosw, user.u_ioch);
		mvwprintw(main_win, 24, 2, "        argv[0]: %8x, argv[1]: %8x, argv[2] %8x", user.u_arg[0], user.u_arg[1], user.u_arg[2]);
		mvwprintw(main_win, 25, 2, "        argv[3]: %8x, argv[4]: %8x, argv[5] %8x", user.u_arg[3], user.u_arg[4], user.u_arg[5]);

		wmove(main_win, 27, 13);
	} else
		wmove(main_win, 15, 13);
	if (user.u_ofile[0] == (file_t *) 0)
		waddstr(main_win, "  No files open.");
	else
		(void) print_files(&proc, i);
		/* (void) print_files(pp_ptr->p_pid, i); */

	wnoutrefresh(main_win);
	return(0);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void do_swapped()                         Date: 92/04/28 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays a zombie message.                                  |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void do_swapped(int i)
{

	draw_page();
	wattron(main_win, A_REVERSE);
	mvwprintw(main_win, 2, 74, "%d", i);
	wattroff(main_win, A_REVERSE);

	/* uid/gid info */
	mvwaddstr(main_win, 3, 40, "                                     ");
	mvwaddstr(main_win, 3, 2, "        Slot Avaialable for use - process being swapped out.    ");

	/* process info */
	mvwaddstr(main_win, 5, 40, "                                     ");
	mvwaddstr(main_win, 5, 2, "        time: usr: N/A, sys: N/A, c_usr: N/A, c_sys: N/A");
	mvwaddstr(main_win, 6, 2, "        tty: N/A                         ");

	mvwaddstr(main_win, 6, 61, ": N/A     ");

	/* misc info */
	mvwaddstr(main_win, 8, 2, "        args: N/A                                                          ");
	mvwaddstr(main_win, 9, 40, "                                     ");
	mvwaddstr(main_win, 9, 2, "        pid: N/A, ppid: N/A, memory: N/A, type: N/A, state: <swapping> ");
	mvwaddstr(main_win, 10, 2, "        dir inode: N/A, start: N/A                         ");

	/* file info */
	mvwaddstr(main_win, 12, 40, "                                     ");
	mvwaddstr(main_win, 12, 2, "        file mode, N/A, file offset: N/A, I/O bytes: N/A");
	mvwaddstr(main_win, 13, 40, "                                     ");
	mvwaddstr(main_win, 13, 2, "        usr addr: N/A, seg: N/A, umask: N/A, ulimit: N/A");

	if (size_flag) {
		mvwaddstr(main_win, 27, 12, ":                                                               ");
		mvwaddstr(main_win, 28, 12, ":                                                               ");
		mvwaddstr(main_win, 30, 12, ":                                                               ");
		mvwaddstr(main_win, 30, 12, ":                                                               ");
		mvwaddstr(main_win, 31, 12, ":                                                               ");
		mvwaddstr(main_win, 33, 12, ":                                                               ");
		mvwaddstr(main_win, 34, 12, ":                                                               ");
		mvwaddstr(main_win, 27, 13, "  Can't trace files, process being swapped.                   ");
	} else {
		mvwaddstr(main_win, 14, 12, ".                                                               ");
		mvwaddstr(main_win, 15, 12, ":                                                               ");
		mvwaddstr(main_win, 16, 12, ":                                                               ");
		mvwaddstr(main_win, 15, 13, "  Can't trace files, process being swapped.                   ");
	}
	
	wnoutrefresh(main_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void do_free()                            Date: 92/04/28 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays a zombie message.                                  |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void do_free(int i)
{

	draw_page();
	wattron(main_win, A_REVERSE);
	mvwprintw(main_win, 2, 74, "%d", i);
	wattroff(main_win, A_REVERSE);

	/* uid/gid info */
	mvwaddstr(main_win, 3, 2, "        Slot Avaialable for use.                                  ");

	/* process info */
	mvwaddstr(main_win, 5, 2, "        time: usr: N/A, sys: N/A, c_usr: N/A, c_sys: N/A");
	mvwaddstr(main_win, 6, 2, "        tty: N/A                         ");

	mvwaddstr(main_win, 6, 61, ": N/A     ");

	/* misc info */
	mvwaddstr(main_win, 8, 2, "        args: N/A                                                          ");
	mvwaddstr(main_win, 9, 2, "        pid: N/A, ppid: N/A, memory: N/A, type: N/A, state: Free ");
	mvwaddstr(main_win, 10, 2, "        dir inode: N/A, start: N/A                         ");

	/* file info */
	mvwaddstr(main_win, 12, 2, "        file mode, N/A, file offset: N/A, I/O bytes: N/A                ");
	mvwaddstr(main_win, 13, 2, "        usr addr: N/A, seg: N/A, umask: N/A, ulimit: N/A                ");

	if (size_flag) {
		mvwaddstr(main_win, 15, 2, "        N/A.                                                              ");

		mvwaddstr(main_win, 27, 13, "  No files open.                                              ");
	} else {
		mvwaddstr(main_win, 15, 13, "  No files open.                                              ");
	}

	wnoutrefresh(main_win);
	return;

}

/*
 * +--------------------------------------------------------------------+
 * | Function: void found_zombie(int i)                  Date: 92/04/28 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays a zombie message.                                  |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void found_zombie(int i)
{

int counter;
	draw_page();
	wattron(main_win, A_REVERSE);
	mvwprintw(main_win, 2, 74, "%d", i);
	wattroff(main_win, A_REVERSE);

	/* uid/gid info */
	mvwaddstr(main_win, 3, 40, "                                     ");
	mvwaddstr(main_win, 3, 2, "        uid = 0(root) - Zombie Process.                             ");

	/* process info */
	mvwaddstr(main_win, 5, 40, "                                     ");

	mvwaddstr(main_win, 5, 2, "        time: usr: 0, sys: 0, c_usr: 0, c_sys: 0");
	mvwaddstr(main_win, 6, 2, "        tty: N/A                         ");

	mvwaddstr(main_win, 6, 61, ": N/A     ");

	/* misc info */
	mvwaddstr(main_win, 8, 2, "        args: <defunct>                                                    ");
	mvwaddstr(main_win, 9, 40, "                                     ");
	mvwprintw(main_win, 9, 2, "        pid: %d, ppid: %d, memory: N/A, type: N/A, state: <zombie>  ",
	    procs[i].p_pid, procs[i].p_ppid);
	mvwaddstr(main_win, 10, 2, "        dir inode: N/A, start: N/A                         ");

	/* file info */
	mvwaddstr(main_win, 12, 40, "                                     ");
	mvwaddstr(main_win, 12, 2, "        file mode, N/A, file offset: N/A, I/O bytes: N/A");
	mvwaddstr(main_win, 13, 40, "                                     ");
	mvwaddstr(main_win, 13, 2, "        usr addr: N/A, seg: N/A, umask: N/A, ulimit: N/A");

	if (size_flag) {
		mvwaddstr(main_win, 15, 2, "        N/A.                                                              ");
		for (counter = 16; counter <= 25; counter++)
			mvwaddstr(main_win,counter, 2, "                                                                          ");

		mvwaddstr(main_win, 27, 13, "  No files open.                                              ");
	} else {
		mvwaddstr(main_win, 15, 13, "  No files open.                                              ");
		mvwaddstr(main_win, 16, 13, "                                                              ");
	}

	wnoutrefresh(main_win);
	return;

}

/*
 * +--------------------------------------------------------------------+
 * | Function: void print_user(short pid);               Date: 92/07/07 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        gets the open fd's and files.                               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 *
 *   COMMENT:
 *	    I would like to thank nathen of sco for telling me that the
 *	    file area ia located at the end of the u-area.
 */

void print_files(proc_t *procs, int slot)
{

register loop, counter, position;
int ubsize, total, c, no_files, c_files, size_position;
unsigned int offset;

	ubsize = (sizeof(user_t)+ ((sizeof(file_t *)+ 1)* v.v_nofiles)+ 1)& ~1;

	/* Max read is 2 pages: 8K */
	total = sysi86(RDUBLK, procs->p_pid,(char *) users, ubsize);

	if (!((users->u_ruid == procs->p_uid)||(users->u_ruid == procs->p_suid))) {
		beep();
		do_swapped(slot);
		return;
	}
	size_position = 27;

	for (loop = no_files = 0; loop < v.v_nofiles; loop++)
		if (users->u_ofile[loop] != 0)
			no_files++;
	for (counter = loop = c_files = 0, position = 13; loop < v.v_nofiles; loop++) {
		if (users->u_ofile[loop] != 0) {
			c_files++;

			offset = ((unsigned)users->u_ofile[loop] - namelist[NM_FILE].n_value) / sizeof(file_t);
			if (size_flag) {
				mvwprintw(main_win,size_position, position, "%4d",loop);
				mvwprintw(main_win,size_position + 1, position, "%4d",offset);
				position += 5;
				counter++;
				if (c_files == 12) {
					counter = 0;
					position = 13;
					size_position = 30;
				} else if (c_files == 24) {
					counter = 0;
					position = 13;
					size_position = 33;
				} else if (c_files == 36) {
					counter = 0;
					position = 13;
					size_position = 27;

					wrefresh(main_win);
					mvwaddstr(bottom_win, 1, 2, " More open file(s) press any key to continue: ");
					wclrtoeol(bottom_win);
					Draw_A_Box(bottom_win, DEFAULT_BOX);
					mvwaddstr(bottom_win, 1, 47, " ");
					wrefresh(bottom_win);
					c = wgetch(bottom_win);
	
					any_key();
					wrefresh(bottom_win);
				}
			} else {
				mvwprintw(main_win, 15, position, "%4d",loop);
				mvwprintw(main_win, 16, position, "%4d",offset);
				position += 5;
				counter++;
				if (counter == 12 && no_files > 12) {
					wrefresh(main_win);
					counter = 0;
					position = 13;
					if (monitoring_flag) {
						nap(1000);
						mvwaddstr(main_win, 15, 13, "                                                            ");
						mvwaddstr(main_win, 16, 13, "                                                            ");
					} else {
						beep();
						mvwaddstr(bottom_win, 1, 2, " More open file(s) press any key to continue: ");
						wclrtoeol(bottom_win);
						Draw_A_Box(bottom_win, DEFAULT_BOX);
						mvwaddstr(bottom_win, 1, 47, " ");
						wrefresh(bottom_win);
						c = wgetch(bottom_win);
						any_key();
						wrefresh(bottom_win);
						mvwaddstr(main_win, 15, 13, "                                                            ");
						mvwaddstr(main_win, 16, 13, "                                                            ");
					}

				}
			}
		}
	}
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function: int get_item()                            Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	ask the user for a value between min_val and max_val.       |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int find_item(int *new_value, int min_val, int max_val)
{

WINDOW *tmp1;
char tmp_val[16];

	tmp_val[0] = '\0';
	tmp1 = newwin(3, 46, 12, 10);
	Draw_A_Box(tmp1, DEFAULT_BOX);
	wattron(tmp1, A_REVERSE);
	mvwaddstr(tmp1, 0, 1, " Slot Search ");
	wattroff(tmp1, A_REVERSE);
	while (TRUE) {
		mvwprintw(tmp1, 1, 25, "                   ");
		mvwprintw(tmp1, 1, 2, "Select a value between %d and %d: ",
		    min_val, max_val);
		wrefresh(tmp1);
		curs_set(CUR_VIS);
		wgetstr(tmp1, tmp_val);
		curs_set(CUR_INVIS);
		if (tmp_val[0] == 'q') {
			delwin(tmp1);
			touchwin(main_win);
			wrefresh(main_win);
			return(1);
		}

		*new_value = atoi(tmp_val);
		if (atoi(tmp_val) >= min_val && atoi(tmp_val) <= max_val) {
			delwin(tmp1);
			touchwin(main_win);
			wrefresh(main_win);
			return(0);
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

void watch_user(int *old_slot, int max_val)
{

int cmd, naptime;
int slot;
int get_value = 0;

	slot = *old_slot;

	wattron(main_win, A_REVERSE | A_BLINK);
	mvwaddstr(main_win, 0, 64, "Medium");
	wattroff(main_win, A_REVERSE | A_BLINK);
	naptime = 1000;

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		kmem_read(procs, namelist[NM_PROC].n_value,
			sizeof(proc_t) * v.v_proc);
		getuser(slot);
		wnoutrefresh(main_win);
		an_option();
		doupdate();

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
				drawdisp(16);
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 50, "Lookup");
				wattroff(main_win, A_REVERSE);
				wrefresh(main_win);
				get_value = 0;
				find_item(&get_value, 0, (max_val - 1));
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

			case 'S' :
			case 's' :
				drawdisp(16);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 58, "Slow");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 2000;
				break;

			case 'M' :
			case 'm' :
				drawdisp(16);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 64, "Medium");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 1000;
				break;

			case 'F' :
			case 'f' :
				drawdisp(16);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 72, "Fast");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 500;
				break;

			case KEY_PPAGE:
			case KEY_UP:
			case '-' :
				if (slot MATCHES) {
					slot = 0;
					beep();
				} else
					slot -= 1;
				break;
	
			case KEY_NPAGE:
			case KEY_DOWN:
			case '+' :
				if (slot >= (max_val - 1)) {
					slot = (max_val - 1);
					beep();
				} else
					slot++;
				break;

			case 'Q' :
			case 'q' :
				/* return the last slot used */
				*old_slot = slot;
				return;

			default :
				beep();
				break;
			}
		}
		nap(naptime);
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

void draw_page(void)
{

register counter;

	for (counter = 1; counter <= page_len - 1; counter++)
		mvwaddstr(main_win,counter, 2, "                                                                          ");

	mvwaddstr(main_win, 2, 72, ":    ");
	mvwaddstr(main_win, 4, 14, ".                                                              ");
	mvwaddstr(main_win, 7, 12, ".                                                                ");
	mvwaddstr(main_win, 11, 11, ".                                                                ");
	if (size_flag) {
		mvwaddstr(main_win, 26, 12, ".                                                               ");
		mvwaddstr(main_win, 27, 2, "      :                                                               ");
		mvwaddstr(main_win, 28, 2, "      :                                                               ");
		mvwaddstr(main_win, 30, 2, "      :                                                               ");
		mvwaddstr(main_win, 30, 2, "      :                                                               ");
		mvwaddstr(main_win, 31, 2, "      :                                                               ");
		mvwaddstr(main_win, 33, 2, "      :                                                               ");
		mvwaddstr(main_win, 34, 2, "      :                                                               ");
	} else {
		mvwaddstr(main_win, 14, 12, ".                                                               ");
		mvwaddstr(main_win, 15, 2, "      :                                                               ");
		mvwaddstr(main_win, 16, 2, "      :                                                               ");
	}
	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 2, "User/Group ID's");
	mvwaddstr(main_win, 2, 68, "Slot");
	mvwaddstr(main_win, 4, 2, "Process Info");
	mvwaddstr(main_win, 6, 56, "Locks");
	mvwaddstr(main_win, 7, 2, "Misc. Info");
	mvwaddstr(main_win, 11, 2, "File Info");
	if (size_flag) {
		mvwaddstr(main_win, 14, 2, "Extra u-area Info");

		mvwaddstr(main_win, 26, 2, "Open Files");
		mvwaddstr(main_win, 27, 10, "fd");
		mvwaddstr(main_win, 28, 10, "fs");
		mvwaddstr(main_win, 30, 10, "fd");
		mvwaddstr(main_win, 31, 10, "fs");
		mvwaddstr(main_win, 33, 10, "fd");
		mvwaddstr(main_win, 34, 10, "fs");
	} else {
		mvwaddstr(main_win, 14, 2, "Open Files");
		mvwaddstr(main_win, 15, 10, "fd");
		mvwaddstr(main_win, 16, 10, "fs");
	}
	wattroff(main_win, A_BOLD);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
