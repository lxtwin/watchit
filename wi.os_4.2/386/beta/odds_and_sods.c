/*
 * +--------------------------------------------------------------------+
 * | Function: odds+sods.c                               Date: 92/04/04 |
 * | Author: Paul Ready.                                              |
 * |                                                                    |
 * | Notes:                                                             |
 * |           The bit that holds all the odds and sods.                |
 * +--------------------------------------------------------------------+
 *
 * Note:
 *		None Yet.
 *
 * Updates:   
 *          None yet.
 *
 * Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%	Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%	Version %I% %D% - Dapix"
#endif /* __STDC__ */

#include "wi.h"

#include <fcntl.h>				/* File controll header */
#include <sys/machdep.h>			/* machine dependent fl.*/
#include <sys/immu.h>				/* process flags */
#include <sys/region.h>				/* process flags */
#include <sys/proc.h>				/* process flags */

#include   <sys/unistd.h>

#define ESC '\033'

#define MSCSI	"/etc/conf/cf.d/mscsi"

#define F_SIZE 512
#define BUFSIZE F_SIZE + 1

char ha[F_SIZE],
 attach[F_SIZE];

void find_mscsi(char *buf);

extern int	w_kmemfd;
extern int	num_of_hash_disks;
extern int ansi_flag;

char *ttyname();
char *getenv();

void kmem_write(daddr_t k_addr, caddr_t c_addr, int len);
void kmem_read(caddr_t c_addr, daddr_t k_addr, int len);
void mem_read(caddr_t c_addr, daddr_t m_addr, int len);
void swap_read(caddr_t c_addr, daddr_t s_addr, int len);

int get_item(int *new_value, int min_val, int max_val);
int find_item(int *new_value, int min_val, int max_val);
int get_detial(int *new_value, int min_val, int max_val);
int check_for_dev(int major, int minor, char *dev_name);

void usage(void);
void help_message(void);
void an_option(void);
void any_key(void);
void Show_Graphics(void);

int change_time(int type_flag);
int Change_Sample_Type(void);
int Change_Disk_Type(int type_flag);
int get_Stp(void);
void screen_dump(void);
void Get_The_Avg(void);

void Fill_A_Box(register WINDOW *win, int S_Colour, int E_Colour);
void Join_A_Box(register WINDOW *win, int width, int nrows, int ncols, int begy, int begx, int S_Colour, int E_Colour);
void Draw_A_Box(register WINDOW *win, int width, int nrows, int ncols, int begy, int begx, int S_Colour, int E_Colour);
void Draw_A_Line(register WINDOW *win, int length, int width, int type, int begy, int begx, int S_Colour, int E_Colour);
void Draw_A_Bar(register WINDOW *win, int bar_value, int X_Pos, int Y_Pos, int Type, int Warning_Start, int Alarm_Start);
void Draw_The_Text(register WINDOW *win, int percent, int X_Pos, int Y_Pos, int Type, int Warning_Start, int Alarm_Start);

void w_clrtoeol(register WINDOW *win, int S_Colour, int E_Colour);
int set_run_colour(WINDOW *win, int run_flag);

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/03/20 |
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

void kmem_write(daddr_t k_addr, caddr_t c_addr, int len)
{

daddr_t lseek();

	if (lseek(w_kmemfd, k_addr, 0) == -1L) {
		(void)printf("/dev/kmem seek error addr %08lx", k_addr);
		beep();
	}
	if (write(w_kmemfd, c_addr, len) != len) {
		(void)printf("/dev/kmem write error len %d addr %08lx", len, k_addr);
		beep();
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void kmem_read()                          Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	A newer more moden read in one go.                          |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void kmem_read(caddr_t c_addr, daddr_t k_addr, int len)
{

extern int errno;
daddr_t lseek();

	if (lseek(kmemfd, k_addr, 0) == -1L) {
		printf("/dev/kmem read seek error addr %08lx", k_addr);
		beep();
	}

	if (read(kmemfd, c_addr, len) != len) {
		printf("/dev/kmem read error %d, len %d, addr %08lx",
			errno, len, k_addr);
		beep();
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void mem_read()                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	A newer mem read.                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void mem_read(caddr_t c_addr, daddr_t m_addr, int len)
{

extern int errno;
daddr_t lseek();

	if (lseek(memfd, m_addr, 0) == -1L) {
		printf("/dev/mem seek err (%08lx)", m_addr);
		beep();
	}

	if (read(memfd, c_addr, len) != len) {
		printf("/dev/mem read errno [%d] len %d addr %08lx", errno, len, m_addr);
		beep();
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void swap_read()                          Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	A newer moden swap read in one go.                          |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void swap_read(caddr_t c_addr, daddr_t s_addr, int len)
{

daddr_t lseek();

	if (lseek(swapfd, s_addr, 0) == -1L) {
		(void)printf("/dev/swap seek err (%08lx)", s_addr);
		beep();
	}

	if (read(swapfd, c_addr, len) != len) {
		(void)printf("/dev/swap read len %d addr %08lx", len, s_addr);
		beep();
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int get_item()                            Date: 93/02/27 |
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

int get_item(int *new_value, int min_val, int max_val)
{

WINDOW *win;
char tmp_val[16];

	tmp_val[0] = '\0';

	win = newwin(3, 46, 12, 10);
	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);
	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 0, 1, " Slot Search ");
	Set_Colour(win, Normal);
	while (TRUE) {
		mvwprintw(win, 1, 25, "                   ");
		mvwprintw(win, 1, 2, "Select a value between %d and %d: ",
			min_val, max_val);
		wrefresh(win);
		wgetstr(win, tmp_val);
		if (tmp_val[0] == 'q') {
			delwin(win);
			touchwin(main_win);
			wrefresh(main_win);
			return(1);
		}
		*new_value = atoi(tmp_val);
		if (atoi(tmp_val) >= min_val && atoi(tmp_val) <= max_val) {
			if (size_flag) {
				if (atoi(tmp_val) >= (max_val - 31))
					*new_value = (max_val - 30);
			} else {
				if (atoi(tmp_val) >= (max_val - 12))
					*new_value = (max_val - 12);
			}
			delwin(win);
			touchwin(main_win);
			wrefresh(main_win);
			return(0);
		}
	}
}



/*
 * +--------------------------------------------------------------------+
 * | Function: int find_item()                            Date: %D% |
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

WINDOW *win;
char tmp_val[16];

	tmp_val[0] = '\0';

	win = newwin(3, 46, 12, 10);
	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);
	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 0, 1, " Slot Lookup ");
	Set_Colour(win, Normal);


	while (TRUE) {
		mvwprintw(win, 1, 25, "                   ");
		mvwprintw(win, 1, 2, "Select a value between %d and %d: ",
			min_val, max_val);
		wrefresh(win);
		wgetstr(win, tmp_val);
		if (tmp_val[0] == 'q') {
			delwin(win);
			touchwin(main_win);
			wrefresh(main_win);
			return(1);
		}

		*new_value = atoi(tmp_val);
		if (atoi(tmp_val) >= min_val && atoi(tmp_val) <= max_val) {
			delwin(win);
			touchwin(main_win);
			wrefresh(main_win);
			return(0);
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int get_detail()                          Date: 93/02/27 |
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

int get_detial(int *new_value, int min_val, int max_val)
{

WINDOW *win;
char tmp_val[16];

	tmp_val[0] = '\0';

	win = newwin(3, 46, 12, 12);
	Draw_A_Box(win, BUTTON_BOX);

	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 0, 1, " Detail Search ");
	Set_Colour(win, Normal);
	while (TRUE) {
		mvwprintw(win, 1, 25, "                    ");
		mvwprintw(win, 1, 1, " Select a value between %d and %d: ",
			min_val, max_val);
		wrefresh(win);

		wgetstr(win, tmp_val);
		if (tmp_val[0] == 'q') {
			delwin(win);
			touchwin(main_win);
			return(1);
		}
		*new_value = atoi(tmp_val);
		if (atoi(tmp_val) >= min_val && atoi(tmp_val) <= max_val) {
			delwin(win);
			touchwin(main_win);
			return(0);
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void check_for_dev()                      Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        A function to check the major and minor node numbers        |
 * |        against an array that was passed.                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int check_for_dev(int major, int minor, char *dev_name)
{

register ctr;

	for (ctr = 0; ctr <= num_of_hash_disks; ctr++) {
		if ((major == nodelist[ctr].major_node) &&
		    (minor == nodelist[ctr].minor_node)) {
			strcpy(dev_name, nodelist[ctr].bdev_name);
			return(0);
		}
	}
	return(1);
}


/*
 * +--------------------------------------------------------------------+
 * | Function: int usage()                               Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	Print out the usage and exit.                               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void usage()
{
	fprintf(stderr, "\007usage: wi -dlvhtosr -N[namelist] -C[corefile] -S[swapfile]\n");
	exit(1);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int usage()                               Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	Print out the usage and exit.                               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void help_message()
{
	printf("\007usage: wi -dlvhtosr -N[namelist] -C[corefile] -S[swapfile]\n\n");
	printf("	-d	-	use device numbers not names		(option)\n");
	printf("	-l	-	start with long screen display		(option)\n");
	printf("	-t	-	don't build hash table			(option)\n");
	printf("	-o	-	over ride screen length			(option)\n");
	printf("	-v	-	print the version			(option)\n");
	printf("	-s	-	print the serial number			(option)\n");
	printf("	-N X	-	use namelist X   		 	(option)\n");
	printf("	-C Y	-	use corefile Y   		 	(option)\n");
	printf("	-S Z	-	use swapfile Z   		 	(option)\n");
	printf("	-r	-	run the split version of wi		(option)\n");
	printf("	-h	-	print this message			(option)\n");
	printf("\nExamples");
	printf("\n	wi -l -N /unix.old		<Enter>\n");
	printf("\n	wi -r	<Enter> (all other flags are disabled)\n\n");
	exit(1);
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

void an_option()
{

extern WINDOW *bottom_win;

	mvwaddstr(bottom_win, 1, 8, ": An option or Quit:                              ");
	Set_Colour(bottom_win, Colour_White);
	mvwaddstr(bottom_win, 1, 2, "Select");
	Set_Colour(bottom_win, Colour_Banner);
	mvwaddstr(bottom_win, 1, 23, "Q");
	Set_Colour(bottom_win, Normal);
	Draw_A_Box(bottom_win, DEFAULT_BOX);
	wmove(bottom_win, 1, 29);
	wrefresh(bottom_win);

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

void any_key()
{

extern WINDOW *bottom_win;

	mvwaddstr(bottom_win, 1, 8, ": Any key to continue:   ");
	w_clrtoeol(bottom_win, 0, 0);
	Set_Colour(bottom_win, Colour_White);
	mvwaddstr(bottom_win, 1, 2, "Select");
	Set_Colour(bottom_win, Colour_Banner);
	mvwaddstr(bottom_win, 1, 10, "Any key");
	Set_Colour(bottom_win, Normal);
	Draw_A_Box(bottom_win, DEFAULT_BOX);
	wmove(bottom_win, 1, 30);
	wnoutrefresh(bottom_win);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int change_time(void)                     Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        change the time when the arrow keys are used.               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int change_time(int type_flag)
{

extern int Sleep_Time;
WINDOW *win;

	if (size_flag)
		win = newwin(3, 24, 23, 10);
	else
		win = newwin(3, 24, 12, 10);

	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);
	mvwaddstr(win, 1, 1, " Sample Time at:    ");
	Set_Colour(win, Colour_Banner);
	mvwprintw(win, 1, 18, "%d", Sleep_Time);
	Set_Colour(win, Normal);
	wrefresh(win);

	while (TRUE) {
		mvwaddstr(bottom_win, 1, 2, "Press Q when finished:");
		w_clrtoeol(bottom_win, 0, 0);
		Set_Colour(bottom_win, Colour_Banner);
		mvwaddstr(bottom_win, 1, 8, "Q");
		Set_Colour(bottom_win, Normal);
		mvwaddstr(bottom_win, 1, 24, " ");
		wrefresh(bottom_win);

		switch(wgetch(bottom_win)) {
		case '+' : /* increase sleep time */
		case KEY_UP:
		case KEY_RIGHT:

			if (type_flag) {
				Set_Colour(main_win, Colour_Banner);
				mvwaddstr(main_win, 0, 72, "-");
				wattron(main_win, A_BLINK);
				mvwaddstr(main_win, 0, 75, "+");
				wrefresh(main_win);
			}
			Set_Colour(main_win, Normal);

			if (Sleep_Time >= Sleep_Time_Max) {
				mvwaddstr(win, 1, 2, " Sample Time at:   ");
				Set_Colour(win, Colour_Banner);
				mvwprintw(win, 1, 19, "%d", Sleep_Time);
				Set_Colour(win, Normal);
				beep();
			} else {
				Sleep_Time++;
				mvwaddstr(win, 1, 2, "New Sample Time:   ");
				Set_Colour(win, Colour_Banner);
				mvwprintw(win, 1, 19, "%d", Sleep_Time);
				Set_Colour(win, Normal);
			}
			wrefresh(win);
			continue;

		case '-' : /* decrease sleep time */
		case KEY_DOWN:
		case KEY_LEFT:

			if (type_flag) {
				Set_Colour(main_win, Colour_Banner);
				mvwaddstr(main_win, 0, 72, "+");
				wattron(main_win, A_BLINK);
				mvwaddstr(main_win, 0, 75, "-");
				wrefresh(main_win);
			}
			Set_Colour(win, Normal);

			if (Sleep_Time <= Sleep_Time_Min) {
				mvwaddstr(win, 1, 2, " Sample Time at:   ");
				Set_Colour(win, Colour_Banner);
				mvwprintw(win, 1, 19, "%d", Sleep_Time);
				beep();
			} else {
				Sleep_Time--;
				mvwaddstr(win, 1, 2, "New Sample Time:   ");
				Set_Colour(win, Colour_Banner);
				mvwprintw(win, 1, 19, "%d", Sleep_Time);
			}
			Set_Colour(win, Normal);
			wrefresh(win);
			continue;

		case 'Q' :	/* Quit and exit */
		case 'q' :
		case ESC:

			if (type_flag) {
				Set_Colour(main_win, Colour_Banner);
				mvwaddstr(main_win, 0, 72, "+");
				mvwaddstr(main_win, 0, 75, "-");
				Set_Colour(main_win, Normal);
			}

			delwin(win);
			touchwin(main_win);
			if (type_flag)
				wrefresh(main_win);

			return(0);

		default :
			continue;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int Change_Sample_Type(void)              Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        change the time when the arrow keys are used.               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int Change_Sample_Type()
{

extern int Sleep_Time;
WINDOW *win;

	if (size_flag)
		win = newwin(5, 22, 27, 10);
	else
		win = newwin(5, 22, 12, 10);

	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);
	mvwaddstr(win, 1, 2, "The Current Sample");
	mvwaddstr(win, 2, 2, "Since System Boot");
	mvwaddstr(win, 3, 2, "From Watchit Start");
	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 1, 6, "C");
	mvwaddstr(win, 2, 15, "B");
	mvwaddstr(win, 3, 7, "W");
	Set_Colour(win, Normal);
	wnoutrefresh(win);
	an_option();
	doupdate();

	while (TRUE) {
		switch(wgetch(bottom_win)) {
		case 'B' :
		case 'b' :
			The_Current_Avg = BOOT_AVG;
			mvwaddstr(bottom_win, 1, 2, " Watchit average is from the system boot.");
			break;

		case 'W' :
		case 'w' :
			The_Current_Avg = WATCHIT_AVG;
			mvwaddstr(bottom_win, 1, 2, " Watchit average is from start of watchit.");
			break;

		case 'C' :
		case 'c' :
			The_Current_Avg = CURRENT_AVG;
			mvwaddstr(bottom_win, 1, 2, " Watchit average is current average.");
			break;

		case 'Q' :	/* Quit and exit */
		case 'q' :
		case ESC:

			delwin(win);
			touchwin(main_win);
			return(0);

		default :
			continue;
		}
		Draw_The_Mode();
		wnoutrefresh(bottom_win);
		doupdate();
		nap(500);
		delwin(win);
		touchwin(main_win);
		return(0);
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

void Show_Graphics(void)
{

int	NAP_Time = 0;

	/*
		Set up the time.
	*/
	NAP_Time = 1000;

	if (Graphics_Mode == SCREEN_D_MODE) {
		Graphics_Mode = GRAPHICS_MODE;
		mvwaddstr(bottom_win, 1, 2, " Console Capture graphics mode disabled.");
	} else {
		Graphics_Mode = SCREEN_D_MODE;
		mvwaddstr(bottom_win, 1, 2, " Console Capture graphics mode enabled.");
	}

	w_clrtoeol(bottom_win, 0, 0);
	Draw_A_Box(bottom_win, DEFAULT_BOX);
	wrefresh(bottom_win);
	nap(NAP_Time);
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

void clear_the_screen(void)
{

	mvwaddstr(bottom_win, 1, 2, " Re-setting the screen - Please wait.......");
	w_clrtoeol(bottom_win, 0, 0);
	Draw_A_Box(bottom_win, DEFAULT_BOX);
	wrefresh(bottom_win);
	nap(500);

	clearok(main_win, TRUE);
	touchwin(main_win);
	wrefresh(main_win);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: screen_dump(void)                         Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |                                                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          Added a quite mode.
 *							Sat Aug 21 23:33:35 EDT 1993 - PKR.
 * Bugs:
 *          None yet.
 */

void screen_dump(void)
{

struct {

int s_code;
int s_cols;
int s_lines;
} scrtab[] = {
	{ M_B40x25,         40, 25 },
	{ M_C40x25,         40, 25 },
	{ M_B80x25,         80, 25 },
	{ M_C80x25,         80, 25 },
	{ M_EGAMONO80x25,   80, 25 },
	{ M_ENH_B40x25,     40, 25 },
	{ M_ENH_C40x25,     40, 25 },
	{ M_ENH_B80x25,     80, 25 },
	{ M_ENH_C80x25,     80, 25 },
	{ M_VGA_40x25,      40, 25 },
	{ M_VGA_80x25,      80, 25 },
	{ M_VGA_M80x25,     80, 25 },
	{ M_ENH_B80x43,     80, 43 },
	{ M_ENH_C80x43,     80, 43 },
	{ -1,               -1, -1 },
};

FILE *fd;

char	logfile[32] = "",
		 *disp_mem;

int	i, disp_fd, num_cols,
	disp_mode, num_lines;

int	NAP_Time = 1000;

	/*
		Set up the time.
	*/
	NAP_Time = 1000;

	/*
		OK check to see if the mcap prog is there.
	*/
	if ((access("/etc/watchit/bin/mcap", X_OK)) == -1) {
		mvwaddstr(bottom_win, 1, 2, " Sorry I could not find \"/etc/watchit/bin/mcap\"");
		w_clrtoeol(bottom_win, 0, 0);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wrefresh(bottom_win);
		nap(NAP_Time);
		return;
	}

 	sprintf(logfile , "/tmp/mcap%s", The_Current_tty);

	if ((disp_fd = open("/dev/tty", O_RDWR)) < 0) {
		mvwaddstr(bottom_win, 1, 2, " Sorry I could not open \"/dev/tty\".");
		w_clrtoeol(bottom_win, 0, 0);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wrefresh(bottom_win);
		nap(NAP_Time);
		return;
	}

	if ((disp_mode = ioctl(disp_fd, CONS_GET, 0)) < 0) {
		mvwprintw(bottom_win, 1, 2, " Sorry I could not get the display type for /dev/tty%s ", The_Current_tty);
		w_clrtoeol(bottom_win, 0, 0);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wrefresh(bottom_win);
		close(disp_fd);
		nap(NAP_Time);
		return;
	}

	if ((disp_mem = (char *) ioctl(disp_fd, MAPCONS, 0)) == NULL) {
		mvwaddstr(bottom_win, 1, 2, " Sorry I could not map display memory.");
		w_clrtoeol(bottom_win, 0, 0);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wrefresh(bottom_win);
		nap(NAP_Time);
		close(disp_fd);
		return;
	}

	for (i = 0; scrtab[i].s_code >= 0 && scrtab[i].s_code != disp_mode; ++i);

	num_cols = scrtab[i].s_cols;
	num_lines = scrtab[i].s_lines;
	if (num_cols < 0 || num_lines < 0) {
		mvwaddstr(bottom_win, 1, 2, " Sorry this display is not supported.");
		w_clrtoeol(bottom_win, 0, 0);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wrefresh(bottom_win);
		nap(NAP_Time);
		close(disp_fd);
		return;
	}
	close(disp_fd);

	if ((fd = fopen(logfile, "a")) == NULL) {
		mvwprintw(bottom_win, 1, 2, " Sorry I could not open %s", logfile);
		w_clrtoeol(bottom_win, 0, 0);
		Draw_A_Box(bottom_win, DEFAULT_BOX);
		wrefresh(bottom_win);
		nap(NAP_Time);
		return;
	}

	chmod(logfile, 0x1a4);
	fputc('\014', fd);
	fputc('\n', fd);
	fflush(fd);
	fclose(fd);

	mvwprintw(bottom_win, 1, 2, " Capturing the screen to file %s - Please wait.......", logfile);
	w_clrtoeol(bottom_win, 0, 0);
	Draw_A_Box(bottom_win, DEFAULT_BOX);
	wrefresh(bottom_win);

	nap(500);
	system("/etc/watchit/bin/mcap");
	nap(NAP_Time);
	mvwaddstr(bottom_win, 1, 2, "                                                                  ");
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

 void Get_The_Avg(void)
 {

extern int The_Current_Avg;

	switch(The_Current_Avg) {
		case CURRENT_AVG:
			The_Current_Avg = BOOT_AVG;
			mvwaddstr(bottom_win, 1, 2, " Watchit average is from the system boot.");
			break;

		case BOOT_AVG:
			The_Current_Avg = WATCHIT_AVG;
			mvwaddstr(bottom_win, 1, 2, " Watchit average is from start of watchit.");
			break;

		case WATCHIT_AVG:
			The_Current_Avg = CURRENT_AVG;
			mvwaddstr(bottom_win, 1, 2, " Watchit average is current average.");
			break;
	}
	wnoutrefresh(bottom_win);
	return;
 }

/*
 * +--------------------------------------------------------------------+
 * | Function: int Change_Disk_Type(void)                Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        change the time when the arrow keys are used.               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int Change_Disk_Type(int type_flag)
{

extern int Sleep_Time;
extern int The_Disk_Avg;

WINDOW *win;

	if (size_flag) {
		win = newwin(8, 22, 11, 10);
		Set_Colour(win, Normal);
		Fill_A_Box(win, 0, 0);
		Draw_A_Box(win, BUTTON_BOX);
		mvwaddstr(win, 1, 2, "Disk Averages (1)");
		mvwaddstr(win, 2, 2, "Disk Averages (2)");
		mvwaddstr(win, 4, 2, "The Current Sample");
		mvwaddstr(win, 5, 2, "Since System Boot");
		mvwaddstr(win, 6, 2, "From Watchit Start");
		Set_Colour(win, Colour_Banner);
		mvwaddstr(win, 1, 17, "1");
		mvwaddstr(win, 2, 17, "2");
		mvwaddstr(win, 4, 6, "C");
		mvwaddstr(win, 5, 15, "B");
		mvwaddstr(win, 6, 7, "W");
		Set_Colour(win, Normal);
	} else {
		win = newwin(4, 22, 11, 10);
		Set_Colour(win, Normal);
		Fill_A_Box(win, 0, 0);
		Draw_A_Box(win, BUTTON_BOX);
		mvwaddstr(win, 1, 2, "Disk Averages (1)");
		mvwaddstr(win, 2, 2, "Disk Averages (2)");
		Set_Colour(win, Colour_Banner);
		mvwaddstr(win, 1, 17, "1");
		mvwaddstr(win, 2, 17, "2");
	}
	Set_Colour(win, Normal);

	wnoutrefresh(win);
	an_option();
	doupdate();
	while (TRUE) {
		switch(wgetch(bottom_win)) {
		case '1' :
			The_Disk_Avg = DISK_AVG_1;
			mvwaddstr(bottom_win, 1, 2, " Watchit disk average set to default.");
			break;

		case '2' :
			The_Disk_Avg = DISK_AVG_2;
			mvwaddstr(bottom_win, 1, 2, " Watchit disk average set to sample 2.");
			break;

		case 'B' :
		case 'b' :
			if (size_flag) {
				The_Current_Avg = BOOT_AVG;
				mvwaddstr(bottom_win, 1, 2, " Watchit average is from the system boot.");
			} else {
				beep();
				continue;
			}
			break;

		case 'W' :
		case 'w' :
			if (size_flag) {
				The_Current_Avg = WATCHIT_AVG;
				mvwaddstr(bottom_win, 1, 2, " Watchit average is from start of watchit.");
			} else {
				beep();
				continue;
			}
			break;

		case 'C' :
		case 'c' :
			if (size_flag) {
				The_Current_Avg = CURRENT_AVG;
				mvwaddstr(bottom_win, 1, 2, " Watchit average is current average.");
			} else {
				beep();
				continue;
			}
			break;

		case 'Q' :	/* Quit and exit */
		case 'q' :
		case ESC:
			delwin(win);
			if (type_flag)
				drawdisp(20);
			else
				drawdisp(21);

			touchwin(main_win);
			return(0);

		default :
			an_option();
			beep();
			continue;
		}

		delwin(win); /* added this */
		touchwin(main_win);
		if (size_flag)
			Draw_The_Mode();

		if (type_flag)
			drawdisp(20);
		else
			drawdisp(21);

		disks_header();
		return(0);
	}
}


/*
 * +--------------------------------------------------------------------+
 * | Function: get_Stp(void)                             Date: %D% |
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

int get_Stp(void)
{

FILE	*infile_1;

char	buf[BUFSIZE];

int Stp_ctr = 0;

	if ((infile_1 = fopen(MSCSI, "r")) == NULL)
		return(1);

	while ((fgets(buf, sizeof buf, infile_1)) != NULL) {
		if (buf[0] != 42) {
			find_mscsi(buf);
			if ((strcmp(attach, "Stp")) MATCHES)
				Stp_ctr++;
		}
	}

	/*
		Can't find Stp in mtune file.
	*/
	fclose(infile_1);
	return(Stp_ctr);

	/* Now tell the user that we can't find the name he entered. */
}


/*
 * +--------------------------------------------------------------------+
 * | Function: find_mscsi()                              Date: %D% |
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

void find_mscsi(char *buf)
{

char *field_ptr;

	/*
		get the first string.
	*/
	for (field_ptr = ha;
		*buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++ = *buf++);
	*field_ptr = '\0';
	for (; *buf == '\t' || *buf == ' '; ++buf);

	/*
		Get the 2nd string.
	*/
	for (field_ptr = attach; *buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++= *buf++);
	*field_ptr = '\0';
	for (; *buf == '\t' || *buf == ' '; ++buf);
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

void Fill_A_Box(register WINDOW *win, int S_Colour, int E_Colour)
{

int cols = 0;
int lines = 0;

int register x = 0, y = 0;

	cols = win->_maxx;
	lines = win->_maxy - 1;

	if (E_Colour)
		Set_Colour(win, E_Colour);

	wmove(win, 0, 0);
	for (y = 0; y < lines; y++)
		for (x = 0; x < cols; x++)
			waddch(win,' ');

	wnoutrefresh(win);

	if (E_Colour)
		Set_Colour(win, E_Colour);
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

void Draw_A_Box(register WINDOW *win, int width, int nrows, int ncols, int begy, int begx, int S_Colour, int E_Colour)
{

int register x_ctr, y_ctr;

	if (!(ansi_flag))
		width = THIN_LINE;

	if (!(S_Colour))
		S_Colour = Colour_White;

	if (!(E_Colour))
		E_Colour = Colour_Black;

	/*
		Check to see if a default "box" should be drawn
	*/
	if (!(nrows && ncols)) {
		begy = 0;
		begx = 0;
		ncols = win->_maxx - 1;
		nrows = win->_maxy - 1;
	}

	Set_Colour(win, Normal);
	wattron(win, A_ALTCHARSET);
	for (x_ctr = 1; x_ctr < ncols; x_ctr++) {
		Colour(win, S_Colour);
		mvwaddch(win, begy, (begx + x_ctr), ACS_HLINE);
		wattroff(win, A_BOLD);
		Colour(win, E_Colour);
		if (width == THIN_LINE)
			mvwaddch(win, (begy + nrows), (begx + x_ctr), ACS_HLINE);
		else
			mvwaddch(win, (begy + nrows), (begx + x_ctr), ANSI_BOX_HLINE);
	}

	for (y_ctr = 1; y_ctr < nrows; y_ctr++) {
		Colour(win, S_Colour);
		mvwaddch(win, (begy + y_ctr), begx, ACS_VLINE);
		wattroff(win, A_BOLD);
		Colour(win, E_Colour);
		if (width == THIN_LINE)
			mvwaddch(win, (begy + y_ctr), (begx + ncols), ACS_VLINE);
		else
			mvwaddch(win, (begy + y_ctr), (begx + ncols), ANSI_BOX_VLINE);
	}

	Colour(win, S_Colour);
	mvwaddch(win, begy, begx, ACS_ULCORNER);
	mvwaddch(win, begy + nrows, begx, ACS_LLCORNER);
	if (width == THICK_LINE)
		mvwaddch(win, begy + nrows, begx, ANSI_BOX_LLCORNER);
	wattroff(win, A_BOLD);

	Colour(win, E_Colour);
	mvwaddch(win, begy, begx + ncols, ACS_URCORNER);
	mvwaddch(win, begy + nrows, begx + ncols, ACS_LRCORNER);
	if (width == THICK_LINE) {
		mvwaddch(win, begy, begx + ncols, ANSI_BOX_URCORNER);
		mvwaddch(win, begy + nrows, begx + ncols, ANSI_BOX_LRCORNER);
	}
	Set_Colour(win, Normal);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |         The code can check if y = x then draw vert.                |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void Draw_A_Line(register WINDOW *win, int length, int width, int type, int begy, int begx, int S_Colour, int E_Colour)
{

register int counter = 0;

	if (!(ansi_flag))
		width = THIN_LINE;

	if (S_Colour)
		Set_Colour(win, S_Colour);
	wattron(win, A_ALTCHARSET);
	if (type == VERT_LINE) {
		/*
			Draw a vertical line. 
		*/
		for (counter = 1; counter < length; counter++) {
			if (width == THIN_LINE)
				mvwaddch(win, (begx + counter), begx, ACS_VLINE);
			else
				mvwaddch(win, (begx + counter), begx, ANSI_ACS_VTHICK);
		}
	} else {
		/*
			Draw a Horozental line. 
		*/
		for (counter = 1; counter < length; counter++) {
			if (width == THIN_LINE)
				mvwaddch(win, begy, (begx + counter), ACS_HLINE);
			else
				mvwaddch(win, begy, (begx + counter), ANSI_ACS_HTHICK);
		}
	}

	wattroff(win, A_ALTCHARSET);
	if (E_Colour)
		Set_Colour(win, E_Colour);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: Draw_A_Bar(lots and lots)                 Date: %D% |
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

void Draw_A_Bar(register WINDOW *win, int percent, int X_Pos, int Y_Pos, int Type, int Warning_Start, int Alarm_Start)
{

register int ctr = 0;
register int pos = 0;
char bar_chart[64];
int bar_type;

	/*
		The bar_value, Warning_Start and Alarm_Start will allways a
		percentage therefore I will divide it by 2
	*/

	Set_Colour(win, Normal);

	/*
		Build the light background colour, this is a rough one and I
		should be able to do this on one pass.
	*/
	wattron(win, A_ALTCHARSET);
	for (pos = 0, ctr = 0; ctr <= 100; ctr +=2, pos++) {
	/*
		Set the type
	*/
		/*
			Set the colour and print out the char
		*/
		if (ctr >= percent) {
			bar_type = ANSI_ACS_CKBOARD;
			if (Type == NORMAL) {
				if (ctr < Warning_Start)
					Colour(win, State_Normal);
				else if (ctr >= Alarm_Start)
					Colour(win, State_Alarm);
				else
					Colour(win, State_Warning);
			} else {
				if (ctr < Alarm_Start)
					Colour(win, State_Alarm);
				else if (ctr < Warning_Start)
					Colour(win, State_Warning);
				else
					Colour(win, State_Normal);
			}
		} else {
			bar_type = ANSI_ACS_SOLID;
			if (Type == NORMAL) {
				if (percent < Warning_Start)
					Colour(win, State_Normal);
				else if (percent >= Alarm_Start)
					Colour(win, State_Alarm);
				else
					Colour(win, State_Warning);
			} else {
				if (percent < Alarm_Start)
					Colour(win, State_Alarm);
				else if (percent < Warning_Start)
					Colour(win, State_Warning);
				else
					Colour(win, State_Normal);
			}
		}

		/* Print out the value and an exter char if it == 100% */
		if ((ctr == 100) && (percent == 100))
			bar_type = ANSI_ACS_SOLID;

		mvwaddch(win, X_Pos, Y_Pos + pos, bar_type);
		wattroff(win, A_BOLD);
	}
	wnoutrefresh(win);
	Set_Colour(win, Normal);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: Draw_The_Text(lots and lots)               Date: %D% |
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

void Draw_The_Text(register WINDOW *win, int percent, int X_Pos, int Y_Pos, int Type, int Warning_Start, int Alarm_Start)
{

	/*
		Set the colour and print out the char
	*/
	Set_Colour(win, Normal);
	if (Type == NORMAL) {
		if (percent < Warning_Start)
			Colour(win, Text_Normal);
		else if (percent >= Alarm_Start)
			Colour(win, Text_Alarm);
		else
			Colour(win, Text_Warning);
	} else {
		if (percent < Alarm_Start)
			Colour(win, Text_Alarm);
		else if (percent < Warning_Start)
			Colour(win, Text_Warning);
		else
			Colour(win, Text_Normal);
	}
	/*
		If the X_Pos and Y_Pos was set to COLOUR_ONLY then return
	*/
	if ((X_Pos == -696969) && (X_Pos == -696969))
		return;

	mvwprintw(win, X_Pos, Y_Pos, "%3d", percent);
	wnoutrefresh(win);
	Set_Colour(win, Normal);
	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function:                                            Date: %D% |
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


void w_clrtoeol(register WINDOW *win, int S_Colour, int E_Colour)
{

register int x;
int end;

	x = win->_curx;
	end = win->_maxx;
	if (S_Colour)
		Set_Colour(win, S_Colour);

	for (x++ ; x < end; x++)
		waddch(win, ' ');

	wnoutrefresh(win);
	if (E_Colour)
		Set_Colour(win, E_Colour);
	return;
}

/*
 +-----------------------------------------------------------------------+
 |                            END OF PROGRAM.                            |
 +-----------------------------------------------------------------------+
 */

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

void Join_A_Box(register WINDOW *win, int width, int nrows, int ncols, int begy, int begx, int S_Colour, int E_Colour)
{

	if (size_flag)
		return;

	if (!(ansi_flag))
		width = THIN_LINE;

	if (!(S_Colour))
		S_Colour = Colour_White;

	if (!(E_Colour))
		E_Colour = Colour_Black;

	/*
		Check to see if a default "box" should be drawn
	*/
	if (!(nrows && ncols)) {
		begy = 0;
		begx = 0;
		ncols = win->_maxx - 1;
		nrows = win->_maxy - 1;
	}

	wattron(win, A_ALTCHARSET);
	Colour(win, S_Colour);
	mvwaddch(win, begy + nrows, begx, ACS_LTEE);
	if (width == THICK_LINE)
		mvwaddch(win, begy + nrows, begx, ANSI_BOX_LTEE);
	wattroff(win, A_BOLD);

	Colour(win, E_Colour);
	mvwaddch(win, begy + nrows, begx + ncols, ACS_RTEE);
	if (width == THICK_LINE) {
		mvwaddch(win, begy + nrows, begx + ncols, ANSI_BOX_RTEE);
	}
	Set_Colour(win, Normal);
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



int set_run_colour(WINDOW *win, int run_flag)
{

	switch(run_flag) {
		case	0 :			/* Free */
			Set_Colour(win, Text_Warning);
			break;

		case	SSLEEP :		/* awaiting an event */
			Set_Colour(win, Colour_Black);
			break;

		case	SRUN   :		/* running */
			Set_Colour(win, Text_Normal);
			break;

		case	SZOMB  :		/* process terminated but not waited for */
			Set_Colour(win, Text_Alarm);
			break;

		case	SSTOP  :		/* process stopped by debugger */
			Set_Colour(win, Colour_Blue);
			break;

		case	SIDL   :		/* intermediate state in process creation */
			Set_Colour(win, Colour_White);
			break;

		case	SONPROC :		/* process is being run on a processor */
			Set_Colour(win, Colour_Brown);
			break;

		case SXBRK   :		/* process being xswapped */
			Set_Colour(win, Colour_Magenta);
			break;

		default :			/* Default Yellow */
			Set_Colour(win, Colour_Magenta);
			break;
		}

	return(0);
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

