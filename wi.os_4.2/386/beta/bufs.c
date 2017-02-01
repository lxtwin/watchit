/*
 * +--------------------------------------------------------------------+
 * | Function: bufs.c                                    Date: %D% |
 * | Author: Paul Ready.                                              |
 * |                                                                    |
 * | Notes:                                                             |
 * |         gets buffer info.                                          |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 * (1) v2.1: This version has forward/backward movement, serches and
 *		 long/ short name listing.
 *
 *    Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%			Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%			Version %I% %D% - Dapix "
#endif /* __STDC__ */

#include "wi.h"

#include <sys/var.h>
#include <sys/buf.h>
#include <signal.h>

int	pg_ctr = FALSE;

long	b_ptr = 0L;

void wi_bufs(void);
void buf_header(void);
void getbuf(int slot, int pg_ctr, buf_t *bbuf);

int draw_buf_screen(int slot, int max_val);
void watch_bufs(int *main_slot, int max_val);
void size_of_buf_table(void);
static void sig_alrm_buffs(int signo);


/*
 * +--------------------------------------------------------------------+
 * | Function: void wi_bufs()                           Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Start point of the bufs.                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void wi_bufs(void)
{

int	get_value = FALSE;
int	slot = FALSE;
buf_t	bbuf;
register max_val = FALSE;
int option;

	Curent_Saved_Value = 0;
	kmem_read(&b_ptr, namelist[NM_BUFFER].n_value, sizeof(b_ptr));

	buf_header();
	pg_ctr = PAGE_COUNTER;

	max_val = v.v_buf;

	for (slot = 0; ;) {
		kmem_read(&bbuf, b_ptr + (slot * sizeof(bbuf)), sizeof(bbuf));
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			an_option();
			size_of_buf_table();
			doupdate();
			option = wgetch(bottom_win);
			switch (option) {
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
				wmove(bottom_win, 1, 35);
				wrefresh(bottom_win);
				nap(750);

				if (size_flag)
					if (slot <= 31)
						slot = FALSE;
					else
						slot -= 31;
				else
					if (slot <= 13)
						slot = FALSE;
					else
						slot -= 13;
				buf_header();
				break;

			case 'b' :
			case 'B' :
			case KEY_PPAGE:
				if (size_flag) {
					if (slot <= 61) {
						beep();
						slot = FALSE;
					} else
						slot -= 62;
				} else {
					if (slot <= 25) {
						beep();
						slot = FALSE;
					} else
						slot -= 26;
				}
				break;

			case 'f' :
			case 'F' :
			case KEY_NPAGE:
				if (size_flag) {
					if (slot >= (max_val - 30)) {
						slot = (max_val - 31);
						beep();
					}
				} else {
					if (slot >= (max_val - 12)) {
						slot = (max_val - 13);
						beep();
					}
				}
				break;

			case KEY_UP:
			case '-' :
				if (size_flag) {
					if (slot <= 32) {
						beep();
						slot = FALSE;
					} else
						slot -= 32;
				} else {
					if (slot <= 13) {
						beep();
						slot = FALSE;
					} else
						slot -= 14;
				}
				break;

			case KEY_DOWN:
			case '+' :
				if (slot == (max_val)) {
					if (size_flag)
						slot = (max_val - 31);
					else
						slot = (max_val - 13);
					beep();
				} else {
					if (size_flag) {
						if (slot <= 30)
							slot = FALSE;
						else
							slot -= 30;
					} else {
						if (slot <= 12)
							slot = FALSE;
						else
							slot -= 12;
					}
				}
				break;

			case 'w' :
			case 'W' :
				if (size_flag)
					if (slot <= 31)
						slot = FALSE;
					else
						slot -= 31;
				else
					if (slot <= 13)
						slot = FALSE;
					else
						slot -= 13;
				drawdisp(28);
				watch_bufs(&slot, max_val);
				drawdisp(4);
				break;

			case 'c' :
			case 'C' :
				Set_Colour(main_win, Colour_Banner);
				mvwaddstr(main_win, 0, 38, "Cache");
				Set_Colour(main_win, Normal);
				wrefresh(main_win);
				buf_cache();
				touchwin(main_win);
				drawdisp(4);
				wrefresh(main_win);
				if (size_flag)
					if (slot <= 31)
						slot = FALSE;
					else
						slot -= 31;
				else
					if (slot <= 13)
						slot = FALSE;
					else
						slot -= 13;
				break;

			case 's' :
			case 'S' :
				drawdisp(4);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 70, "Search");
				Set_Colour(main_win, Normal);
				wrefresh(main_win);
				get_value = FALSE;
				get_item(&get_value, 0, (max_val - 1));
				slot = get_value;
				drawdisp(4);
				wrefresh(main_win);
				break;

			case 'q' :
			case 'Q' :
				return;

			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				if (size_flag)
					if (slot <= 31)
						slot = FALSE;
					else
						slot -= 31;
				else
					if (slot <= 13)
						slot = FALSE;
					else
						slot -= 13;
				break;

			case 'P' :
			case 'p' :
				screen_dump();

			default :
				beep();
				if (size_flag)
					if (slot <= 31)
						slot = FALSE;
					else
						slot -= 31;
				else
					if (slot <= 13)
						slot = FALSE;
					else
						slot -= 13;
				break;

			}

			buf_header();
			pg_ctr = PAGE_COUNTER;
		} else {
			/* Fill up the page */
			getbuf(slot, pg_ctr, &bbuf);

			if (slot == (max_val)) {
				pg_ctr++;
				/* mvwaddstr(main_win, pg_ctr, 2, "                                                                      "); */
				slot = FALSE;
			} else
				slot++;
			pg_ctr++;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void getbuf(int slot, int pg_ctr)            Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Get and display the buffer info.                            |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void getbuf(int slot, int pg_ctr, buf_t *bbuf)
{

char	dev_name[32];

	dev_name[0] = '\0';
	wmove(main_win, pg_ctr, 1);
	w_clrtoeol(main_win, 0, 0);

	if (major_flag) {
		if ((bbuf->b_blkno MATCHES) && (minor(bbuf->b_dev) == 255))
			mvwprintw(main_win, pg_ctr, 2, " %4d  -   -  device mnt point %3x %x %d %d", 
			    slot, bbuf->b_blkno, bbuf->b_un.b_addr, 
			    bbuf->b_bcount, bbuf->b_error);
		else {
			if (check_for_dev(major(bbuf->b_dev) & 0377, minor(bbuf->b_dev), dev_name) != 0)
				print_dev(1, major(bbuf->b_dev) & 0377, minor(bbuf->b_dev), dev_name, 0);
			mvwprintw(main_win, pg_ctr, 2, " %4d %0.3d %0.3d /dev/%-8s %6x %x %d %d", 
			    slot, major(bbuf->b_dev) & 0377, minor(bbuf->b_dev), 
			    dev_name, bbuf->b_blkno, bbuf->b_un.b_addr, 
			    bbuf->b_bcount, bbuf->b_error);
		}
	} else {
		if ((bbuf->b_blkno MATCHES) && (minor(bbuf->b_dev) == 255))
			mvwprintw(main_win, pg_ctr, 2, " %4d  -   -  %6x %x %d %d", 
			    slot, bbuf->b_blkno, bbuf->b_un.b_addr, 
			    bbuf->b_bcount, bbuf->b_error);
		else
			mvwprintw(main_win, pg_ctr, 2, " %4d %0.3d %0.3d %6x %x %d %d", 
			    slot, major(bbuf->b_dev) & 0377, minor(bbuf->b_dev), 
			    bbuf->b_blkno, bbuf->b_un.b_addr, 
			    bbuf->b_bcount, bbuf->b_error);
	}
	if (bbuf->b_flags & B_READ) {
		waddstr(main_win, " read");
		if (bbuf->b_flags & B_WRITE)
			waddstr(main_win, " write");

	} else
		waddstr(main_win, " write");

	if (bbuf->b_flags & B_ERROR)
		if (major_flag)
			waddstr(main_win, " err");
		else
			waddstr(main_win, " error");

	if (bbuf->b_flags & B_BUSY)
		waddstr(main_win, " busy");

	if (bbuf->b_flags & B_PHYS)
		if (major_flag)
			waddstr(main_win, " phys");
		else
			waddstr(main_win, " physical");

	if (bbuf->b_flags & B_MAP)
		waddstr(main_win, " map");

	if (bbuf->b_flags & B_WANTED)
		waddstr(main_win, " wanted");

	if (bbuf->b_flags & B_AGE)
		waddstr(main_win, " age");

	if (bbuf->b_flags & B_ASYNC)
		waddstr(main_win, " async");

	if (bbuf->b_flags & B_DELWRI)
		if (major_flag)
			waddstr(main_win, " del/write");
		else
			waddstr(main_win, " delete/write");

	if (bbuf->b_flags & B_OPEN)
		waddstr(main_win, " open");

	if (bbuf->b_flags & B_STALE)
		waddstr(main_win, " stale");

	if (bbuf->b_flags & B_VERIFY)
		waddstr(main_win, " verify");

	if (bbuf->b_flags & B_FLUSH)
		waddstr(main_win, " flush");

	if (bbuf->b_flags & B_FORMAT)
		waddstr(main_win, " format");

	if (bbuf->b_flags & B_REMOTE)
		waddstr(main_win, " remote");

	if (bbuf->b_flags & B_S52K)
		waddstr(main_win, " 2k");

	if (bbuf->b_flags & B_PRIVLG)
		if (major_flag)
			waddstr(main_win, " priv");
		else
			waddstr(main_win, " privg");

	if (bbuf->b_flags & B_SEQ)
		waddstr(main_win, " seq");

	if (bbuf->b_flags & B_CLUSTER)
		if (major_flag)
			waddstr(main_win, " clust");
		else
			waddstr(main_win, " cluster");

	if (bbuf->b_flags & B_HASMEM)
		waddstr(main_win, " paddr val");

	if (bbuf->b_flags & B_AIO)
		waddstr(main_win, " a_i/o");

	if (bbuf->b_flags & B_RAWIO)
		waddstr(main_win, " raw_i/o");

	if (bbuf->b_flags & B_REMAP)
		waddstr(main_win, " remap");

	if (bbuf->b_flags & B_DONE)
		waddstr(main_win, " done");

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void buf_header()                         Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Draw the buffer header                                      |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void buf_header(void)
{

	Set_Colour(main_win, Normal);
/* Set_Colour(main_win, Colour_White); */
	wmove(main_win, 2, 3);
	w_clrtoeol(main_win, 0, 0);

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 2, 3, "Slot");
	if (major_flag) {
		mvwaddstr(main_win, 2, 8, "Maj");
		mvwaddstr(main_win, 2, 12, "Min");
		mvwaddstr(main_win, 2, 16, "Device name ");
		mvwaddstr(main_win, 2, 31, "Block ");
		mvwaddstr(main_win, 2, 38, "Address ");
		mvwaddstr(main_win, 2, 46, "Bcnt ");
		mvwaddstr(main_win, 2, 51, "E ");
		mvwaddstr(main_win, 2, 55, "Flags ");
	} else {
		mvwaddstr(main_win, 2, 8, "Maj");
		mvwaddstr(main_win, 2, 12, "Min");
		mvwaddstr(main_win, 2, 17, "Block");
		mvwaddstr(main_win, 2, 24, "Address");
		mvwaddstr(main_win, 2, 32, "Bcnt");
		mvwaddstr(main_win, 2, 37, "E");
		mvwaddstr(main_win, 2, 41, "Flags");
	}
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

int draw_buf_screen(int slot, int max_val)
{

buf_t	bbuf;
register counter = FALSE;
int pg_ctr = FALSE;

	buf_header();
	kmem_read(&bbuf, b_ptr + (slot * sizeof(bbuf)), sizeof(bbuf));

	pg_ctr = PAGE_COUNTER;
	for (counter = 0; ;) {
		kmem_read(&bbuf, b_ptr + (slot * sizeof(bbuf)), sizeof(bbuf));
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			return(0);
		} else {
			getbuf(slot, pg_ctr, &bbuf);
			if (slot == (max_val)) {
				pg_ctr++;
				/* mvwaddstr(main_win, pg_ctr, 2, "                                                                      ");*/
				slot = FALSE;
			} else
				slot++;
			pg_ctr++;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void size_of_buf_table(void)              Date: %Z% |
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


void size_of_buf_table()
{

int position = 0;

	position = sizeof(v.v_buf);

	Set_Colour(bottom_win, Normal);
	mvwprintw(bottom_win, 1, 57, "            ");
	Set_Colour(bottom_win, Colour_White);
	mvwprintw(bottom_win, 1, (69 - position), "NBUF");
	Set_Colour(bottom_win, Normal);
	mvwprintw(bottom_win, 1, (73 - position), ": %d",  v.v_buf);
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

void watch_bufs(int *main_slot, int max_val)
{

int get_value = 0;
int cmd = 0;
int slot = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;
int ffast = FALSE;


	buf_header();
	slot = *main_slot;
	naptime = Sleep_Time;

	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);
	wnoutrefresh(top_win);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		draw_buf_screen(slot, max_val);
		wnoutrefresh(main_win);
		an_option();
		size_of_buf_table();
		doupdate();

		/*
			Set up the signal handler
		*/
		cmd = 0;
		if (ffast == TRUE) {
			nap(400);
			if (rdchk(0))
				cmd = wgetch(bottom_win);
		} else {
			if (signal(SIGALRM, sig_alrm_buffs) == SIG_ERR)
				printf("\nsignal(SIGALRM) error\n");

			alarm(naptime);	/* set the alarm timer */
			if ((cmd = wgetch(bottom_win)) < 0) {
				alarm(0);			/* stop the alarm timer */
			} else {
				alarm(0);			/* stop the alarm timer */
			}
		}

		if (cmd != 0) {
			switch(cmd) {
			case -1 :
				break;

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
				wmove(bottom_win, 1, 35);
				wrefresh(bottom_win);
				nap(750);
				break;

			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				break;

			case 'C' :
			case 'c' :
				ffast = FALSE;
				change_time(FALSE);
				touchwin(main_win);
				drawdisp(28);
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
				drawdisp(28);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 49, "Lookup");
				Set_Colour(main_win, Normal);
				wrefresh(main_win);

				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				slot = get_value;

				mvwaddstr(main_win, 0, 49, "Lookup");
				Set_Colour(main_win, Colour_Banner);
				mvwaddstr(main_win, 0, 49, "L");
				Set_Colour(main_win, Blink_Banner);
				if (Sleep_Time_Flag) {
					mvwaddstr(main_win, 0, 57, "Time");
				} else {
					if (naptime == 4)
						mvwaddstr(main_win, 0, 63, "Slow");
					else if (naptime == 2)
						mvwaddstr(main_win, 0, 68, "Med");
					else if (naptime == 1)
						mvwaddstr(main_win, 0, 72, "Fast");
					else {
						Set_Colour(main_win, Red_Blink_Banner);
						mvwaddstr(main_win, 0, 72, "Fast");
					}
				}
				Set_Colour(main_win, Normal);
				break;

			case 'I' :
			case 'i' :
				ffast = FALSE;
				drawdisp(28);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				ffast = FALSE;
				drawdisp(28);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				ffast = FALSE;
				drawdisp(28);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
				ffast = TRUE;
				naptime = -1;
				major_flag = 0;
				drawdisp(28);
				Set_Colour(main_win, Red_Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				Sleep_Time_Flag = FALSE;
				break;

			case 'f' :
				ffast = FALSE;
				drawdisp(28);
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
 *	nothing to do, just return to interrupt the read.
 */

static void sig_alrm_buffs(int signo)
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

