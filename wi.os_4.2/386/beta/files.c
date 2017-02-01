/*
 * +--------------------------------------------------------------------+
 * | Function: files.c                                   Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the file info.                                         |
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

#include "wi.h"

#include <sys/var.h>
#include <sys/file.h>
#include <sys/inode.h>
#include	<signal.h>		/* The signals and handler */

file_t *files;			/* kernel file structre		*/

int naptime;

void wi_files(void);
void draw_file_screen(int slot, int max_val);
void watch_files(int *main_slot, int max_val);
void getfile(int i, int pg_ctr);
void file_header(void);
void num_of_open_files(void);
static void sig_alrm_files(int signo);

/*
 * +--------------------------------------------------------------------+
 * | Function: wi_files(void)                                Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        get the file list                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int    pg_ctr;

void wi_files()
{

int	i = 0;
int	get_value = 0;
register max_val;

	files = (file_t *) malloc (v.v_file * sizeof(file_t));
	kmem_read(files, namelist[NM_FILE].n_value, sizeof(file_t) * v.v_file);

	file_header();

	max_val = v.v_file;
	pg_ctr = 4;
	for (i = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			an_option();
			num_of_open_files();
			wnoutrefresh(bottom_win);
			doupdate();
			switch(wgetch(bottom_win)) {
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
				drawdisp(24);
				watch_files(&i, max_val);
				drawdisp(6);
				break;

			case 's' :
			case 'S' :
				drawdisp(6);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 70, "Search");
				Set_Colour(main_win, Normal);
				wrefresh(main_win);
				get_value = 0;
				get_item(&get_value, 0, (max_val - 1));
				i = get_value;
				drawdisp(6);
				wrefresh(main_win);
				break;

			case 'q' :
			case 'Q' :
				free((char *) files);
				return;

			case 'u' :
			case 'U' :
				kmem_read(files, namelist[NM_FILE].n_value,
				    sizeof(file_t) * v.v_file);
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
			file_header();
			pg_ctr = 4;
		} else {
			getfile(i, pg_ctr);
			if (i == (max_val)) {
				pg_ctr++;
				/* mvwaddstr(main_win, pg_ctr, 2, "                                                                      "); */
				i = 0;
			} else
				i++;
			pg_ctr++;
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

void draw_file_screen(int slot, int max_val)
{

register counter;
int pg_ctr;

	kmem_read(files, namelist[NM_FILE].n_value, sizeof(file_t) * v.v_file);

	pg_ctr = 4;
	for (counter = 0; ;) {
		if (pg_ctr == page_len) {
			wnoutrefresh(main_win);
			return;
		} else {
			getfile(slot, pg_ctr);
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
 * | Function: getfile(int i, int pg_ctr);               Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displayes the file info.                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */


void getfile(int i, int pg_ctr)
{

	wmove(main_win, pg_ctr, 1);
	w_clrtoeol(main_win, 0, 0);

	if (files[i].f_count MATCHES || files[i].f_flag MATCHES) {
		mvwprintw(main_win, pg_ctr, 4, "%4d - ", i);
		Set_Colour(main_win, Text_Warning);
		waddstr(main_win, "Free");
		Set_Colour(main_win, Normal);
		return;
	}
	mvwprintw(main_win,pg_ctr, 4, "%4d %5d   %6d   %10lx  %8d   %03o   %s%s%s%s%s%s%s%s",
	    i,
	    files[i].f_count,
	    files[i].f_inode - (inode_t *) namelist[NM_INODE].n_value,
	    files[i].f_offset,
	    files[i].f_up.f_unext - (file_t *) namelist[NM_FILE].n_value,
	    files[i].f_flag & FMASK,
	    (files[i].f_flag & FREAD)   ? "read ": "",
	    (files[i].f_flag & FWRITE)  ? "write ": "",
	    (files[i].f_flag & FNDELAY) ? "no_delay ": "",
	    (files[i].f_flag & FAPPEND) ? "append ": "",
	    (files[i].f_flag & FSYNC)   ? "sync ": "",
	    (files[i].f_flag & FCREAT)  ? "create ": "",
	    (files[i].f_flag & FTRUNC)  ? "trunc ": "",
	    (files[i].f_flag & FEXCL)   ? "excl ": "");
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function: file_header(void)                             Date: %Z% |
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

void file_header(void)
{

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 2, 5, "Slot");
	mvwaddstr(main_win, 2, 11, "Count");
	mvwaddstr(main_win, 2, 18, "Inode");
	mvwaddstr(main_win, 2, 25, "Byte offset");
	mvwaddstr(main_win, 2, 38, "Freelist");
	mvwaddstr(main_win, 2, 48, "Umask");
	mvwaddstr(main_win, 2, 55, "Flags");
	Set_Colour(main_win, Normal);
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function: void num_of_open_files(void)                   Date: %Z% |
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


void num_of_open_files()
{

int file_ctr  = 0;
file_t *tmp_files;
register i;

int position = 0;

	/* get file count */
	tmp_files = (file_t *) malloc(v.v_file * sizeof(file_t));
	kmem_read(tmp_files, namelist[NM_FILE].n_value,
	    sizeof(file_t) * v.v_file);

	for (i = 0; i < v.v_file; i++) {
		if (tmp_files[i].f_count MATCHES || files[i].f_flag MATCHES)
			continue;
		file_ctr++;
	}
	free((char *) tmp_files);

	position = sizeof(file_ctr);

	mvwprintw(bottom_win, 1, 57, "            ");
	Set_Colour(bottom_win, Colour_White);
	mvwprintw(bottom_win, 1, (64 - position), "Files Open");
	Set_Colour(bottom_win, Normal);
	mvwprintw(bottom_win, 1, (74 - position), ": %d",  file_ctr);
	mvwprintw(bottom_win, 1, 28, " ");
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

void watch_files(int *main_slot, int max_val)
{

int get_value = 0;
int cmd = 0;
int slot = 0;
int Sleep_Time_Flag = TRUE;

int ffast = FALSE;

	slot = *main_slot;

	naptime = Sleep_Time;

	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);
	wnoutrefresh(top_win);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		draw_file_screen(slot, max_val);
		wnoutrefresh(main_win);
		an_option();
		num_of_open_files();
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
			if (signal(SIGALRM, sig_alrm_files) == SIG_ERR)
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

			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				break;

			case 'C' :
			case 'c' :
				ffast = FALSE;
				change_time(FALSE);
				touchwin(main_win);
				drawdisp(24);
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
				drawdisp(24);
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
				drawdisp(24);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				ffast = FALSE;
				drawdisp(24);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				ffast = FALSE;
				drawdisp(24);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
				ffast = TRUE;
				naptime = -1;
				drawdisp(24);
				Set_Colour(main_win, Red_Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				Sleep_Time_Flag = FALSE;
				break;

			case 'f' :
				ffast = FALSE;
				drawdisp(24);
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

static void sig_alrm_files(int signo)
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
