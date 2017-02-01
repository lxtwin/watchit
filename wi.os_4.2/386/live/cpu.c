/*
 * +--------------------------------------------------------------------+
 * | Function: cpu.c                                     Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the cpu and wait stats                                 |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *(1) v2.2	The XENIX version for long screen is neater as I completed 
 *		it last, in v3.X I will rewrite the UNIX CPU screen.
 *						Sat Jul 17 18:34:47 EDT 1993 - PKR.
 *
 * (2) v2.3	I will make a compressed screen so that it will show the 
 *		wait state, this was orginal left out because it was ported
 *		from xenix and xenix does not have a wait state listed in 
 *		the kernel. I will set up a pipe system to pass the screen
 *		format to the child proc.
 *						Sat Jul 17 18:34:47 EDT 1993 - PKR.
 *
 *		I have decided to use shm as the speep times is better.
 *						Mon Aug 09 14:13:54 EDT 1993 - PKR.
 * (3) v3.0
 *		Updated to take out the shm bit.
 *						Tue Nov 09 10:20:26 EST 1993 - PKR.
 *		And a major re-write, as it is now split into sections.
 *		Removed all ref's to cpu_win, added a function Draw_A_Box
 *		works, well.
 *						Tue Nov 30 00:13:48 EST 1993 - PKR.
 *
 * Finished:
 *(1) v2.2 Rewrote the CPU calc. looks better but could be rewritten,
 *		for smaller code. I don't like repeating my self.
 *						Mon Jul 19 18:34:47 EDT 1993 - PKR.
 *
 * (2) v2.3	Added the compress screen using shm, cut down the size
 *		of the segments. from 64L to 8L - OK.
 *						Mon Aug 09 15:10:13 EDT 1993 - PKR.
 *
 *		Set it up so that It would always be reset at the start of
 *		the CPU screen because other parts might use it.
 *						Tue Aug 17 22:29:54 EDT 1993 - PKR.
 *
 *    Bugs:
 *(1) v2.3	The swap part on the cpu screen cannot hold over 99 Meg
 *		for swap the display.				
 *						Sat Jul 17 18:34:47 EDT 1993 - PKR / Done.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%			Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%			Version %I% %D% - Dapix"
#endif /* __STDC__ */

#include "wi.h"

#include <sys/sysinfo.h>
#include	  <signal.h>				/* The signals and handler */

#define BARC_1 9
#define BARC_2 24

#define NO_SLEEP 0
#define DO_SLEEP 1

int compressed_flag = FALSE;

int cpu_menu(void);
void watch_cpu(void);
int wi_cpu(int sleep_flag);
void do_system_stats(void);
void do_small_stats(void);
void do_large_stats(int compressed_flag);
void get_cpu_stats(void);
void update_cpu_stats(void);
static void sig_alrm_cpu(int signo);
void Draw_cpu_screen(int Sleep_Time_Flag);

/*
 * +--------------------------------------------------------------------+
 * | Function: int cpu_menu(void)                        Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Do the cpu menu                                             |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int cpu_menu(void)
{

register int loop_ctr = 0;
int option;

	/*
		Reset all the flags.
	*/
	compressed_flag = TRUE;
	Fill_A_Box(main_win, 0, 0);
	Draw_A_Box(main_win, DEFAULT_BOX);
	drawdisp(5);
	mvwaddstr(main_win, 2, 4, "Reading /unix..........");
	wrefresh(main_win);
	mvwaddstr(main_win, 2, 4, "                         ");
	wi_cpu(DO_SLEEP);
	while (TRUE) {
		drawdisp(1);
		wrefresh(top_win);
		Set_Colour(bottom_win, Normal);
		mvwaddstr(bottom_win, 1, 1, "       : Watchit or Quit: ");
		w_clrtoeol(bottom_win, 0, 0);
		mvwprintw(bottom_win, 1, 72, ":");
		Set_Colour(bottom_win, Colour_White);
		mvwaddstr(bottom_win, 1, 2, "Select");
		mvwprintw(bottom_win, 1, 61, "Sample time");
		Set_Colour(bottom_win, Colour_Banner);
		mvwaddstr(bottom_win, 1, 10, "W");
		mvwaddstr(bottom_win, 1, 21, "Q");
		mvwprintw(bottom_win, 1, 74, "%d", Sleep_Time);
		Set_Colour(bottom_win, Normal);
		wmove(bottom_win, 1, 27);
		wnoutrefresh(bottom_win);
		doupdate();
		option = wgetch(bottom_win);
		switch (option) {
		case 'L' & 0x1F :
		case 'R' & 0x1F :
			clear_the_screen();
			continue;

		case 'P' :
		case 'p' :
			screen_dump();
			continue;

		case 'R' :
		case 'r' :
			cpu_averages[_AVG_CPU_IDLE] 	 = 0;
			cpu_averages[_AVG_CPU_USER] 	 = 0;
			cpu_averages[_AVG_CPU_KERNEL]	 = 0;
			cpu_averages[_AVG_CPU_WAIT]	 = 0;
			cpu_averages[_AVG_CPU_SXBRK]	 = 0;
			cpu_averages[_AVG_CPU_TOTAL]	 = 0;
			cpu_averages[_AVG_CPU_TOTAL_2] = 0;

			cpu_averages[_AVG_W_IO]		 = 0;
			cpu_averages[_AVG_W_SWAP]	 = 0;
			cpu_averages[_AVG_W_PIO]	 = 0;
			cpu_averages[_AVG_W_TOTAL]	 = 0;

			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_cpu(DO_SLEEP);
			continue;

		case 'G' :
		case 'g' :
			Show_Graphics();
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_cpu(DO_SLEEP);
			continue;

		case 'D' :
		case 'd' :
			if (size_flag) {
				beep();
				continue;
			}
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			if (!(size_flag)) {
				for (loop_ctr = 8; loop_ctr <= 17; loop_ctr++)
					mvwaddstr(main_win, loop_ctr, 1, "                                                                          ");
				if (compressed_flag == COMPRESSED_DISP) {
					compressed_flag = UNCOMPRESSED_DISP;
					mvwaddstr(bottom_win, 1, 2, " Wait I/O mode disabled.");
				} else {
					compressed_flag = COMPRESSED_DISP;
					mvwaddstr(bottom_win, 1, 2, " Wait I/O mode enabled.");
				}
			}
			w_clrtoeol(bottom_win, 0, 0);
			wrefresh(bottom_win);
			wi_cpu(DO_SLEEP);
			continue;

		case 'U' :
		case 'u' :
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_cpu(DO_SLEEP);
			continue;

		case 'W' :
		case 'w' :
			watch_cpu();
			drawdisp(5);
			wnoutrefresh(main_win);
			doupdate();
			continue;

		case 'Q' :
		case 'q' :
			return(0);

		default :
			beep();
			continue;
		}
	}
}


/*
 * +--------------------------------------------------------------------+
 * | Function: void get_cpu_stats(void)                  Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        the main cpu loop                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          Moved to allow alarm to work.
 *    Bugs:
 *          None yet.
 */

void get_cpu_stats(void)
{

	kmem_read(&last_minfo, namelist[NM_MINFO].n_value, sizeof(struct minfo));
	kmem_read(&last_sysinfo, namelist[NM_SYSINFO].n_value, sizeof(struct sysinfo));
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void update_cpu_stats(void)               Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        the main cpu loop                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          Moved to allow alarm to work.
 *    Bugs:
 *          None yet.
 */

time_t current_idle   = 0L;
time_t current_user   = 0L;
time_t current_kernel = 0L;
time_t current_wait   = 0L;
time_t current_total  = 0L;
time_t current_total2 = 0L;

time_t current_sxbrk      = 0L;
time_t current_io         = 0L;
time_t current_wswap      = 0L;
time_t current_pio        = 0L;
time_t current_wait_total = 0L;

void update_cpu_stats(void)
{

	current_idle   = 0L;
	current_user   = 0L;
	current_kernel = 0L;
	current_wait   = 0L;
	current_total  = 0L;

	current_sxbrk      = 0L;
	current_io         = 0L;
	current_wswap      = 0L;
	current_pio        = 0L;
	current_wait_total = 0L;

	kmem_read(&minfo, namelist[NM_MINFO].n_value, sizeof(struct minfo));
	kmem_read(&sysinfo, namelist[NM_SYSINFO].n_value, sizeof(struct sysinfo));

	kmem_read(&nswap, namelist[NM_SWAPBLKS].n_value, sizeof(nswap));
	kmem_read(&kavailmem, namelist[NM_KAVAILMEM].n_value, sizeof(kavailmem));

	current_user   = sys_value(cpu[CPU_USER]);
	current_idle   = sys_value(cpu[CPU_IDLE]);
	current_kernel = sys_value(cpu[CPU_KERNEL]);
	current_wait   = sys_value(cpu[CPU_WAIT]);
	current_sxbrk  = sys_value(cpu[CPU_SXBRK]);
	current_total  = current_user + current_idle + current_kernel +
		current_wait;
	current_total2 = current_user + current_idle + current_kernel;

	current_io         = sys_value(wait[W_IO]);
	current_wswap      = sys_value(wait[W_SWAP]);
	current_pio        = sys_value(wait[W_PIO]);
	current_wait_total = current_io + current_wswap + current_pio;

/*
 * +--------------------------------------------------------------------+
 * |                AVG TO BE WORKED OUT HERE                           |
 * +--------------------------------------------------------------------+
 */

	cpu_averages[_AVG_CPU_IDLE] 		+= current_idle;
	cpu_averages[_AVG_CPU_USER] 		+= current_user;
	cpu_averages[_AVG_CPU_KERNEL]		+= current_kernel;
	cpu_averages[_AVG_CPU_WAIT]		+= current_wait;
	cpu_averages[_AVG_CPU_SXBRK]		+= current_sxbrk;
	cpu_averages[_AVG_CPU_TOTAL]		+= current_total;
	cpu_averages[_AVG_CPU_TOTAL_2]	+= current_total2;

	cpu_averages[_AVG_W_IO]			+= current_io;
	cpu_averages[_AVG_W_SWAP]		+= current_wswap;
	cpu_averages[_AVG_W_PIO]		+= current_pio;
	cpu_averages[_AVG_W_TOTAL]		+= current_wait_total;

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void wi_cpu(int sleep_flag)                 Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        the main cpu loop                                           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          Will include the pipe stuff.
 *		Pipe, shm and other stuff taken out.
 *
 *    Bugs:
 *          None yet.
 */

int wi_cpu(int sleep_flag)
{

	if (sleep_flag) {
		get_cpu_stats();
		nap(1000);
		update_cpu_stats();
	}

	if (!(size_flag)) {
		if (compressed_flag)
			do_large_stats(compressed_flag);
		else
			do_small_stats();
	} else {
		do_small_stats();
		do_large_stats(FALSE);
	}

	do_system_stats();
	if (sleep_flag) {
		wnoutrefresh(main_win);
		doupdate();
	}
	return(0);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: do_system_stats()                         Date: 93/03/21 |
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

void do_system_stats(void)
{

int extra_bit = 0;

	/*
		Draw the box for all of this stuff.
	*/
	extra_bit = 0;
	if (size_flag) {
		extra_bit = 1;
		Draw_A_Box(main_win, THIN_LINE, 5, 71, 3, 3, Colour_White, Colour_Black);
	} else
		Draw_A_Box(main_win, THIN_LINE, 5, 71, 2, 3, Colour_White, Colour_Black);

	Set_Colour(main_win, Normal);
	mvwaddstr(main_win, 3 + extra_bit, 5, "User:      %");
	mvwaddstr(main_win, 4 + extra_bit, 5, "Kernel:    %");
	mvwaddstr(main_win, 5 + extra_bit, 5, "Idle:      %");
	mvwaddstr(main_win, 6 + extra_bit, 5, "Wait:      %");

	mvwaddstr(main_win, 3 + extra_bit, 19, "Sxbrk:        ");
	mvwaddstr(main_win, 4 + extra_bit, 19, "I/O:     :   %");
	mvwaddstr(main_win, 5 + extra_bit, 19, "PI/O:    :   %");
	mvwaddstr(main_win, 6 + extra_bit, 19, "Swap:    :   %");

	mvwaddstr(main_win, 3 + extra_bit, 34, "Total mem:       k");
	mvwaddstr(main_win, 4 + extra_bit, 34, "User mem:        k");
	mvwaddstr(main_win, 5 + extra_bit, 34, "Free mem:        k");
	mvwaddstr(main_win, 6 + extra_bit, 34, "Mem used:        %");

	mvwaddstr(main_win, 3 + extra_bit, 54, "Total swap:       k");
	mvwaddstr(main_win, 4 + extra_bit, 54, "Used swap:        k");
	mvwaddstr(main_win, 5 + extra_bit, 54, "Free swap:        k");
	mvwaddstr(main_win, 6 + extra_bit, 54, "Swap used:        %");

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 2 + extra_bit, 7, "Cpu Info");
	mvwaddstr(main_win, 2 + extra_bit, 21, "Wait Info");
	mvwaddstr(main_win, 2 + extra_bit, 38, "Memory Info");
	mvwaddstr(main_win, 2 + extra_bit, 59, "Swap Info");
	Set_Colour(main_win, Normal);

/*
 * +--------------------------------------------------------------------+
 * |                         CPU STATS                                  |
 * +--------------------------------------------------------------------+
 */

	Set_Colour(main_win, Normal);
	Draw_The_Text(main_win, ((current_user * 100) / current_total),   extra_bit + 3, 13, USER_BAR);
	Draw_The_Text(main_win, ((current_kernel * 100) / current_total), extra_bit + 4, 13, KERNEL_BAR);
	Draw_The_Text(main_win, ((current_idle * 100) / current_total),   extra_bit + 5, 13, IDLE_BAR);
	Draw_The_Text(main_win, ((current_wait * 100) / current_total),   extra_bit + 6, 13, WAIT_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                       WAITING STATS                                |
 * +--------------------------------------------------------------------+
 */

	mvwprintw(main_win, 3 + extra_bit, 25, "%3d", current_sxbrk);

	if (current_wait_total) {
		mvwprintw(main_win, 4 + extra_bit, 25, "%3d", current_io);
		mvwprintw(main_win, 4 + extra_bit, 29, "%3d", (current_io * 100) / current_wait_total);

		mvwprintw(main_win, 5 + extra_bit, 25, "%3d", current_pio);
		mvwprintw(main_win, 5 + extra_bit, 29, "%3d", (current_pio * 100) / current_wait_total);

		mvwprintw(main_win, 6 + extra_bit, 25, "%3d", current_wswap);
		mvwprintw(main_win, 6 + extra_bit, 29, "%3d", (current_wswap * 100) / current_wait_total);

	} else {

		mvwprintw(main_win, 4 + extra_bit, 25, "%3d", current_io);
		mvwaddstr(main_win, 4 + extra_bit, 29, " 33");

		mvwprintw(main_win, 5 + extra_bit, 25, "%3d", current_pio);
		mvwaddstr(main_win, 5 + extra_bit, 29, " 33");

		mvwprintw(main_win, 6 + extra_bit, 25, "%3d", current_wswap);
		mvwaddstr(main_win, 6 + extra_bit, 29, " 33");
	}

/*
 * +--------------------------------------------------------------------+
 * |                     MEMORY STATS                                   |
 * +--------------------------------------------------------------------+
 */
	mvwprintw(main_win, 3 + extra_bit, 45, "%6d", (kphysmem * NBPC) / 1024);
	mvwprintw(main_win, 4 + extra_bit, 45, "%6d", (kmaxmem * NBPC) / 1024);
	mvwprintw(main_win, 5 + extra_bit, 45, "%6d", (kavailmem * NBPC) / 1024);

	Draw_The_Text(main_win, (100 - (int)((kavailmem * 100) / kmaxmem)), extra_bit + 6, 48, MEMORY_BAR);


/*
 * +--------------------------------------------------------------------+
 * |                         SWAP STATS                                 |
 * +--------------------------------------------------------------------+
 */
	mvwprintw(main_win, 3 + extra_bit, 66, "%6d", (nswap * NBPSCTR) / 1024);
	mvwprintw(main_win, 4 + extra_bit, 66, "%6d", 
	    ((nswap * NBPSCTR) / 1024) - ((minfo.freeswap * NBPSCTR) / 1024));
	mvwprintw(main_win, 5 + extra_bit, 66, "%6ld", (minfo.freeswap * NBPSCTR) / 1024);

	Draw_The_Text(main_win, (100 - (int)((minfo.freeswap * 100) / nswap)), extra_bit + 6, 69, SWAP_BAR);

	Set_Colour(main_win, Normal);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: Do_Small_Stats()                          Date: 93/03/21 |
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

void do_small_stats(void)
{


int	extra_bit = 0;

	if (size_flag)
		extra_bit = 3;
	else
		extra_bit = 0;

/*
 * +--------------------------------------------------------------------+
 * |                       HEADER INFO                                  |
 * +--------------------------------------------------------------------+
 */
	Set_Colour(main_win, Normal);
	mvwprintw(main_win, BARC_1 + extra_bit, 10, "%s   used    avg", SCALE);
	mvwprintw(main_win, BARC_1 + extra_bit + 3, 10, "%s   used    avg",SCALE);
	mvwprintw(main_win, BARC_1 + extra_bit + 6, 10, "%s   used    avg", SCALE);

	mvwaddstr(main_win, BARC_1 + extra_bit + 1, 3, "User");
	mvwaddstr(main_win, BARC_1 + extra_bit + 4, 3, "Kernel");
	mvwaddstr(main_win, BARC_1 + extra_bit + 7, 3, "Idle");

/*
 * +--------------------------------------------------------------------+
 * |                         USER INFO                                  |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, BARC_1 + extra_bit + 1, 67, "%      %");
	Draw_The_Text(main_win, ((current_user * 100) / current_total2), BARC_1 + extra_bit + 1, 64, USER_BAR);

	Draw_The_Text(main_win,
	((cpu_averages[_AVG_CPU_USER] * 100) / cpu_averages[_AVG_CPU_TOTAL_2]),
	BARC_1 + extra_bit + 1, 71, USER_BAR);

	Draw_A_Bar(main_win, ((current_user * 100) / current_total2), BARC_1 + extra_bit + 1, 10, USER_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         KERNEL INFO                                |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, BARC_1 + extra_bit + 4, 67, "%      %");
	Draw_The_Text(main_win, ((current_kernel * 100) / current_total2), BARC_1 + extra_bit + 4, 64, KERNEL_BAR);

	Draw_The_Text(main_win, ((cpu_averages[_AVG_CPU_KERNEL] * 100) / cpu_averages[_AVG_CPU_TOTAL_2]), BARC_1 + extra_bit + 4, 71, KERNEL_BAR);

	Draw_A_Bar(main_win, ((current_kernel * 100) / current_total2), BARC_1 + extra_bit + 4, 10, KERNEL_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         IDLE INFO                                  |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, BARC_1 + extra_bit + 7, 67, "%      %");

	Draw_The_Text(main_win, ((current_idle * 100) / current_total2), BARC_1 + extra_bit + 7, 64, IDLE_BAR);

	Draw_The_Text(main_win, (100 - (((cpu_averages[_AVG_CPU_USER] * 100) / cpu_averages[_AVG_CPU_TOTAL_2]) + ((cpu_averages[_AVG_CPU_KERNEL] * 100) / cpu_averages[_AVG_CPU_TOTAL_2]))), BARC_1 + extra_bit + 7, 71, IDLE_BAR);

	Draw_A_Bar(main_win, ((current_idle * 100) / current_total2), BARC_1 + extra_bit + 7, 10, IDLE_BAR);

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: Do_Large_Stats()                          Date: 93/03/21 |
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

void do_large_stats(int compressed_flag)
{

int position = 0;

	Set_Colour(main_win, Normal);
	if (compressed_flag) {
		position = BARC_1;

		mvwprintw(main_win, position,     10, "%s   used    Avg", SCALE);
		mvwprintw(main_win, position + 2, 10, "%s   used    Avg", SCALE);
		mvwprintw(main_win, position + 5, 10, "%s   used    Avg", SCALE);
		mvwprintw(main_win, position + 7, 10, "%s   used    Avg", SCALE);

		mvwaddstr(main_win, position + 1, 3, "User");
		mvwaddstr(main_win, position + 3, 3, "Kernel");
		mvwaddstr(main_win, position + 6, 3, "Idle");
		mvwaddstr(main_win, position + 8, 3, "Wait");

	} else {
		position = BARC_2;
		mvwprintw(main_win, position, 10, "%s   used    Avg", SCALE);
		mvwprintw(main_win, position + 3, 10, "%s   used    avg", SCALE);
		mvwprintw(main_win, position + 6, 10, "%s   used    avg", SCALE);
		mvwprintw(main_win, position + 9, 10, "%s   used    avg", SCALE);

		mvwaddstr(main_win, position + 1, 3, "User");
		mvwaddstr(main_win, position + 4, 3, "Kernel");
		mvwaddstr(main_win, position + 7, 3, "Idle");
		mvwaddstr(main_win, position + 10, 3, "Wait");

	}

/*
 * +--------------------------------------------------------------------+
 * |                         USER INFO                                  |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, position + 1, 67, "%      %");
	Draw_The_Text(main_win, ((current_user * 100) / current_total), position + 1, 64, USER_BAR);

	Draw_The_Text(main_win,
	((cpu_averages[_AVG_CPU_USER] * 100) / cpu_averages[_AVG_CPU_TOTAL]),
	position + 1, 71, USER_BAR);

	Draw_A_Bar(main_win, ((current_user * 100) / current_total), position + 1, 10, USER_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         KERNEL INFO                                |
 * +--------------------------------------------------------------------+
 */

	if (compressed_flag)
		position--;

	mvwaddstr(main_win, position + 4, 67, "%      %");
	Draw_The_Text(main_win, ((current_kernel * 100) / current_total), position + 4, 64, KERNEL_BAR);

	Draw_The_Text(main_win,
	((cpu_averages[_AVG_CPU_KERNEL] * 100) / cpu_averages[_AVG_CPU_TOTAL]),
	position + 4, 71, KERNEL_BAR);

	Draw_A_Bar(main_win, ((current_kernel * 100) / current_total), position + 4, 10, KERNEL_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         IDLE INFO                                  |
 * +--------------------------------------------------------------------+
 */

	mvwaddstr(main_win, position + 7, 67, "%      %");
	Draw_The_Text(main_win, ((current_idle * 100) / current_total), position + 7, 64, IDLE_BAR);

	Draw_The_Text(main_win,
	((cpu_averages[_AVG_CPU_IDLE] * 100) / cpu_averages[_AVG_CPU_TOTAL]),
	position + 7, 71, IDLE_BAR);

	Draw_A_Bar(main_win, ((current_idle * 100) / current_total), position + 7, 10, IDLE_BAR);

/*
 * +--------------------------------------------------------------------+
 * |                         WAIT INFO                                  |
 * +--------------------------------------------------------------------+
 */

	if (compressed_flag)
		position--;

	mvwaddstr(main_win, position + 10, 67, "%      %");

	Draw_The_Text(main_win, ((current_wait * 100) / current_total), position + 10, 64, WAIT_BAR);

	Draw_The_Text(main_win,

	    100 - (((cpu_averages[_AVG_CPU_USER] * 100) / cpu_averages[_AVG_CPU_TOTAL]) + 
	    ((cpu_averages[_AVG_CPU_KERNEL] * 100) / cpu_averages[_AVG_CPU_TOTAL]) + 
	    ((cpu_averages[_AVG_CPU_IDLE] * 100) / cpu_averages[_AVG_CPU_TOTAL])),
	position + 10, 71, WAIT_BAR);

	Draw_A_Bar(main_win, ((current_wait * 100) / current_total), position + 10, 10, WAIT_BAR);

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: watch_cpu(void)                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the tty header.                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          Also re-written and stripped.
 *							Sat Dec 04 13:45:13 EST 1993 - PKR.
 *    Bugs:
 *          None yet.
 */

void watch_cpu(void)
{

int cmd = FALSE;
int naptime = FALSE;
int Sleep_Time_Flag = TRUE;
int register loop_ctr = 0;

	naptime = Sleep_Time;

	drawdisp(29);
	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);
	mvwaddstr(main_win, 1, 7, "Reading /unix..........");
	wrefresh(main_win);
	mvwaddstr(main_win, 1, 7, "                         ");
	wnoutrefresh(top_win);

	while (TRUE) {

		get_cpu_stats();
		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_cpu) == SIG_ERR)
			printf("\nsignal(SIGALRM) error\n");

		alarm(naptime);	/* set the alarm timer */

		if ((cmd = wgetch(bottom_win)) < 0) {
			alarm(0);			/* stop the alarm timer */
			update_cpu_stats();
			wi_cpu(NO_SLEEP);
			Draw_cpu_screen(Sleep_Time_Flag);
			continue;
		} else {
			alarm(0);			/* stop the alarm timer */
			switch(cmd) {
			case 'r' :
			case 'R' :
				cpu_averages[_AVG_CPU_IDLE] 	 = 0;
				cpu_averages[_AVG_CPU_USER] 	 = 0;
				cpu_averages[_AVG_CPU_KERNEL]	 = 0;
				cpu_averages[_AVG_CPU_WAIT]	 = 0;
				cpu_averages[_AVG_CPU_SXBRK]	 = 0;
				cpu_averages[_AVG_CPU_TOTAL]	 = 0;
				cpu_averages[_AVG_CPU_TOTAL_2] = 0;

				cpu_averages[_AVG_W_IO]		 = 0;
				cpu_averages[_AVG_W_SWAP]	 = 0;
				cpu_averages[_AVG_W_PIO]	 = 0;
				cpu_averages[_AVG_W_TOTAL]	 = 0;
				mvwaddstr(bottom_win, 1, 2, " The CPU Averages have been reset.");
				w_clrtoeol(bottom_win, 0, 0);
				Draw_A_Box(bottom_win, DEFAULT_BOX);
				wrefresh(bottom_win);
				nap(1500);
				Draw_cpu_screen(Sleep_Time_Flag);
				break;

			case 'L' & 0x1F:
			case 'R' & 0x1F:
				clear_the_screen();
				break;

			case 'C' :
			case 'c' :
				change_time(FALSE);
				drawdisp(29);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				mvwaddstr(main_win, 1, 7, "Reading /unix..........");
				wrefresh(main_win);
				doupdate();
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				Draw_cpu_screen(Sleep_Time_Flag);
				mvwaddstr(main_win, 1, 7, "                         ");
				wnoutrefresh(top_win);
				break;

			case 'P' :
			case 'p' :
				get_cpu_stats();
				screen_dump();
				update_cpu_stats();
				wi_cpu(NO_SLEEP);
				break;

			case 'I' :
			case 'i' :
				drawdisp(29);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				Draw_cpu_screen(Sleep_Time_Flag);
				break;

			case 'S' :
			case 's' :
				drawdisp(29);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				Draw_cpu_screen(Sleep_Time_Flag);
				break;

			case 'M' :
			case 'm' :
				drawdisp(29);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				Draw_cpu_screen(Sleep_Time_Flag);
				break;

			case 'F' :
			case 'f' :
				drawdisp(29);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				Draw_cpu_screen(Sleep_Time_Flag);
				break;

			case 'D' :
			case 'd' :
				if (size_flag) {
					beep();
					continue;
				}
				if (!(size_flag)) {
					for (loop_ctr = 8; loop_ctr <= 17; loop_ctr++)
						mvwaddstr(main_win, loop_ctr, 1, "                                                                          ");
					if (compressed_flag == COMPRESSED_DISP) {
						compressed_flag = UNCOMPRESSED_DISP;
						mvwaddstr(bottom_win, 1, 2, " Wait I/O mode disabled.");
					} else {
						compressed_flag = COMPRESSED_DISP;
						mvwaddstr(bottom_win, 1, 2, " Wait I/O mode enabled.");
					}
				}
				w_clrtoeol(bottom_win, 0, 0);
				wrefresh(bottom_win);
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
 * | Function:                                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |                                                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None yet.
 *
 *    Bugs:
 *          None yet.
 */

static void sig_alrm_cpu(int signo)
{
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
 *          None yet.
 *
 *    Bugs:
 *          None yet.
 */

void Draw_cpu_screen(int Sleep_Time_Flag)
{

	if (Sleep_Time_Flag) {
		mvwaddstr(bottom_win, 1, 72, ":   ");
		Set_Colour(bottom_win, Colour_White);
		mvwaddstr(bottom_win, 1, 61, "Sample time");
		Set_Colour(bottom_win, Colour_Banner);
		mvwprintw(bottom_win, 1, 74, "%d", Sleep_Time);
		Set_Colour(bottom_win, Normal);
	} else
		mvwaddstr(bottom_win, 1, 61, "               ");

	wnoutrefresh(main_win);
	wnoutrefresh(bottom_win);
	an_option();
	doupdate();
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
