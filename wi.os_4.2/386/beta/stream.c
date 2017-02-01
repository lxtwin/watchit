/*
 * +--------------------------------------------------------------------+
 * | Function: streams.c                                 Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the streams info.                                      |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *(1) 2.3: Graphics, and all that good stuff.
 *							Sun Aug 22 13:16:36 EDT 1993 - PKR.
 * Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix"
#endif /* __STDC__ */


#include "wi.h"

#include <sys/var.h>
#include <sys/ascii.h>
#include <sys/param.h>

#include <sys/param.h>
#include <sys/stream.h>
#include <sys/strstat.h>
#include <signal.h>

struct strstat	strst;

ushort rbsize[NCLASS];
unsigned stream_size[NCLASS + 4];

#define _NUM_STREAMS	0
#define _NUM_QUEUES	1
#define _NUM_MESSAGES	2
#define _NUM_NBLK		3
#define _NUM_NBLK4	4
#define _NUM_NBLK16	5
#define _NUM_NBLK64	6
#define _NUM_NBLK128	7
#define _NUM_NBLK256	8
#define _NUM_NBLK512	9
#define _NUM_NBLK1024	10
#define _NUM_NBLK2048	11
#define _NUM_NBLK4096	12


void no_streams(void);
void get_streams(void);
void watch_streams(void);
void stream_stats(void);
void draw_stream_header(void);
void update_streams(void);
int check_for_streams(void);
void extra_streams(void);
static void sig_alrm_streams(int signo);
void Draw_stream_screen(int Sleep_Time_Flag);

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
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

void stream_stats(void)
{

int	i = 0;

	if (check_for_streams())
		get_streams();
	else {
		no_streams();
		return;
	}

	get_streams();
	draw_stream_header();
	update_streams();
	wnoutrefresh(main_win);

	if (size_flag)
		extra_streams();

	doupdate();

	for (i = 0; ;) {
		an_option();
		doupdate();

		switch(wgetch(bottom_win)) {
		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			break;

		case 'P' :
		case 'p' :
			screen_dump();
			break;

		case 'q' :
		case 'Q' :
			Draw_stream_screen(FALSE);
			return;

		case 'u' :
		case 'U' :
			update_streams();
			wrefresh(main_win);
			break;

		case 'w' :
		case 'W' :
			drawdisp(22);
			watch_streams();
			drawdisp(23);
			wrefresh(main_win);
			break;

		default :
			beep();
			break;
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

int check_for_streams(void)
{

	if (namelist[NM_STRST].n_value)
		return(1);
	else
		return(0);
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

void no_streams(void)
{

	Set_Colour(bottom_win, Text_Warning);
	mvwaddstr(bottom_win, 1, 2, "Sorry streams is not installed on your system");
	Set_Colour(bottom_win, Normal);
	w_clrtoeol(bottom_win, 0, 0);
	wmove(bottom_win, 1, 49);
	wrefresh(bottom_win);
	wgetch(bottom_win);

}

/*
 * +--------------------------------------------------------------------+
 * | Function: update_streams()                          Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
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

void update_streams(void)
{

int extra_bit;
int percent_used, percent_max;
int i, x_pos, y_pos;
alcdat	*strdat;

	y_pos = 3;
	x_pos = 23;

	extra_bit = 0;

	kmem_read((caddr_t) &strst,  namelist[NM_STRST].n_value,
	    sizeof(struct strstat));

	if (size_flag)
		extra_bit = 1;
	strdat = (alcdat *)&strst;
	for (i = 0; i < NCLASS + 4; i++) {
		if (i == 4)
			if (!(size_flag))
				extra_bit = 1;

		percent_used = stream_size[i] ? (strdat->use * 100 / stream_size[i]) : 0;
		percent_max = stream_size[i] ? (strdat->max *100 / stream_size[i]) : 0;

		mvwprintw(main_win, y_pos + i + extra_bit, 0 + x_pos, "%6d %6d %6d",
			stream_size[i], strdat->use, stream_size[i] - strdat->use);

		Draw_The_Text(main_win, (100 - percent_used), y_pos + i + extra_bit, 22 + x_pos, STREAM_BAR);
		waddstr(main_win, "%%");

		mvwprintw(main_win, y_pos + i + extra_bit, 26 + x_pos, "%10d %5d",
			strdat->total, strdat->max);

		Draw_The_Text(main_win, (100 - percent_max), y_pos + i + extra_bit, 44 + x_pos, STREAM_BAR);
		waddstr(main_win, "%%");

		if (strdat->fail)
			Set_Colour(main_win, Text_Warning);
		else
			Set_Colour(main_win, Text_Normal);

		mvwprintw(main_win, y_pos + i + extra_bit, 48 + x_pos, "%5d", strdat->fail);
		Set_Colour(main_win, Normal);

		strdat++;
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: get_streams()                             Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
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

void get_streams(void)
{

int nmblock, i;

	kmem_read((caddr_t) rbsize,  namelist[NM_RBSIZE].n_value,
	    sizeof(ushort) * NCLASS);

	stream_size[_NUM_STREAMS] = v.v_nstream;
	stream_size[_NUM_QUEUES] = v.v_nqueue;

	kmem_read((caddr_t) &nmblock,  namelist[NM_NMBLOCK].n_value, sizeof(int));

	stream_size[_NUM_MESSAGES] = nmblock;
	stream_size[_NUM_NBLK4]    = v.v_nblk4;
	stream_size[_NUM_NBLK16]   = v.v_nblk16;
	stream_size[_NUM_NBLK64]   = v.v_nblk64;
	stream_size[_NUM_NBLK128]  = v.v_nblk128;
	stream_size[_NUM_NBLK256]  = v.v_nblk256;
	stream_size[_NUM_NBLK512]  = v.v_nblk512;
	stream_size[_NUM_NBLK1024] = v.v_nblk1024;
	stream_size[_NUM_NBLK2048] = v.v_nblk2048;
	stream_size[_NUM_NBLK4096] = v.v_nblk4096;
	stream_size[_NUM_NBLK] = 0;

	for (i = _NUM_NBLK4; i <= _NUM_NBLK4096; i++)
		stream_size[_NUM_NBLK] += stream_size[i];
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
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

void draw_stream_header(void)
{

int i, y_pos, x_pos;
int extra_bit;

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 2, 8, "Resource");
	mvwaddstr(main_win, 2, 23, "Config");
	mvwaddstr(main_win, 2, 31, "Alloc");
	mvwaddstr(main_win, 2, 39, "Free");
	mvwaddstr(main_win, 2, 45, "%Free");
	mvwaddstr(main_win, 2, 54, "Total");
	mvwaddstr(main_win, 2, 62, "Max");
	mvwaddstr(main_win, 2, 67, "%Max");
	mvwaddstr(main_win, 2, 72, "Fail");
	Set_Colour(main_win, Normal);

	x_pos = y_pos = 3;
	extra_bit = 0;
	if (size_flag)
		extra_bit = 1;

	mvwaddstr(main_win, y_pos + extra_bit, x_pos, "Streams");
	mvwaddstr(main_win, y_pos + 1 + extra_bit, x_pos, "Queues");
	mvwaddstr(main_win, y_pos + 2 + extra_bit, x_pos, "Message blocks");
	mvwaddstr(main_win, y_pos + 3 + extra_bit, x_pos, "Data blocks");
	for (i = 0; i < NCLASS; i++)
		mvwprintw(main_win, (y_pos + i + 5), x_pos, "Class %d, %4lu bytes",
			i, rbsize[i]);

	wnoutrefresh(main_win);
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

void extra_streams(void)
{

char na[] = "---";

char	conf_value[64];
int	conf_current = -1;
int	conf_default = -1;
int	conf_min     = -1;
int	conf_max     = -1;

conf_value[0] = '\0';

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 19, 8, "Parameter");
	mvwaddstr(main_win, 19, 28, "Kernel Name");
	mvwaddstr(main_win, 19, 41, "Minimum");
	mvwaddstr(main_win, 19, 50, "Maximum");
	mvwaddstr(main_win, 19, 59, "Default");
	mvwaddstr(main_win, 19, 68, "Current");
	Set_Colour(main_win, Normal);

	strcpy(conf_value, "NSTREAM");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 21, 1, "  total stream headers     %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, v.v_nstream);
	else
		mvwprintw(main_win, 21, 1, "  total stream headers     %11s  %7d  %7d  %7s  %7d",
		conf_value, 1, 512, na, v.v_nstream);


	strcpy(conf_value, "NQUEUE");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 22, 1, "  total stream queues      %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, v.v_nqueue);
	else
		mvwprintw(main_win, 22, 1, "  total stream queues      %11s  %7d  %7d  %7s  %7d",
		conf_value, 4, 1024, na, v.v_nqueue);


	strcpy(conf_value, "NMUXLINK");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 24, 1, "  multiplexer links        %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 24, 1, "  multiplexer links        %11s  %7d  %7d  %7s  %7d",
		conf_value, 1, 87, na, na);


	strcpy(conf_value, "NSTRPUSH");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 25, 1, "  modules on a stream      %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 25, 1, "  modules on a stream      %11s  %7d  %7d  %7s  %7s",
		conf_value, 9, 9, na, na);


	strcpy(conf_value, "NSTREVENT");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 26, 1, "  stream event structures  %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 26, 1, "  stream event structures  %11s  %7d  %7d  %7s  %7s",
		conf_value, 256, 512, na, na);


	strcpy(conf_value, "MAXSEPGCNT");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 27, 1, "  max stream page count    %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 27, 1, "  max stream page count    %11s  %7d  %7d  %7s  %7s",
		conf_value, 0, 32, na, na);


	strcpy(conf_value, "STRMSGSZ");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 29, 1, "  max size of data message %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 29, 1, "  max size of data message %11s  %7d  %7d  %7s  %7s",
		conf_value, 4096, 4096, na, na);


	strcpy(conf_value, "STRCTLSZ");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 30, 1, "  max size of ctl message  %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 30, 1, "  max size of ctl message  %11s  %7d  %7d  %7s  %7s",
		conf_value, 1024, 1024, na, na);


	strcpy(conf_value, "STRLOFRAC");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 31, 1, "  low buffers per class    %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 31, 1, "  low buffers per class    %11s  %7d  %7d  %7s  %7s",
		conf_value, 0, 95, na, na);


	strcpy(conf_value, "STRMEDFRAC");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 32, 1, "  med buffers per class    %11s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 32, 1, "  med buffers per class    %11s  %7d  %7d  %7s  %7s",
		conf_value, 80, 100, na, na);


	strcpy(conf_value, "NUMSP");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 33, 1, "  Num of /dev/sp devices %13s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 33, 1, "  Num of /dev/sp devices %13s  %7d  %7d  %7s  %7s",
		conf_value, 1, 256, na, na);

	strcpy(conf_value, "NLOG");
	if (get_tune(conf_value, &conf_current, &conf_default, &conf_min, &conf_max) MATCHES)
		mvwprintw(main_win, 34, 1, "  Num of log devices     %13s  %7d  %7d  %7d  %7d",
		conf_value, conf_min, conf_max, conf_default, conf_current);
	else
		mvwprintw(main_win, 34, 1, "  Num of log devices     %13s  %7d  %7d  %7s  %7s",
		conf_value, 3, 3, na, na);

	wnoutrefresh(main_win);
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

void watch_streams(void)
{

int get_value = 0;
int cmd = 0;
int slot = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;
int ffast = FALSE;

	naptime = Sleep_Time;

	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		Draw_stream_screen(Sleep_Time_Flag);
		update_streams();
		wnoutrefresh(main_win);
		an_option();
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
			if (signal(SIGALRM, sig_alrm_streams) == SIG_ERR)
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
				drawdisp(22);
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

			case 'I' :
			case 'i' :
				ffast = FALSE;
				drawdisp(22);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				ffast = FALSE;
				drawdisp(22);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				ffast = FALSE;
				drawdisp(22);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
				ffast = TRUE;
				naptime = -1;
				drawdisp(22);
				Set_Colour(main_win, Red_Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				Sleep_Time_Flag = FALSE;
				break;

			case 'f' :
				ffast = FALSE;
				drawdisp(22);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				break;

			case 'Q' :
			case 'q' :
				Draw_stream_screen(FALSE);
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

static void sig_alrm_streams(int signo)
{
	return;
}

void Draw_stream_screen(int Sleep_Time_Flag)
{

	if (Sleep_Time_Flag) {
		mvwaddstr(bottom_win, 1, 72, ":   ");
		Set_Colour(bottom_win, Colour_White);
		mvwaddstr(bottom_win, 1, 61, "Sample time");
		Set_Colour(bottom_win, Colour_Banner);
		mvwprintw(bottom_win, 1, 74, "%d", Sleep_Time);
	} else
		mvwaddstr(bottom_win, 1, 61, "               ");

	Set_Colour(bottom_win, Normal);
	wnoutrefresh(bottom_win);
	wnoutrefresh(bottom_win);
	an_option();
	doupdate();
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */





