/*
 * +--------------------------------------------------------------------+
 * | Function: wi.c                                      Date: 92/04/04 |
 * | Author: Paul Ready.                                              |
 * |                                                                    |
 * | Notes:                                                             |
 * |           The bit that opens /unix and gets all the info.          |
 * +--------------------------------------------------------------------+
 *
 * Note:
 *		This is the unix version of watchit I don't think that it is
 *		going to be as easy to convert to unix from xenix as people
 *		in the world think.......
 *		As I have to learn the header files all over again.
 *
 *								   14th June 1992 - PKR.
 * Updates:   
 * 1) v2.1	This update will include different screen colours,
 *		and command line args.
 *
 * 2) v2.3c Add a structure for all the default routes.
 *						Tue Aug 10 20:49:01 EDT 1993 - PKR.
 *
 * 3) v3.0a The start of version 3.0, this version removes shm.
 *						Tue Nov 09 09:58:29 EST 1993 - PKR.
 *
 * 4) v3.0b The start of coding the wi.small.
 *						Fri Jan 07 00:42:54 EST 1994 - PKR.
 *
 *    Bugs:
 *(1) v2.3 Now shows an error if the screen can not change to the long
 *		screen format.
 *						Sun Jul 18 21:40:39 EDT 1993 - PKR / Done.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%			Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%			Version %I% %D% - Dapix "
#endif /* __STDC__ */


#include "wi.h"

#include <sys/utsname.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/dir.h>
#include <sys/var.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/user.h>
#include <sys/immu.h>
#include <sys/region.h>
#include <sys/proc.h>
#include <sys/sysinfo.h>
#include <sys/stream.h>
#include <sys/strstat.h>
#include <sys/callo.h>
#include <sys/page.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/buf.h>
#include <sys/stream.h>
#include <sys/signal.h>
#include <sys/flock.h>

#define SPLIT_WI "/etc/watchit/bin/wi.sh"

struct xdevmap	xdevmap[] = {		/* Dummy item to stop errors	*/
	0,	0
};

int	nxdevmaps = FALSE;			/* Dummy item to stop errors	*/

/*
	Misc flags.
*/
int     demo_ver = FALSE;		/* is a demo version ?			*/
time_t demo_time = 0L;			/* time to end demo			*/

int colour_flag	 = FALSE;		/* is colour set				*/
int override_flag = FALSE;		/* override length ?			*/
int os_ver		 = FALSE;		/* a 3.2v4.2 version			*/
int nice_enabled	 = FALSE;		/* enable nice change			*/
int no_kmem_write = FALSE;		/* write enable for nice ?		*/

short demo_pid	 = FALSE;		/* demo pid to kill			*/

int	disptype	 = FALSE;		/* display type (ioctl)			*/
int major_flag	 = FALSE;		/* is a major ?				*/
int size_flag	 = FALSE;		/* long format ?				*/
int socket_flag	 = FALSE;		/* is socket set				*/
int socksys_id	 = FALSE;		/* if true socket major			*/
int knmuxlink	 = FALSE;		/* flag for streams			*/

int page_len	 = PAGE_LEN;		/* current page length			*/
int num_of_hash_disks	 = FALSE;	/* the size of the hash table		*/
int Sleep_Time = 	FALSE;

int nswap	 = FALSE;			/* size of swap				*/

int Graphics_Mode = GRAPHICS_MODE;	/* graphics mode for Console Capture	*/
int The_Current_Avg = CURRENT_AVG;	/* The current avg. value		*/

char The_Current_tty[4];		/* The current tty number		*/
int Curent_Saved_Value = FALSE;	/* The current saved display item.	*/
int ansi_flag	 = FALSE;		/* is term ansi of not			*/
int Con_Cap		 = FALSE;		/* Will Console Capture work.		*/

int The_Disk_Avg = DISK_AVG_1;		/* Setup the disk average.		*/

#ifdef _ALPHA
int Doc_Flag	 = FALSE;		/* if doing docs set this		*/
#endif

#ifdef _BETA
int Doc_Flag	 = FALSE;		/* if doing docs set this		*/
#endif

char term_type[64];			/* The current TERM value		*/
int lines	= 0;
int cols	= 0;

/*
	Kernel values and structures.
*/
daddr_t swplo;					/* swap value			*/
time_t  ktime;					/* kernel time			*/
time_t  klbolt;					/* sys up time			*/

int	unmask	 = FALSE;			/* current umask value		*/

unsigned kmaxmem;					/* size of user mem: 12Mb	*/
unsigned kavailmem;				/* available mem left: 4Mb	*/
unsigned kphysmem;				/* the physical mem: 16Mb	*/

char	*namefile = "/unix";			/* default name			*/
char	*corefile = "/dev/mem";			/* default core name		*/
char	*kmemfile = "/dev/kmem";		/* default kernel name name	*/
char	*swapfile = "/dev/swap";		/* default swap name		*/

file_t		*files;			/* kernel file structre		*/
inode_t		*inodes;			/* kernel inode structre	*/
struct region	*regions;			/* kernel region structre	*/
struct minfo	minfo;			/* kernel minfo structre	*/
struct minfo	last_minfo;			/* kernel minfo last structre	*/
struct minfo	max_minfo;			/* kernel minfo max structre	*/
struct msginfo	msginfo;			/* kernel msg structre		*/
struct mount	*mounts;			/* kernel mount structre	*/
proc_t		*procs;			/* kernel poocess structre	*/
struct scoutsname	scoutsname;			/* kernel uname structre	*/
struct seminfo	seminfo;			/* kernel sem structre		*/
struct shminfo	shminfo;			/* kernel shm structre		*/
struct stat		f_info;			/* kernel stat structre		*/
struct sysinfo	sysinfo;			/* kernel sysinfo structre	*/
struct sysinfo	last_sysinfo;		/* kernel last sysinf struct	*/
struct sysinfo	max_sysinfo;		/* kernel max sysinf struct	*/
user_t		user;				/* kernel user structre		*/
user_t		*users;			/* kernel ptr to user struct	*/
struct utsname	utsname;			/* kernel sysname structre	*/
struct var		v;				/* kernel var structre		*/
struct syserr	syserr;			/* kernel table overflows	*/
struct flckinfo	flckinfo;			/* kernel flckinfo overflows	*/

struct system_stats wi_start;			/* stats when watchit started	*/

long	s5hits	  = FALSE;			/* kernel cach hits		*/
long	last_s5hits   = FALSE;			/* kernel last cach hits	*/
long	s5misses	  = FALSE;			/* kernel cach misses 		*/
long	last_s5misses = FALSE;			/* kernel last cach misses	*/

time_t start_ticks	 = 0L;			/* kernel ticks			*/
time_t current_ticks	 = 0L;			/* kernel ticks			*/

proc_t *current_procs	 = (proc_t *)0;	/* kernel ptr to proc struct	*/
proc_t **ptr_procs	 = (proc_t **)0;	/* kernel ptr to ptr to proc	*/

int cpu_averages[11];

/*
	system fd's.
*/
int	memfd		 = FALSE;			/* mem fd, read			*/
int	kmemfd	 = FALSE;			/* kmem fd, read			*/
int	w_kmemfd	 = FALSE;			/* kmem fd, write			*/
int	swapfd	 = FALSE;			/* mem fd, read			*/

/*
	defined functions
*/
char *ttyname();					/* get current tty name		*/
char *getenv();					/* get an env value		*/
void any_key();					/* get any key input		*/

/*
	brand values.
*/
static char  brand[]	    =  "DEMO_VERSION";	/* If demo or not?		*/
static char	dummy1[]	    =  "LIVE_VERSION"; 	/* dummy demo flag		*/
static char	dummy2[]	    = "UNIX_VERSION"; 	/* dummy demo flag		*/
static char	sys_serial_no[] =  "9999999999";	/* the system wi can run on */

/*
	This variable is used to set up the demo date
*/
static char	end_demo_date[] = "324563153";	/* the end of the demo	*/
static char	wi_key[] =  "wi0123456789";		/* the system wi can run on	*/

/*
	system limits.
*/
int mnt_max_limit   = MNT_MAX_LIMIT;		/* max mnt limit		*/
int cpu_max_limit   = CPU_MAX_LIMIT;		/* max cpu limit		*/
int cpu_min_limit   = CPU_MIN_LIMIT;		/* min cpu limit		*/
int per_max_ninode  = PER_MAX_NINODE;		/* max ninode limit	*/
int per_max_nfile   = PER_MAX_NFILE;		/* max nfile limit	*/
int per_max_nmount  = PER_MAX_NMOUNT;		/* max nmount limit	*/
int per_max_nproc   = PER_MAX_NPROC;		/* max nproc limit	*/
int per_max_ncall   = PER_MAX_NCALL;		/* max ncall limit	*/
int per_max_nregion = PER_MAX_NREGION;		/* max nregion limit	*/

/*
	I wonder how many of these will be here and how many I will have to change
	If you wan't to know then find out yourself.
*/
struct nlist namelist[] = {
	{ "v",			0,	0,	0,	0,	0, },
	{ "file",			0,	0,	0,	0,	0, },
	{ "inode",			0,	0,	0,	0,	0, },
	{ "proc",			0,	0,	0,	0,	0, },
	{ "mount",			0,	0,	0,	0,	0, },
	{ "buf",			0,	0,	0,	0,	0, },
	{ "swplo",			0,	0,	0,	0,	0, },
	{ "time",			0,	0,	0,	0,	0, },
	{ "lbolt",			0,	0,	0,	0,	0, },
	{ "utsname",		0,	0,	0,	0,	0, },
	{ "u",			0,	0,	0,	0,	0, },
	{ "maxmem",			0,	0,	0,	0,	0, },
	{ "msginfo",		0,	0,	0,	0,	0, },
	{ "seminfo",		0,	0,	0,	0,	0, },
	{ "nmuxlink",		0,	0,	0,	0,	0, },
	{ "sysinfo",		0,	0,	0,	0,	0, },
	{ "freemem",		0,	0,	0,	0,	0, },
	{ "physmem",		0,	0,	0,	0,	0, },
	{ "shminfo",		0,	0,	0,	0,	0, },
	{ "region",			0,	0,	0,	0,	0, },
	{ "minfo",			0,	0,	0,	0,	0, },
	{ "nswap",			0,	0,	0,	0,	0, },
	{ "streams",		0,	0,	0,	0,	0, },
	{ "sio_tty",		0,	0,	0,	0,	0, },
	{ "callout",		0,	0,	0,	0,	0, },
	{ "flstats",		0,	0,	0,	0,	0, },
	{ "wd1010stats",		0,	0,	0,	0,	0, },
	{ "ctstats",		0,	0,	0,	0,	0, },
	{ "Sdskstats",		0,	0,	0,	0,	0, },
	{ "Stpstats",		0,	0,	0,	0,	0, },
	{ "Sdsk_num_disks",	0,	0,	0,	0,	0, },
	{ "s5cachehits",		0,	0,	0,	0,	0, },
	{ "s5cachemisses",	0,	0,	0,	0,	0, },
	{ "strst",			0,	0,	0,	0,	0, },
	{ "rbsize",			0,	0,	0,	0,	0, },
	{ "nmblock",		0,	0,	0,	0,	0, },
	{ "Sromstats",		0,	0,	0,	0,	0, },
	{ "Swrmstats",		0,	0,	0,	0,	0, },
	{ "idastats",		0,	0,	0,	0,	0, },
	{ "esdistats",		0,	0,	0,	0,	0, },
	{ "syserr",			0,	0,	0,	0,	0, },
	{ "flckinfo",		0,	0,	0,	0,	0, },
	{ (char *) 0,		0,	0,	0,	0,	0, }
};

/*
	Hash table for disk info.
*/
struct default_nodes nodelist[] = {
	{ 1, 40, "root"    }, 		/*	default name for root filesys    */
	{ 1, 41, "swap"    }, 		/*	default name for swap filesys    */
	{ 1, 42, "u"       }, 		/*	default name for u filesys       */
	{ 1, 43, "user1"   }, 		/*	default name for user 1 filesys  */
	{ 1, 44, "user2"   }, 		/*	default name for user 2 filesys  */
	{ 1, 45, "user3"   }, 		/*	default name for user 3 filesys  */
	{ 1, 46, "recover" }, 		/*	default name for recover filesys */
	{ 1, 47, "hd0a"    },  		/*	default name for all hard disk   */

	{ 1, 104, "root"    }, 		/*	default name for root filesys    */
	{ 1, 105, "swap"    }, 		/*	default name for swap filesys    */
	{ 1, 106, "u"       }, 		/*	default name for u filesys       */
	{ 1, 107, "user1"   }, 		/*	default name for user 1 filesys  */
	{ 1, 108, "user2"   }, 		/*	default name for user 2 filesys  */
	{ 1, 109, "user3"   }, 		/*	default name for user 3 filesys  */
	{ 1, 110, "recover" }, 		/*	default name for recover filesys */
	{ 1, 111, "hd10"    }, 		/*	default name for all hard disk   */

	{ 45, 40, "root"    }, 		/*	default name for root filesys    */
	{ 45, 41, "swap"    }, 		/*	default name for swap filesys    */
	{ 45, 42, "u"       }, 		/*	default name for u filesys       */
	{ 45, 43, "user1"   }, 		/*	default name for user 1 filesys  */
	{ 45, 44, "user2"   }, 		/*	default name for user 2 filesys  */
	{ 45, 45, "user3"   }, 		/*	default name for user 3 filesys  */
	{ 45, 46, "recover" }, 		/*	default name for recover filesys */
	{ 45, 47, "hd10"    }, 		/*	default name for all hard disk   */

	{ 54, 40, "root"    }, 		/*	default name for root filesys    */
	{ 54, 41, "swap"    }, 		/*	default name for swap filesys    */
	{ 54, 42, "u"       }, 		/*	default name for u filesys       */
	{ 54, 43, "user1"   }, 		/*	default name for user 1 filesys  */
	{ 54, 44, "user2"   }, 		/*	default name for user 2 filesys  */
	{ 54, 45, "user3"   }, 		/*	default name for user 3 filesys  */
	{ 54, 46, "recover" }, 		/*	default name for recover filesys */
	{ 54, 47, "hd10"    } 		/*	default name for all hard disk   */

};

struct default_nodes	nodelist[];

/*
	curses windows.
*/
WINDOW  *top_win, *main_win, *bottom_win, *cache_win, *full_win;

/*
 * +--------------------------------------------------------------------+
 * | Function: int end_of_demo(int sig)                  Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Catch the alarm call & cleanup                              |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void end_of_demo(void)
{

	beep();
	mvwaddstr(bottom_win, 1, 1, "Sorry your DEMO time has elapsed - cleaning up. ");
	w_clrtoeol(bottom_win, 0, 0);
	wrefresh(bottom_win);
	if (demo_pid != 999999)
		kill(demo_pid, 15);
	sleep(2);

	fflush(stdout);
	fflush(stderr);
	clear();
	endwin();
	if (size_flag) {
		if (! ((disptype = ioctl(0, CONS_CURRENT))FAILS)) {
			switch(disptype) {
			case (CGA):
				ioctl(0, SW_ENHC80x25, 0L);
				break;

			case (EGA):
				ioctl(0, SW_ENHC80x25, 0L);
				break;

			case (VGA):
				ioctl(0, SW_VGA80x25, 0L);
				break;

			default :
				break;
			}
		}
	}
	printf("\f\nThe demo time has expired.\nbye bye....\n");
	exit(1);
}


/*
 * +--------------------------------------------------------------------+
 * | Function: int interupt(int sig)                     Date: @(#) |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Catch any signals and cleanup                               |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void interupt()
{

	kill(demo_pid, 15);
	fflush(stdout);
	fflush(stderr);
	clear();
	endwin();

	if (size_flag) {
		if (! ((disptype = ioctl(0, CONS_CURRENT))FAILS)) {
			switch(disptype) {
			case (CGA):
				ioctl(0, SW_ENHC80x25, 0L);
				break;
			case (EGA):
				ioctl(0, SW_ENHC80x25, 0L);
				break;
			case (VGA):
				ioctl(0, SW_VGA80x25, 0L);
				break;
			default :
				break;
			}
		}
	}
	printf("\f\007\nOuch !\n\nbye bye....\n");
	exit(1);
}

/*
 * +--------------------------------------------------------------------+
 * | Function:                                           Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |        This is where we should put the wi start info.              |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

void Get_Start_Stats(void)
{

register int counter = 0;

	kmem_read(&wi_start.wi_start_time, namelist[NM_TIME].n_value,
		sizeof(ktime));

	kmem_read(&wi_start.wi_sysinfo, namelist[NM_SYSINFO].n_value,
		sizeof(struct sysinfo));

	kmem_read(&wi_start.wi_minfo, namelist[NM_MINFO].n_value,
		sizeof(struct minfo));

	kmem_read(&wi_start.wi_s5hits, namelist[NM_S5CACHEHITS].n_value,
		sizeof(s5hits));

	kmem_read(&wi_start.wi_s5misses, namelist[NM_S5CACHEMISSES].n_value,
		sizeof(s5misses));

	for (counter = 0; counter <= 11; counter++)
		cpu_averages[counter] = 0;


/*
	A test to write a logfile, and read it back into a structure

long fd;

struct system_stats wi_start;
struct system_stats w2;


	if ((fd = open("/etc/watchit/log1", (O_RDWR | O_CREAT), 0666)) < 0) {
		perror("open on logfile");
		sleep(2);
	}
	chmod("/etc/watchit/log1", 0644);

	if (write(fd, &wi_start, sizeof(struct system_stats))FAILS) {
		perror("write on logfile");
	}

	lseek(fd, 0L, 0);
	
	if (read(fd, &w2, sizeof(struct system_stats))FAILS) {
		perror("read on logfile");
	}
	close(fd);
*/
	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: int main()                                Date: %D% |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |       	Where it all starts.                                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *    Bugs:
 *          None yet.
 */

int main(int argc, char **argv)
{

register int x = 0, y = 0;

char	logfile[32]   = "";
char	o_logfile[32] = "";
int	c	 = FALSE;
int  err_id	 = FALSE;
short pid;

extern int	optind;
extern char	*optarg;

#ifdef _PROTECTED
register ctr2 = 0;
char utstest[12];
#endif

char terment[1024];
char stdoutbuf[2048];
extern char term_type[64];
char *tty_ptr;
register ctr;

	ansi_flag = FALSE;
	unmask = umask();
	Sleep_Time = Sleep_Time_Def;

	__scoinfo(&scoutsname, sizeof(struct scoutsname));

#ifdef _PROTECTED

	/*
		check to see if I can run at all.
	*/
	if (atoi(end_demo_date) == 324563153) {
		printf("\007watchit: DEMONSTRATION VERSION - NOT INSTALLED.\n");
		exit(1);

	}
	if (strcmp(brand, "DEMO_VERSION") MATCHES) {
		if (time((long *) 0)  >= atol(end_demo_date)) {
			printf("\007watchit: DEMONSTRATION VERSION - DATE STAMP EXPIRED.\n");
			exit(1);
		}
		demo_ver = TRUE;
		demo_time = time((long *) 0) + DEMO_TIME;
	} else {

		demo_ver = FALSE;

		/* Convert to a number only.  */
		for (ctr = ctr2 = 0; scoutsname.sysserial[ctr] != '\0'; ctr++)
			if (scoutsname.sysserial[ctr] >= '0' && scoutsname.sysserial[ctr] <= '9')
				utstest[ctr2++] = scoutsname.sysserial[ctr];
		utstest[ctr2] = '\0';
			
		if (!(atoi(sys_serial_no) == atoi(utstest))) {
			printf("\007watchit: error on serialization for %s.\n",
				scoutsname.sysserial);
			exit(1);
		}
	}
#else
	demo_ver = FALSE;
#endif

	/*
		OK They are either running a DEMO version or they paid for it
		now this setvbuf realy speeds things up.

		Sort out command line arguments.
	*/

	num_of_hash_disks = NUM_OF_HASH_DISKS;

	/*
		Check for the O/S Version for disk I/O
	*/
	os_ver	 = FALSE;
	if ((strcmp(scoutsname.release, "3.2v4.2"))MATCHES)
		os_ver = TRUE;

	nice_enabled  = FALSE;
	override_flag = FALSE;

	major_flag    = TRUE;
	size_flag     = FALSE;

#ifdef _ALPHA
	while ((c = getopt(argc, argv, "DsrntlhdvgoC:N:S:")) != EOF) {
		switch (c) {
		case 'D' :
			Doc_Flag = TRUE;
			system("/usr/bin/setcolour -r hi_white black");
			break;

#else
#	ifdef _BETA
	while ((c = getopt(argc, argv, "DsrntlhdvgoC:N:S:")) != EOF) {
		switch (c) {
		case 'D' :
			Doc_Flag = TRUE;
			system("/usr/bin/setcolour -r hi_white black");
			break;

#	else

	while ((c = getopt(argc, argv, "srntlhdvgoC:N:S:")) != EOF) {
		switch (c) {
#	endif
#endif
		case 'r' :
			/*
				Check to see if the shellscript version exists.
				if not the just continue
			*/
			if ((access(SPLIT_WI, X_OK)) == 0) {
				if ((pid = fork()) < 0)		/* fork failed.  */
					system(SPLIT_WI);
				else if (pid == 0)		/* the parent */
					exit(0);
				else					/* the child */
					execl("/bin/sh", "sh", "-c", SPLIT_WI, (char *)NULL);
				exit(0);
			} else {
				printf("\n\007wi: sorry I'm unable to access \"%s\", running in standard mode\n", SPLIT_WI);
				sleep(4);
			}
			break;

		case 'o' :
			override_flag = TRUE;
			break;

		case 'G' :
		case 'g' :
			Graphics_Mode = SCREEN_D_MODE;
			break;

		case 'n' :
			nice_enabled = TRUE;
			break;

		case 't' :
			num_of_hash_disks = 7;
			break;

		case 'l' :
			size_flag = TRUE;
			break;

		case 'h' :
			help_message();
			break;

		case 'd' :
			major_flag = FALSE;
			break;

		case 's' :
				printf("watchit's serial number is: %s - Dapix.\n", wi_key);
			exit(0);

		case 'v' :
#ifdef _BETA
#	ifdef _OLD_OS
				printf("watchit(pre v4.2)		Version %sb %D% - Dapix.\n", WI_VERSION);
#	else
				printf("watchit(v4.2)		Version %sb %D% - Dapix.\n", WI_VERSION);
#	endif
#else
#	ifdef _OLD_OS
				printf("watchit(pre v4.2)		Version %sc %D% - Dapix.\n", WI_VERSION);
#	else
				printf("watchit(v4.2)		Version %sc %D% - Dapix.\n", WI_VERSION);
#	endif
#endif
			exit(0);
			break;

		case 'C' :
			corefile = optarg;
			kmemfile = optarg;
			break;

		case 'N' :
			namefile = optarg;
			break;

		case 'S' :
			swapfile = optarg;
			break;

		default :
			usage();
		}
	}


	/*
		Get the current screen size.
	*/
	if ((tty_ptr = getenv("TERM")) == NULL) {
		printf("\007wi: Sorry I kneed the \"TERM\" variable set\n");
		exit(1);
	}

	sprintf(term_type,"%s", tty_ptr);

	if (!tgetent(terment, term_type)) {
		printf("\007wi: Sorry I can't find %s in /etc/termcap\n", tty_ptr);
		exit(1);
	}

	lines = tgetnum("li");
	cols  = tgetnum("co");

	if ((tty_ptr = getenv("LINES")) != NULL)
		lines = strtol(tty_ptr, NULL, 10);
	if ((tty_ptr = getenv("COLS")) != NULL)
		cols = strtol(tty_ptr, NULL, 10);

	tty_ptr = (char *) NULL;

	/*
		get the current tty number
	*/

	tty_ptr = ttyname(0);
	for (ctr = 0; ctr <= 7; tty_ptr++, ctr++);
	strcpy(The_Current_tty, tty_ptr);

	setvbuf(stdout, stdoutbuf, _IOFBF, sizeof(stdoutbuf));
	setbuf(stderr, NULL);

	if (strcmp(term_type, "ansi") MATCHES) {
		ansi_flag = TRUE;
	}

	if (! ((disptype = ioctl(0, CONS_CURRENT))FAILS)) {
		switch(disptype) {
		case (CGA):
		case (EGA):
		case (VGA):
			if (size_flag) {
				if ((err_id = ioctl(0, SW_ENHC80x43, 0L)) == -1) {
					size_flag = FALSE;
					page_len = PAGE_LEN;
					fprintf(stderr, "\007watchit: 43 lines is not supported.\n");
					fflush(stderr);
					sleep(2);
				} else {
					page_len = L_PAGE_LEN;
				}
			}
			break;

		default :
			/* I don't know who I am........  */
			if (override_flag) {
				size_flag = TRUE;
				page_len = L_PAGE_LEN;
			}
			break;
		}
	} else if (override_flag) {
		size_flag = TRUE;
		page_len = L_PAGE_LEN;
	}

	/*
		Go for it.
	*/

	system("/usr/bin/tput clear");

#ifdef _ALPHA
	if (Doc_Flag)
		fprintf(stderr, "\n\n\n\nwatchit ALPHA version %s DOCUMENT REL.\n", WI_VERSION);
	else
		fprintf(stderr, "\n\n\n\nwatchit ALPHA version %s NOT FOR RESALE.\n", WI_VERSION);
	fprintf(stderr, "Copyright(C) Dapix Technologies 1991-1994.\n");
	fprintf(stderr, "\n\n\n\n\nwatchit now entering the system..............");
#else
#	ifdef _BETA
	if (Doc_Flag)
		fprintf(stderr, "\n\n\n\nwatchit BETA version %s DOCUMENT REL.\n", WI_VERSION);
	else
		fprintf(stderr, "\n\n\n\nwatchit BETA version %s NOT FOR RESALE.\n", WI_VERSION);
	fprintf(stderr, "Copyright(C) Dapix Technologies 1991-1994.\n");
	fprintf(stderr, "\n\n\n\n\nwatchit now entering the system..............");
#	else

	fprintf(stderr, "\n\n\n\n\n\nwatchit now entering the system..............");
#	endif
#endif

	/*
		This bit takes a little while.
	*/
	if (nlist(namefile, namelist) != 0) {
		perror("nlist: can't open /unix for read - namelist");
		exit(1);
	}

	if ((memfd = open(corefile, O_RDONLY)) < 0) {
		perror("open: can't open /dev/mem for read - corefile");
		exit(1);
	}

	if ((kmemfd = open(kmemfile, O_RDONLY)) < 0) {
		perror("open: can't open /dev/kmem for read - kmemfile");
		exit(1);
	}

	if ((swapfd = open(swapfile, O_RDONLY)) < 0) {
		perror("open: can't open /dev/swap for read - swapfile");
		exit(1);
	}

	no_kmem_write = FALSE;
	if ((w_kmemfd = open(kmemfile, O_RDWR)) < 0) {
		no_kmem_write = 1;
	}

	kmem_read(&v,        namelist[NM_V].n_value,            sizeof(v));
	kmem_read(&swplo,    namelist[NM_SWPLO].n_value,        sizeof(swplo));
	kmem_read(&nswap,    namelist[NM_SWAPBLKS].n_value,     sizeof(nswap));
	kmem_read(&kmaxmem,  namelist[NM_KMAXMEM].n_value,      sizeof(kmaxmem));
	kmem_read(&kphysmem, namelist[NM_KPHYSMEM].n_value,     sizeof(kphysmem));
	kmem_read(&msginfo, namelist[NM_MSGINFO].n_value, sizeof(struct msginfo));
	kmem_read(&seminfo, namelist[NM_SEMINFO].n_value, sizeof(struct seminfo));
	kmem_read(&shminfo, namelist[NM_SHMINFO].n_value, sizeof(struct shminfo));

	/*
		Do we have sockets ? I should check as this will
		speed up the inode screen.
		NOTE: access returns 0 on OK.
	*/

	socket_flag = FALSE;
	socksys_id = -696969;

	if (!(access("/dev/socksys", F_OK))) {
		socket_flag = 1;
		if (stat("/dev/socksys", &f_info) FAILS)
			socket_flag = FALSE;
		else
			socksys_id = major(f_info.st_rdev);
	}

	if (!(current_procs = (proc_t *)malloc(sizeof(proc_t) * v.v_proc))) {
		printf("wi: Sorry I can't allocate enough memory to start.\n");
		interupt();
	}
	if (!(ptr_procs = (proc_t **)malloc(sizeof(proc_t *) * v.v_proc))) {
		printf("wi: Sorry I can't allocate enough memory to start.\n");
		interupt();
	}

	putenv("LINES=43");

	initscr();
	typeahead(-1); /*  Without this it is too slow */
	curs_set(CUR_INVIS);

	/*
		check for colour stuff
	*/
	if ((colour_flag = has_colors()) == 0)
		printf("\no colour...\n");
	else {
		(void) start_color();
		init_pair(Normal,        COLOR_BLACK,  COLOR_WHITE);
		init_pair(State_Normal,  COLOR_GREEN,  COLOR_WHITE);
		init_pair(State_Warning, COLOR_YELLOW, COLOR_WHITE);
		init_pair(State_Alarm,   COLOR_RED,    COLOR_WHITE);
		init_pair(Colour_White,  COLOR_WHITE,  COLOR_WHITE);
		init_pair(Colour_Bold,   COLOR_WHITE,  COLOR_BLACK);
		init_pair(Colour_Banner, COLOR_WHITE,  COLOR_BLUE);
		init_pair(Colour_Blue,   COLOR_BLUE,   COLOR_WHITE);
		init_pair(Colour_Cyan,   COLOR_CYAN,   COLOR_WHITE);
	}

	/*
		Create the windows.
	*/
	top_win = TOP_WIN;
	if (size_flag) {
		main_win = LONG_MAIN_WIN;
		bottom_win = LONG_BOTTOM_WIN;
	} else {
		main_win = MAIN_WIN;
		if (lines < 25)
			bottom_win = SMALL_WIN;
		else
			bottom_win = BOTTOM_WIN;
	}

	Set_Colour(top_win, Normal);
	Set_Colour(bottom_win, Normal);
	Set_Colour(main_win, Normal);

	Fill_A_Box(top_win, 0, 0);
	Fill_A_Box(bottom_win, 0, 0);
	Fill_A_Box(main_win, 0, 0);

	Draw_A_Box(top_win, DEFAULT_BOX);
	Draw_A_Box(bottom_win, DEFAULT_BOX);
	Draw_A_Box(main_win, DEFAULT_BOX);
	/* if (lines < 25) */
		Join_A_Box(main_win, DEFAULT_BOX);

	drawdisp(1);
	drawdisp(2);

	wnoutrefresh(top_win);
	wnoutrefresh(bottom_win);
	wnoutrefresh(main_win);

	doupdate();
/*
 * +--------------------------------------------------------------------+
 * |		NOTE SIGTERM TAKES THE DAM THING OUT OF RAW MODE.           |
 * |		IF THE LINE BELOW IS PUT IN IT REALY SCREWS THINGS UP.      |
 * +--------------------------------------------------------------------+
 */
	
	signal(SIGINT, interupt);	/* and setup the handler */
	signal(SIGSEGV, interupt);	/* and setup the handler */

	demo_pid = 9999;

	if (demo_ver) {
		signal(SIGALRM, end_of_demo); /* set up signal for demo ver */
		alarm(DEMO_TIME);
	}

	Set_Colour(bottom_win, Text_Warning);
	mvwaddstr(bottom_win, 1, 2, " Watchit reading device entries from /dev - Please wait.......");
	Set_Colour(bottom_win, Normal);
	wrefresh(bottom_win);

	/*	
		As my main device table is setup I should get the devices.
	*/
	for (ctr = 0; ctr <= num_of_hash_disks; ctr++)
		print_dev(1, nodelist[ctr].major_node
		    , nodelist[ctr].minor_node, nodelist[ctr].bdev_name, 0);

	Get_Start_Stats();

 	sprintf(logfile , "/tmp/mcap%s", The_Current_tty);
 	sprintf(o_logfile , "/tmp/o_mcap%s", The_Current_tty);

	if ((access(logfile, F_OK)) == 0) {
		mvwprintw(bottom_win, 1, 2, " Moving old Monitor CAPture file from %s to %s.",
		logfile, o_logfile);
		w_clrtoeol(bottom_win, 0, 0);
		wrefresh(bottom_win);
		link(logfile, o_logfile);
		unlink(logfile);
		beep();
		nap(2500);
	}

	while (! (interface()));

	clear();
	refresh();
	endwin();

	close(swapfd);
	close(w_kmemfd);
	close(kmemfd);
	close(memfd);

	if (size_flag) {
		if (! ((disptype = ioctl(0, CONS_CURRENT))FAILS)) {
			switch(disptype) {
			case (CGA):
				ioctl(0, SW_ENHC80x25, 0L);
				break;
			case (EGA):
				ioctl(0, SW_ENHC80x25, 0L);
				break;
			case (VGA):
				ioctl(0, SW_VGA80x25, 0L);
				break;
			default :
				break;
			}
		}
	}
	printf("\f\nbye bye....\n");
	exit(0);
}

/*
 +-----------------------------------------------------------------------+
 |                            END OF PROGRAM.                            |
 +-----------------------------------------------------------------------+
 */
