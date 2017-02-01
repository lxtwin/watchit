/*
 * +--------------------------------------------------------------------+
 * | Function: disks.c                                   Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the disks info.                                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 * (1) 2.3:
 *		Graphics, and all that good stuff.
 *							Sun Aug 22 12:51:37 EDT 1993 - PKR.
 *
 * (2) 3.0:
 *		Re-written, and added avg stuff, changed layout, and written a
 *		function for getting # of Stp's from mtune.
 *							Sat Dec 11 13:16:03 EST 1993 - PKR;
 * Bugs:
 * 	v2.3  Add a second tape drive if there is only One scsi tape
 *		unit.
 *							Sat Jul 17 18:34:47 EDT 1993 - PKR.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%			Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%			Version %I% %D% - Dapix"
#endif /* __STDC__ */

#include "wi.h"

#include <sys/ascii.h>
#include <sys/param.h>
#include <sys/elog.h>

#include <signal.h>

extern int The_Disk_Avg;

/* Floppy Drives */
#define NUM_FD_DRIVES	2
static struct iotime fdfirst[NUM_FD_DRIVES];

/* ct60 Drives */
#define NUM_CT_DRIVES	1
static struct iotime ctfirst[NUM_CT_DRIVES];

/* SCSI tape Drives */
#define NUM_STP_DRIVES	2
static struct iotime stpfirst[NUM_STP_DRIVES];

/* SCSI ROM Drives */
#define NUM_ROM_DRIVES	1
static struct iotime romfirst[NUM_ROM_DRIVES];

/* SCSI WRM Drives */
#define NUM_WRM_DRIVES	1
static struct iotime wrmfirst[NUM_WRM_DRIVES];

/* WD Driver */
#define NUM_WD		2
#define NUM_CTLR		2
#define NUM_WD_TOTAL	NUM_CTLR * NUM_WD
static struct iotime wdfirst[NUM_WD_TOTAL];

/* ESDI Driver */
#define NUM_ESDI		2
#define NUM_ESDI_CTLR	2
#define NUM_ESDI_TOTAL	NUM_ESDI_CTLR * NUM_ESDI
static struct iotime esdifirst[NUM_ESDI_TOTAL];

/* IDA Driver */
#define NUM_IDA		2
#define NUM_IDA_CTLR	2
#define NUM_IDA_TOTAL	NUM_IDA_CTLR * NUM_IDA
static struct iotime idafirst[NUM_IDA_TOTAL];

/* SCSI Driver */
#define NUM_SCSI		7
#define NUM_SCSI_CTLR	2
#define NUM_SCSI_TOTAL	NUM_SCSI_CTLR * NUM_SCSI
static struct iotime scsifirst[NUM_SCSI_TOTAL];

int num_scsi_disks;
extern int os_ver;

static void sig_alrm_disks(int signo);
void Draw_disk_screen(int Sleep_Time_Flag);

void disk_stats(void);
void get_disks(void);
void disks_header(void);
void watch_disks(void);
void disk_stats(void);
void draw_screen(void);
void get_fd(void);
void get_wd(void);
void get_ida(void);
void get_ct(void);
void get_scsi(void);
void get_esdi(void);
void get_stp(void);
void get_rom(void);
void get_wrm(void);

void update_disks(void);
void update_wd(void);
void update_ida(void);
void update_fd(void);
void update_ct(void);
void update_scsi(void);
void update_esdi(void);
void update_stp(void);
void update_wrm(void);
void update_rom(void);

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

void disk_stats(void)
{

int	i = 0;

	The_Disk_Avg = DISK_AVG_1;

	mvwaddstr(main_win, 2, 4, "Reading /unix..........");
	wrefresh(main_win);
	mvwaddstr(main_win, 2, 4, "                         ");

/*
 * +--------------------------------------------------------------------+
 * |               GET THE NUMBER OF SCSI DISKS ON THE SYSTEM           |
 * +--------------------------------------------------------------------+
 */
	kmem_read(&num_scsi_disks, namelist[NM_SDSK_NUM].n_value,
	    sizeof(num_scsi_disks));

	disks_header();

	get_disks();
	draw_screen();
	if (size_flag) {
		dsk_cache();
		get_cache_stats();
	}
	nap(1000);
	update_disks();
	if (size_flag) {
		update_cache_stats();
		update_cache_display();
	}
	wnoutrefresh(main_win);
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
			if (size_flag)
				end_cache();
			return;

		case 'B' :
			if (!(size_flag)) {
				beep();
				continue;
			}
			The_Current_Avg = BOOT_AVG;
			Draw_The_Mode();
			mvwaddstr(main_win, 2, 2, "                                  ");
			mvwaddstr(main_win, 2, 2, "  Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 2, 2, "                                  ");
			disks_header();
			get_disks();
			dsk_cache();
			get_cache_stats();
			nap(1000);
			update_disks();
			update_cache_stats();
			update_cache_display();
			break;

		case 'W' :
			if (!(size_flag)) {
				beep();
				continue;
			}
			The_Current_Avg = WATCHIT_AVG;
			Draw_The_Mode();
			mvwaddstr(main_win, 2, 2, "                                  ");
			mvwaddstr(main_win, 2, 2, "  Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 2, 2, "                                  ");
			disks_header();
			get_disks();
			dsk_cache();
			get_cache_stats();
			nap(1000);
			update_disks();
			update_cache_stats();
			update_cache_display();
			break;

		case 'A' :
			if (!(size_flag)) {
				beep();
				continue;
			}
			The_Current_Avg = CURRENT_AVG;
			Draw_The_Mode();
			mvwaddstr(main_win, 2, 2, "                                  ");
			mvwaddstr(main_win, 2, 2, "  Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 2, 2, "                                  ");
			disks_header();
			get_disks();
			dsk_cache();
			get_cache_stats();
			nap(1000);
			update_disks();
			update_cache_stats();
			update_cache_display();
			break;

		case KEY_UP:
		case KEY_DOWN:
		case KEY_LEFT:
		case KEY_RIGHT:
		case 't' :
		case 'T' :
			Set_Colour(main_win, Blink_Banner);
			mvwaddstr(main_win, 0, 54, "Type");
			Set_Colour(main_win, Normal);
			wnoutrefresh(main_win);
			Change_Disk_Type(TRUE);
			Draw_disk_screen(FALSE);

		case 'u' :
		case 'U' :
			mvwaddstr(main_win, 2, 2, "                                  ");
			mvwaddstr(main_win, 2, 2, "  Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 2, 2, "                                  ");
			disks_header();
			get_disks();
			if (size_flag) {
				dsk_cache();
				get_cache_stats();
			}
			nap(1000);
			update_disks();
			if (size_flag) {
				update_cache_stats();
				update_cache_display();
			}
			wrefresh(main_win);
			break;

		case 'w' :
			drawdisp(21);
			Draw_disk_screen(TRUE);
			watch_disks();
			drawdisp(20);
			Draw_disk_screen(FALSE);
			continue;

		default :
			beep();
			break;
		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: get_disks()                                Date: 92/04/04 |
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


void get_disks(void)
{

	kmem_read(&start_ticks,  namelist[NM_LBOLT].n_value,
	    sizeof(start_ticks));

	if (namelist[NM_FLSTATS].n_sclass)
		get_fd();
	if (namelist[NM_CTSTATS].n_sclass)
		get_ct();
	if (namelist[NM_STPSTATS].n_sclass)
		get_stp();

	if (namelist[NM_WDSTATS].n_sclass)
		get_wd();
	if (namelist[NM_IDASTATS].n_sclass)
		get_ida();

	if (namelist[NM_SDSKSTATS].n_sclass)
		get_scsi();
	if (namelist[NM_SROMSTATS].n_sclass)
		get_rom();
	if (namelist[NM_SWRMSTATS].n_sclass)
		get_wrm();
	if (namelist[NM_ESDISTATS].n_sclass)
		get_esdi();
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: get_disks()                                Date: 92/04/04 |
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

void update_disks(void)
{

	if (namelist[NM_FLSTATS].n_sclass)
		update_fd();
	if (namelist[NM_CTSTATS].n_sclass)
		update_ct();
	if (namelist[NM_STPSTATS].n_sclass)
		update_stp();

	if (namelist[NM_WDSTATS].n_sclass)
		update_wd();
	if (namelist[NM_IDASTATS].n_sclass)
		update_ida();

	if (namelist[NM_SDSKSTATS].n_sclass)
		update_scsi();
	if (namelist[NM_SROMSTATS].n_sclass)
		update_rom();
	if (namelist[NM_SWRMSTATS].n_sclass)
		update_wrm();

	if (namelist[NM_ESDISTATS].n_sclass)
		update_esdi();
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

void get_fd(void)
{
	kmem_read(&fdfirst, namelist[NM_FLSTATS].n_value,
	    sizeof(struct iotime) * NUM_FD_DRIVES);
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

void get_wd(void)
{
	kmem_read(&wdfirst, namelist[NM_WDSTATS].n_value,
	    sizeof(struct iotime) * NUM_WD_TOTAL);
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

void get_ida(void)
{
	kmem_read(&idafirst, namelist[NM_IDASTATS].n_value,
	    sizeof(struct iotime) * NUM_IDA_TOTAL);
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

void get_esdi(void)
{
	kmem_read(&esdifirst, namelist[NM_ESDISTATS].n_value,
	    sizeof(struct iotime) * NUM_ESDI_TOTAL);
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

void get_ct(void)
{
	kmem_read(&ctfirst, namelist[NM_CTSTATS].n_value,
	    sizeof(struct iotime) * NUM_CT_DRIVES);
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

void get_scsi(void)
{
	kmem_read(&scsifirst, namelist[NM_SDSKSTATS].n_value,
	    sizeof(struct iotime) * NUM_SCSI_TOTAL);
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

void get_stp(void)
{
	kmem_read(&stpfirst, namelist[NM_STPSTATS].n_value,
	    sizeof(struct iotime) * NUM_STP_DRIVES);
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

void get_rom(void)
{
	kmem_read(&romfirst, namelist[NM_SROMSTATS].n_value,
	    sizeof(struct iotime) * NUM_ROM_DRIVES);
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

void get_wrm(void)
{
	kmem_read(&wrmfirst, namelist[NM_SWRMSTATS].n_value,
	    sizeof(struct iotime) * NUM_WRM_DRIVES);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: disks_header()                             Date: %D% |
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


void disks_header(void)
{

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, 2, 2, "Device");
	mvwaddstr(main_win, 2, 31, "%Busy");
	mvwaddstr(main_win, 2, 42, "Device");
	mvwaddstr(main_win, 2, 71, "%Busy");

	if (The_Disk_Avg == DISK_AVG_1) {
		mvwaddstr(main_win, 2,  9, "Jobs/s");
		mvwaddstr(main_win, 2, 16, " r+w/s");
		mvwaddstr(main_win, 2, 24, "Blks/s");

		mvwaddstr(main_win, 2, 49, "Jobs/s");
		mvwaddstr(main_win, 2, 56, " r+w/s");
		mvwaddstr(main_win, 2, 64, "Blks/s");
	} else {
		mvwaddstr(main_win, 2,  9, " avque");
		mvwaddstr(main_win, 2, 16, "avwait");
		mvwaddstr(main_win, 2, 24, "avserv");

		mvwaddstr(main_win, 2, 49, " avque");
		mvwaddstr(main_win, 2, 56, "avwait");
		mvwaddstr(main_win, 2, 64, "avserv");
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

void draw_screen(void)
{

int ctlr = 0, drive = 0;

/*
 * +--------------------------------------------------------------------+
 * |                      THE FLOPPY DISK INFO                          |
 * +--------------------------------------------------------------------+
 */
	if (namelist[NM_FLSTATS].n_sclass)
		for (drive = 0; drive < NUM_FD_DRIVES; drive++)
			mvwprintw(main_win,(2 + drive + 1), 2, "fd-%1d:", drive);
	else
		for (drive = 0; drive < NUM_FD_DRIVES; drive++)
			mvwprintw(main_win,(2 + drive + 1), 2, "fd-%1d:       ---    ---     ---   ---", drive);


	/*
 * +--------------------------------------------------------------------+
 * |                     THE CT DRIVE INFO                              |
 * +--------------------------------------------------------------------+
 */
	drive = 0;
	if (namelist[NM_CTSTATS].n_sclass) {
		mvwprintw(main_win, 6, 3, "ct-%1d:", drive);
		if (namelist[NM_STPSTATS].n_sclass)
			mvwprintw(main_win, 7, 2, "Stp-%1d:", drive);
	} else {
		if (namelist[NM_STPSTATS].n_sclass) {
			mvwprintw(main_win, 6, 2, "Stp-%1d:", drive);
			mvwprintw(main_win, 7, 2, "Stp-%1d:    ---    ---     ---   ---", drive + 1);
		} else
			mvwprintw(main_win, 6, 2, "tape-%1d: - no tape drive found", drive);
	}

	/*
 * +--------------------------------------------------------------------+
 * |                      WD1010, IDE DISK STATS                        |
 * +--------------------------------------------------------------------+
 */

	if (namelist[NM_WDSTATS].n_sclass)
		for (ctlr = 0; ctlr < NUM_CTLR; ctlr++)
			for (drive = 0; drive < NUM_WD; drive++)
				mvwprintw(main_win, (8 +(ctlr*NUM_WD)+ drive+ 1), 2, "wd-%1d%1d:",
				    ctlr, drive);
	else
		for (ctlr = 0; ctlr < NUM_CTLR; ctlr++)
			for (drive = 0; drive < NUM_WD; drive++)
				mvwprintw(main_win, (8 +(ctlr*NUM_WD)+ drive+ 1), 2, "wd-%1d%1d:    ---    ---     ---   ---",
				    ctlr, drive);


	/*
 * +--------------------------------------------------------------------+
 * |                      IDA (COMPAQ)                                  |
 * +--------------------------------------------------------------------+
 */

	if (namelist[NM_IDASTATS].n_sclass)
		for (ctlr = 0; ctlr < NUM_CTLR; ctlr++)
			for (drive = 0; drive < NUM_IDA; drive++)
				mvwprintw(main_win, (13 +(ctlr*NUM_IDA)+ drive+ 1), 2, "ida-%1d%1d:",
				    ctlr, drive);
	else
		for (ctlr = 0; ctlr < NUM_CTLR; ctlr++)
			for (drive = 0; drive < NUM_IDA; drive++)
				mvwprintw(main_win, (13 +(ctlr*NUM_IDA)+ drive+ 1), 2, "ida-%1d%1d:   ---    ---     ---   ---",
				    ctlr, drive);


	/*
 * +--------------------------------------------------------------------+
 * |                      SCSI DISK INFO                                |
 * +--------------------------------------------------------------------+
 */

	for (drive = 0; drive < NUM_SCSI_TOTAL - 2; drive++) {
		mvwprintw(main_win, (6 + drive), 42, "Sdsk-%1d:", drive);
		mvwprintw(main_win, (6 + drive), 50, "  ---    ---     ---   ---");
	}

	/*
 * +--------------------------------------------------------------------+
 * |                      MISC SCSI INFO                                |
 * +--------------------------------------------------------------------+
 */

	mvwprintw(main_win, 3, 42, "Srom-0:", drive);
	mvwprintw(main_win, 3, 50, "  ---    ---     ---   ---");
	mvwprintw(main_win, 4, 42, "Swrm-0:", drive);
	mvwprintw(main_win, 4, 50, "  ---    ---     ---   ---");

/*
 * +--------------------------------------------------------------------+
 * |                      ESDI DISK STATS                               |
 * +--------------------------------------------------------------------+
 */

	if (namelist[NM_ESDISTATS].n_sclass) {
		mvwprintw(main_win, 13, 42, "                                   ");
		for (ctlr = 0; ctlr < NUM_CTLR; ctlr++)
			for (drive = 0; drive < NUM_ESDI; drive++) {
				mvwprintw(main_win, (14 +(ctlr*NUM_ESDI)+ drive), 42, "esdi-%1d%1d:", ctlr, drive);
				mvwprintw(main_win, (14 +(ctlr*NUM_ESDI)+ drive), 50, "  ---    ---     ---   ---");
			}
	}
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

void update_fd(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;
time_t elapsed_ticks = 0L;

static struct iotime  fdlast[NUM_FD_DRIVES];

	kmem_read(&fdlast, namelist[NM_FLSTATS].n_value,
	    sizeof(struct iotime) * NUM_FD_DRIVES);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;

	for (i = 0; i < (NUM_FD_DRIVES); i++) {
		if (The_Disk_Avg == DISK_AVG_1) {
			col_1 = ((float) (fdlast[i].ios.io_qcnt -
			    fdfirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

			col_2 = ((float) (fdlast[i].ios.io_ops -
			    fdfirst[i].ios.io_ops) / elapsed_ticks) * HZ;

			if (os_ver)
				col_3 = ((float) (fdlast[i].io_bcnt -
				    fdfirst[i].io_bcnt) / elapsed_ticks) * HZ;
			else
				col_3 = (((float) (fdlast[i].io_bcnt -
				    fdfirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;

		} else {
			curr_resp = (float)fdlast[i].io_resp - fdfirst[i].io_resp;
			curr_act  = (float)fdlast[i].io_act - fdfirst[i].io_act;
			curr_cnt  = (float)fdlast[i].io_cnt - fdfirst[i].io_cnt;

			if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else {
				col_1 = (float) curr_resp / curr_act;
				col_2 = (float) (((float)(curr_resp - curr_act) /
					curr_cnt) * 10);
				col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
			}
		}

		col_4 = ((float) (fdlast[i].io_act -
			fdfirst[i].io_act) / elapsed_ticks) * HZ;

		if (The_Disk_Avg == DISK_AVG_1) {
			mvwprintw(main_win, 3 + i, 9, "%6.1f",
		    		(col_1 > 999999) ? 999999 : col_1);
			mvwprintw(main_win, 3 + i, 23, "%7.1f",
		    		(col_3 > 9999999) ? 9999999 : col_3);
		} else {
			mvwprintw(main_win, 3 + i, 9, "%6.2f",
		    		(col_1 > 999999) ? 999999 : col_1);
			mvwprintw(main_win, 3 + i, 23, "%7.2f",
		    		(col_3 > 9999999) ? 9999999 : col_3);
		}
		mvwprintw(main_win, 3 + i, 16, "%6.1f",
		    (col_2 > 999999) ? 999999 : col_2);
		Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
		mvwprintw(main_win, 3 + i, 31, "%5.1f",
		    (col_4 > 100) ? 100 : col_4);
		Set_Colour(main_win, Normal);

	}
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_ct(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;
time_t elapsed_ticks = 0L;

static struct iotime  ctlast[NUM_CT_DRIVES];

	kmem_read(&ctlast, namelist[NM_CTSTATS].n_value,
	    sizeof(struct iotime) * NUM_CT_DRIVES);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;

	if (The_Disk_Avg == DISK_AVG_1) {
		col_1 = ((float) (ctlast[i].ios.io_qcnt -
			ctfirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

		col_2 = ((float) (ctlast[i].ios.io_ops -
			ctfirst[i].ios.io_ops) / elapsed_ticks) * HZ;

		if (os_ver)
			col_3 = ((float) (ctlast[i].io_bcnt -
				ctfirst[i].io_bcnt) / elapsed_ticks) * HZ;
		else
			col_3 = (((float) (ctlast[i].io_bcnt -
				ctfirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;
	} else {
		curr_resp = (float)ctlast[i].io_resp - ctfirst[i].io_resp;
		curr_act  = (float)ctlast[i].io_act - ctfirst[i].io_act;
		curr_cnt  = (float)ctlast[i].io_cnt - ctfirst[i].io_cnt;

		if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
			col_1 = 0;
			col_2 = 0;
			col_3 = 0;
		} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
			col_1 = 0;
			col_2 = 0;
			col_3 = 0;
		} else {
			col_1 = (float) curr_resp / curr_act;
			col_2 = (float) (((float)(curr_resp - curr_act) / curr_cnt) * 10);
			col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
		}
	}

	col_4 = ((float) (ctlast[i].io_act -
		ctfirst[i].io_act) / elapsed_ticks) * HZ;

	if (The_Disk_Avg == DISK_AVG_1) {
		mvwprintw(main_win, 6, 9, "%6.1f", (col_1 > 99999) ? 99999 : col_1);
		mvwprintw(main_win, 6, 23, "%7.1f", (col_3 > 99999) ? 99999 : col_3);
	} else {
		mvwprintw(main_win, 6, 9, "%6.2f", (col_1 > 99999) ? 99999 : col_1);
		mvwprintw(main_win, 6, 23, "%7.2f", (col_3 > 99999) ? 99999 : col_3);
	}
	mvwprintw(main_win, 6, 16, "%6.1f", (col_2 > 99999) ? 99999 : col_2);
	Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
	mvwprintw(main_win, 6, 31, "%5.1f", (col_4 > 100) ? 100 : col_4);
	Set_Colour(main_win, Normal);

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_wd(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;
time_t elapsed_ticks = 0L;

static struct iotime  wdlast[NUM_WD_TOTAL];

	kmem_read(&wdlast, namelist[NM_WDSTATS].n_value,
	    sizeof(struct iotime) * NUM_WD_TOTAL);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;

	for (i = 0; i < (NUM_WD_TOTAL); i++) {
		if (The_Disk_Avg == DISK_AVG_1) {
			col_1 = ((float) (wdlast[i].ios.io_qcnt -
			    wdfirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

			col_2 = ((float) (wdlast[i].ios.io_ops -
			    wdfirst[i].ios.io_ops) / elapsed_ticks) * HZ;

			if (os_ver)
				col_3 = ((float) (wdlast[i].io_bcnt -
				    wdfirst[i].io_bcnt) / elapsed_ticks) * HZ;
			else
				col_3 = (((float) (wdlast[i].io_bcnt -
				    wdfirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;

		} else {
			curr_resp = (float)wdlast[i].io_resp - wdfirst[i].io_resp;
			curr_act  = (float)wdlast[i].io_act - wdfirst[i].io_act;
			curr_cnt  = (float)wdlast[i].io_cnt - wdfirst[i].io_cnt;

			if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else {
				col_1 = (float) curr_resp / curr_act;
				col_2 = (float) (((float)(curr_resp - curr_act) /
					curr_cnt) * 10);
				col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
			}

		}

		col_4 = ((float) (wdlast[i].io_act -
			wdfirst[i].io_act) / elapsed_ticks) * HZ;

		if (The_Disk_Avg == DISK_AVG_1) {
			mvwprintw(main_win, 9 + i, 9, "%6.1f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 9 + i, 23, "%7.1f",
		    		(col_3 > 99999) ? 99999 : col_3);
		} else {
			mvwprintw(main_win, 9 + i, 9, "%6.2f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 9 + i, 23, "%7.2f",
		    		(col_3 > 99999) ? 99999 : col_3);
		}
		mvwprintw(main_win, 9 + i, 16, "%6.1f", (col_2 > 99999) ? 99999 : col_2);
		Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
		mvwprintw(main_win, 9 + i, 31, "%5.1f", (col_4 > 100) ? 100 : col_4);
		Set_Colour(main_win, Normal);
	}
	return;
}



/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_ida(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;
time_t elapsed_ticks = 0L;

static struct iotime  idalast[NUM_IDA_TOTAL];

	kmem_read(&idalast, namelist[NM_IDASTATS].n_value,
	    sizeof(struct iotime) * NUM_IDA_TOTAL);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;

	for (i = 0; i < (NUM_IDA_TOTAL); i++) {
		if (The_Disk_Avg == DISK_AVG_1) {
			col_1 = ((float) (idalast[i].ios.io_qcnt -
				idafirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

			col_2 = ((float) (idalast[i].ios.io_ops -
				idafirst[i].ios.io_ops) / elapsed_ticks) * HZ;

			if (os_ver)
				col_3 = ((float) (idalast[i].io_bcnt -
					idafirst[i].io_bcnt) / elapsed_ticks) * HZ;
			else
				col_3 = (((float) (idalast[i].io_bcnt -
					idafirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;
		} else {
			curr_resp = (float) idalast[i].io_resp -  idafirst[i].io_resp;
			curr_act  = (float) idalast[i].io_act -  idafirst[i].io_act;
			curr_cnt  = (float) idalast[i].io_cnt -  idafirst[i].io_cnt;

			if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else {
				col_1 = (float) curr_resp / curr_act;
				col_2 = (float) (((float)(curr_resp - curr_act) /
					curr_cnt) * 10);
				col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
			}
		}

		col_4 = ((float) (idalast[i].io_act -
			idafirst[i].io_act) / elapsed_ticks) * HZ;

		if (The_Disk_Avg == DISK_AVG_1) {
			mvwprintw(main_win, 14 + i, 9, "%6.1f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 14 + i, 23, "%7.1f",
		    		(col_3 > 99999) ? 99999 : col_3);
		} else {
			mvwprintw(main_win, 14 + i, 9, "%6.2f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 14 + i, 23, "%7.2f",
		    		(col_3 > 99999) ? 99999 : col_3);
		}
		mvwprintw(main_win, 14 + i, 16, "%6.1f",
		    (col_2 > 99999) ? 99999 : col_2);
		Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
		mvwprintw(main_win, 14 + i, 31, "%5.1f",
		    (col_4 > 100) ? 100 : col_4);
		Set_Colour(main_win, Normal);
	}

	return;
}


/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_scsi(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;

time_t elapsed_ticks = 0L;
struct iotime  scsilast[NUM_SCSI_TOTAL];
extern struct iotime  scsifirst[NUM_SCSI_TOTAL];

	kmem_read(&scsilast, namelist[NM_SDSKSTATS].n_value,
	    sizeof(struct iotime) * NUM_SCSI_TOTAL);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;

/*
 * +--------------------------------------------------------------------+
 * |     A DEBUG LINE USED TO GET NUMBER OF DISKS CONFIGURED            |
 * +--------------------------------------------------------------------+
 */

	for (i = 0; i < num_scsi_disks; i++) {
		if (The_Disk_Avg == DISK_AVG_1) {
			col_1 = ((float) (scsilast[i].ios.io_qcnt -
				scsifirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

			col_2 = ((float) (scsilast[i].ios.io_ops -
				scsifirst[i].ios.io_ops) / elapsed_ticks) * HZ;

			if (os_ver)
				col_3 = ((float) (scsilast[i].io_bcnt -
					scsifirst[i].io_bcnt) / elapsed_ticks) * HZ;
			else
				col_3 = (((float) (scsilast[i].io_bcnt -
					scsifirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;
		} else {
			curr_resp = (float)scsilast[i].io_resp - scsifirst[i].io_resp;
			curr_act  = (float)scsilast[i].io_act - scsifirst[i].io_act;
			curr_cnt  = (float)scsilast[i].io_cnt - scsifirst[i].io_cnt;

			if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else {
				col_1 = (float) curr_resp / curr_act;
				col_2 = (float) (((float)(curr_resp - curr_act) /
					curr_cnt) * 10);
				col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
			}
		}

		col_4 = ((float) (scsilast[i].io_act -
			scsifirst[i].io_act) / elapsed_ticks) * HZ;

		if (The_Disk_Avg == DISK_AVG_1) {
			mvwprintw(main_win, 6 + i, 49, "%6.1f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 6 + i, 63, "%7.1f",
		    		(col_3 > 99999) ? 99999 : col_3);
		} else {
			mvwprintw(main_win, 6 + i, 49, "%6.2f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 6 + i, 63, "%7.2f",
		    		(col_3 > 99999) ? 99999 : col_3);
		}
		mvwprintw(main_win, 6 + i, 56, "%6.1f",
		    (col_2 > 99999) ? 99999 : col_2);
		Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
		mvwprintw(main_win, 6 + i, 71, "%5.1f",
		    (col_4 > 100) ? 100 : col_4);
		Set_Colour(main_win, Normal);
	}
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_stp(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;

int Number_of_tapes = 1;
int extra_bit = 0;

time_t elapsed_ticks = 0L;
static struct iotime  stplast[NUM_STP_DRIVES];

	i = 0;
	extra_bit = 0;

	if (namelist[NM_CTSTATS].n_sclass) {
		extra_bit = 1;
		Number_of_tapes = 1;
	} else {
		Number_of_tapes = get_Stp();
		if (Number_of_tapes >= 3)
			Number_of_tapes = 2;
	}

	kmem_read(&stplast, namelist[NM_STPSTATS].n_value,
	    sizeof(struct iotime) * NUM_STP_DRIVES);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;


	for (i = 0; i < Number_of_tapes; i++) {

		if (The_Disk_Avg == DISK_AVG_1) {
			col_1 = ((float) (stplast[i].ios.io_qcnt -
				stpfirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

			col_2 = ((float) (stplast[i].ios.io_ops -
				stpfirst[i].ios.io_ops) / elapsed_ticks) * HZ;

			if (os_ver)
				col_3 = ((float) (stplast[i].io_bcnt -
					stpfirst[i].io_bcnt) / elapsed_ticks) * HZ;
			else
				col_3 = (((float) (stplast[i].io_bcnt -
					stpfirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;
		} else {
			curr_resp = (float) stplast[i].io_resp - stpfirst[i].io_resp;
			curr_act  = (float) stplast[i].io_act  - stpfirst[i].io_act;
			curr_cnt  = (float) stplast[i].io_cnt  - stpfirst[i].io_cnt;

			if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else {
				col_1 = (float) curr_resp / curr_act;
				col_2 = (float) (((float)(curr_resp - curr_act) /
					curr_cnt) * 10);
				col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
			}
		}

		col_4 = ((float) (stplast[i].io_act -
			stpfirst[i].io_act) / elapsed_ticks) * HZ;

		if (The_Disk_Avg == DISK_AVG_1) {
			mvwprintw(main_win, 6 + i + extra_bit, 9, "%6.1f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 6 + i + extra_bit, 23, "%7.1f",
		    		(col_3 > 99999) ? 99999 : col_3);
		} else {
			mvwprintw(main_win, 6 + i + extra_bit, 9, "%6.2f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 6 + i + extra_bit, 23, "%7.2f",
		    		(col_3 > 99999) ? 99999 : col_3);
		}
		mvwprintw(main_win, 6 + i + extra_bit, 16, "%6.1f",
		    (col_2 > 99999) ? 99999 : col_2);
		Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
		mvwprintw(main_win, 6 + i + extra_bit, 31, "%5.1f",
		    (col_4 > 100) ? 100 : col_4);
		Set_Colour(main_win, Normal);
	}
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_rom(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;
time_t elapsed_ticks = 0L;

static struct iotime  romlast[NUM_ROM_DRIVES];

	i = 0;

	kmem_read(&romlast, namelist[NM_SROMSTATS].n_value,
	    sizeof(struct iotime) * NUM_ROM_DRIVES);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;

	if (The_Disk_Avg == DISK_AVG_1) {
		col_1 = ((float) (romlast[i].ios.io_qcnt -
			romfirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

		col_2 = ((float) (romlast[i].ios.io_ops -
			romfirst[i].ios.io_ops) / elapsed_ticks) * HZ;

		if (os_ver)
			col_3 = ((float) (romlast[i].io_bcnt -
				romfirst[i].io_bcnt) / elapsed_ticks) * HZ;
		else
			col_3 = (((float) (romlast[i].io_bcnt -
				romfirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;
	} else {
		curr_resp = (float) romlast[i].io_resp - romfirst[i].io_resp;
		curr_act  = (float) romlast[i].io_act  - romfirst[i].io_act;
		curr_cnt  = (float) romlast[i].io_cnt  - romfirst[i].io_cnt;

		if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
			col_1 = 0;
			col_2 = 0;
			col_3 = 0;
		} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
			col_1 = 0;
			col_2 = 0;
			col_3 = 0;
		} else {
			col_1 = (float) curr_resp / curr_act;
			col_2 = (float) (((float)(curr_resp - curr_act) /
				curr_cnt) * 10);
			col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
		}
	}
	col_4 = ((float) (romlast[i].io_act -
		romfirst[i].io_act) / elapsed_ticks) * HZ;

	if (The_Disk_Avg == DISK_AVG_1) {
		mvwprintw(main_win, 3, 49, "%6.1f",
	    		(col_1 > 99999) ? 99999 : col_1);
		mvwprintw(main_win, 3, 63, "%7.1f",
	    		(col_3 > 99999) ? 99999 : col_3);
	} else {
		mvwprintw(main_win, 3, 49, "%6.2f",
	    		(col_1 > 99999) ? 99999 : col_1);
		mvwprintw(main_win, 3, 63, "%7.2f",
	    		(col_3 > 99999) ? 99999 : col_3);
	}
	mvwprintw(main_win, 3, 56, "%6.1f", (col_2 > 99999) ? 99999 : col_2);

	Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
	mvwprintw(main_win, 3, 71, "%5.1f", (col_4 > 100) ? 100 : col_4);
		Set_Colour(main_win, Normal);

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_wrm(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;
time_t elapsed_ticks = 0L;

static struct iotime  wrmlast[NUM_WRM_DRIVES];

	i = 0;

	kmem_read(&wrmlast, namelist[NM_SWRMSTATS].n_value,
	    sizeof(struct iotime) * NUM_WRM_DRIVES);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;


	if (The_Disk_Avg == DISK_AVG_1) {
		col_1 = ((float) (wrmlast[i].ios.io_qcnt -
			wrmfirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

		col_2 = ((float) (wrmlast[i].ios.io_ops -
			wrmfirst[i].ios.io_ops) / elapsed_ticks) * HZ;

		if (os_ver)
			col_3 = ((float) (wrmlast[i].io_bcnt -
				wrmfirst[i].io_bcnt) / elapsed_ticks) * HZ;
		else
			col_3 = (((float) (wrmlast[i].io_bcnt -
				wrmfirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;
	} else {
		curr_resp = (float) wrmlast[i].io_resp - wrmfirst[i].io_resp;
		curr_act  = (float) wrmlast[i].io_act  - wrmfirst[i].io_act;
		curr_cnt  = (float) wrmlast[i].io_cnt  - wrmfirst[i].io_cnt;

		if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
			col_1 = 0;
			col_2 = 0;
			col_3 = 0;
		} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
			col_1 = 0;
			col_2 = 0;
			col_3 = 0;
		} else {
			col_1 = (float) curr_resp / curr_act;
			col_2 = (float) (((float)(curr_resp - curr_act) /
				curr_cnt) * 10);
			col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
		}
	}
	col_4 = ((float) (wrmlast[i].io_act -
		wrmfirst[i].io_act) / elapsed_ticks) * HZ;

	if (The_Disk_Avg == DISK_AVG_1) {
		mvwprintw(main_win, 4, 49, "%6.1f",
	    		(col_1 > 99999) ? 99999 : col_1);
		mvwprintw(main_win, 4, 63, "%7.1f",
	    		(col_3 > 99999) ? 99999 : col_3);
	} else {
		mvwprintw(main_win, 4, 49, "%6.2f",
	    		(col_1 > 99999) ? 99999 : col_1);
		mvwprintw(main_win, 4, 63, "%7.2f",
	    		(col_3 > 99999) ? 99999 : col_3);
	}
	mvwprintw(main_win, 4, 56, "%6.1f", (col_2 > 99999) ? 99999 : col_2);

	Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
	mvwprintw(main_win, 4, 71, "%5.1f", (col_4 > 100) ? 100 : col_4);
	Set_Colour(main_win, Normal);

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: 93/02/27 |
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

void update_esdi(void)
{

register  i = 0;
float col_1 = 0;
float col_2 = 0;
float col_3 = 0;
float col_4 = 0;

float curr_act  = 0;
float curr_cnt  = 0;
float curr_resp = 0;

time_t elapsed_ticks = 0L;
static struct iotime  esdilast[NUM_ESDI_TOTAL];

	kmem_read(&esdilast, namelist[NM_ESDISTATS].n_value,
	    sizeof(struct iotime) * NUM_ESDI_TOTAL);

	kmem_read(&current_ticks, namelist[NM_LBOLT].n_value, 
	    sizeof(current_ticks));

	elapsed_ticks = current_ticks - start_ticks;

	for (i = 0; i < (NUM_ESDI_TOTAL); i++) {
		if (The_Disk_Avg == DISK_AVG_1) {
			col_1 = ((float) (esdilast[i].ios.io_qcnt -
				esdifirst[i].ios.io_qcnt) / elapsed_ticks) * HZ;

			col_2 = ((float) (esdilast[i].ios.io_ops -
				esdifirst[i].ios.io_ops) / elapsed_ticks) * HZ;

			if (os_ver)
				col_3 = ((float) (esdilast[i].io_bcnt -
					esdifirst[i].io_bcnt) / elapsed_ticks) * HZ;
			else
				col_3 = (((float) (esdilast[i].io_bcnt -
					esdifirst[i].io_bcnt) / elapsed_ticks) * HZ) * 4;
		} else {
			curr_resp = (float) esdilast[i].io_resp - esdifirst[i].io_resp;
			curr_act  = (float) esdilast[i].io_act - esdifirst[i].io_act;
			curr_cnt  = (float) esdilast[i].io_cnt - esdifirst[i].io_cnt;

			if ((curr_resp == 0) || (curr_act == 0) || (curr_cnt == 0)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else if ((curr_resp < curr_act) || (curr_act < curr_cnt)) {
				col_1 = 0;
				col_2 = 0;
				col_3 = 0;
			} else {
				col_1 = (float) curr_resp / curr_act;
				col_2 = (float) (((float)(curr_resp - curr_act) /
					curr_cnt) * 10);
				col_3 = (float) (((float)(curr_act)/ curr_cnt) * 10);
			}
		}
		col_4 = ((float) (esdilast[i].io_act -
			esdifirst[i].io_act) / elapsed_ticks) * HZ;

		if (The_Disk_Avg == DISK_AVG_1) {
			mvwprintw(main_win, 14 + i, 49, "%6.1f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 14 + i, 63, "%7.1f",
		    		(col_3 > 99999) ? 99999 : col_3);
		} else {
			mvwprintw(main_win, 14 + i, 49, "%6.2f",
		    		(col_1 > 99999) ? 99999 : col_1);
			mvwprintw(main_win, 14 + i, 63, "%7.2f",
		    		(col_3 > 99999) ? 99999 : col_3);
		}
		mvwprintw(main_win, 14 + i, 56, "%6.1f", (col_2 > 99999) ? 99999 : col_2);

		Draw_The_Text(main_win, (int) ((col_4 > 100) ? 100 : col_4), COLOUR_ONLY, IO_BAR);
		mvwprintw(main_win, 14 + i, 71, "%5.1f", (col_4 > 100) ? 100 : col_4);
		Set_Colour(main_win, Normal);
	}
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

void watch_disks(void)
{

int cmd = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;

	if (size_flag)
		touchwin(cache_win);

	naptime = Sleep_Time;
	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);
	wnoutrefresh(top_win);
	Draw_disk_screen(Sleep_Time_Flag);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		wnoutrefresh(main_win);
		if (size_flag) {
			touchwin(cache_win);
			wnoutrefresh(cache_win);
		}
		an_option();
		doupdate();

		get_disks();
		if (size_flag)
			get_cache_stats();

		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_disks) == SIG_ERR)
			printf("\nsignal(SIGALRM) error\n");

		alarm(naptime);	/* set the alarm timer */

		if ((cmd = wgetch(bottom_win)) < 0) {
			alarm(0);			/* stop the alarm timer */
			update_disks();
			if (size_flag) {
				update_cache_stats();
				update_cache_display();
				wnoutrefresh(cache_win);
			}
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
				drawdisp(21);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				Draw_disk_screen(Sleep_Time_Flag);
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
				drawdisp(21);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				Draw_disk_screen(Sleep_Time_Flag);
				break;

			case 'S' :
			case 's' :
				drawdisp(21);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				Draw_disk_screen(Sleep_Time_Flag);
				break;

			case 'M' :
			case 'm' :
				drawdisp(21);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				Draw_disk_screen(Sleep_Time_Flag);
				break;

			case 'F' :
			case 'f' :
				drawdisp(21);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				Draw_disk_screen(Sleep_Time_Flag);
				break;

			case 't' :
			case 'T' :
			case KEY_UP:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
				drawdisp(21);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 51, "Type");
				Set_Colour(main_win, Normal);
				wnoutrefresh(main_win);
				if (size_flag) {
					touchwin(cache_win);
					wnoutrefresh(cache_win);
				}

				Change_Disk_Type(FALSE);
				drawdisp(21);

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
				wnoutrefresh(main_win);
				Draw_disk_screen(FALSE);
				doupdate();
				break;

			case 'B' :
				if (!(size_flag)) {
					beep();
					continue;
				}
				The_Current_Avg = BOOT_AVG;
				Draw_The_Mode();
				mvwaddstr(main_win, 2, 2, "                                  ");
				mvwaddstr(main_win, 2, 2, "  Reading /unix..........");
				wrefresh(main_win);
				mvwaddstr(main_win, 2, 2, "                                  ");
				disks_header();
				get_disks();
				dsk_cache();
				get_cache_stats();
				nap(1000);
				update_disks();
				update_cache_stats();
				update_cache_display();
				break;

			case 'W' :
				if (!(size_flag)) {
					beep();
					continue;
				}
				The_Current_Avg = WATCHIT_AVG;
				Draw_The_Mode();
				mvwaddstr(main_win, 2, 2, "                                  ");
				mvwaddstr(main_win, 2, 2, "  Reading /unix..........");
				wrefresh(main_win);
				mvwaddstr(main_win, 2, 2, "                                  ");
				disks_header();
				get_disks();
				dsk_cache();
				get_cache_stats();
				nap(1000);
				update_disks();
				update_cache_stats();
				update_cache_display();
				break;

			case 'A' :
				if (!(size_flag)) {
					beep();
					continue;
				}
				The_Current_Avg = CURRENT_AVG;
				Draw_The_Mode();
				mvwaddstr(main_win, 2, 2, "                                  ");
				mvwaddstr(main_win, 2, 2, "  Reading /unix..........");
				wrefresh(main_win);
				mvwaddstr(main_win, 2, 2, "                                  ");
				disks_header();
				get_disks();
				dsk_cache();
				get_cache_stats();
				nap(1000);
				update_disks();
				update_cache_stats();
				update_cache_display();
				break;

			case 'Q' :
			case 'q' :
				mvwaddstr(bottom_win, 1, 61, "               ");
				wnoutrefresh(bottom_win);
				if (size_flag) {
					touchwin(cache_win);
					wnoutrefresh(cache_win);
				}
				doupdate();
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

static void sig_alrm_disks(int signo)
{
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

void Draw_disk_screen(int Sleep_Time_Flag)
{

	Set_Colour(main_win, Normal);
	if (Sleep_Time_Flag) {
		mvwaddstr(bottom_win, 1, 72, ":   ");
		mvwaddstr(bottom_win, 1, 61, "Sample time");
		Set_Colour(main_win, Colour_Banner);
		mvwprintw(bottom_win, 1, 74, "%d", Sleep_Time);
		Set_Colour(main_win, Normal);
	} else
		mvwaddstr(bottom_win, 1, 61, "               ");

	wnoutrefresh(main_win);
	wnoutrefresh(bottom_win);
	if (size_flag) {
		touchwin(cache_win);
		wnoutrefresh(cache_win);
	}
	an_option();
	doupdate();
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
