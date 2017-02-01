/*
 * +--------------------------------------------------------------------+
 * | Function: regions.c                                 Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the region info.                                       |
 * +--------------------------------------------------------------------+
 *
 * Notes:
 *          Get and display the regions I have yet to fing out how to
 *		referance the pointers to structures within a structure.
 *		Soon I hope.
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


#include "wi.h"

#include <sys/var.h>
#include <sys/immu.h>
#include <sys/region.h>
#include <sys/page.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <signal.h>

int  pg_ctr;
void wi_regions(void);
void getregion(int i, int pg_ctr);
void region_header(void);

int draw_region_screen(int slot, int max_val);
void watch_regions(int *main_slot, int max_val);
void num_of_open_regions(void);
static void sig_alrm_regions(int signo);

extern int size_flag;

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

void wi_regions(void)
{

int   i = 0;
int	get_value = 0;
register max_val;

	regions = (struct region *) malloc(v.v_region * sizeof(struct region));
	kmem_read(regions, namelist[NM_REGION].n_value,
		sizeof(struct region) * v.v_region);

	region_header();

	max_val = v.v_region; 
	pg_ctr = 4;
	for (i = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			an_option();
			num_of_open_regions();
			doupdate();
			switch(wgetch(bottom_win)) {
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
				drawdisp(26);
				watch_regions(&i, max_val);
				drawdisp(10);
				break;

			case 's' :
			case 'S' :
				drawdisp(10);
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 70, "Search");
				wattroff(main_win, A_REVERSE);
				wrefresh(main_win);
				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				i = get_value;
				drawdisp(10);
				wrefresh(main_win);
				break;

			case 'q' :
			case 'Q' :
				free((char *) regions);
				return;

			case 'u' :
			case 'U' :
				kmem_read(regions, namelist[NM_REGION].n_value,
					sizeof(struct region) * v.v_region);
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
	
			case 'G' :
			case 'g' :
				Show_Graphics();

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
				beep();
				break;
			}
			region_header();
			pg_ctr = 4;
		} else {
			getregion(i, pg_ctr);
			if (i == (max_val)) {
				pg_ctr++;
				mvwaddstr(main_win, pg_ctr, 2, "                                                                      ");
				i = 0;
			} else
				i++;
			pg_ctr++;
		}
	}
}


/*
 * +--------------------------------------------------------------------+
 * | Function: getregion(int i, int pg_ctr)             Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays region info.                                       |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */


void getregion(int i, int pg_ctr)
{

struct region *rp;

	rp = &regions[i];

	mvwaddstr(main_win, pg_ctr, 1, "                                                                            ");
	if (rp->r_pgsz MATCHES) {
			mvwprintw(main_win,pg_ctr, 2, "%4d - Free", i);
			return;
	}

	mvwaddstr(main_win, pg_ctr, 1, "                                                                            ");
	mvwprintw(main_win, pg_ctr, 2, "%4d %3ld %3ld %3d %3d %3d %6x %4d %4d %4d %s%s%s%s%s%s %8lx %s%s%s%s%s%s%s%s%s%s%s%s ",
	    i, rp->r_pgsz, rp->r_nvalid, rp->r_waitcnt, rp->r_refcnt,
	    rp->r_swapcnt, rp->r_filesz,
	    rp->r_forw - (struct region *) namelist[NM_REGION].n_value,
	    rp->r_back - (struct region *) namelist[NM_REGION].n_value,
 	    (rp->r_filesz MATCHES) ? 0:
	    	rp->r_iptr - (inode_t *) namelist[NM_INODE].n_value,
	    (rp->r_type == RT_UNUSED)  ? "uuse": "",
	    (rp->r_type == RT_PRIVATE) ? "priv": "",
	    (rp->r_type == RT_STEXT)   ? "stxt": "",
	    (rp->r_type == RT_SHMEM)   ? "shmm": "",
	    (rp->r_type == RT_MAPFILE) ? "mapf": "",
	    (rp->r_type == RT_VM86)    ? "vm86": "",
	     rp->r_list,
	    (rp->r_flags & RG_NOFREE)  ? "nofr ": "",
	    (rp->r_flags & RG_DONE)    ? "done ": "",
	    (rp->r_flags & RG_NOSHARE) ? "nshare ": "",
	    (rp->r_flags & RG_LOCK)    ? "lock ": "",
	    (rp->r_flags & RG_WANTED)  ? "want ": "",
	    (rp->r_flags & RG_WAITING) ? "wait ": "",
	    (rp->r_flags & RG_WASTEXT) ? "wastxt ": "",
	    (rp->r_flags & RG_DOWN)    ? "stack ": "",
	    (rp->r_flags & RG_SWAP)    ? "swap ": "",
	    (rp->r_flags & RG_WEITEK)  ? "weitek ": "",
	    (rp->r_flags & RG_VPIX)    ? "vpix ": "",
	    (rp->r_flags & RG_AGED)    ? "age": "");
}

/*
 * +--------------------------------------------------------------------+
 * | Function: region_header()                           Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays region header                                      |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void region_header(void)
{

	wattron(main_win, A_BOLD);
	mvwaddstr(main_win, 2, 2, "Slot");
	mvwaddstr(main_win, 2, 7, "Psz");
	mvwaddstr(main_win, 2, 11, "#vl");
	mvwaddstr(main_win, 2, 15, "#wt");
	mvwaddstr(main_win, 2, 19, "Rct");
	mvwaddstr(main_win, 2, 23, "Swp");
	mvwaddstr(main_win, 2, 28, "Fsize");
	mvwaddstr(main_win, 2, 34, "Forw");
	mvwaddstr(main_win, 2, 39, "Back");
	mvwaddstr(main_win, 2, 45, "Ino");
	mvwaddstr(main_win, 2, 49, "Type");
	mvwaddstr(main_win, 2, 54, "Listaddr");
	mvwaddstr(main_win, 2, 63, "Flags");
	wattroff(main_win, A_BOLD);
	mvwaddstr(main_win, 3, 2, "                                                                        ");
	mvwaddstr(main_win, 16, 4, "                                                                       ");
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

draw_region_screen(int slot, int max_val)
{

register counter;
int pg_ctr;

	kmem_read(regions, namelist[NM_REGION].n_value,
		sizeof(struct region) * v.v_region);

	pg_ctr = 4;
	for (counter = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			pg_ctr = 4;
			return;
		} else {
			getregion(slot, pg_ctr);
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
 * | Function: void num_of_open_regions()                     Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Draw the header for regions                                 |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void num_of_open_regions(void)
{

int region_ctr  = 0;
struct region *tmp_regions;
int position = 0;
register i;

	/* get regions */
	tmp_regions = (struct region *) malloc(v.v_region * sizeof(struct region));
	kmem_read(tmp_regions, namelist[NM_REGION].n_value,
	    sizeof(struct region) * v.v_region);

	for (i = 0; i < v.v_region; i++) {
		if (tmp_regions[i].r_pgsz MATCHES)
			continue;
		region_ctr++;
	}
	free((char *) tmp_regions);

	position = sizeof(region_ctr);

	mvwprintw(bottom_win, 1, 57, "            ");
	wattron(bottom_win, A_BOLD);
	mvwprintw(bottom_win, 1, (62 - position), "Regions Open");
	wattroff(bottom_win, A_BOLD);
	mvwprintw(bottom_win, 1, (74 - position), ": %d",  region_ctr);
	mvwaddstr(bottom_win, 1, 28, " ");
	wnoutrefresh(bottom_win);
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

void watch_regions(int *main_slot, int max_val)
{

int get_value = 0;
int cmd = 0;
int slot = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;

	slot = *main_slot;

	naptime = Sleep_Time;

	wattron(main_win, A_REVERSE | A_BLINK);
	mvwaddstr(main_win, 0, 57, "Time");
	wattroff(main_win, A_REVERSE | A_BLINK);
	wnoutrefresh(top_win);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		draw_region_screen(slot, max_val);
		wnoutrefresh(main_win);
		an_option();
		num_of_open_regions();
		doupdate();

		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_regions) == SIG_ERR)
			printf("\nsignal(SIGALRM) error\n");

		alarm(naptime);	/* set the alarm timer */

		if ((cmd = wgetch(bottom_win)) < 0) {
			alarm(0);			/* stop the alarm timer */
		} else {
			alarm(0);			/* stop the alarm timer */
			switch(cmd) {
			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				break;

			case 'C' :
			case 'c' :
				change_time(FALSE);
				touchwin(main_win);
				drawdisp(26);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 57, "Time");
				wattroff(main_win, A_REVERSE | A_BLINK);
				doupdate();
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
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
				drawdisp(26);
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 49, "Lookup");
				wattroff(main_win, A_REVERSE);
				wrefresh(main_win);

				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				slot = get_value;

				mvwaddstr(main_win, 0, 49, "Lookup");
				wattron(main_win, A_REVERSE);
				mvwaddstr(main_win, 0, 49, "L");
				wattroff(main_win, A_REVERSE);
				wattron(main_win, A_REVERSE | A_BLINK);
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
				wattroff(main_win, A_REVERSE | A_BLINK);
				break;

			case 'I' :
			case 'i' :
				drawdisp(26);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 57, "Time");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				drawdisp(26);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 63, "Slow");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				drawdisp(26);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 68, "Med");
				wattroff(main_win, A_REVERSE | A_BLINK);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
			case 'f' :
				drawdisp(26);
				wattron(main_win, A_REVERSE | A_BLINK);
				mvwaddstr(main_win, 0, 72, "Fast");
				wattroff(main_win, A_REVERSE | A_BLINK);
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
 *	nothing to do, just return to interrupt the read.
 */

static void sig_alrm_regions(int signo)
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

