/*
 * +--------------------------------------------------------------------+
 * | Function: mounts.c                                  Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the mount info.                                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 * (1) v2.3:
 *		 Added Console Capture, graphics, etc.
 *							Sun Aug 22 10:14:10 EDT 1993 -PKR.
 * (2) v3.0:
 *		 Fixed problem with One filesystem only, removed mount ptr,
 *		 added EFS, AEFS and N/A for file systems. converted and
 *		 removed shm bit. Added device, changing.
 *							Mon Dec 06 23:02:59 EST 1993 - PKR.
 *    Bugs:
 *          None yet.
 *
 *   Notes: 
 *		Under xenix we seek to /dev/mem to get the file system info
 *		but it appaires that we have to seek in /dev/kmem in UNIX.
 *		I don't know why but it works, and it only took me 3 hours.
 *		Including tea time and changing the structures.
 *		This is the hardest thing in converting form xenix to unix.
 *		I have one unhappy wife so I hope that our company takes off.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */


#include "wi.h"

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/fs/s5param.h>
#include <sys/fs/s5filsys.h>
#include <sys/fs/hsfilsys.h>
#include <sys/fs/dosfilsys.h>
#include "xnx_filsys.h"

#include <sys/var.h>
#include <sys/inode.h>
#include <sys/buf.h>
#include <sys/filsys.h>
#include <sys/mount.h>

#include <signal.h>

#define MENU_POS	3

int percent(int slot, int b_pcent, int i_pcent);
int mount_menu(void);

int mount_menu(void);
void wi_mounts(void);
void getmount(int i);
void getfree(int i);
void watch_mounts(void);
void size_of_mnt_table(void);

static void sig_alrm_mounts(int signo);

extern short demo_pid;

int last_mounts;

int last_slot = 0;
int slot_ctr = 0;
int root_fs = 0;

/*
 * +--------------------------------------------------------------------+
 * | Function: void mount_menu(void)                     Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        The main monitor loop for wi_mounts                         |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int mnt_ctr;

int mount_menu(void)
{

last_slot = 0;
last_mounts = 1;
slot_ctr = 0;


	drawdisp(8);
	mvwaddstr(main_win, 2, 4, "Reading /unix..........");
	wrefresh(main_win);
	mvwaddstr(main_win, 2, 4, "                         ");

	mounts = (struct mount *) malloc(v.v_mount * sizeof(struct mount));
	kmem_read(mounts, namelist[NM_MOUNT].n_value,
	    sizeof(struct mount) * v.v_mount);
	root_fs = mounts[0].m_fstyp;
	free((char *) mounts);


	wi_mounts();
	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		Set_Colour(bottom_win, Normal);
		mvwaddstr(bottom_win, 1, 1, "       : Watchit or Quit:");
		w_clrtoeol(bottom_win, 0, 0);
		Set_Colour(bottom_win, Colour_White);
		mvwaddstr(bottom_win, 1, 2, "Select");
		Set_Colour(bottom_win, Colour_Banner);
		mvwaddstr(bottom_win, 1, 10, "W");
		mvwaddstr(bottom_win, 1, 21, "Q");
		Set_Colour(bottom_win, Normal);

		size_of_mnt_table();
		wmove(bottom_win, 1, 27);
		wnoutrefresh(top_win);
		doupdate();
		switch (wgetch(bottom_win)) {
		case 'L' & 0x1F:
		case 'R' & 0x1F:
			clear_the_screen();
			continue;

		case 'P' :
		case 'p' :
			screen_dump();
			continue;

		case 'G' :
		case 'g' :
			Show_Graphics();
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_mounts();
			continue;

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

		case 'U' :
		case 'u' :
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_mounts();
			break;

		case 'W' :
		case 'w' :
			drawdisp(30);
			watch_mounts();
			drawdisp(8);
			wrefresh(main_win);
			continue;

		case 'Q' :
		case 'q' :
			return(0);

		default :
			continue;

		}
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int wi_mounts(void)                        Date: %Z% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        the main mount loop                                         |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void wi_mounts(void)
{

int    i = 0;
register loop_ctr = 0;
int slot_pos = 0;

	last_slot = FALSE;
	mounts = (struct mount *) malloc(v.v_mount * sizeof(struct mount));
	kmem_read(mounts, namelist[NM_MOUNT].n_value,
	    sizeof(struct mount) * v.v_mount);

	/*
		count the number of entries.
	*/
	slot_ctr = 0;
	mnt_ctr = 0;
	for (i = 0; i < v.v_mount; i++) {
		if (mounts[i].m_flags MATCHES)
			continue;
		mnt_ctr++;
		last_slot = i;
	}

	slot_pos = 2;

	if (mnt_ctr == 4)
		slot_pos--;
	else if (mnt_ctr == 1)
		slot_pos++;

	for (loop_ctr = 1; loop_ctr < (page_len + 1); loop_ctr++)
		mvwaddstr(main_win, loop_ctr, 2, "                                                                           ");

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, slot_pos, 2, "Slot");
	mvwaddstr(main_win, slot_pos, 7, "Maj");
	mvwaddstr(main_win, slot_pos, 11, "Min");
	mvwaddstr(main_win, slot_pos, 15, "FS");
	mvwaddstr(main_win, slot_pos, 18, "BSIZ");
	mvwaddstr(main_win, slot_pos, 23, "Flags");
	mvwaddstr(main_win, slot_pos, 29, "Type");
	if (major_flag)
		mvwaddstr(main_win, slot_pos, 34, "Device_name");
	else
		mvwaddstr(main_win, slot_pos, 34, "Device_number");

	mvwaddstr(main_win, slot_pos, 49, "Blocks");
	mvwaddstr(main_win, slot_pos, 57, "Bfree");
	mvwaddstr(main_win, slot_pos, 63, "Inodes");
	mvwaddstr(main_win, slot_pos, 71, "Ifree");
	Set_Colour(main_win, Normal);

	for (i = 0; i < v.v_mount; i++) {
		if (mounts[i].m_flags MATCHES) {
			if ((mnt_ctr != (last_slot + 1)) && ( i < mnt_ctr))
				getfree(i);
			continue;
		}
		getmount(i);
	}
	free((char *) mounts);
	wmove(bottom_win, 1, 28);
	wnoutrefresh(main_win);
	wnoutrefresh(bottom_win);
	doupdate();
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: getfree                                   Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the mount info.                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void getfree(int i)
{

int extra = 0;

	if (mnt_ctr NOT_MATCH)
		extra++;

	if (mnt_ctr == 4)
		extra--;

	wmove(main_win, i + MENU_POS + extra, 1);
	w_clrtoeol(main_win, 0, 0);

	mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d - ", i);
	Set_Colour(main_win, Text_Warning);
	waddstr(main_win, "Free");
	Set_Colour(main_win, Normal);
	return;

}

/*
 * +--------------------------------------------------------------------+
 * | Function: getmount                                  Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Displays the mount info.                                    |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *		Look at filesys.s_magic, then case on FsMAGIC and FsEMAGIC
 *    Bugs:
 *          None yet.
 */

void getmount(int i)
{

int extra = 0;
char   dev_name[32];
struct filsys filsys;
struct dosfilsys dosfilsys;
struct xnx_filsys xnx_filsys;
struct mount *m_ptr = &mounts[i];
buf_t buf;
int slot_pos = 0;

inode_t *ip = (struct inode *) namelist[NM_INODE].n_value;

	/*
	 * zero before using since unused mount entries don't have
	 * buffers and such.
	 */

	memset(&buf, 0, sizeof(buf));
	memset(&filsys, 0, sizeof(filsys));

	if (mounts[i].m_flags != 0) {
		kmem_read(&buf, (long) m_ptr->m_bufp, sizeof(buf));
		kmem_read(&filsys, (long) buf.b_un.b_addr,  sizeof(filsys));
		kmem_read(&dosfilsys, (long) buf.b_un.b_addr,  sizeof(dosfilsys));
		kmem_read(&xnx_filsys, (long) buf.b_un.b_addr,  sizeof(xnx_filsys));
	}

	if (mnt_ctr NOT_MATCH)
		extra++;

	slot_pos = 2;

	if (mnt_ctr == 4)
		extra--;

		fprintf(stderr, "\n\n\n[%c]\n", *m_ptr->m_name);
		fflush(stderr);
		sleep(4);

	if (major_flag) {
		print_dev(1, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), dev_name, 0);
		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d %4ld %s%s%s%s%s %s%s%s%s %s",
		    i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev),
		    m_ptr->m_fstyp, m_ptr->m_bsize,
		    (m_ptr->m_flags & MFREE)     ? " free" : "",
		    (m_ptr->m_flags & MINUSE)    ? "inuse" : "",
		    (m_ptr->m_flags & MINTER)    ? "mount" : "",
		    (m_ptr->m_flags & MRDONLY)   ? "ronly" : "",
		    (m_ptr->m_flags & MHADBAD)   ? "isbad" : "",
		    (filsys.s_magic == FsEMAGIC) ? "EAFS " : "",
		    (filsys.s_magic == FsMAGIC)  ? "AFS  " : "",
		    (m_ptr->m_fstyp == root_fs +1)        ? "XENIX" : "",
		    (m_ptr->m_fstyp != root_fs +1) && (filsys.s_magic != FsEMAGIC) && (filsys.s_magic != FsMAGIC) ? "N/A. " : "",
		    dev_name);

	} else {		
		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d %4ld %s%s%s%s%s %s%s%s%s  (%0.2d), (%0.2d)",
		    i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev),
		    m_ptr->m_fstyp, m_ptr->m_bsize,
		    (m_ptr->m_flags & MFREE)     ? " free" : "",
		    (m_ptr->m_flags & MINUSE)    ? "inuse" : "",
		    (m_ptr->m_flags & MINTER)    ? "mount" : "",
		    (m_ptr->m_flags & MRDONLY)   ? "ronly" : "",
		    (m_ptr->m_flags & MHADBAD)   ? "isbad" : "",
		    (filsys.s_magic == FsEMAGIC) ? "EAFS " : "",
		    (filsys.s_magic == FsMAGIC)  ? "AFS  " : "",
		    (m_ptr->m_fstyp == root_fs +1)        ? "XENIX" : "",
		    (m_ptr->m_fstyp != root_fs +1) && (filsys.s_magic != FsEMAGIC) && (filsys.s_magic != FsMAGIC) ? "N/A. " : "",
		    bmajor(m_ptr->m_dev), minor(m_ptr->m_dev));

	}

	if ((m_ptr->m_fstyp == root_fs + 1) && (filsys.s_magic != FsEMAGIC) && (filsys.s_magic != FsMAGIC)) {
		mvwprintw(main_win, i + MENU_POS + extra, 48," %6ld %6ld %6ld %6ld",
		    xnx_filsys.s_fsize, (long) xnx_filsys.s_tfree,
		    (xnx_filsys.s_isize - 2) * INOPB, (long) xnx_filsys.s_tinode);

		percent(i, (100 - (int)((xnx_filsys.s_tfree  * 100) / xnx_filsys.s_fsize)),
		    (100 - (int)((xnx_filsys.s_tinode  * 100) / ((xnx_filsys.s_isize - 2) * INOPB))));
	} else if ((filsys.s_magic != FsEMAGIC) && (filsys.s_magic != FsMAGIC)) {
		/*
		mvwprintw(main_win, i + MENU_POS + extra, 48," %6s %6s %6s %6s",
		    "------", "------", "------", "------");
		percent(i, -999, -999);
			MSDOS
			*/
		mvwprintw(main_win, i + MENU_POS + extra, 48," %6ld %6ld %6ld %6ld",
		    dosfilsys.s_root, (long) dosfilsys.s_rootsz,
		    dosfilsys.s_sectors, (long) (dosfilsys.s_sectors*512));
		percent(i, -999, -999);
	} else {
		mvwprintw(main_win, i + MENU_POS + extra, 48," %6ld %6ld %6ld %6ld",
		    filsys.s_fsize, (long) filsys.s_tfree,
		    (filsys.s_isize - 2) * INOPB, (long) filsys.s_tinode);

		percent(i, (100 - (int)((filsys.s_tfree  * 100) / filsys.s_fsize)),
		    (100 - (int)((filsys.s_tinode  * 100) / ((filsys.s_isize - 2) * INOPB))));
	}
}

/*
 * +--------------------------------------------------------------------+
 * | Function: percent()                                 Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        works out math calc.                                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int percent(int slot, int b_pcent, int i_pcent)
{

register counter = 0;
int amount;
int off_set;
int slot_pos;

	/*
		OK I have too many filesystems to show mounted so skip it
	*/

	slot_pos = slot;

	if ((size_flag MATCHES && mnt_ctr >= 5) || (mnt_ctr >= 8))
		return(0);

	if (size_flag) {
		off_set = 5;
		if (mnt_ctr <= 4)
			amount = 14;
		else if (mnt_ctr == 5)
			amount = 10;
		else if (mnt_ctr >= 6) {
			off_set = 3;
			amount = 14;
		}
	} else {
		if (mnt_ctr <= 2) {
			amount = 7;
			off_set = 5;
		} else if (mnt_ctr == 4) {
			amount = 7;
			off_set = 3;
		} else {
			amount = 8;
			off_set = 3;
		}
	}

/*
 * +--------------------------------------------------------------------+
 * |                           For Data                                 |
 * +--------------------------------------------------------------------+
 */
	if (mnt_ctr != (last_slot + 1))
		slot_pos = slot_ctr;

	Set_Colour(main_win, Normal);
	mvwprintw(main_win, (slot_pos * off_set) + amount + 1, 4, "Slot %d", slot);
	mvwprintw(main_win, (slot_pos * off_set) + amount, 14, "%s   %% used", SCALE);
	mvwaddstr(main_win,(slot_pos * off_set) + amount + 1, 73, "%");

	Draw_The_Text(main_win, ((b_pcent <= 0) ? 0 : b_pcent), (slot_pos * off_set) + amount + 1, 70, MOUNT_BAR);

	if ((b_pcent == -999) && (b_pcent == -999)) {
		Set_Colour(main_win, Text_Warning);
		mvwaddstr(main_win, (slot_pos * off_set) + amount + 1, 71, "N/A");
		Set_Colour(main_win, Normal);
	} else
		Draw_A_Bar(main_win, b_pcent, ((slot_pos * off_set) + amount + 1), 14, MOUNT_BAR);


/*
 * +--------------------------------------------------------------------+
 * |                           For Inodes                               |
 * +--------------------------------------------------------------------+
 */
	if (size_flag NOT_MATCH || mnt_ctr <= 2) {
		if (mnt_ctr >= 6)
			return(0);

		Set_Colour(main_win, Colour_Grey);
		mvwaddstr(main_win, (slot_pos * off_set) + amount + 3, 4, "Inodes");
		Set_Colour(main_win, Normal);
		mvwaddstr(main_win, (slot_pos * off_set) + amount + 2, 14, "                                                      % used");

		mvwaddstr(main_win,(slot_pos * off_set) + amount + 3, 73, "%");

		Draw_The_Text(main_win, ((i_pcent <= 0) ? 0 : i_pcent), (slot_pos * off_set) + amount + 3, 70, MOUNT_BAR);

		if ((i_pcent == -999) && (i_pcent == -999)) {
			Set_Colour(main_win, Text_Warning);
			mvwaddstr(main_win, (slot_pos * off_set) + amount + 3, 71, "N/A");
			Set_Colour(main_win, Normal);
		} else
			Draw_A_Bar(main_win, i_pcent, ((slot_pos * off_set) + amount + 3), 14, MOUNT_BAR);
		}

	slot_ctr++;
	return(0);
}

/*
 * +--------------------------------------------------------------------+
 * | Function: void size_of_mnt_table(void)              Date: %Z% |
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

void size_of_mnt_table()
{

int position = 0;

	position = sizeof(v.v_mount);

	mvwprintw(bottom_win, 1, 58, "            ");
	Set_Colour(bottom_win, Colour_White);
	mvwprintw(bottom_win, 1, (68 - position), "NMOUNT");
	Set_Colour(bottom_win, Normal);
	mvwprintw(bottom_win, 1, (74 - position), ": %d",  v.v_mount);
	mvwaddstr(bottom_win, 1, 28, " ");
	wnoutrefresh(bottom_win);
	return;

}

/*
 *	nothing to do, just return to interrupt the read.
 */

static void sig_alrm_mounts(int signo)
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

void watch_mounts()
{

int get_value = 0;
int cmd = 0;
int slot = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;

	drawdisp(30);

	naptime = Sleep_Time;
	Set_Colour(main_win, Blink_Banner);
	mvwaddstr(main_win, 0, 57, "Time");
	Set_Colour(main_win, Normal);
	wnoutrefresh(top_win);

	while (TRUE) {
		drawdisp(1);
		wnoutrefresh(top_win);
		wnoutrefresh(main_win);
		doupdate();
		an_option();
		wi_mounts();
		wnoutrefresh(main_win);
		size_of_mnt_table();
		an_option();
		doupdate();

		/*
			Set up the signal handler
		*/
		if (signal(SIGALRM, sig_alrm_mounts) == SIG_ERR)
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
				drawdisp(30);
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
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
			case 'f' :
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
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
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
