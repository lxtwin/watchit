/*
 * +--------------------------------------------------------------------+
 * | Function: print_dev(int major, int minor,char *dev_name)  92/04/12 |
 * | Author: Paul Ready.                                              |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Prints the device name of major and minor passed.           |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          I will have to put in it a selection for block or char
 *		devices. I think that I will use char as default.
 *    Bugs:
 *          The user.dev what ever returns 0 on /dev/tty01 and
 *		unknown things. So at the moment I can't disquingish
 *		between the two of them may be later.
 *		OK It was to slow for a listing but I have fixed it 
 *		by using a function defined in pm.c called
 *		check_for_dev(int major, int minor,char *dev_name)
 *		Yes, Yes I know the name is close but what the hell.
 *
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */


#include  <sys/types.h>			 	/* File type hedder fl. */
#include   <sys/stat.h>			 	/* File stats info.     */
#include   <sys/ndir.h>				/* Directory infor i/o.	*/

#define DIR_NAME	   "/dev"
#define highbyte(w)  (((w) >> 8) & 0377)	/* minor node number.	*/
#define lowbyte(w)   ((w) & 0377)		/* major node number.	*/

#define MATCHES == 0
#define NOT_MATCH == -1

DIR *dir_ptr;

int print_dev(int type, int major, int minor, char *dev_name, int type_flag);

int print_dev(int type, int major, int minor, char *dev_name, int type_flag)
{

struct direct *dirinfo;
struct stat input_file;

char dirpath[32];
long majornode, minornode;

	if ((dir_ptr = (opendir(DIR_NAME))) == NULL) {
		if (type_flag)
			strcpy(dev_name, "tty??");
		else
			strcpy(dev_name, "??");
		return(1);
	}

	while ((dirinfo = readdir(dir_ptr)) != NULL) {
		if (strcmp(dirinfo->d_name, ".") MATCHES
		    ||   strcmp(dirinfo->d_name, "..") MATCHES)
			continue;

		dirpath[0] = '\0';
		sprintf(dirpath, "/dev/%s", dirinfo->d_name);

		if (stat(dirpath, &input_file) NOT_MATCH)
			continue;

		majornode = minornode = input_file.st_rdev;

		/* If block, char or nothing */
		if (((input_file.st_mode & S_IFMT) == S_IFBLK) && type == 1) {
			if (major == highbyte(majornode) && minor == lowbyte(minornode)) {
				if (type_flag) {
					strcpy(dev_name, dirinfo->d_name);
				} else
					strcpy(dev_name, dirinfo->d_name);
				closedir(dir_ptr);
				return(0);
			}
		}
		else if (((input_file.st_mode & S_IFMT) == S_IFCHR) && type MATCHES) {
			if (major == highbyte(majornode) && minor == lowbyte(minornode)) {
				if (type_flag) {
					strcpy(dev_name, dirinfo->d_name);
				} else
					strcpy(dev_name, dirinfo->d_name);
				closedir(dir_ptr);
				return(0);
			}
		}
		else if ((((input_file.st_mode & S_IFMT) == S_IFCHR) || ((input_file.st_mode & S_IFMT) == S_IFBLK))&& type
		    >= 2) {
			if (major == highbyte(majornode) && minor == lowbyte(minornode)) {
				if (type_flag) {
					strcpy(dev_name, dirinfo->d_name);
				} else
					strcpy(dev_name, dirinfo->d_name);
				closedir(dir_ptr);
				return(0);
			}
		}
	}
	strcpy(dev_name, "unknown");
	closedir(dir_ptr);
	return(1);
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
