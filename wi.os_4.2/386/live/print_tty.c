/*
 * +--------------------------------------------------------------------+
 * | Function: get_dev(int major, int minor,char *dev_name)    92/04/18 |
 * | Author: Paul Ready.                                              |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Prints the device name of pid passed via utmp.              |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *		None.
 *    Bugs:
 *		None.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */


#include  <sys/types.h>			 	/* File type hedder fl. */
#include      <fcntl.h>			 	/* utmp file header     */
#include       <utmp.h>			 	/* utmp file header     */

#define	MAX_UTMP 64

int num_of_utmps = 0;
struct utmp utmps[MAX_UTMP];

char *print_tty(int pid, int type_flag);

char *print_tty(int pid, int type_flag)
{

int fd;
register struct utmp *ptr_utmp = utmps;

	num_of_utmps = 0;
	if (pid == 0)
		if (type_flag)
			return("tty??");
		else
			return("??");

	if ((fd = open("/etc/utmp", O_RDONLY, 755)) < 0)
		return("??");

	while(read(fd, (char *)(ptr_utmp++), sizeof(struct utmp)) > 0) {

		if (pid == ptr_utmp->ut_pid) {
			(void) close(fd);
			if (strcmp(ptr_utmp->ut_line, ""))
				if (type_flag)
					return(ptr_utmp->ut_line);
				else
					return(&ptr_utmp->ut_line[3]);
			else
				if (type_flag)
					return("tty??");
				else
					return("??");
		}

		if (++num_of_utmps == MAX_UTMP) {
			(void) close(fd);
			if (type_flag)
				return("tty??");
			else
				return("??");
		}
	}
	(void) close(fd);
	if (type_flag)
		return("tty??");
	else
		return("??");
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */

