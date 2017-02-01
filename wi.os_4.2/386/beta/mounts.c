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
 * v2.3:
 *		 Added Console Capture, graphics, etc.
 *							Sun Aug 22 10:14:10 EDT 1993 -PKR.
 * v3.0:
 *		 Fixed problem with One filesystem only, removed mount ptr,
 *		 added EFS, AEFS and N/A for file systems. converted and
 *		 removed shm bit. Added device, changing.
 *							Mon Dec 06 23:02:59 EST 1993 - PKR.
 *
 * 		Added type and colour stuff also added support for all devices
 *		NFS, XENIX, CDROM and MSDOS.
 *							Sun May 15 12:44:43 EDT 1994 - PKR.
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
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix"
#endif /* __STDC__ */

#include "wi.h"

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/fs/s5param.h>
#include <sys/fs/s5filsys.h>
#include <sys/fs/hsfilsys.h>
#include <sys/fs/dosfilsys.h>
#include <sys/fs/xxfilsys.h>

#include <sys/var.h>
#include <sys/buf.h>
#include <sys/filsys.h>
#include <sys/mount.h>
#include <sys/conf.h>

#include <mnttab.h>
#include <sys/statfs.h>

#include <signal.h>

#include <sys/tiuser.h>
#include <sys/fs/nfs/nfs_clnt.h>

#define MENU_POS	3

#define TOTAL_FS	8

#define BY_COMBIN	0
#define BY_BLOCKS	1
#define BY_INODES	2

struct mount *mounts;			/* kernel mount structre	*/
struct fsinfo	*fsys_info;			/* kernel filsystem info	*/

extern short demo_pid;

int last_mounts;

int last_slot	= 0;
int slot_ctr	= 0;

int  percent(int slot, int b_pcent, int i_pcent);
int  mount_menu(void);
int  Change_Mount_Type(int *type_flag);
void wi_mounts(int mount_type);
void mount_header(int position, int mount_type);
void getmount(int i, int mount_type);
void getfree(int i);
void watch_mounts(int *mount_type);
void size_of_mnt_table(void);
void get_statfs(int slot, struct statfs *stat_fs);

static void sig_alrm_mounts(int signo);

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

int mount_type;

last_slot = 0;
last_mounts = 1;
slot_ctr = 0;

	mount_type = BY_BLOCKS;

	drawdisp(8);
	mvwaddstr(main_win, 2, 4, "Reading /unix..........");
	wrefresh(main_win);
	mvwaddstr(main_win, 2, 4, "                         ");

	fsys_info = (struct fsinfo *) malloc(TOTAL_FS * sizeof(struct fsinfo));
	kmem_read(fsys_info, namelist[NM_FSINFO].n_value,
		sizeof(struct fsinfo) * TOTAL_FS);

	wi_mounts(mount_type);
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
			wnoutrefresh(bottom_win);
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wnoutrefresh(main_win);
			doupdate();
			nap(500);
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_mounts(mount_type);
			break;

		case 'c' :
		case 'b' :
		case 'i' :
		case 't' :
		case 'C' :
		case 'B' :
		case 'I' :
		case 'T' :
			Change_Mount_Type(&mount_type);
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wnoutrefresh(main_win);
			doupdate();
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_mounts(mount_type);
			break;

		case 'U' :
		case 'u' :
			mvwaddstr(main_win, 1, 7, "Reading /unix..........");
			wrefresh(main_win);
			mvwaddstr(main_win, 1, 7, "                         ");
			wi_mounts(mount_type);
			break;

		case 'W' :
		case 'w' :
			drawdisp(30);
			watch_mounts(&mount_type);
			drawdisp(8);
			wrefresh(main_win);
			continue;

		case 'Q' :
		case 'q' :
			free((char *) fsys_info);
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

void mount_header(int position, int mount_type)
{

register int ctr = 0;

	for (ctr = 1; ctr < (page_len + 1); ctr++)
		mvwaddstr(main_win, ctr, 2, "                                                                           ");

	Set_Colour(main_win, Colour_Blue);
	mvwaddstr(main_win, position, 2, "Slot");
	mvwaddstr(main_win, position, 7, "Maj");
	mvwaddstr(main_win, position, 11, "Min");
	mvwaddstr(main_win, position, 15, "FS");
	mvwaddstr(main_win, position, 18, "BSIZ");
	mvwaddstr(main_win, position, 23, "Type");
	mvwaddstr(main_win, position, 29, "Flags");
	w_clrtoeol(main_win, 0, 0);
	if (major_flag)
		mvwaddstr(main_win, position, 35, "Device_name");
	else
		mvwaddstr(main_win, position, 35, "Device_number");

	switch (mount_type) {
	case BY_BLOCKS :
		mvwaddstr(main_win, position, 56, "Blocks");
		mvwaddstr(main_win, position, 65, "Bfree");
		mvwaddstr(main_win, position, 72, "%used");
		break;

	case BY_INODES :
		mvwaddstr(main_win, position, 56, "Inodes");
		mvwaddstr(main_win, position, 65, "Ifree");
		mvwaddstr(main_win, position, 72, "%used");
		break;

	default :
		mvwaddstr(main_win, position, 49, "Blocks");
		mvwaddstr(main_win, position, 58, "Bfree");
		mvwaddstr(main_win, position, 64, "Inodes");
		mvwaddstr(main_win, position, 72, "Ifree");
		break;
	}

	Set_Colour(main_win, Normal);
	return;
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

void wi_mounts(int mount_type)
{

int    i = 0;
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

	mount_header(slot_pos, mount_type);

	for (i = 0; i < v.v_mount; i++) {
		if (mounts[i].m_flags MATCHES) {
			if ((mnt_ctr != (last_slot + 1)) && (i < mnt_ctr))
				getfree(i);
			continue;
		}
		getmount(i, mount_type);
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

	position = 2;

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

void getmount(int i, int mount_type)
{

int	extra = 0;
char  fstyp_name[32];
char  dev_name[32];

struct filsys	filsys;
struct hsfilsys	hsfilsys;
struct dosfilsys	dosfilsys;
struct xxfilsys	xxfilsys;
struct mntinfo	mntinfo;

struct mount *m_ptr = &mounts[i];

struct statfs stat_fs;

buf_t buf;
int slot_pos = 0;

	/*
	 *	This will set up the file system type.
	 */
	kmem_read(&fstyp_name, fsys_info[m_ptr->m_fstyp].fs_name,
		sizeof(fstyp_name));

	if (mnt_ctr NOT_MATCH)
		extra++;

	slot_pos = 2;

	if (mnt_ctr == 4)
		extra--;

	/*
	 *	Now look at the fs_type to check the type of filesystem.
	 */

	if (strcmp(fstyp_name, "S51K") MATCHES) {

		/*
		 * +----------------------------------------------------------+
		 * |                OK we have AFS, EAFS.                     |
		 * +----------------------------------------------------------+
		 */

		memset(&buf, 0, sizeof(buf));
		kmem_read(&buf, (long) m_ptr->m_bufp, sizeof(buf));

		memset(&filsys, 0, sizeof(filsys));
		kmem_read(&filsys, (long) buf.b_un.b_addr,  sizeof(filsys));

		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d",
		i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), m_ptr->m_fstyp);

		if (major_flag) {
			print_dev(1, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), dev_name, 0);
			if (mount_type)
				mvwprintw(main_win, i + MENU_POS + extra, 35, "/dev/%s",
					dev_name);
			else
				mvwprintw(main_win, i + MENU_POS + extra, 35, "%s", dev_name);
		} else
			mvwprintw(main_win, i + MENU_POS + extra, 35, "(%0.2d), (%0.2d)", bmajor(m_ptr->m_dev), minor(m_ptr->m_dev));

		mvwprintw(main_win, i + MENU_POS + extra, 18, "%s%s%s %s ",
		    (filsys.s_type == Fs1b) ? " 512" : "",
		    (filsys.s_type == Fs2b) ? "1024" : "",
		    (filsys.s_type == Fs4b) ? "2048" : "",
		    (filsys.s_magic == FsEMAGIC) ? "EAFS " : "AFS  ");

		if (m_ptr->m_flags & MRDONLY)
			waddstr(main_win, "ronly");
		else if (m_ptr->m_flags & MFREE)
			waddstr(main_win, "free ");
		else if (m_ptr->m_flags & MINUSE)
			waddstr(main_win, "inuse");
		else if (m_ptr->m_flags & MINTER)
			waddstr(main_win, "mount");
		else if (m_ptr->m_flags & MHADBAD)
			waddstr(main_win, "isbad");
		else
			waddstr(main_win, "N/A  ");

		switch (mount_type) {
		case BY_BLOCKS :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
		    	filsys.s_fsize, (long) filsys.s_tfree);

			Draw_The_Text(main_win, 
				(100 - (int)((filsys.s_tfree  * 100) / filsys.s_fsize)),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		case BY_INODES :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
		    	(filsys.s_isize - 2) * INOPB, (long) filsys.s_tinode);
			Draw_The_Text(main_win, 
		    		(100 - (int)((filsys.s_tinode  * 100) / ((filsys.s_isize - 2) * INOPB))),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		default :
			mvwprintw(main_win, i + MENU_POS + extra, 47,"%8ld%8ld %6ld %6ld",
		    	filsys.s_fsize, (long) filsys.s_tfree,
		    	(filsys.s_isize - 2) * INOPB, (long) filsys.s_tinode);
			break;
		}

		percent(i, (100 - (int)((filsys.s_tfree  * 100) / filsys.s_fsize)),
		    (100 - (int)((filsys.s_tinode  * 100) / ((filsys.s_isize - 2) * INOPB))));

	} else if (strcmp(fstyp_name, "XENIX") MATCHES) {

		/*
		 * +----------------------------------------------------------+
		 * |                  OK we have XENIX.                       |
		 * +----------------------------------------------------------+
		 */

		memset(&buf, 0, sizeof(buf));
		kmem_read(&buf, (long) m_ptr->m_bufp, sizeof(buf));

		memset(&xxfilsys, 0, sizeof(xxfilsys));
		kmem_read(&xxfilsys, (long) buf.b_un.b_addr, sizeof(xxfilsys));

		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d",
		i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), m_ptr->m_fstyp);

		if (major_flag) {
			print_dev(1, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), dev_name, 0);
			if (mount_type)
				mvwprintw(main_win, i + MENU_POS + extra, 35, "/dev/%s",
					dev_name);
			else
				mvwprintw(main_win, i + MENU_POS + extra, 35, "%s", dev_name);

		} else
			mvwprintw(main_win, i + MENU_POS + extra, 35, "(%0.2d), (%0.2d)", bmajor(m_ptr->m_dev), minor(m_ptr->m_dev));

		mvwprintw(main_win, i + MENU_POS + extra, 18, "%s%s%s %s ",
		    (xxfilsys.s_type == Fs1b) ? " 512" : "",
		    (xxfilsys.s_type == Fs2b) ? "1024" : "",
		    (xxfilsys.s_type == Fs4b) ? "2048" : "",
		    (xxfilsys.s_magic == S_S3MAGIC) ? "XENIX" : "N/A  ");

		if (m_ptr->m_flags & MRDONLY)
			waddstr(main_win, "ronly");
		else if (m_ptr->m_flags & MFREE)
			waddstr(main_win, "free ");
		else if (m_ptr->m_flags & MINUSE)
			waddstr(main_win, "inuse");
		else if (m_ptr->m_flags & MINTER)
			waddstr(main_win, "mount");
		else if (m_ptr->m_flags & MHADBAD)
			waddstr(main_win, "isbad");
		else
			waddstr(main_win, "N/A  ");


		switch (mount_type) {
		case BY_BLOCKS :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
		    	xxfilsys.s_fsize, (long) xxfilsys.s_tfree);


			Draw_The_Text(main_win, 
				(100 - (int)((xxfilsys.s_tfree  * 100) / xxfilsys.s_fsize)),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		case BY_INODES :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",

		    	(xxfilsys.s_isize - 2) * INOPB, (long) xxfilsys.s_tinode);
			Draw_The_Text(main_win, 
		    		(100 - (int)((xxfilsys.s_tinode  * 100) / ((xxfilsys.s_isize - 2) * INOPB))),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		default :
			mvwprintw(main_win, i + MENU_POS + extra, 47,"%8ld%8ld %6ld %6ld",
		    	xxfilsys.s_fsize, (long) xxfilsys.s_tfree,
		    	(xxfilsys.s_isize - 2) * INOPB, (long) xxfilsys.s_tinode);
		}

		percent(i, (100 - (int)((xxfilsys.s_tfree * 100)/xxfilsys.s_fsize)),
		    (100 - (int)((xxfilsys.s_tinode  * 100)/((xxfilsys.s_isize - 2) * INOPB))));

	} else if (strcmp(fstyp_name, "NFS") MATCHES) {

		/*
		 * +----------------------------------------------------------+
		 * |                  OK we have NFS/RFS.                     |
		 * +----------------------------------------------------------+
		 */

		memset(&buf, 0, sizeof(buf));
		kmem_read(&buf, (long) m_ptr->m_bufp, sizeof(buf));

		kmem_read(&mntinfo, (long) m_ptr->m_bufp, sizeof(mntinfo));
		/* kmem_read(&buf, mntinfo.mi_addr.buf, sizeof(buf)); */

		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d",
			i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), m_ptr->m_fstyp);

		get_statfs(i, &stat_fs);

		if (major_flag) {
			if (mount_type)
				mvwprintw(main_win, i+ MENU_POS+ extra, 35, "NFS:%.12s",
					mntinfo.mi_hostname);
			else
				mvwprintw(main_win, i + MENU_POS + extra, 35, "%.12s",
					mntinfo.mi_hostname);
		} else
			mvwprintw(main_win, i + MENU_POS + extra, 35, "%.12s",
				"NFS Filesys.");

		mvwprintw(main_win, i + MENU_POS + extra, 18, "%4d %s ",
		    stat_fs.f_bsize, "NFS  ");

		if (m_ptr->m_flags & MRDONLY)
			waddstr(main_win, "ronly");
		else if (m_ptr->m_flags & MFREE)
			waddstr(main_win, "free ");
		else if (m_ptr->m_flags & MINUSE)
			waddstr(main_win, "inuse");
		else if (m_ptr->m_flags & MINTER)
			waddstr(main_win, "mount");
		else if (m_ptr->m_flags & MHADBAD)
			waddstr(main_win, "isbad");
		else
			waddstr(main_win, "N/A  ");


		switch (mount_type) {
		case BY_BLOCKS :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
				stat_fs.f_blocks, stat_fs.f_bfree);
			Draw_The_Text(main_win, 
				(100- (int)((stat_fs.f_bfree* 100)/ stat_fs.f_blocks)),
				i+ MENU_POS+ extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		case BY_INODES :
			mvwprintw(main_win, i+ MENU_POS+ extra, 54,"%8ld%8ld", 0, 0);
			Set_Colour(main_win, Colour_Brown);
			mvwprintw(main_win, i+ MENU_POS+ extra, 73,"%3d", 100);
			Set_Colour(main_win, Normal);
			waddstr(main_win, "%");
			break;

		default :
			mvwprintw(main_win, i + MENU_POS + extra, 47,"%8d%8d %6d %6d",
				stat_fs.f_blocks, stat_fs.f_bfree, 0, 0);

		}

		percent(i, (100- (int)((stat_fs.f_bfree * 100) / stat_fs.f_blocks)),
			-999);


	} else if (strcmp(fstyp_name, "HS") MATCHES) {

		/*
		 * +----------------------------------------------------------+
		 * |                  OK we have CDROM.                       |
		 * +----------------------------------------------------------+
		 */

		memset(&buf, 0, sizeof(buf));
		kmem_read(&buf, (long) m_ptr->m_bufp, sizeof(buf));

		memset(&hsfilsys, 0, sizeof(hsfilsys));
		kmem_read(&hsfilsys, (long) m_ptr->m_bufp, sizeof(hsfilsys));

		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d",
			i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), m_ptr->m_fstyp);

		if (major_flag) {
			print_dev(1, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev),
				dev_name, 0);
			if (mount_type)
				mvwprintw(main_win, i + MENU_POS + extra, 35, "/dev/%s",
					dev_name);
			else
				mvwprintw(main_win, i + MENU_POS + extra, 35, "%s",
					dev_name);
		} else
			mvwprintw(main_win, i+ MENU_POS+ extra, 35, "(%0.2d), (%0.2d)"
				, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev));

		mvwprintw(main_win, i + MENU_POS + extra, 18, "%4ld %s ",
		    hsfilsys.s_lbsize,
		    (hsfilsys.s_type & ISO9660 )  ? "I9660" : "HSFS ");

		if (m_ptr->m_flags & MRDONLY)
			waddstr(main_win, "ronly");
		else if (m_ptr->m_flags & MFREE)
			waddstr(main_win, "free ");
		else if (m_ptr->m_flags & MINUSE)
			waddstr(main_win, "inuse");
		else if (m_ptr->m_flags & MINTER)
			waddstr(main_win, "mount");
		else if (m_ptr->m_flags & MHADBAD)
			waddstr(main_win, "isbad");
		else
			waddstr(main_win, "N/A  ");


		switch (mount_type) {
		case BY_BLOCKS :
			mvwprintw(main_win, i+ MENU_POS+ extra, 54,"%8ld%8ld",
		    		(hsfilsys.s_volspacesz > 999999) ? 350000 :
		    		hsfilsys.s_volspacesz, 0);

			Set_Colour(main_win, Colour_Brown);
			mvwprintw(main_win, i+ MENU_POS+ extra, 73,"%3d", 100);
			Set_Colour(main_win, Normal);
			waddstr(main_win, "%");
			break;

		case BY_INODES :
			mvwprintw(main_win, i+ MENU_POS+ extra, 54,"%8ld%8ld", 0, 0);
			Set_Colour(main_win, Colour_Brown);
			mvwprintw(main_win, i+ MENU_POS+ extra, 73,"%3d", 100);
			Set_Colour(main_win, Normal);
			waddstr(main_win, "%");
			break;

		default :
			mvwprintw(main_win, i + MENU_POS + extra, 47,"%8d%8d %6d %6d",
		    		(hsfilsys.s_volspacesz > 999999) ? 350000 :
		    		hsfilsys.s_volspacesz, 0, 0);

		}

		percent(i, 100, -999);


	} else if (strcmp(fstyp_name, "DOS") MATCHES) {

		/*
		 * +----------------------------------------------------------+
		 * |                    OK we have DOS.                       |
		 * +----------------------------------------------------------+
		 */

		memset(&buf, 0, sizeof(buf));
		kmem_read(&buf, (long) m_ptr->m_bufp, sizeof(buf));

		memset(&dosfilsys, 0, sizeof(dosfilsys));
		kmem_read(&dosfilsys, (long) m_ptr->m_bufp, sizeof(dosfilsys));

		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d",
			i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), m_ptr->m_fstyp);

		if (major_flag) {
			print_dev(1, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev),
				dev_name, 0);
			if (mount_type)
				mvwprintw(main_win, i + MENU_POS + extra, 35, "/dev/%s",
					dev_name);
			else
				mvwprintw(main_win, i + MENU_POS + extra, 35, "%s",
					dev_name);
		} else
			mvwprintw(main_win, i+ MENU_POS+ extra, 35, "(%0.2d), (%0.2d)"
				, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev));

		mvwprintw(main_win, i + MENU_POS + extra, 18, "%4ld %s ",
		    m_ptr->m_bsize,
		    (m_ptr->m_bsize == 512)  ? "MSDOS" : "HSFS ");

		if (m_ptr->m_flags & MRDONLY)
			waddstr(main_win, "ronly");
		else if (m_ptr->m_flags & MFREE)
			waddstr(main_win, "free ");
		else if (m_ptr->m_flags & MINUSE)
			waddstr(main_win, "inuse");
		else if (m_ptr->m_flags & MINTER)
			waddstr(main_win, "mount");
		else if (m_ptr->m_flags & MHADBAD)
			waddstr(main_win, "isbad");
		else
			waddstr(main_win, "N/A  ");

		get_statfs(i, &stat_fs);

		switch (mount_type) {
		case BY_BLOCKS :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
				stat_fs.f_blocks, stat_fs.f_bfree);

			Draw_The_Text(main_win, 
				(100- (int)((stat_fs.f_bfree* 100)/ stat_fs.f_blocks)),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		case BY_INODES :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
				stat_fs.f_files, stat_fs.f_ffree);
			Draw_The_Text(main_win, 
		    		(100 - (int)((stat_fs.f_ffree * 100) / (stat_fs.f_files - 2))),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		default :
			mvwprintw(main_win, i + MENU_POS + extra, 47,"%8d%8d %6d %6d",
				stat_fs.f_blocks, stat_fs.f_bfree,
				stat_fs.f_files, stat_fs.f_ffree);
		}

		percent(i, (100 - (int)((stat_fs.f_bfree * 100)/stat_fs.f_blocks)),
		    -999);

	} else {

		/*
		 * +----------------------------------------------------------+
		 * |              Unknown file system type                    |
		 * +----------------------------------------------------------+
		 */

		memset(&buf, 0, sizeof(buf));
		kmem_read(&buf, (long) m_ptr->m_bufp, sizeof(buf));

		mvwprintw(main_win, i + MENU_POS + extra, 3, " %2d %3d %3d %2d",
			i, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev), m_ptr->m_fstyp);

		if (major_flag) {
			print_dev(1, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev),
				dev_name, 0);
			if (mount_type)
				mvwprintw(main_win, i + MENU_POS + extra, 35, "/dev/%s",
					dev_name);
			else
				mvwprintw(main_win, i+ MENU_POS+ extra, 35, "%s",
					dev_name);
		} else
			mvwprintw(main_win, i+ MENU_POS+ extra, 35, "(%0.2d), (%0.2d)"
				, bmajor(m_ptr->m_dev), minor(m_ptr->m_dev));

		get_statfs(i, &stat_fs);
		mvwprintw(main_win, i + MENU_POS + extra, 18, "%4ld %5s ",
		    stat_fs.f_bsize, fstyp_name);

		if (m_ptr->m_flags & MRDONLY)
			waddstr(main_win, "ronly");
		else if (m_ptr->m_flags & MFREE)
			waddstr(main_win, "free ");
		else if (m_ptr->m_flags & MINUSE)
			waddstr(main_win, "inuse");
		else if (m_ptr->m_flags & MINTER)
			waddstr(main_win, "mount");
		else if (m_ptr->m_flags & MHADBAD)
			waddstr(main_win, "isbad");
		else
			waddstr(main_win, "N/A  ");

		switch (mount_type) {
		case BY_BLOCKS :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
				stat_fs.f_blocks, stat_fs.f_bfree);

			Draw_The_Text(main_win, 
				(100 - (int)((stat_fs.f_bfree* 100)/ stat_fs.f_blocks)),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		case BY_INODES :
			mvwprintw(main_win, i + MENU_POS + extra, 54,"%8ld%8ld",
				stat_fs.f_files, stat_fs.f_ffree);
			Draw_The_Text(main_win, 
		    		(100- (int)((stat_fs.f_ffree* 100)/ (stat_fs.f_files- 2))),
				i + MENU_POS + extra, 73 ,MOUNT_BAR);
			waddstr(main_win, "%");
			break;

		default :
			mvwprintw(main_win, i + MENU_POS + extra, 47,"%8d%8d %6d %6d",
				stat_fs.f_blocks, stat_fs.f_bfree,
				stat_fs.f_files, stat_fs.f_ffree);
		}

		percent(i, (100- (int)((stat_fs.f_bfree * 100) / stat_fs.f_blocks)),
		    (100 - (int)((stat_fs.f_ffree * 100) / (stat_fs.f_files - 2))));

	}
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

void get_statfs(int slot, struct statfs *stat_fs)
{

FILE *mnt_fd;

struct mnttab mnt;

char *mnttab = "/etc/mnttab";
register int ctr;


    	mnt_fd = fopen(mnttab, "r");
	for (ctr = 0; ctr <= slot; fread(&mnt, sizeof mnt, 1, mnt_fd), ++ctr);

	statfs(mnt.mt_filsys, (struct stat_fs*)stat_fs, sizeof(* stat_fs), 0);
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int Change_Mnt_Type(void)                 Date: %Z% |
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

int Change_Mount_Type(int *mount_type)
{

extern int Sleep_Time;

WINDOW *win;

	win = newwin(5, 21, 11, 10);
	Set_Colour(win, Normal);
	Fill_A_Box(win, 0, 0);
	Draw_A_Box(win, BUTTON_BOX);
	mvwaddstr(win, 1, 2, "Display by Blocks");
	mvwaddstr(win, 2, 2, "Display by Inodes");
	mvwaddstr(win, 3, 2, "Display Combined");
	Set_Colour(win, Colour_Banner);
	mvwaddstr(win, 1, 13, "B");
	mvwaddstr(win, 2, 13, "I");
	mvwaddstr(win, 3, 10, "C");
	
	Set_Colour(win, Normal);

	wnoutrefresh(win);
	an_option();
	doupdate();
	while (TRUE) {
		switch(wgetch(bottom_win)) {
		case 'C' :
		case 'c' :
			*mount_type = BY_COMBIN;
			mvwaddstr(bottom_win, 1, 2, " Watchit mount average set to both.");
			break;

		case 'B' :
		case 'b' :
			*mount_type = BY_BLOCKS;
			mvwaddstr(bottom_win, 1, 2, " Watchit mount average set to blocks.");
			break;

		case 'I' :
		case 'i' :
			*mount_type = BY_INODES;
			mvwaddstr(bottom_win, 1, 2, " Watchit mount average set to inodes.");
			break;

		case 'Q' :	/* Quit and exit */
		case 'q' :
		case 27  :
			delwin(win);
			touchwin(main_win);
			return(0);

		default :
			an_option();
			beep();
			continue;
		}

		delwin(win); /* added this */
		touchwin(main_win);
		return(0);
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

void watch_mounts(int *mount_type)
{

int cmd = 0;
int Sleep_Time_Flag = TRUE;
int naptime = 0;
int ffast = FALSE;

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
		wi_mounts(*mount_type);
		wnoutrefresh(main_win);
		size_of_mnt_table();
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
			if (signal(SIGALRM, sig_alrm_mounts) == SIG_ERR)
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
				ffast = FALSE;
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 57, "Time");
				Set_Colour(main_win, Normal);
				naptime = Sleep_Time;
				Sleep_Time_Flag = TRUE;
				break;

			case 'S' :
			case 's' :
				ffast = FALSE;
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 63, "Slow");
				Set_Colour(main_win, Normal);
				naptime = 4;
				Sleep_Time_Flag = FALSE;
				break;

			case 'M' :
			case 'm' :
				ffast = FALSE;
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 68, "Med");
				Set_Colour(main_win, Normal);
				naptime = 2;
				Sleep_Time_Flag = FALSE;
				break;

			case 'F' :
				ffast = TRUE;
				naptime = -1;
				drawdisp(30);
				Set_Colour(main_win, Red_Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				Sleep_Time_Flag = FALSE;
				break;

			case 'f' :
				ffast = FALSE;
				drawdisp(30);
				Set_Colour(main_win, Blink_Banner);
				mvwaddstr(main_win, 0, 72, "Fast");
				Set_Colour(main_win, Normal);
				naptime = 1;
				Sleep_Time_Flag = FALSE;
				break;

			case 't' :
			case 'T' :
				Change_Mount_Type((int *)mount_type);
				mvwaddstr(main_win, 1, 7, "Reading /unix..........");
				wnoutrefresh(main_win);
				doupdate();
				mvwaddstr(main_win, 1, 7, "                         ");
				wi_mounts(*mount_type);
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

