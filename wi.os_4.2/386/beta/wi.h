/*
 *	%Z% %M%	Version %I% %D% - PKR.
 *
 *	Copyright (C) Paul Ready 1992.
 *	This Module contains Proprietary Information of PKR,
 *	and should be treated as Confidential.
 *
 * Notes:
 *		I should look at taking out some of the kernel entries in
 *		the nlist structre to save space.
 *						Sun Jul 18 15:08:14 EDT 1993 - PKR.
 */

/*
 *	%M% for wi
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/sysinfo.h>

#include <nlist.h>
#include <curses.h>
#include <sys/machdep.h>

/*
	Structure for getting all root u swap etc.....
*/
struct default_nodes {
	int	major_node;
	int	minor_node;
	char	bdev_name[DIRSIZ];
};

struct system_stats {
	time_t wi_start_time;
	struct sysinfo wi_sysinfo;
	struct minfo wi_minfo;

	long   wi_s5hits;
	long   wi_s5misses;
};

extern int	memfd;
extern int	kmemfd;
extern int	swapfd;

#define FAILS     == -1
#define MATCHES   == 0
#define NOT_MATCH == 1

#define WI_VERSION "v3.0"
#define WI_REV 97

/*
 *	Below are 3 macro's to keep the largest info available
 */

/*
 *	#define sys_value(x) ((sysinfo.x - last_sysinfo.x) / Sleep_Time)
 */

#define sys_value(x) (sysinfo.x - last_sysinfo.x)
#define Max_Sysinfo_Val(x) ((sys_value(x) > max_sysinfo.x) ? max_sysinfo.x = sys_value(x), sys_value(x) : max_sysinfo.x)

/*
 *	#define minfo_value(x) ((minfo.x - last_minfo.x) / Sleep_Time)
 */

#define minfo_value(x) (minfo.x - last_minfo.x)
#define Max_Minfo_Val(x) ((minfo_value(x) > max_minfo.x) ? max_minfo.x = minfo_value(x), minfo_value(x) : max_minfo.x)

#define min(a,b) (((a) > (b)) ? (b) : (a))

#define NUM_OF_HASH_DISKS 31

#define TOTAL_COLOURS	20

#define NM_V		0
#define NM_FILE		1
#define NM_INODE		2
#define NM_PROC		3
#define NM_MOUNT		4
#define NM_BUFFER		5
#define NM_SWPLO		6
#define NM_TIME		7
#define NM_LBOLT		8
#define NM_UTSNAME	9
#define NM_USER		10
#define NM_KMAXMEM	11

#define NM_MSGINFO	12
#define NM_SEMINFO	13

#define NM_KNMUXLINK	14

#define NM_SYSINFO	15

#define NM_KAVAILMEM	16
#define NM_KPHYSMEM	17
#define NM_SHMINFO	18
#define NM_REGION		19

#define NM_MINFO		20
#define NM_SWAPBLKS	21
#define NM_STREAM		22

#define NM_SIOTTY		23
#define NM_CALLOUT	24
#define NM_FLSTATS	25
#define NM_WDSTATS	26
#define NM_CTSTATS	27
#define NM_SDSKSTATS	28
#define NM_STPSTATS	29
#define NM_SDSK_NUM	30
#define NM_S5CACHEHITS	31
#define NM_S5CACHEMISSES	32
#define NM_STRST		33
#define NM_RBSIZE		34
#define NM_NMBLOCK	35
#define NM_SROMSTATS	36
#define NM_SWRMSTATS	37
#define NM_IDASTATS	38
#define NM_ESDISTATS	39
#define NM_SYSERR		40
#define NM_FLCKINFO	41
#define NM_FSINFO		42

#define NM_NAMES		43

#define SCALE "0...10...20...30...40...50...60...70...80...90..100"

#define PAGE_LEN		17
#define L_PAGE_LEN	35

/* for mounts.c */
#define MNT_MAX_LIMIT	80

/* for cpu.c */
#define CPU_MAX_LIMIT	80

#define CPU_MIN_LIMIT	15

/* for vars.c */
#define PER_MAX_NINODE	85
#define PER_MAX_NFILE 	80
#define PER_MAX_NMOUNT	95
#define PER_MAX_NPROC 	80
#define PER_MAX_NREGION	80
#define PER_MAX_NCALL	80

extern int mnt_max_limit;
extern int cpu_max_limit;
extern int cpu_min_limit;
extern int per_max_ninode;
extern int per_max_nfile;
extern int per_max_nmount;
extern int per_max_nproc;
extern int per_max_nregion;
extern int per_max_ncall;

extern int page_len;

extern WINDOW *top_win, *main_win, *bottom_win, *cache_win;

#define Sleep_Time_Def	2
#define Sleep_Time_Min	1
#define Sleep_Time_Max	99

#define DEMO_TIME		120

extern int     nswap;
extern daddr_t swplo;
extern time_t  ktime;
extern time_t  klbolt;

extern struct syserr syserr;
extern struct flckinfo flckinfo;
extern struct system_stats wi_start;
extern struct default_nodes	nodelist[];
extern struct file	*files;
extern struct inode	*inodes;
extern struct region	*regions;
extern struct mount	*mounts;
extern struct proc	*procs;
extern struct sysinfo	last_sysinfo;
extern struct sysinfo	max_sysinfo;
extern struct sysinfo	sysinfo;
extern struct syswait	syswait;
extern struct syswait	max_syswait;
extern struct minfo	minfo;
extern struct minfo	last_minfo;
extern struct minfo	max_minfo;
extern struct msginfo	msginfo;
extern struct seminfo	seminfo;
extern struct shminfo	shminfo;
extern struct user	user;
extern struct user	*users;
extern struct utmp	utmps[64];
extern struct utsname	utsname;
extern struct var		v;
extern struct nlist	namelist[];
extern struct fsinfo	*fsys_info;

extern long	s5hits;
extern long	last_s5hits;
extern long	s5misses;
extern long	last_s5misses;

extern time_t start_ticks;
extern time_t current_ticks;

extern unsigned kmaxmem;

extern unsigned kavailmem;
extern unsigned kphysmem;
extern unsigned kusermem;

extern int Sleep_Time;

#define DEMO_DATE 710611200		/* time format (Joanie's Birthday) */

/*
 * +--------------------------------------------------------------------+
 * |                          New Vars. - v2.3                          |
 * +--------------------------------------------------------------------+
 */

#define MAX_NICE_VALUE 40
#define MIN_NICE_VALUE 0

#define CHANGE_NICE

extern int nice_enabled;

extern int major_flag;			/* flag to see if to get dev or not	*/
extern int	disptype;
extern int size_flag;
extern int socket_flag;
extern int socksys_id;

extern short pid;
extern int demo_ver;

extern int Misc_Info;

extern int disptype;
extern int size_flag;
extern int page_len;

extern long demo_time;

extern int Graphics_Mode;
extern int The_Current_Avg;
extern char The_Current_tty[];
extern int Curent_Saved_Value;

#define UNCOMPRESSED_DISP 0
#define COMPRESSED_DISP	1
#define GRAPHICS_MODE	2
#define SCREEN_D_MODE	3
#define CURRENT_AVG	4
#define WATCHIT_AVG	5
#define BOOT_AVG		6
#define SCREEN_CLEAR	7
#define SCREEN_DUMP	8
#define NOT_DEFINED	9
#define BY_TIME		10
#define BY_PERCNT		11
#define BY_ERRORS		12
#define A_DUMMY		99

#define G_NORMAL	0
#define G_QUITE	1

#define SD_NORMAL	0
#define SD_QUITE	1

#define DISK_AVG_1 0
#define DISK_AVG_2 1

/*
	Defines for the menu.
*/

#define PAGE_COUNTER 4

#define CUR_INVIS	0
#define CUR_VIS	1

/*
 * +--------------------------------------------------------------------+
 * |                          New Vars. - v3.0                          |
 * +--------------------------------------------------------------------+
 */

#define PID		0
#define SCPU	1
#define UCPU	2
#define NICE	3
#define PRI		4
#define SIZE	5

extern int System_Sleep_Time;

#define _AVG_CPU_IDLE		0
#define _AVG_CPU_USER		1
#define _AVG_CPU_KERNEL		2
#define _AVG_CPU_WAIT		3
#define _AVG_CPU_SXBRK		4
#define _AVG_CPU_TOTAL		5
#define _AVG_CPU_TOTAL_2	6

#define _AVG_W_IO			7
#define _AVG_W_SWAP		8
#define _AVG_W_PIO		9
#define _AVG_W_TOTAL		10

extern int cpu_averages[11];

#define F_OK	0
#define ONEAC_UPS	"/etc/watchit/bin/watchups"

enum states {
	Normal = 1, State_Normal, State_Warning, State_Alarm,
	Text_Normal, Text_Warning, Text_Alarm, Colour_Banner,
	Blink_Banner, Colour_Black, Colour_White, Colour_Bold,
	Colour_Blue, Colour_Magenta, Colour_Grey, Colour_Brown,
	Red_Banner, Red_Blink_Banner, Colour_Ltcyan, Colour_Ltblue
};

extern long colour[];
extern long mono[];
extern int colour_flag;

#define Set_Colour(win, colour_name) wattrset(win, (colour_flag) ? colour[colour_name] : mono[colour_name])

#define Colour(win, colour_name) wattron(win, (colour_flag) ? colour[colour_name] : mono[colour_name])

#define ANSI_ACS_CKBOARD '0'
#define ANSI_ACS_SOLID   '['
#define ANSI_ACS_VTHICK  ']'
#define ANSI_ACS_HTHICK  '_'

#define ANSI_BOX_RTEE  '9'
#define ANSI_BOX_LTEE  'F'
#define ANSI_BOX_VLINE ':'
#define ANSI_BOX_HLINE 'M'
#define ANSI_BOX_ULCORNER ACS_ULCORNER
#define ANSI_BOX_LLCORNER 'T'
#define ANSI_BOX_URCORNER '7'
#define ANSI_BOX_LRCORNER '<'

#define HORZ_LINE 0
#define VERT_LINE 1

#define THICK_LINE 0
#define THIN_LINE 1

#define NORMAL  0
#define REVERSE 1
#define COLOUR_ONLY   -696969, -696969

#define DEFAULT_BAR  NORMAL,  80, 90
#define NORMAL_BAR   NORMAL,  80, 95
#define REVERSE_BAR  REVERSE, 20, 10
#define CACHE_BAR    REVERSE, 85, 70

#define USER_BAR     NORMAL,  70, 85
#define KERNEL_BAR   NORMAL,  70, 85
#define WAIT_BAR     NORMAL,  70, 85
#define IDLE_BAR     REVERSE, 40, 20
 
#define MEMORY_BAR   NORMAL,  80, 90
#define SWAP_BAR     NORMAL,  50, 85

#define MOUNT_BAR    NORMAL,  80, 90
#define IO_BAR       NORMAL,  75, 90
#define STREAM_BAR  REVERSE,  25, 10

#define DEFAULT_BOX  THICK_LINE, 0, 0, 0, 0, Colour_White, Colour_Black
#define BUTTON_BOX   THIN_LINE, 0, 0, 0, 0, Colour_White, Colour_Black

#define TOP_WIN         newwin(3, 79, 0, 0)

#define MAIN_WIN        newwin(19, 79, 3, 0)
#define LONG_MAIN_WIN   newwin(37, 79, 3, 0)

#define SMALL_WIN       newwin(3, 79, 21, 0)
#define BOTTOM_WIN      newwin(3, 79, 22, 0)
#define LONG_BOTTOM_WIN newwin(3, 79, 40, 0)

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
