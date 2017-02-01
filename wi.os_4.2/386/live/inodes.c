/*
 * +--------------------------------------------------------------------+
 * | Function: inodes.c                                  Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the inode info.                                        |
 * +--------------------------------------------------------------------+
 *
 * Notes:
 *          This /usr/include/sys/inode.h is very different from the one
 *          for Xenix so once I get it working then I will try to change
 *          and update it.
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix"
#endif /* __STDC__ */

#include "wi.h"

#include <sys/dir.h>
#include <sys/var.h>
#include <sys/inode.h>
#include <sys/stream.h>

#include <signal.h>

void wi_inodes(void);
void draw_inode_screen(int slot, int max_val);
void watch_inodes(int *main_slot, int max_val);
static void sig_alrm_inodes(int signo);

void   getinode(int i, int pg_ctr);
void	inode_header(void);
void num_of_open_inodes(void);

extern int socket_flag;
extern int socksys_id;

/*
 * +--------------------------------------------------------------------+
 * | Function: void wi_inodes()                           Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        draw the inodes                                             |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int    pg_ctr;

void wi_inodes()
{

int   i = 0;
int	get_value = 0;
register max_val;

	inodes = (inode_t *) malloc(v.v_inode * sizeof(inode_t));
	kmem_read(inodes, namelist[NM_INODE].n_value, sizeof(inode_t) * v.v_inode);

	inode_header();

	max_val = v.v_inode; 
	pg_ctr = 4;
	for (i = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			an_option();
			num_of_open_inodes();
			doupdate();

			switch(wgetch(bottom_win)) {
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
				nap(1000);

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

			case 's' :
			case 'S' :
				drawdisp(7);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 70, "Search");
				Set_Colour(main_win, Normal);
				wrefresh(main_win);
				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				i = get_value;
				drawdisp(7);
				wrefresh(main_win);
				break;

			case 'w' :
			case 'W' :
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
				drawdisp(25);
				watch_inodes(&i, max_val);
				drawdisp(7);
				break;

			case 'q' :
			case 'Q' :
				free((char *) inodes);
				return;

			case 'u' :
			case 'U' :
				kmem_read(inodes, namelist[NM_INODE].n_value,
	    				sizeof(inode_t) * v.v_inode);
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
			inode_header();
			pg_ctr = 4;
		} else {
			getinode(i, pg_ctr);
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
 * | Function: getinode(int i, int pg_ctr)              Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays inode info.                                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */


void getinode(int i, int pg_ctr)
{

int	tmp_flag;
char	*modes = " pcCd bBf l";
inode_t *ip;
char dev_name[32];

	ip = &inodes[i];
	tmp_flag = 0;

	wmove(main_win, pg_ctr, 1);
	w_clrtoeol(main_win, 0, 0);

	if (ip->i_count MATCHES) {
		mvwprintw(main_win,pg_ctr, 2, "%4d - ", i);
		Set_Colour(main_win, Text_Warning);
		waddstr(main_win, "Free");
		Set_Colour(main_win, Normal);
		return;
	}

	if ((major(ip->i_rdev) & 0377) == socksys_id) {
		if (socket_flag) {
			tmp_flag = 1;
			if (major_flag)
				mvwprintw(main_win, pg_ctr, 2, "%4d   Socket", i);
			else
				mvwprintw(main_win, pg_ctr, 2, "%4d  %0.3d %0.3d", i,
					major(ip->i_dev) & 0377, minor(ip->i_dev));
		} else
			mvwprintw(main_win, pg_ctr, 2, "%4d unknown", i);
	} else {
		if (major_flag) {
			if ((ip->i_ftype & IFMT) == IFCHR) {
				if (ip->i_un.i_sp == NULL) {
					if (check_for_dev(major(ip->i_dev) & 0377,minor(ip->i_dev),dev_name) != 0)
						print_dev(1, major(ip->i_dev) & 0377, minor(ip->i_dev), dev_name, 0);
					if (strcmp(dev_name, "unknown"))
						mvwprintw(main_win, pg_ctr, 2, "%4d /dev/%-7s", i, dev_name);
					else
						mvwprintw(main_win, pg_ctr, 2, "%4d unknown", i);
				} else
					if ((ip->i_un.i_sp - (struct stdata *) namelist[NM_STREAM].n_value < 0) ||
			    		    (ip->i_un.i_sp - (struct stdata *) namelist[NM_STREAM].n_value >= 300))
						mvwprintw(main_win, pg_ctr, 2, "%4d unknown", i);
					else
						mvwprintw(main_win, pg_ctr, 2, "%4d   Stream", i);
			} else {
				if (check_for_dev(major(ip->i_dev) & 0377,minor(ip->i_dev),dev_name) != 0)
					print_dev(1, major(ip->i_dev) & 0377, minor(ip->i_dev), dev_name, 0);
				mvwprintw(main_win, pg_ctr, 2, "%4d /dev/%-7s", i, dev_name);
			}
		} else
			mvwprintw(main_win, pg_ctr, 2, "%4d  %0.3d %0.3d", i,
				major(ip->i_dev) & 0377, minor(ip->i_dev));
	}

	mvwprintw(main_win, pg_ctr, 19, "%5d %3d %3d %3d %3d %7lx %c",
	    ip->i_number, ip->i_count, ip->i_nlink,
	    ip->i_uid, ip->i_gid, ip->i_size, 
	    modes[(ip->i_ftype & IFMT) >> 12]);

	/*
		This is one of the most complicated if's I have written for a long 
		time.
	*/
	if (tmp_flag) {
		if (major_flag)
			wprintw(main_win, " Socket, Min: %d", minor(ip->i_rdev));
		else
			wprintw(main_win, " %0.3d %0.3d", major(ip->i_rdev) & 0377, minor(ip->i_rdev));
	}
	else {
		if (!(((ip->i_ftype & IFMT) == IFDIR) || 
	    	   ((ip->i_ftype & IFMT) == IFREG) || 
	    	   ((ip->i_ftype & IFMT) == IFIFO))) {
			if (major_flag) {
				if ((ip->i_ftype & IFMT) == IFCHR) {
					if (ip->i_un.i_sp == NULL) {
						if ((ip->i_count NOT_MATCH) && (ip->i_nlink MATCHES) &&
				    			(ip->i_uid MATCHES) && (ip->i_gid MATCHES) && (ip->i_size MATCHES))
							wprintw(main_win, " Maj:%d, Min:%d", major(ip->i_rdev) & 0377, minor(ip->i_rdev));
						else {
							print_dev(0, major(ip->i_rdev) & 0377, minor(ip->i_rdev), dev_name, 0);
							wprintw(main_win, " /dev/%-7s", dev_name);
						}
					} else
						if ((ip->i_un.i_sp - (struct stdata *) namelist[NM_STREAM].n_value < 0) ||
			     	    			(ip->i_un.i_sp - (struct stdata *) namelist[NM_STREAM].n_value >= 300))
							wprintw(main_win, " Maj:%d, Min:%d", major(ip->i_rdev) & 0377, minor(ip->i_rdev));
						else
							wprintw(main_win, " Stream: %d", ip->i_un.i_sp - (struct stdata *) namelist[NM_STREAM].n_value);
				} else {
					print_dev(1, major(ip->i_rdev) & 0377, minor(ip->i_rdev), dev_name, 0);
					wprintw(main_win, " /dev/%-7s", dev_name);
				}
			} else
				wprintw(main_win, " %0.3d %0.3d", major(ip->i_rdev) & 0377, minor(ip->i_rdev));
		} else {
			waddstr(main_win, "  -   -");	/* special file stuff */
			wmove(main_win, pg_ctr, 61);
		}
	}

	if (ip->i_flag & ILOCK)
		waddstr(main_win, " lc");
	if (ip->i_flag & IUPD)
		waddstr(main_win, " up");
	if (ip->i_flag & IACC)
		waddstr(main_win, " ac");
	if (ip->i_flag & IMOUNT)
		waddstr(main_win, " mt");
	if (ip->i_flag & IWANT)
		waddstr(main_win, " wn");
	if (ip->i_flag & ITEXT)
		waddstr(main_win, " tx");
	if (ip->i_flag & ICHG)
		waddstr(main_win, " ch");
	if (ip->i_flag & ISYN)
		waddstr(main_win, " sy");
	if (ip->i_flag & IADV)
		waddstr(main_win, " ad");
	if (ip->i_flag & IDOTDOT)
		waddstr(main_win, " ..");
	if (ip->i_flag & IRMOUNT)
		waddstr(main_win, " rmt");
	if (ip->i_flag & IISROOT)
		waddstr(main_win, " ir");
	if (ip->i_flag & IWROTE)
		waddstr(main_win, " wr");
	if (ip->i_flag & IXLOCKED)
		waddstr(main_win, " xl");
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void inode_header(void)                   Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Draw the inode header                                       |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void inode_header(void)
{

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 2, 2, "Slot");
	mvwaddstr(main_win, 2, 8, "M_Device");
	mvwaddstr(main_win, 2, 17, "Inumber");
	mvwaddstr(main_win, 2, 25, "Ref");
	mvwaddstr(main_win, 2, 29, "Lnk");
	mvwaddstr(main_win, 2, 33, "Uid");
	mvwaddstr(main_win, 2, 37, "Gid");
	mvwaddstr(main_win, 2, 43, "Size");
	mvwaddstr(main_win, 2, 49, "T");
	mvwaddstr(main_win, 2, 52, "Device");
	mvwaddstr(main_win, 2, 62, "Flags");
	Set_Colour(main_win, Normal);
	return;
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

void draw_inode_screen(int slot, int max_val)
{

register counter;
int pg_ctr;

	kmem_read(inodes, namelist[NM_INODE].n_value,
	    sizeof(inode_t) * v.v_inode);

	pg_ctr = 4;
	for (counter = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			return;
		} else {
			getinode(slot, pg_ctr);
			if (slot == (max_val)) {
				pg_ctr++;
				slot = 0;
			} else
				slot++;
			pg_ctr++;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void num_of_open_inodes(void)              Date: %Z% |
 * | Author: Paul Ready.                                                |
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


void num_of_open_inodes()
{

int inode_ctr  = 0;
inode_t *tmp_inodes;
register i;

int position = 0;

	/* get icount */
	tmp_inodes = (inode_t *) malloc(v.v_inode * sizeof(inode_t));
	kmem_read(tmp_inodes, namelist[NM_INODE].n_value,
	    sizeof(inode_t) * v.v_inode);

	for (i = 0; i < v.v_inode; i++) {
		if (tmp_inodes[i].i_count MATCHES)
			continue;
		inode_ctr++;
	}
	free((char *) tmp_inodes);

	position = sizeof(inode_ctr);

	mvwprintw(bottom_win, 1, 57, "            ");
	Set_Colour(bottom_win, Colour_White);
	mvwprintw(bottom_win, 1, (63 - position), "Inodes Open");
	Set_Colour(bottom_win, Normal);
	mvwprintw(bottom_win, 1, (74 - position), ": %d",  inode_ctr);
	mvwaddstr(bottom_win, 1, 28, " ");
	wnoutrefresh(bottom_win);
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

void watch_inodes(int *main_slot, int max_val)
{

int get_value = 0;
int cmd = 0;
int slot = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;


	inode_header();
	slot = *main_slot;
	naptime = Sleep_Time;

	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		draw_inode_screen(slot, max_val);
		wnoutrefresh(main_win);
		an_option();
		num_of_open_inodes();
		doupdate();

		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_inodes) == SIG_ERR)
			printf("\nsignal(SIGALRM) error\n");

		alarm(naptime);	/* set the alarm timer */

		if ((cmd = wgetch(bottom_win)) < 0) {
			alarm(0);			/* stop the alarm timer */
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
				drawdisp(25);
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

			case 'l' :
			case 'L' :
				drawdisp(25);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 49, "Lookup");
				Set_Colour(main_win, Normal);
				wrefresh(main_win);

				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				slot = get_value;

				mvwaddstr(main_win, 0, 49, "Lookup");
				Set_Colour(main_win, Colour_Blue);
				mvwaddstr(main_win, 0, 49, "L");
				Set_Colour(main_win, Blink_Banner);
				if (Sleep_Time_Flag) {
					mvwaddstr(main_win, 0, 57, "Time");
				} else {
					if (naptime == 4)
						mvwaddstr(main_win, 0, 63, "Slow");
					else if (naptime == 2)
						mvwaddstr(main_win, 0, 68, "Med");
					else
						mvwaddstr(main_win, 0, 72, "Fast");
				}
				Set_Colour(main_win, Normal);
				break;

			case 'I' :
			case 'i' :
				drawdisp(25);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				drawdisp(25);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				drawdisp(25);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
			case 'f' :
				drawdisp(25);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
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

static void sig_alrm_inodes(int signo)
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
