/*
 * +--------------------------------------------------------------------+
 * | Function: ttys.c                                    Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the ttys info.                                         |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 * (1) 3.0:
 *		started off a rewrite for just for alarm, added a detial screen,
 *		and changing the screen format to tha same as regions, etc.
 *							Tue Dec 21 00:13:06 EST 1993 - PKR;
 * (2) 3.0: 
 *		Added colour and other things, including uucp.
 *							Sat May 07 21:03:58 EDT 1994 - PKR.
 *    Bugs:
 *          None yet.
 *
 *   Notes:
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%			Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%			Version %I% %D% - Dapix"
#endif /* __STDC__ */

#include "wi.h"

#include <string.h>
#include <sys/param.h>
#include <sys/tty.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>

#define FILE_NAME 0
#define FILE_SIZE 1

extern int errno;
extern int sys_nerr;
extern char *sys_errlist[];

int last_tty_mode = FILE_NAME;

static void sig_alrm_sio(int signo);
int detail_tty(void);
void watch_one_sio(char *dev_name);
void draw_sio_display(WINDOW *win, char *tty_name);
void update_sio_display(WINDOW *win, char *tty_name, struct tty *term);
void update_status(WINDOW *win, char *tty_name);

#define MODES	5
#define SIO_TTYS 16

#ifndef IMAXBEL
#	define IMAXBEL 0020000				/* future use. */
#endif

char *tty_num[] = {
	"1a", "1b", "1c", "1d",
	"1e", "1f", "1g", "1h",
	"2a", "2b", "2c", "2d",
	"2e", "2f", "2g", "2h"
};

int term_to_speed[17] = {
	0,	50,	75,	110,
	134,	150,	200,	300,
	600,	1200,	1800,	2400,
	4800,	9600,	19200, 38400,
	56800
};

struct tty *sios;

int num_sio;

#define IFLAG 1
#define OFLAG 2
#define LFLAG 3
#define CFLAG 4
#define STATE 5

extern int size_flag;
int max_sio_to_display;

int curr_tty = FALSE;

void get_sio(void);
void sio_header(void);
void watch_sio(void);
void display_sio(int y, int slot);
void sio_help(int screen_num);

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

void tty_io(void)
{

int	screen_num = 1, i = 0;

	curr_tty = FALSE;
	sio_header();
	curr_tty = 0;

	if (size_flag)
		max_sio_to_display = 30;
	else
		max_sio_to_display = 11;

	get_sio();
	wrefresh(main_win);

	for (i = 0; ;) {
		an_option();
		doupdate();
		switch(wgetch(bottom_win)) {
		case 'd' :
		case 'D' :
			Set_Colour(main_win, Colour_Banner);
			mvwaddstr(main_win, 0, 37, "Detail");
			Set_Colour(main_win, Normal);
			wrefresh(main_win);
			detail_tty();
			drawdisp(18);
			wrefresh(main_win);
			break;

		case 'b' :
		case 'B' :
		case KEY_PPAGE:
		case 'f' :
		case 'F' :
		case KEY_NPAGE:
			if (size_flag)
				beep();
			else {
				if (curr_tty == TRUE) {
					curr_tty = FALSE;
				} else {
					curr_tty = TRUE;
				}
			}
			get_sio();
			wrefresh(main_win);
			break;

		case '+' :
		case KEY_UP:
		case KEY_RIGHT:
			if (screen_num >= MODES)
				screen_num = 1;
			else
				screen_num++;
			sio_help(screen_num);
			doupdate();
			break;

		case '-' :
		case KEY_DOWN:
		case KEY_LEFT:
			if (screen_num <= 1)
				screen_num = MODES;
			else
				screen_num--;
			sio_help(screen_num);
			doupdate();
			break;

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
			return;

		case 'u' :
		case 'U' :
			get_sio();
			wrefresh(main_win);
			break;

		case 'w' :
		case 'W' :
			drawdisp(19);
			if (size_flag)
				max_sio_to_display = 30;
			else
				max_sio_to_display = 11;
			watch_sio();
			drawdisp(18);
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
 * | Function: get_sio()                                 Date: 92/04/04 |
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

void get_sio(void)
{

register int sio_reg;
register struct tty *tmp_sio;
int pos = 0;
int first_loop = TRUE;

	sios = (struct tty *) malloc(SIO_TTYS * sizeof(struct tty));
	kmem_read(sios, namelist[NM_SIOTTY].n_value, sizeof(struct tty)*SIO_TTYS);

	first_loop = TRUE;
	pos = 0;
	num_sio = 0;

	for (pos = 0, sio_reg = 0; sio_reg < SIO_TTYS; sio_reg++, pos++) {
		if (!(size_flag)) {
			if (curr_tty == TRUE) {
				if (first_loop == TRUE) {
					sio_reg = 5;
				}
				first_loop = FALSE;
			} else {
				if (sio_reg >= 11) {
					free((char *) sios);
					return;
				}
			}
		}
		display_sio(pos + 3, sio_reg);
		num_sio++;
	}

	free((char *) sios);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: sio_header()                             Date: %D% |
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


void sio_header(void)
{

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 2, 2, "tty");
	mvwaddstr(main_win, 2, 7, "speed");
	mvwaddstr(main_win, 2, 15, "t_state");
	mvwaddstr(main_win, 2, 26, "rawq");
	mvwaddstr(main_win, 2, 31, "canq");
	mvwaddstr(main_win, 2, 36, "outq");

	mvwaddstr(main_win, 2, 42, "t_iflag");
	mvwaddstr(main_win, 2, 51, "t_oflag");
	mvwaddstr(main_win, 2, 60, "t_cflag");
	mvwaddstr(main_win, 2, 69, "t_lflag");
	Set_Colour(main_win, Normal);

	sio_help(1);
	wnoutrefresh(main_win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: sio_help()                               Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the sio help screen.                              |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */


void sio_help(int screen_num)
{

	switch(screen_num) {
	case 1:
		mvwaddstr(main_win, page_len - 1, 2, "wait for open, open, carrier on, busy, stopped, timeout, wakeup on output");
		mvwaddstr(main_win, page_len, 2, "wakeup on input, block, extern, active, esc, raw timeout, wait, xon, xoff");

		Set_Colour(main_win, Colour_Blue);
		mvwaddstr(main_win, page_len - 2, 68, "t_state");
		mvwaddstr(main_win, page_len - 1, 2, "W");
		mvwaddstr(main_win, page_len - 1, 17, "O");
		mvwaddstr(main_win, page_len - 1, 23, "C");
		mvwaddstr(main_win, page_len - 1, 35, "B");
		mvwaddstr(main_win, page_len - 1, 41, "S");
		mvwaddstr(main_win, page_len - 1, 50, "T");
		mvwaddstr(main_win, page_len - 1, 69, "o");
		mvwaddstr(main_win, page_len, 12, "I");
		mvwaddstr(main_win, page_len, 19, "b");
		mvwaddstr(main_win, page_len, 26, "E");
		mvwaddstr(main_win, page_len, 34, "A");
		mvwaddstr(main_win, page_len, 42, "e");
		mvwaddstr(main_win, page_len, 47, "R");
		mvwaddstr(main_win, page_len, 60, "w");
		mvwaddstr(main_win, page_len, 66, "X");
		mvwaddstr(main_win, page_len, 73, "F");
		break;

	case 2:
		mvwaddstr(main_win, page_len - 1, 2, "I=IGNBRK,  B=BRKINT,  P=INPCK,  S=ISTRIP,  N=INCLR,  C=ICRNL,  G=IGNPAR,   ");
		mvwaddstr(main_win, page_len, 2, "U=IUCLC,  X=IXON,  F=IXOFF                                                ");

		Set_Colour(main_win, Colour_Blue);
		mvwaddstr(main_win, page_len - 2, 68, "t_iflag");
		mvwaddstr(main_win, page_len - 1, 2, "I");
		mvwaddstr(main_win, page_len - 1, 13, "B");
		mvwaddstr(main_win, page_len - 1, 24, "P");
		mvwaddstr(main_win, page_len - 1, 34, "S");
		mvwaddstr(main_win, page_len - 1, 45, "N");
		mvwaddstr(main_win, page_len - 1, 55, "C");
		mvwaddstr(main_win, page_len - 1, 65, "G");
		mvwaddstr(main_win, page_len, 2, "U");
		mvwaddstr(main_win, page_len, 12, "X");
		mvwaddstr(main_win, page_len, 21, "F");
		break;

	case 3:
		mvwaddstr(main_win, page_len - 1, 2, "P=OPOST,  L=OLCUC,  N=ONLCR,  C=OCRNL,  n=ONOCR,  R=ONLRET,  F=OFILL,    ");
		mvwaddstr(main_win, page_len, 2, "D=NLDLY                                                                    ");
		Set_Colour(main_win, Colour_Blue);
		mvwaddstr(main_win, page_len - 2, 68, "t_oflag");
		mvwaddstr(main_win, page_len - 1, 2, "P");
		mvwaddstr(main_win, page_len - 1, 12, "L");
		mvwaddstr(main_win, page_len - 1, 22, "N");
		mvwaddstr(main_win, page_len - 1, 32, "C");
		mvwaddstr(main_win, page_len - 1, 42, "n");
		mvwaddstr(main_win, page_len - 1, 52, "R");
		mvwaddstr(main_win, page_len - 1, 63, "F");
		mvwaddstr(main_win, page_len, 2, "D");
		break;

	case 4:
		mvwaddstr(main_win, page_len - 1, 2, "5=5 bit, 6=6 bit, 7=7 bit, 8=8 bit, O=odd, E=even, N=none, 1=1 stopbit, ");
		mvwaddstr(main_win, page_len, 2, "2=2 stopbit, C=CTSFLOW, R=RTSFLOW,both, H=HUPCL, L=CLOCAL, D=CREAD, K=LOBLK");

		Set_Colour(main_win, Colour_Blue);
		mvwaddstr(main_win, page_len - 2, 68, "t_cflag");
		mvwaddstr(main_win, page_len - 1, 2, "5");
		mvwaddstr(main_win, page_len - 1, 11, "6");
		mvwaddstr(main_win, page_len - 1, 20, "7");
		mvwaddstr(main_win, page_len - 1, 29, "8");
		mvwaddstr(main_win, page_len - 1, 38, "O");
		mvwaddstr(main_win, page_len - 1, 45, "E");
		mvwaddstr(main_win, page_len - 1, 53, "N");
		mvwaddstr(main_win, page_len - 1, 61, "1");
		mvwaddstr(main_win, page_len, 2, "2");
		mvwaddstr(main_win, page_len, 15, "C");
		mvwaddstr(main_win, page_len, 26, "R");
		mvwaddstr(main_win, page_len, 36, "b");
		mvwaddstr(main_win, page_len, 42, "H");
		mvwaddstr(main_win, page_len, 51, "L");
		mvwaddstr(main_win, page_len, 61, "D");
		mvwaddstr(main_win, page_len, 70, "K");
		break;

	case 5:
		mvwaddstr(main_win, page_len - 1, 2, "S=ISIG,  C=ICANON,  X=XCASE,  E=ECHO,  e=ECHOE,  K=ECHOK,  n=ECHONL,     ");
		mvwaddstr(main_win, page_len, 2, "F=NOFLSH                                                                   ");

		Set_Colour(main_win, Colour_Blue);
		mvwaddstr(main_win, page_len - 2, 68, "t_lflag");
		mvwaddstr(main_win, page_len - 1, 2, "S");
		mvwaddstr(main_win, page_len - 1, 11, "C");
		mvwaddstr(main_win, page_len - 1, 22, "X");
		mvwaddstr(main_win, page_len - 1, 32, "E");
		mvwaddstr(main_win, page_len - 1, 41, "e");
		mvwaddstr(main_win, page_len - 1, 51, "K");
		mvwaddstr(main_win, page_len - 1, 61, "n");
		mvwaddstr(main_win, page_len, 2, "F");
		break;
	}

	Set_Colour(main_win, Normal);
	wnoutrefresh(main_win);
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

void display_sio(int y, int slot)
{

register struct tty *display_sio;

register unsigned int tmp_value;

int counter = 0;

	char i_stat[12];
	char i_flg[9];
	char o_flg[9];
	char c_flg[9];
	char l_flg[9];

	display_sio = &sios[slot];

	strcpy(i_stat, "............");
	strcpy(i_flg, "........");
	strcpy(o_flg, "........");
	strcpy(c_flg, "........");
	strcpy(l_flg, "........");

	mvwaddstr(main_win, y, 3, "                                                                         ");
	mvwprintw(main_win, y, 3, "%.2s", tty_num[slot]);

	if (!(display_sio->t_state & (ISOPEN | WOPEN))) {
		waddstr(main_win, " - ");
		Set_Colour(main_win, Colour_Grey);
		waddstr(main_win, "Port Closed");
		Set_Colour(main_win, Normal);
		return;
	}

	if (display_sio->t_state & WOPEN)
		i_stat[0] = 'W';
	else if (display_sio->t_state & ISOPEN)
		i_stat[0] = 'O';
	if (display_sio->t_state & CARR_ON)
		i_stat[1] = 'C';
	if (display_sio->t_state & BUSY)
		i_stat[2] = 'B';
	if (display_sio->t_state & TTSTOP)
		i_stat[2] = 'S';
	if (display_sio->t_state & TIMEOUT)
		i_stat[3] = 'T';
	if (display_sio->t_state & OASLP)
		i_stat[4] = 'o';
	if (display_sio->t_state & IASLP)
		i_stat[5] = 'I';
	if (display_sio->t_state & TBLOCK)
		i_stat[6] = 'b';
	if (display_sio->t_state & EXTPROC)
		i_stat[7] = 'E';
	if (display_sio->t_state & TACT)
		i_stat[8] = 'A';
	if (display_sio->t_state & CLESC)
		i_stat[9] = 'e';
	if (display_sio->t_state & RTO)
		i_stat[10] = 'R';
	if (display_sio->t_state & TTIOW)
		i_stat[11] = 'w';
	if (display_sio->t_state & TTXON)
		i_stat[11] = 'X';
	if (display_sio->t_state & TTXOFF)
		i_stat[11] = 'F';

	mvwprintw(main_win, y, 7, "%5d %s", term_to_speed[display_sio->t_cflag & CBAUD], i_stat);

	wmove(main_win, y, 25);

	tmp_value = (unsigned)display_sio->t_rawq.c_cc;

	if (tmp_value > 100)
		Set_Colour(main_win, Text_Warning);
	else if (tmp_value > 30)
		Set_Colour(main_win, Text_Alarm);
	else
		Set_Colour(main_win, Text_Normal);
	
	wprintw(main_win, "%5d", tmp_value);
	Set_Colour(main_win, Normal);

	tmp_value = (unsigned)display_sio->t_canq.c_cc;

	wmove(main_win, y, 30);


	if (tmp_value > 20)
		Set_Colour(main_win, Text_Warning);
	else if (tmp_value > 10)
		Set_Colour(main_win, Text_Alarm);
	else
		Set_Colour(main_win, Text_Normal);

	wprintw(main_win, "%5d", tmp_value);
	Set_Colour(main_win, Normal);

	tmp_value = (unsigned)display_sio->t_outq.c_cc + display_sio->t_tbuf.c_count;

	wmove(main_win, y, 35);

	if (tmp_value > 75)
		Set_Colour(main_win, Text_Warning);
	else if (tmp_value > 25)
		Set_Colour(main_win, Text_Alarm);
	else
		Set_Colour(main_win, Text_Normal);

	wprintw(main_win, "%5d", tmp_value);
	Set_Colour(main_win, Normal);

	/*
		INPUT MODES
	*/
	if (display_sio->t_iflag & IGNBRK)
		i_flg[0] = 'I';
	if (display_sio->t_iflag & BRKINT)
		i_flg[1] = 'B';
	if (display_sio->t_iflag & INPCK)
		i_flg[2] = 'P';
	if (display_sio->t_iflag & ISTRIP)
		i_flg[3] = 'S';
	if (display_sio->t_iflag & INLCR)
		i_flg[4] = 'N';
	if (display_sio->t_iflag & ICRNL)
		i_flg[4] = 'C';
	if (display_sio->t_iflag & IGNPAR)
		i_flg[5] = 'G';
	if (display_sio->t_iflag & IUCLC)
		i_flg[6] = 'U';
	if (display_sio->t_iflag & IXON)
		i_flg[7] = 'X';
	if (display_sio->t_iflag & IXOFF)
		i_flg[7] = 'F';

	/*
		OUTPUT MODES
	*/
	if (display_sio->t_oflag & OPOST)
		o_flg[0] = 'P';
	if (display_sio->t_oflag & OLCUC)
		o_flg[1] = 'L';
	if (display_sio->t_oflag & ONLCR)
		o_flg[2] = 'N';
	if (display_sio->t_oflag & OCRNL)
		o_flg[3] = 'C';
	if (display_sio->t_oflag & ONOCR)
		o_flg[4] = 'n';
	if (display_sio->t_oflag & ONLRET)
		o_flg[5] = 'R';
	if (display_sio->t_oflag & OFILL)
		o_flg[6] = 'F';
	if (display_sio->t_oflag & NLDLY)
		o_flg[7] = 'D';

	/*
		CONTROL MODES
	*/
	if (display_sio->t_cflag & CS5)
		c_flg[0] = '5';
	if (display_sio->t_cflag & CS6)
		c_flg[0] = '6';
	if (display_sio->t_cflag & CS7)
		c_flg[0] = '7';
	if (display_sio->t_cflag & CS8)
		c_flg[0] = '8';
	if (display_sio->t_cflag & PARENB) {
		if (display_sio->t_cflag & PARODD)
			c_flg[1] = 'O';
		else
			c_flg[1] = 'E';
	} else
		c_flg[1] = 'N';

	if (display_sio->t_cflag & CSTOPB)
		c_flg[2] = '2';
	else
		c_flg[2] = '1';
	if ((display_sio->t_cflag & CTSFLOW) && (!(display_sio->t_cflag & RTSFLOW)))
		c_flg[3] = 'C';
	if ((display_sio->t_cflag & RTSFLOW) && (!(display_sio->t_cflag & CTSFLOW)))
		c_flg[3] = 'R';
	if ((display_sio->t_cflag & RTSFLOW) && (display_sio->t_cflag & CTSFLOW))
		c_flg[3] = 'b';
	if (display_sio->t_cflag & HUPCL)
		c_flg[4] = 'H';
	if (display_sio->t_cflag & CLOCAL)
		c_flg[5] = 'L';
	if (display_sio->t_cflag & CREAD)
		c_flg[6] = 'D';
	if (display_sio->t_cflag & LOBLK)
		c_flg[7] = 'K';

	/*
		LOCAL / LINE MODES
	*/
	if (display_sio->t_lflag & ISIG)
		l_flg[0] = 'S';
	if (display_sio->t_lflag & ICANON)
		l_flg[1] = 'C';
	if (display_sio->t_lflag & XCASE)
		l_flg[2] = 'X';
	if (display_sio->t_lflag & ECHO)
		l_flg[3] = 'E';
	if (display_sio->t_lflag & ECHOE)
		l_flg[4] = 'e';
	if (display_sio->t_lflag & ECHOK)
		l_flg[5] = 'K';
	if (display_sio->t_lflag & ECHONL)
		l_flg[6] = 'N';
	if (display_sio->t_lflag & NOFLSH)
		l_flg[7] = 'F';

	mvwprintw(main_win, y, 41, "%s %s %s %s", i_flg, o_flg, c_flg, l_flg);
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

void watch_sio(void)
{

	int get_value = 0;
	int cmd = 0;
	int slot = 0;
	int Sleep_Time_Flag = TRUE;
	int naptime = 0;
	int screen_num = 1;

	if (size_flag)
		max_sio_to_display = 30;
	else
		max_sio_to_display = 11;

	naptime = Sleep_Time;

	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);
	wnoutrefresh(top_win);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		get_sio();
		wnoutrefresh(main_win);
		an_option();
		doupdate();

		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_sio) == SIG_ERR)
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
				drawdisp(19);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				doupdate();
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case KEY_PPAGE:
			case KEY_NPAGE:
				if (size_flag)
					beep();
				else {
					if (curr_tty == TRUE) {
						curr_tty = FALSE;
					} else {
						curr_tty = TRUE;
					}
				}
				break;

			case 'P' :
			case 'p' :
				screen_dump();
				break;

			case 'd' :
			case 'D' :
				drawdisp(19);
				Set_Colour(main_win, Colour_Banner);
				mvwaddstr(main_win, 0, 49, "Detail");
				Set_Colour(main_win, Normal);
				wrefresh(main_win);
				if (detail_tty() == 1)
					beep();

				Set_Colour(main_win, Normal);
				mvwaddstr(main_win, 0, 49, "Detail");
				Set_Colour(main_win, Colour_Banner);
				mvwaddstr(main_win, 0, 49, "D");
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
				drawdisp(19);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				drawdisp(19);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				drawdisp(19);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
			case 'f' :
				drawdisp(19);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				break;

			case '+' :
			case KEY_UP:
			case KEY_RIGHT:
				if (screen_num >= MODES)
					screen_num = 1;
				else
					screen_num++;
				sio_help(screen_num);
				doupdate();
				break;

			case '-' :
			case KEY_DOWN:
			case KEY_LEFT:
				if (screen_num <= 1)
					screen_num = MODES;
				else
					screen_num--;
				sio_help(screen_num);
				doupdate();
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

static void sig_alrm_sio(int signo)
{
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int detail_tty()                          Date: 93/02/27 |
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

int detail_tty(void)
{

WINDOW *win;
char tmp_val[6];

	tmp_val[0] = '\0';

	last_tty_mode = FILE_SIZE;

	win = newwin(3, 28, 12, 10);
	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);
	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 0, 1, " Detail on a tty ");
	Set_Colour(win, Normal);
	while (TRUE) {
		mvwaddstr(win, 1, 2, "Select a tty, /dev/tty");
		wrefresh(win);
		tmp_val[0] = wgetch(win);
		if (tmp_val[0] == 'q') {
			delwin(win);
			touchwin(main_win);
			wrefresh(main_win);
			return(1);
		}
		if ((tmp_val[0] != '1') && (tmp_val[0] != '2')) {
			beep();
			continue;
		}
		/*
			Get the second char
		*/
		tmp_val[1] = wgetch(win);
		if (((tmp_val[1] >= 'A') && (tmp_val[1] <= 'H')) ||
		    ((tmp_val[1] >= 'a') && (tmp_val[1] <= 'h'))) {
			if ((tmp_val[1] >= 'A') && (tmp_val[1] <= 'H')) {
				tmp_val[1] += 32;
			}
			tmp_val[2] = '\0';
			/*
				OK I now have a valied ttyXX so monitor it.
			*/
			delwin(win);
			touchwin(main_win);
			watch_one_sio(tmp_val);
			return(0);
		} else {
			beep();
			continue;
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

void watch_one_sio(char *dev_name)
{

WINDOW *win;
int cmd = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;
int screen_num = 1;
char tty_name[12] = "/dev/tty";

register int counter;

	/*
		Check to see if the file will open before starting a loop.
	*/
	strcmp(tty_name, "/dev/tty");
	strcat(tty_name, dev_name);

	if ((access(tty_name, R_OK)) != 0) {
		/*
			Can't open the /dev/ttyXX show message.
		*/
		win = newwin(3, 35, 13, 15);
		Set_Colour(win, Normal);
		Fill_A_Box(win, 0, 0);
		Draw_A_Box(win, BUTTON_BOX);
		Set_Colour(win, Text_Alarm);
		mvwprintw(win, 1, 2, "Sorry I can't access %s", tty_name);
		Set_Colour(win, Normal);
		wnoutrefresh(win);
		beep();
		any_key();
		doupdate();
		wgetch(bottom_win);
		delwin(win);
		touchwin(main_win);
		wrefresh(main_win);
		return;
	}
	wrefresh(main_win);

	if (size_flag)
		win = newwin(16, 69, 20, 8);
	else
		win = newwin(16, 69, 5, 8);
	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);

	draw_sio_display(win, tty_name);
	doupdate();

	naptime = Sleep_Time;

	mvwaddstr(win, 0, 48, "Time");
	mvwaddstr(win, 0, 54, "Slow");
	mvwaddstr(win, 0, 59, "Med");
	mvwaddstr(win, 0, 63, "Fast");
	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 0, 49, "i");
	mvwaddstr(win, 0, 54, "S");
	mvwaddstr(win, 0, 59, "M");
	mvwaddstr(win, 0, 63, "F");

	Set_Colour(win, Blink_Banner);
	mvwaddstr(win, 0, 48, "Time");
	Set_Colour(win, Normal);
	wnoutrefresh(top_win);

	while (TRUE) {
		/*
			read the kernel and get the item
		*/
		sios = (struct tty *) malloc(SIO_TTYS * sizeof(struct tty));
		kmem_read(sios, namelist[NM_SIOTTY].n_value,
			sizeof(struct tty) * SIO_TTYS);

		for (counter = 0; counter < SIO_TTYS; counter++) {
			if (strcmp(tty_num[counter], dev_name) MATCHES) {
				break;
			}
		}

		drawdisp(1);
		wnoutrefresh(top_win);
		mvwaddstr(win, 3, 11, "                                                        ");
		draw_sio_display(win, tty_name);
		/*
			check for port closed.
		*/
		Set_Colour(win, Text_Alarm);
		if (!(sios[counter].t_state & (ISOPEN | WOPEN))) {
			mvwaddstr(win, 2, 11, "port closed     ");
			mvwaddstr(win, 3, 11, "port disabled           ");
		} else if (term_to_speed[sios[counter].t_state & CBAUD] == 0)
			mvwaddstr(win, 2, 11, "port disabled   ");
		else {
			Set_Colour(win, Normal);
			update_sio_display(win, tty_name, &sios[counter]);
		}

		free((char *) sios);

		wnoutrefresh(win);
		an_option();
		doupdate();

		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_sio) == SIG_ERR)
			printf("\nsignal(SIGALRM) error\n");

		alarm(naptime);	/* set the alarm timer */

		if ((cmd = wgetch(bottom_win)) < 0) {
			alarm(0);			/* stop the alarm timer */
		} else {
			alarm(0);			/* stop the alarm timer */
			Set_Colour(win, Normal);
			mvwaddstr(win, 0, 48, "Time");
			mvwaddstr(win, 0, 54, "Slow");
			mvwaddstr(win, 0, 59, "Med");
			mvwaddstr(win, 0, 63, "Fast");
			Set_Colour(win, Colour_Banner);
			mvwaddstr(win, 0, 49, "i");
			mvwaddstr(win, 0, 54, "S");
			mvwaddstr(win, 0, 59, "M");
			mvwaddstr(win, 0, 63, "F");
			switch(cmd) {
			case 'C' :
			case 'c' :
				change_time(FALSE);
				touchwin(main_win);
				Set_Colour(win, Blink_Banner);
				mvwaddstr(win, 0, 48, "Time");
				Set_Colour(win, Normal);
				doupdate();
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'I' :
			case 'i' :
				Set_Colour(win, Blink_Banner);
				mvwaddstr(win, 0, 48, "Time");
				Set_Colour(win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				Set_Colour(win, Blink_Banner);
				mvwaddstr(win, 0, 54, "Slow");
				Set_Colour(win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				Set_Colour(win, Blink_Banner);
				mvwaddstr(win, 0, 59, "Med");
				Set_Colour(win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
			case 'f' :
				Set_Colour(win, Blink_Banner);
				mvwaddstr(win, 0, 63, "Fast");
				Set_Colour(win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				break;

			case 'Q' :
			case 'q' :
				delwin(win);
				touchwin(main_win);
				wrefresh(main_win);
				return;

			case 'P' :
			case 'p' :
				screen_dump();

			default :
				beep();
				Set_Colour(win, Blink_Banner);
				if (Sleep_Time_Flag) {
					mvwaddstr(win, 0, 48, "Time");
				} else {
					Set_Colour(win, Colour_Banner);
					mvwaddstr(win, 0, 49, "i");
					Set_Colour(win, Blink_Banner);
					switch(naptime) {
					case 4 :
						mvwaddstr(win, 0, 54, "Slow");
						break;

					case 2 :
						mvwaddstr(win, 0, 59, "Med");
						break;

					default :
						mvwaddstr(win, 0, 63, "Fast");
						break;
					}
				}
				Set_Colour(win, Normal);
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

void draw_sio_display(WINDOW *win, char *tty_name)
{

	Set_Colour(win, Colour_Banner);
	mvwprintw(win, 0, 2, " Detail on %s ", tty_name);
	Set_Colour(win, Normal);

	mvwaddstr(win, 2, 2, "       :                 ");
	mvwaddstr(win, 3, 2, "       :                         ");

	mvwaddstr(win, 5, 2, "       : ignbrk brkint ignpar parmrk inpck istrip inlcr igncr");
	mvwaddstr(win, 6, 2, "         icrnl iuclc ixon ixany ixoff imaxbel dosmode");

	mvwaddstr(win, 7, 2, "       : opost olcuc onlcr ocrnl onocr onlret ofill ofdel");

	mvwaddstr(win, 8, 2, "       : parenb parodd cs8 cstopb hupcl cread clocal loblk xclude");
#ifdef _OLD_OS
		mvwaddstr(win, 9, 2, "         rcv1en xmt1en xloblk crtsfl ctsflow rtsflow");
#else
		mvwaddstr(win, 9, 2, "         rcv1en xmt1en xloblk crtsfl ctsflow rtsflow ortsfl");
#endif
	mvwaddstr(win, 10, 2, "       : isig icanon xcase echo echoe echok echonl noflsh");
	mvwaddstr(win, 11, 2, "         iexten tostop");

	mvwaddstr(win, 12, 2, "       : isopen carr_on busy ttstop timeout oaslp iaslp");
	mvwaddstr(win, 13, 2, "         tblock extproc tact clesc rto ttiow ttxon ttxoff");
	mvwaddstr(win, 14, 2, "         extdly trcoll twcoll tecoll");

	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 2, 4, "speed");
	mvwaddstr(win, 3, 3, "status");
	mvwaddstr(win, 5, 2, "t_iflag");
	mvwaddstr(win, 7, 2, "t_oflag");
	mvwaddstr(win, 8, 2, "t_cflag");
	mvwaddstr(win, 10, 2, "t_lflag");
	mvwaddstr(win, 12, 2, "t_state");
	Set_Colour(win, Normal);
	wnoutrefresh(win);
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

void update_sio_display(WINDOW *win, char *tty_name, struct tty *term)
{

	Set_Colour(win, Colour_Banner);
	mvwprintw(win, 0, 2, " Detail on %s ", tty_name);
	Set_Colour(win, Normal);

	if (term_to_speed[term->t_cflag & CBAUD] == 0) {
		Set_Colour(win, Text_Warning);
		mvwaddstr(win, 2, 11, "resetting");
		Set_Colour(win, Normal);
	} else
		mvwprintw(win, 2, 11, "%d baud         ",
			term_to_speed[term->t_cflag & CBAUD]);

	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 2, 4, "speed");
	mvwaddstr(win, 3, 3, "status");
	Set_Colour(main_win, Normal);

	if (term->t_state & WOPEN) {
		Set_Colour(win, Text_Warning);
		mvwaddstr(win, 3, 11, "modem waiting");
		Set_Colour(win, Normal);
		w_clrtoeol(win, 0, 0);
	} else if (term->t_state & ISOPEN) {
		Set_Colour(win, Normal);
		update_status(win, tty_name);
	}

	/*
		t_iflag stats
	*/
	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 5, 2, "t_iflag");
	Set_Colour(win, Colour_White);
	if (term->t_iflag & IGNBRK)
		mvwaddstr(win, 5, 11, "ignbrk");
	if (term->t_iflag & BRKINT)
		mvwaddstr(win, 5, 18, "brkint");
	if (term->t_iflag & IGNPAR)
		mvwaddstr(win, 5, 25, "ignpar");
	if (term->t_iflag & PARMRK)
		mvwaddstr(win, 5, 32, "parmrk");
	if (term->t_iflag & INPCK)
		mvwaddstr(win, 5, 39, "inpck");
	if (term->t_iflag & ISTRIP)
		mvwaddstr(win, 5, 45, "istrip");
	if (term->t_iflag & INLCR)
		mvwaddstr(win, 5, 52, "inlcr");
	if (term->t_iflag & IGNCR)
		mvwaddstr(win, 5, 58, "igncr");

	if (term->t_iflag & ICRNL)
		mvwaddstr(win, 6, 11, "icrnl");
	if (term->t_iflag & IUCLC)
		mvwaddstr(win, 6, 17, "iuclc");
	if (term->t_iflag & IXON)
		mvwaddstr(win, 6, 23, "ixon");
	if (term->t_iflag & IXANY)
		mvwaddstr(win, 6, 28, "ixany");
	if (term->t_iflag & IXOFF)
		mvwaddstr(win, 6, 34, "ixoff");
	if (term->t_iflag & IMAXBEL)
		mvwaddstr(win, 6, 40, "imaxbel");
	if (term->t_iflag & DOSMODE)
		mvwaddstr(win, 6, 48, "dosmode");


	/*
		t_oflag stats
	*/
	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 7, 2, "t_oflag");
	Set_Colour(win, Colour_White);

	if (term->t_oflag & OPOST)
		mvwaddstr(win, 7, 11, "opost");
	if (term->t_oflag & OLCUC)
		mvwaddstr(win, 7, 17, "olcuc");
	if (term->t_oflag & ONLCR)
		mvwaddstr(win, 7, 23, "onlcr");
	if (term->t_oflag & OCRNL)
		mvwaddstr(win, 7, 29, "ocrnl");
	if (term->t_oflag & ONOCR)
		mvwaddstr(win, 7, 35, "onocr");
	if (term->t_oflag & ONLRET)
		mvwaddstr(win, 7, 41, "onlret");
	if (term->t_oflag & OFILL)
		mvwaddstr(win, 7, 48, "ofill");
	if (term->t_oflag & OFDEL)
		mvwaddstr(win, 7, 54, "ofdel");

/*
	This does not work, I don't know why ? - YET

	wattroff(win, A_BOLD);
	wmove(win, 7, 11);
	if (term->t_oflag & NL0) {
		waddstr(win, "NL0 ");
		waddstr(win, "CR0 ");
		waddstr(win, "TAB0 ");
		waddstr(win, "BS0 ");
		waddstr(win, "VT0 ");
		waddstr(win, "FF0");
	} else {
		waddstr(win, "nldly ");
		if (term->t_oflag & CR3)
			waddstr(win, "crdly ");
		else if (term->t_oflag & CR1)
			waddstr(win, "cr1 ");
		else if (term->t_oflag & CR2)
			waddstr(win, "cr2 ");

		if (term->t_oflag & TAB3)
			waddstr(win, "tabdly ");
		else if (term->t_oflag & TAB2)
			waddstr(win, "tab2 ");
		else if (term->t_oflag & TAB1)
			waddstr(win, "tab1 ");

		waddstr(win, "bsdly ");
		waddstr(win, "vtdly ");
		waddstr(win, "ffdly");
	} 
	wattron(win, A_BOLD);
*/

	/*
		t_cflag stats
	*/
	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 8, 2, "t_cflag");
	Set_Colour(win, Colour_White);

	if (term->t_cflag & PARENB)
		mvwaddstr(win, 8, 11, "parenb");
	if (term->t_cflag & PARENB) {
		if (term->t_cflag & PARODD) {
			mvwaddstr(win, 8, 18, "parodd");
		}
	}
	if (term->t_cflag & CS6)
		mvwaddstr(win, 8, 25, "cs6");
	if (term->t_cflag & CS7)
		mvwaddstr(win, 8, 25, "cs7");
	if (term->t_cflag & CS8)
		mvwaddstr(win, 8, 25, "cs8");
	if (term->t_cflag & CSTOPB)
		mvwaddstr(win, 8, 29, "cstopb");
	if (term->t_cflag & HUPCL)
		mvwaddstr(win, 8, 36, "hupcl");
	if (term->t_cflag & CREAD)
		mvwaddstr(win, 8, 42, "cread");
	if (term->t_cflag & CLOCAL)
		mvwaddstr(win, 8, 48, "clocal");
	if (term->t_cflag & LOBLK)
		mvwaddstr(win, 8, 55, "loblk");
	if (term->t_cflag & XCLUDE)
		mvwaddstr(win, 8, 61, "xclude");

	if (term->t_cflag & RCV1EN)
		mvwaddstr(win, 9, 11, "rcv1en");
	if (term->t_cflag & XMT1EN)
		mvwaddstr(win, 9, 18, "xmt1en");
	if (term->t_cflag & XLOBLK)
		mvwaddstr(win, 9, 25, "xloblk");
	if (term->t_cflag & CRTSFL)
		mvwaddstr(win, 9, 32, "crtsfl");
	if (term->t_cflag & CTSFLOW)
		mvwaddstr(win, 9, 39, "ctsflow");
	if (term->t_cflag & RTSFLOW)
		mvwaddstr(win, 9, 47, "rtsflow");

#ifndef _OLD_OS				/* is it a new version ? */
	if (term->t_cflag & ORTSFL)
		mvwaddstr(win, 9, 55, "ortsfl");
#endif

	/*
		t_lflag stats
	*/
	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 10, 2, "t_lflag");
	Set_Colour(win, Colour_White);

	if (term->t_lflag & ISIG)
		mvwaddstr(win, 10, 11, "isig");
	if (term->t_lflag & ICANON)
		mvwaddstr(win, 10, 16, "icanon");
	if (term->t_lflag & XCASE)
		mvwaddstr(win, 10, 23, "xcase");
	if (term->t_lflag & ECHO)
		mvwaddstr(win, 10, 29, "echo");
	if (term->t_lflag & ECHOE)
		mvwaddstr(win, 10, 34, "echoe");
	if (term->t_lflag & ECHOK)
		mvwaddstr(win, 10, 40, "echok");
	if (term->t_lflag & ECHONL)
		mvwaddstr(win, 10, 46, "echonl");
	if (term->t_lflag & NOFLSH)
		mvwaddstr(win, 10, 53, "noflsh");

	if (term->t_lflag & IEXTEN)
		mvwaddstr(win, 11, 11, "iexten");
	if (term->t_lflag & TOSTOP)
		mvwaddstr(win, 11, 18, "tostop");

	/*
		t_state stats
	*/
	Set_Colour(win, Colour_Blue);
	mvwaddstr(win, 12, 2, "t_state");
	Set_Colour(win, Colour_White);

	if (term->t_state & WOPEN)
		mvwaddstr(win, 12, 11, "wopen ");
	else if (term->t_state & ISOPEN)
		mvwaddstr(win, 12, 11, "isopen");

	if (term->t_state & CARR_ON)
		mvwaddstr(win, 12, 18, "carr_on");
	if (term->t_state & BUSY)
		mvwaddstr(win, 12, 26, "busy");
	if (term->t_state & TTSTOP)
		mvwaddstr(win, 12, 31, "ttstop");
	if (term->t_state & TIMEOUT)
		mvwaddstr(win, 12, 38, "timeout");
	if (term->t_state & OASLP)
		mvwaddstr(win, 12, 46, "oaslp");
	if (term->t_state & IASLP)
		mvwaddstr(win, 12, 52, "iaslp");

	if (term->t_state & TBLOCK)
		mvwaddstr(win, 13, 11, "tblock");
	if (term->t_state & EXTPROC)
		mvwaddstr(win, 13, 18, "extproc");
	if (term->t_state & TACT)
		mvwaddstr(win, 13, 26, "tact");
	if (term->t_state & CLESC)
		mvwaddstr(win, 13, 31, "clesc");
	if (term->t_state & RTO)
		mvwaddstr(win, 13, 37, "rto");
	if (term->t_state & TTIOW)
		mvwaddstr(win, 13, 41, "ttiow");
	if (term->t_state & TTXON)
		mvwaddstr(win, 13, 47, "ttxon");
	if (term->t_state & TTXOFF)
		mvwaddstr(win, 13, 53, "ttxoff");

	if (term->t_xstate & EXTDLY)
		mvwaddstr(win, 14, 11, "extdly");
	if (term->t_xstate & TRCOLL)
		mvwaddstr(win, 14, 18, "trcoll");
	if (term->t_xstate & TWCOLL)
		mvwaddstr(win, 14, 25, "twcoll");
	if (term->t_xstate & TECOLL)
		mvwaddstr(win, 14, 32, "tecoll");

	Set_Colour(win, Normal);
	wrefresh(win);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/03/13 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       Will print out uucp status if being used.                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

#define UUCP_DIR  "/usr/spool/uucp"
#define UUCP_STAT "/usr/spool/uucp/.Status/"
#define UUCP_LOCK "/usr/spool/uucp/LCK..tty"
#define UUCP_SYS  "/usr/spool/uucp/LCK.."

void update_status(WINDOW *win, char *tty_name)
{

register counter = 0;
char *tty_ptr;
char lock_name[128] = UUCP_LOCK;
char full_dir_name[128] = UUCP_SYS;
char stat_dir_name[128] = UUCP_STAT;
char trans_file_name[128] = UUCP_DIR;
char dev_name[3];
struct dirent *dir_ptr;
int uucp_pid = 0;
int LCK_pid = 0;
char sys_name[32];

struct stat file_info;

FILE *fd;
DIR *dir_fd;

tty_ptr = tty_name;

	/*
		Make tty name
	*/
	for (counter = 0; counter <= 7; counter++, tty_ptr++);
	strcpy(dev_name, tty_ptr);

	/*
		make lock name.
	*/
	strcpy(lock_name, UUCP_LOCK);
	strcat(lock_name, dev_name);

	Set_Colour(win, Colour_Blue);
	/*
		Is there a uucp LCK..ttyXX file if so get the uucp info.
	*/
	if ((access(lock_name, F_OK)) == 0) {

		/*
			Now open the lock file, get the pid, and look for the other
			lock files, to see if uucp is talking to a site.
			Note: uucp sets up a LCK..<sysname> straight away therefore
				if it does not exist then cu is being used.
		*/
		if (fd = fopen(lock_name, "r")) {
			fscanf(fd, "%d", &uucp_pid);
			fclose(fd);					/* close it */
			if (uucp_pid) {

				/*
					Now open the directory UUCP_DIR, and read all the
					files until I find one with a matching uucp name.
				*/
				if ((dir_fd = (opendir(UUCP_DIR))) == NULL) {
					mvwprintw(win, 3, 11, "uucp called with pid [%d]", uucp_pid);
				} else {
					while ((dir_ptr = readdir(dir_fd)) != NULL) {
						if (strncmp(dir_ptr->d_name, "LCK..tty", 8) MATCHES)
							continue;
						if (strncmp(dir_ptr->d_name, "LCK..", 5) MATCHES) {
							strcpy(full_dir_name, UUCP_DIR);
							strcat(full_dir_name, "/");
							strcat(full_dir_name, dir_ptr->d_name);
							if (fd = fopen(full_dir_name, "r")) {
								fscanf(fd,"%d", &LCK_pid);
								fclose(fd);

								if (LCK_pid == uucp_pid) {
									strcpy(sys_name, dir_ptr->d_name + 5);
									closedir(dir_fd);
									/*
										Now check for a file
										in the .Status dir.
									*/
									strcpy(stat_dir_name, UUCP_STAT);
									strcat(stat_dir_name, sys_name);
									mvwaddstr(win, 3, 11, "                                                        ");
									if ((access(stat_dir_name, F_OK)) == 0) {
										/*
											This is where I will get the status
										*/
										Set_Colour(win, Text_Warning);
										mvwprintw(win, 3, 11, "uucp TALKING to system \"%s\"", sys_name);
										for (counter = 0; counter <= 999; counter++) {
											sprintf(trans_file_name,"%s/%s/TM.%0.5d.%0.3d", UUCP_DIR, sys_name, uucp_pid, counter);
											if ((access(trans_file_name, F_OK)) == 0) {
												Set_Colour(win, Colour_Brown);
												if ((stat(trans_file_name, &file_info)) == 0) {
													if (last_tty_mode == FILE_NAME) {
														last_tty_mode = FILE_SIZE;
														mvwprintw(win, 3, 11, "uucp RECEIVING from \"%s\", file size: %d bytes", sys_name, file_info.st_size);
													} else {
														last_tty_mode = FILE_NAME;
														mvwprintw(win, 3, 11, "uucp RECEIVING from \"%s\", using TM.%0.5d.%0.3d", sys_name, uucp_pid, counter);
													}
												} else {
													mvwprintw(win, 3, 11, "uucp RECEIVING from \"%s\", using TM.%0.5d.%0.3d", sys_name, uucp_pid, counter);
												}
												counter += 999;
												break;
											}
										}
									} else {
										Set_Colour(win, Colour_Blue);
										mvwprintw(win, 3, 11, "uucp CALLING system \"%s\"", sys_name);
										Set_Colour(win, Normal);
										}
									break;
								}
							}
						} else {
							Set_Colour(win, Text_Normal);
							mvwprintw(win, 3, 11, "cu port lock [%s]", lock_name);
							Set_Colour(win, Normal);
						}
					}
				}
				closedir(dir_fd);
			} else {
				Set_Colour(win, Text_Alarm);
				mvwprintw(win, 3, 11, "can't find pid in file [%s]", lock_name);
				Set_Colour(win, Normal);
			}
		} else {
			Set_Colour(win, Text_Alarm);
			mvwprintw(win, 3, 11, "can't open lock file [%s]", lock_name);
			Set_Colour(win, Normal);
		}
	} else {
		mvwaddstr(win, 3, 11, "terminal enabled");
	}
	Set_Colour(win, Normal);
	return;

}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

