/*
 * +--------------------------------------------------------------------+
 * | Function: init.wi.c                                 Date: 92/04/04 |
 * | Author  : Paul Ready.                                              |
 * |                                                                    |
 * | Notes:                                                             |
 * |           The bit that sets up wi to run OK                        |
 * +--------------------------------------------------------------------+
 *
 * Note:
 *          None.
 * Updates:
 *          None.
 * Bugs:
 *          None yet.
 */

#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/utsname.h>
#include <curses.h>
#include <time.h>
#include <sys/types.h>

#ifndef M_UNIX
#	include <sys/machdep.h>
#else
#	include <sys/console.h>
#endif

#include <sys/vid.h>

#define SET_COLOUR "\033[=0E\033[=7F\033[=0G\033[=15H\033[=4I\033[=2J\033[=0K"

/*
	O/S = 3.2v4.2 - 586
*/
#define    WI_KEY_POSITION 0x55eb4           /* flag for end date for demo   */
#define SYSSERIAL_POSITION 0x55ec4           /* flag for the system id       */
#define DEMO_TIME_POSITION 0x55ed0           /* flag for end date for demo   */
#define LIVE_FLAG_POSITION 0x55edc           /* flag for demo or live        */

#define DEMO_DAYS       30                   /* how long to demo for.        */
#define DEMO_SECONDS    (((60 * 60)* 24)* DEMO_DAYS)

/* for XENIX v1.3 use 69 */
/* for XENIX v2.1 use 28 */

/* for UNIX  v1.3 use 59 */
/* for UNIX  v2.1 use 34 */
/* for UNIX  v2.2 use 34 */
/* for UNIX  v2.3 use 87 */

#define CRYPT_SALT "87"			

void install_live(void);
void install_demo(void);
void remove_it(void);
char *crypt();

int     file_fd;

WINDOW *w1, *w2, *w3;

int r_read(int fd, char *buf, int n)
{
	int     i;
	if ((i = read(fd, buf, n)) == -1) {
		perror("error on read");
		return(-1);
	} else
		return(i);
}

long    l_lseek(int fd, long  offs, int  whence)
{
	long    i;
	long    lseek();

	if ((i = lseek(fd, offs, whence)) == -1L) {
		perror("error on lseek");
		return(-1);
	} else
		return(i);
}

int main()
{

	int disptype;
	char stdoutbuf[2048];

	(void) setvbuf(stdout, stdoutbuf, _IOFBF, sizeof(stdoutbuf));

	/*
                here I should look for the ttytype, termtype and vid type
        */
	if (! ((disptype = ioctl(0, CONS_CURRENT)) == -1)) {
		switch(disptype) {
		case (CGA):
		case (EGA):
		case (VGA):
			printf("%s", SET_COLOUR );
			break;
		default:
			/* I don't know who I am........  */
			break;
		}
	}

	initscr();
	cbreak();

	w1=newwin(3, 80, 0, 0);
	w2=newwin(21, 50, 3, 0);
	w3=newwin(21, 29, 3, 51);
	box(w1,0,0);
	box(w2,0,0);
	box(w3,0,0);

	wattron(w1, A_REVERSE );
	mvwaddstr(w1,1,2," v3.0/586        Watchit - The UNIX Performance Monitor.         v4.2 / 3.0 ");
	wattroff(w1, A_REVERSE );
	mvwaddstr(w2,1,6," Installing Watchit for SCO UNIX.");

	mvwaddstr(w3,1,2,"The Options:");
	mvwaddstr(w3,3,2,"1: Quit the installation.");
	mvwaddstr(w3,5,2,"2: Install a LIVE version");
	mvwaddstr(w3,6,2,"   of watchit.");
	mvwaddstr(w3,8,2,"3: Install a DEMO version");
	mvwaddstr(w3,9,2,"   of watchit.");
	mvwaddstr(w3,11,2,"4: Remove watchit.");
	mvwaddstr(w3,19,2,"(C) Copyright Dapix, 1993.");

	mvwaddstr(w2,3,10,"1 - Quit the installation");
	mvwaddstr(w2,4,10,"2 - Install a LIVE version");
	mvwaddstr(w2,5,10,"3 - Install a DEMO version");
	mvwaddstr(w2,6,10,"4 - Remove Watchit");
	mvwaddstr(w2,8,14,"Select: ");

	wattron(w2, A_BOLD);
	wattron(w3, A_BOLD);
	mvwaddstr(w3,1,2,"The Options");

	mvwaddstr(w3,3,2,"1");
	mvwaddstr(w3,5,2,"2");
	mvwaddstr(w3,8,2,"3");
	mvwaddstr(w3,11,2,"4");

	mvwaddstr(w2,3,10,"1");
	mvwaddstr(w2,4,10,"2");
	mvwaddstr(w2,5,10,"3");
	mvwaddstr(w2,6,10,"4");
	mvwaddstr(w2,8,14,"Select");
	wattroff(w2, A_BOLD);
	wattroff(w3, A_BOLD);

	wnoutrefresh(w1);
	wnoutrefresh(w3);
	wnoutrefresh(w2);
	doupdate();

	while (TRUE) {
		mvwaddstr(w2,8,20,":       ");
		wattron(w2, A_BOLD);
		mvwaddstr(w2,8,14,"Select");
		wattroff(w2, A_BOLD);
		mvwaddstr(w2,8,21," ");
		wrefresh(w2);
		switch(wgetch(w2))
		{
		case '1':
		case 'Q':
		case 'q':
			clear();
			refresh();
			endwin();
			printf("bye bye....\n");
			exit(0);
			break;

		case '2':
		case 'L':
		case 'l':
			mvwaddstr(w2,19,2,"Installing watchit - Please wait");
			wrefresh(w2);
			install_live();
			clear();
			refresh();
			endwin();
			printf("bye bye....\n");
			exit(0);
			break;

		case '3':
		case 'D':
		case 'd':
			mvwaddstr(w2,19,2,"Installing watchit - Please wait");
			wrefresh(w2);
			install_demo();
			clear();
			refresh();
			endwin();
			printf("bye bye....\n");
			exit(0);
			break;

		case '4':
		case 'R':
		case 'r':
			remove_it();
			clear();
			refresh();
			endwin();
			printf("bye bye....\n");
			exit(0);
			break;
		}
	}
	/* I should never get here */
	exit(0);
}

void install_live(void)
{

	struct scoutsname name;

	int     file_fd;
	long    test_time;
	int     counter;
	char    utstest[12];
	char	  wi_key[12];

	char    buf[12];
	char    *buf_ptr;
	char *crypted;
	char password[16];
	char *pass_ptr;
	char *new_crypted;
	char *stat;
	int ctr, ctr2;

	__scoinfo(&name, sizeof(struct scoutsname));

	mvwaddstr(w2,8,14,"Select:       ");

	for ( counter = 3 ; counter != 0 ; counter-- ) {
		mvwaddstr(w2,19,2,"                                            ");
		mvwaddstr(w2,8,14,"Serial #: [__________]         ");
		mvwaddstr(w2,8,14,"Serial #: [");
		wrefresh(w2);
		password[0] = '\0';
		strncpy(password, '\0', 12);
		wgetstr(w2, password);
		pass_ptr = password;
		sprintf(wi_key,"%.12s", password);
		mvwaddstr(w2,8,14,"Activation key: [___________]");
		mvwaddstr(w2,8,14,"Activation key: [");
		wrefresh(w2);
		buf[0] = '\0';
		wgetstr(w2, buf);
		buf_ptr = buf;

		/*
			I perfer to work with pointers so I have converted.
		*/
		sprintf(crypted, "%s%s",CRYPT_SALT, buf_ptr);
		new_crypted = crypt(password,CRYPT_SALT);
		if (strcmp(crypted, new_crypted)) {
			mvwaddstr(w2,19,2,"Invalid Password - Press Enter to continue:");
			wrefresh(w2);
			wgetch(w2);
		} else
			break;
	}
	if (!(counter))
		return;

	for (ctr = ctr2 = 0 ; name.sysserial[ctr] != '\0' ; ctr++)
		/* if (isdigit(name.sysserial[ctr])) */
		if (name.sysserial[ctr] >= '0' && name.sysserial[ctr] <= '9')
			utstest[ctr2++] = name.sysserial[ctr];
	utstest[ctr2] = '\0';
	counter = atoi(utstest);
			
	sprintf(buf,"%0.10d", counter);

	if ((file_fd = open("/etc/wi", O_RDWR)) < 0) {
		mvwaddstr(w2,19,2,"error installing - Press Enter to continue:");
		wrefresh(w2);
		wgetch(w2);
		return;
	}

	l_lseek(file_fd, LIVE_FLAG_POSITION, 0);
	write(file_fd, "LIVE_VERSION", 12);

	l_lseek(file_fd, SYSSERIAL_POSITION, 0);
	write(file_fd, buf, 10);

	sprintf(buf,"%0.8ld", (time((long *) 0) + DEMO_SECONDS));

	l_lseek(file_fd, DEMO_TIME_POSITION, 0);
	write(file_fd, buf, 9);

	l_lseek(file_fd, WI_KEY_POSITION, 0);
	write(file_fd, wi_key, 12);

	close(file_fd);
	mvwaddstr(w2,19,2,"watchit installed - Press Enter to continue:");
	wrefresh(w2);
	wgetch(w2);
	return;
}

void install_demo(void)
{
	int     file_fd;
	long    test_time;

	char    buf[9];

	if ((file_fd = open("/etc/wi", O_RDWR)) < 0) {
		mvwaddstr(w2,19,2,"error installing - Press Enter to continue:");
		wrefresh(w2);
		wgetch(w2);
		return;
	}

	sprintf(buf,"%0.8ld", (time((long *) 0) + DEMO_SECONDS));

	l_lseek(file_fd, DEMO_TIME_POSITION, 0);
	write(file_fd, buf, 9);

	close(file_fd);

	test_time = (time((long *) 0) + DEMO_SECONDS);
	mvwprintw(w2,17,2,"DEMO valid until %.24s", ctime(&test_time));
	mvwaddstr(w2,19,2,"watchit installed - Press Enter to continue:");
	wrefresh(w2);
	wgetch(w2);
	return;
}

void remove_it(void)
{
	system("rm /etc/wi /etc/watchit/* > /dev/null 2>&1");
	system("rmdir -f /etc/watchit > /dev/null 2>&1");
	mvwaddstr(w2,19,3,"watchit removed - Press Enter to continue:");
	wrefresh(w2);
	wgetch(w2);
	return;
}
