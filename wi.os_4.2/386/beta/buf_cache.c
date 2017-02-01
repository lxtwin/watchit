/*
 * +--------------------------------------------------------------------+
 * | Function: buf_cache.c                               Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the buf_cache stats.                                   |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *(1) 2.3: I have added a new graphics mode which allows the abillaity
 *		to Console Capture the running program, I think it looks good
 *		but I don't know about any one else.
 *						Thu Aug 19 00:34:25 EDT 1993 - PKR.
 *
 * (2) 2.3:	Added a structure in wi.h for current avg, avg/boot and 
 *		avg/wi start.
 *						Sat Aug 21 00:07:09 EDT 1993 - PKR.
 * (3) 2.3:	Taken out the excess bar chart stuff, and made a function in
 *		odds+sods.c.
 *						Sat Aug 21 14:25:21 EDT 1993 - PKR.
 *    Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */

#include "wi.h"
#include <sys/sysinfo.h>
#include <signal.h>

#define BAR_CHART 3

void buf_cache(void);
void dsk_cache(void);
void setup_cache(void);
void end_cache(void);
void watch_cache(void);
void get_cache_stats(void);
void update_cache_stats(void);
void draw_cache_screen(void);
void cache_header(void);
void disk_cache_header(void);
void update_cache_display(void);
void Draw_The_Mode(void);
static void sig_alrm_buf_cache(int signo);

int	extra_bit1 = FALSE;
int	extra_bit2 = FALSE;
int	extra_bit3 = FALSE;

long	s5hits	  = FALSE;			/* kernel cach hits		*/
long	last_s5hits   = FALSE;			/* kernel last cach hits	*/
long	s5misses	  = FALSE;			/* kernel cach misses 		*/
long	last_s5misses = FALSE;			/* kernel last cach misses	*/

/*
 * +--------------------------------------------------------------------+
 * | Function: void buf_cache()                          Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Do the buf_cache.c                                          |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void buf_cache(void)
{

	setup_cache();
	cache_header();
	mvwaddstr(cache_win, size_flag + 1, 12, "Reading /unix..........");
	wnoutrefresh(cache_win);
	doupdate();
	mvwaddstr(cache_win, size_flag + 1, 12, "                         ");
	watch_cache();
	end_cache();
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void dsk_cache()                          Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Do the buf_cache.c                                          |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void dsk_cache(void)
{
	setup_cache();
	disk_cache_header();
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void setup_cache()                        Date: %Z% |
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

void setup_cache(void)
{

	extra_bit1 = FALSE;
	extra_bit2 = FALSE;
	extra_bit3 = FALSE;

	if (size_flag)
		cache_win = newwin(16, 75, 23, 2);
	else {
		extra_bit1 = -1;
		extra_bit2 = -2;
		extra_bit3 = -3;
		cache_win = newwin(12, 75, 9, 2);
	}

	Set_Colour(cache_win, Normal);
	Fill_A_Box(cache_win, 0, 0);
	Draw_A_Box(cache_win, BUTTON_BOX);
	draw_cache_screen();
	wnoutrefresh(cache_win);
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function: void draw_cache_screen()                  Date: %Z% |
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

void draw_cache_screen(void)
{

	Set_Colour(cache_win, Normal);
	mvwaddstr(cache_win, BAR_CHART + 1 + extra_bit1, 3, "%rcache");
	mvwaddstr(cache_win, BAR_CHART + 4 + extra_bit1, 3, "%wcache");
	mvwaddstr(cache_win, BAR_CHART + 10 + extra_bit3, 3, "namei()");

	mvwaddstr(cache_win, BAR_CHART + 7 + extra_bit2, 12, "S5cache hits");
	mvwaddstr(cache_win, BAR_CHART + 7 + extra_bit2, 41, "S5cache misses");
	Set_Colour(cache_win, Normal);

	mvwaddstr(cache_win, BAR_CHART + 7 + extra_bit2, 24, ":");
	mvwaddstr(cache_win, BAR_CHART + 7 + extra_bit2, 55, ":");

	Draw_The_Mode();

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void Draw_The_Mode()                      Date: %Z% |
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

void Draw_The_Mode(void)
{

extern int The_Current_Avg;

	switch(The_Current_Avg) {
		case CURRENT_AVG:
			mvwprintw(cache_win, BAR_CHART + extra_bit1, 12, "%s  current ",    SCALE);
			mvwprintw(cache_win, BAR_CHART + extra_bit1 + 3, 12, "%s  current ", SCALE);
			mvwprintw(cache_win, BAR_CHART + extra_bit3 + 9, 12, "%s  current ", SCALE);
			break;

		case BOOT_AVG:
			mvwprintw(cache_win, BAR_CHART + extra_bit1, 12, "%s  sys boot",    SCALE);
			mvwprintw(cache_win, BAR_CHART + extra_bit1 + 3, 12, "%s  sys boot", SCALE);
			mvwprintw(cache_win, BAR_CHART + extra_bit3 + 9, 12, "%s  sys boot", SCALE);
			break;

		case WATCHIT_AVG:
			mvwprintw(cache_win, BAR_CHART + extra_bit1, 12, "%s  wi start",    SCALE);
			mvwprintw(cache_win, BAR_CHART + extra_bit1 + 3, 12, "%s  wi start", SCALE);
			mvwprintw(cache_win, BAR_CHART + extra_bit3 + 9, 12, "%s  wi start", SCALE);
			break;
	}
	return;
 }


/*
 * +--------------------------------------------------------------------+
 * | Function: void end_cache()                          Date: %Z% |
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

void end_cache(void)
{
	delwin(cache_win);
	touchwin(main_win);
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

void get_cache_stats(void)
{
	kmem_read(&sysinfo, namelist[NM_SYSINFO].n_value,
	    sizeof(struct sysinfo));

	kmem_read(&s5hits, namelist[NM_S5CACHEHITS].n_value,
	    sizeof(s5hits));

	kmem_read(&s5misses, namelist[NM_S5CACHEMISSES].n_value,
	    sizeof(s5misses));
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

void update_cache_stats(void)
{
	kmem_read(&last_sysinfo, namelist[NM_SYSINFO].n_value, 
	    sizeof(struct sysinfo));

	kmem_read(&last_s5hits, namelist[NM_S5CACHEHITS].n_value, 
	    sizeof(last_s5hits));

	kmem_read(&last_s5misses, namelist[NM_S5CACHEMISSES].n_value, 
	    sizeof(last_s5misses));
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the cache header                                   |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void disk_cache_header(void)
{
	Draw_A_Box(cache_win, BUTTON_BOX);
	Set_Colour(cache_win, Colour_Banner);
	mvwaddstr(cache_win, 0, 2, "Cache Statistics");
	Set_Colour(cache_win, Normal);
	wnoutrefresh(cache_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the cache header                                   |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void cache_header(void)
{

	Set_Colour(cache_win, Normal);
	Draw_A_Box(cache_win, BUTTON_BOX);
	mvwaddstr(cache_win, 0, 48, "Type");
	mvwaddstr(cache_win, 0, 54, "Time");
	mvwaddstr(cache_win, 0, 60, "Slow");
	mvwaddstr(cache_win, 0, 65, "Med");
	mvwaddstr(cache_win, 0, 69, "Fast");
	Set_Colour(cache_win, Colour_Banner);
	mvwaddstr(cache_win, 0, 2, "Cache Statistics");
	mvwaddstr(cache_win, 0, 48, "T");
	mvwaddstr(cache_win, 0, 55, "i");
	mvwaddstr(cache_win, 0, 60, "S");
	mvwaddstr(cache_win, 0, 65, "M");
	mvwaddstr(cache_win, 0, 69, "F");
	Set_Colour(cache_win, Normal);
	wnoutrefresh(cache_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void update_cache_display()               Date: %Z% |
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

void update_cache_display(void)
{

char bar_chart[64];

unsigned long  bread = 0L,  lread = 0L;
unsigned long bwrite = 0L,  lwrite = 0L;

int	percent_read = FALSE;
int	percent_write = FALSE;

float	percent_cache = FALSE;

long	total_hits	 = 0L;
long	total_misses = 0L;
long	total_hits_and_misses = 0L;

	bar_chart[0] = '\0';

	switch(The_Current_Avg) {
		case CURRENT_AVG:
			total_hits = last_s5hits - s5hits;
			total_misses = last_s5misses - s5misses;

			lread = (unsigned long)last_sysinfo.lread - (unsigned long)sysinfo.lread;
			bread = (unsigned long)last_sysinfo.bread - (unsigned long)sysinfo.bread;
			lwrite = (unsigned long)last_sysinfo.lwrite - (unsigned long)sysinfo.lwrite;
			bwrite = (unsigned long)last_sysinfo.bwrite - (unsigned long)sysinfo.bwrite;

			mvwprintw(cache_win, BAR_CHART + 7 + extra_bit2, 25, "%7d       ",
				last_s5hits - s5hits);
			mvwprintw(cache_win, BAR_CHART + 7 + extra_bit2, 56, "%6d       ",
				last_s5misses - s5misses);
			break;

		case BOOT_AVG:
			total_hits = last_s5hits;
			total_misses = last_s5misses;

			lread = (unsigned long)last_sysinfo.lread;
			bread = (unsigned long)last_sysinfo.bread;
			lwrite = (unsigned long)last_sysinfo.lwrite;
			bwrite = (unsigned long)last_sysinfo.bwrite;

			mvwprintw(cache_win, BAR_CHART + 7 + extra_bit2, 25, " %12-d",
				last_s5hits);
			mvwprintw(cache_win, BAR_CHART + 7 + extra_bit2, 56, " %12-d",
				last_s5misses);
			break;

		case WATCHIT_AVG:
			total_hits = last_s5hits - wi_start.wi_s5hits;
			total_misses = last_s5misses - wi_start.wi_s5misses;

			lread = (unsigned long)last_sysinfo.lread - (unsigned long)wi_start.wi_sysinfo.lread;
			bread = (unsigned long)last_sysinfo.bread - (unsigned long)wi_start.wi_sysinfo.bread;
			lwrite = (unsigned long)last_sysinfo.lwrite - (unsigned long)wi_start.wi_sysinfo.lwrite;
			bwrite = (unsigned long)last_sysinfo.bwrite - (unsigned long)wi_start.wi_sysinfo.bwrite;

			mvwprintw(cache_win, BAR_CHART + 7 + extra_bit2, 25, " %12-d",
				last_s5hits - wi_start.wi_s5hits);
			mvwprintw(cache_win, BAR_CHART + 7 + extra_bit2, 56, " %12-d",
				last_s5misses - wi_start.wi_s5misses);
			break;
	}

	if (lread MATCHES)
		lread = 1;
	percent_read = ((lread == bread) ? 100: 100 - ((bread*100)/lread));
	if (percent_read <= 0)
		percent_read = 0;

	if (lwrite MATCHES)
		lwrite = 1;
	percent_write = ((lwrite == bwrite) ? 100: 100 - ((bwrite*100)/lwrite));
	if (percent_write <= 0)
		percent_write = 0;

	total_hits_and_misses = total_hits + total_misses;
	if (total_hits_and_misses)
		percent_cache = ((float) total_hits / total_hits_and_misses) * 100;
	else
		percent_cache = 100;
	if (percent_cache <= 0)
		percent_cache = 0;

	mvwaddstr(cache_win, BAR_CHART + 1 + extra_bit1, 66, "    %");
	mvwaddstr(cache_win, BAR_CHART + 4 + extra_bit1, 66, "    %");
	mvwaddstr(cache_win, BAR_CHART + 10 + extra_bit3, 66, "    %");

	Draw_The_Text(cache_win, percent_read, BAR_CHART + 1 + extra_bit1, 66, CACHE_BAR);

	Draw_The_Text(cache_win, percent_write, BAR_CHART + 4 + extra_bit1, 66, CACHE_BAR);

	Draw_The_Text(cache_win, (int) percent_cache, BAR_CHART + 10 + extra_bit3, 66, CACHE_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                      CACHE % READ                                  |
 * +--------------------------------------------------------------------+
 */
	Draw_A_Bar(cache_win, percent_read, BAR_CHART + 1 + extra_bit1, 12, CACHE_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                      CACHE % WRITE                                 |
 * +--------------------------------------------------------------------+
 */
	Draw_A_Bar(cache_win, percent_write, BAR_CHART + 4 + extra_bit1, 12, CACHE_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                      % CACHE                                       |
 * +--------------------------------------------------------------------+
 */
	/*
		force this to be an int from a float
	*/
	Draw_A_Bar(cache_win, (int) percent_cache, BAR_CHART + 10 + extra_bit3, 12, CACHE_BAR);

	wnoutrefresh(cache_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: watch_cache(void)                         Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the tty header.                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void watch_cache(void)
{

int cmd = FALSE;
int naptime = FALSE;
int Sleep_Time_Flag = TRUE;
int ffast = FALSE;


	get_cache_stats();
	nap(1000);
	update_cache_stats();
	update_cache_display();

	naptime = Sleep_Time;

	Set_Colour(cache_win, Blink_Banner);
	mvwaddstr(cache_win, 0, 54, "Time");
	Set_Colour(cache_win, Normal);
	wnoutrefresh(top_win);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		wnoutrefresh(cache_win);
		an_option();
		doupdate();
		get_cache_stats();

		/*
			Set up the signal handler
		*/
		cmd = 0;
		if (ffast == TRUE) {
			nap(400);
			if (rdchk(0))
				cmd = wgetch(bottom_win);
			else {
				update_cache_stats();
				update_cache_display();
				wnoutrefresh(cache_win);
				doupdate();
			}
		} else {
			if (signal(SIGALRM, sig_alrm_buf_cache) == SIG_ERR)
				printf("\nsignal(SIGALRM) error\n");

			alarm(naptime);	/* set the alarm timer */
			if ((cmd = wgetch(bottom_win)) < 0) {
				alarm(0);			/* stop the alarm timer */
				update_cache_stats();
				update_cache_display();
				wnoutrefresh(cache_win);
				doupdate();
			} else
				alarm(0);			/* stop the alarm timer */
		}

		if (cmd != 0) {
			switch(cmd) {
			case -1 :
				break;

			case 't' :
			case 'T' :
				cache_header();
				Set_Colour(cache_win, Blink_Banner);
				mvwaddstr(cache_win, 0, 48, "Type");
				Set_Colour(cache_win, Normal);
				wnoutrefresh(cache_win);
				Change_Sample_Type();
				touchwin(cache_win);
				cache_header();
				Set_Colour(cache_win, Blink_Banner);
				if (Sleep_Time_Flag) {
					mvwaddstr(cache_win, 0, 54, "Time");
				} else {
					if (naptime == 4)
						mvwaddstr(cache_win, 0, 60, "Slow");
					else if (naptime == 2)
						mvwaddstr(cache_win, 0, 65, "Med");
					else
						mvwaddstr(cache_win, 0, 69, "Fast");
				}
				Set_Colour(cache_win, Normal);
				wnoutrefresh(cache_win);
				get_cache_stats();
				nap(500);
				update_cache_stats();
				update_cache_display();
				doupdate();
				break;

			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				touchwin(cache_win);
				doupdate();
				break;

			case 'C' :
			case 'c' :
				ffast = FALSE;
				change_time(FALSE);
				touchwin(cache_win);
				cache_header();
				Set_Colour(cache_win, Blink_Banner);
				mvwaddstr(cache_win, 0, 54, "Time");
				Set_Colour(cache_win, Normal);
				doupdate();
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'P' :
			case 'p' :
				get_cache_stats();
				screen_dump();
				update_cache_stats();
				update_cache_display();
				break;

			case 'I' :
			case 'i' :
				ffast = FALSE;
				cache_header();
				Set_Colour(cache_win, Blink_Banner);
				mvwaddstr(cache_win, 0, 54, "Time");
				Set_Colour(cache_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				get_cache_stats();
				nap(500);
				update_cache_stats();
				update_cache_display();
				break;

			case 'S' :
			case 's' :
				ffast = FALSE;
				cache_header();
				Set_Colour(cache_win, Blink_Banner);
				mvwaddstr(cache_win, 0, 60, "Slow");
				Set_Colour(cache_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				ffast = FALSE;
				cache_header();
				Set_Colour(cache_win, Blink_Banner);
				mvwaddstr(cache_win, 0, 65, "Med");
				Set_Colour(cache_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
				ffast = TRUE;
				cache_header();
				Set_Colour(cache_win, Red_Blink_Banner);
				mvwaddstr(cache_win, 0, 69, "Fast");
				Set_Colour(cache_win, Normal);
				Sleep_Time_Flag = FALSE;
				break;

			case 'f' :
				ffast = FALSE;
				cache_header();
				Set_Colour(cache_win, Blink_Banner);
				mvwaddstr(cache_win, 0, 69, "Fast");
				Set_Colour(cache_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				break;

			case 'B' :
				The_Current_Avg = BOOT_AVG;
				Draw_The_Mode();
				get_cache_stats();
				nap(500);
				update_cache_stats();
				update_cache_display();
				break;

			case 'W' :
				The_Current_Avg = WATCHIT_AVG;
				Draw_The_Mode();
				get_cache_stats();
				nap(500);
				update_cache_stats();
				update_cache_display();
				break;

			case 'A' :
			case 'a' :
				The_Current_Avg = CURRENT_AVG;
				Draw_The_Mode();
				get_cache_stats();
				nap(500);
				update_cache_stats();
				update_cache_display();
				break;

			case KEY_UP:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
 				Get_The_Avg();
				Draw_The_Mode();
				doupdate();
				get_cache_stats();
				nap(500);
				update_cache_stats();
				update_cache_display();
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
 *	nothing to do, just return to interrupt the read.
 */

static void sig_alrm_buf_cache(int signo)
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

