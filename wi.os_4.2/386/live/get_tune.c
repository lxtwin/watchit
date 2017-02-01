/*
 * +--------------------------------------------------------------------+
 * | Function: get_tune(char value, int *min, int *max, curt)  92/04/12 |
 * | Author: Paul Ready.                                              |
 * |                                                                    |
 * | Notes:                                                             |
 * |        Gets the values from mtune and stune.                       |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *          None.
 *
 *    Bugs:
 *          None.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */

#include  "wi.h"

#define MTUNE	"/etc/conf/cf.d/mtune"
#define STUNE	"/etc/conf/cf.d/stune"

#define F_SIZE  128 			         /* max. size for a file. */
#define BUFSIZE 512			         /* Infile buffer size.   */

char kern_val[F_SIZE],
     kern_cur[F_SIZE],
     kern_min[F_SIZE],
     kern_max[F_SIZE],
 kern_default[F_SIZE];

int get_tune(char *conf_value, int *conf_current, int *conf_default, int *conf_min, int *conf_max);
void find_mtune(char *buf);
void find_stune(char *buf);

int get_tune(char *conf_value, int *conf_current, int *conf_default, int *conf_min, int *conf_max)
{

FILE	*infile_1;
FILE	*infile_2;

char	buf[BUFSIZE];

*conf_current = 0;
*conf_default = 0;
*conf_min = 0;
*conf_max = 0;

        if ((infile_1 = fopen(MTUNE, "r")) == NULL)
                return(1);

        if ((infile_2 = fopen(STUNE, "r")) == NULL)
                return(1);

	while ((fgets(buf, sizeof buf, infile_1)) != NULL) {
		if (buf[0] != 42) {
			find_mtune(buf);
			if ((strcmp(kern_val, conf_value))MATCHES) {
				*conf_min     = atoi(kern_min);
				*conf_max     = atoi(kern_max);
				*conf_default = atoi(kern_default);
				*conf_current = atoi(kern_default);
				while ((fgets(buf, sizeof buf, infile_2)) != NULL) {
					find_stune(buf);
					if ((strcmp(kern_val, conf_value))MATCHES) {
						*conf_current = atoi(kern_cur);
					}
				}
				fclose(infile_1);
				fclose(infile_2);
        			return(0);
			}
		}
	}

	/*
		Can't find value in mtune file.
	*/
	fclose(infile_1);
	fclose(infile_2);
	return(1);

	/* Now tell the user that we can't find the name he entered. */
}


/*
 * +--------------------------------------------------------------------+
 * | Function: find_mtune()                              Date: %D% |
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

void find_mtune(char *buf)
{

char *field_ptr;

	/*
		get the first string.
	*/
	for (field_ptr = kern_val; *buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++= *buf++);
	*field_ptr = '\0';
	for (; *buf == '\t' || *buf == ' '; ++buf);

	/*
		Get the 2nd string.
	*/
	for (field_ptr = kern_default; *buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++= *buf++);
	*field_ptr = '\0';
	for (; *buf == '\t' || *buf == ' '; ++buf);

	/*
		Get the 3rd string.
	*/
	for (field_ptr = kern_min; *buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++= *buf++);
	*field_ptr = '\0';
	for (; *buf == '\t' || *buf == ' '; ++buf);

	/*
		Now get the 4th field
	*/
	for (field_ptr = kern_max; *buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++= *buf++);
	*field_ptr = '\0';

	return;
}

/*
 * +--------------------------------------------------------------------+
 * | Function: find_stune()                              Date: %D% |
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

void find_stune(char *buf)
{

char *field_ptr;

	/*
		get the first string.
	*/
	for (field_ptr = kern_val; *buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++= *buf++);
	*field_ptr = '\0';
	for (; *buf == '\t' || *buf == ' '; ++buf);

	/*
		Get the 2nd string.
	*/
	for (field_ptr = kern_cur; *buf != ' ' && *buf != '\t' && *buf != '\n'
		; *field_ptr++= *buf++);
	*field_ptr = '\0';

	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
