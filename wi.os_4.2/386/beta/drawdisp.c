/*
 * +--------------------------------------------------------------------+
 * | Function: void drawdisp(int win_no)                       91/01/30 |
 * | Author: Paul.                                                    |
 * |                                                                    |
 * | Notes:                                                             |
 * |           Displays text for a given window.                        |
 * +--------------------------------------------------------------------+
 *
 * Updates:   
 *(1) v2.3	Added "Display" for the CPU screen.
 *						Mon Aug 09 15:08:51 EDT 1993 - PKR.
 *
 *          None.
 *    Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%		Version %I% %D% - Dapix ")
#else
#	ident "%Z% %M%		Version %I% %D% - Dapix "
#endif /* __STDC__ */

#include "wi.h"
#include <time.h>

void drawdisp(int win_no);

extern int Doc_Flag;
extern int lines;

void drawdisp(int win_no)
{

struct tm *tim;
long clock;

#ifdef DEBUG_ON
	fprintf(stderr, "\n Started function %I%(). \n");
#endif

	time(&clock);
	tim = localtime(&clock);

	if (win_no >= 2) {
		Draw_A_Box(main_win, DEFAULT_BOX);
		if (lines < 25)
			Join_A_Box(main_win, DEFAULT_BOX);
	}

	switch (win_no) {
	case 0:
		Set_Colour(top_win, Colour_Banner);
#ifdef _KEYS_HELP
		mvwprintw(top_win, 1, 2, " %02d:%02d            Watchit - Online Help Key System.               %02d/%02d/%02d "
		    , tim->tm_hour, tim->tm_min, tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
#else
		mvwprintw(top_win, 1, 2, " %02d:%02d              Watchit - Online Help System.                 %02d/%02d/%02d "
		    , tim->tm_hour, tim->tm_min, tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
#endif
		Set_Colour(top_win, Normal);
		break;

	case 1:
		Set_Colour(top_win, Colour_Banner);

#ifdef _ALPHA
		if (Doc_Flag)
		mvwprintw(top_win, 1, 2, " %02d:%02d         Watchit - The UNIX Performance Monitor.    %s%c   %02d/%02d/%02d " ,
				tim->tm_hour, tim->tm_min, WI_VERSION, WI_REV,
				tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
		else
		mvwprintw(top_win, 1, 2, " %02d:%02d        Watchit - ALPHA VERSION (NOT FOR RESALE)  %s%c     %02d/%02d/%02d ",
				tim->tm_hour, tim->tm_min, WI_VERSION, WI_REV,
				tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
#else
#	ifdef _BETA
		if (Doc_Flag)
		mvwprintw(top_win, 1, 2, " %02d:%02d         Watchit - The UNIX Performance Monitor.    %s%c   %02d/%02d/%02d " ,
				tim->tm_hour, tim->tm_min, WI_VERSION, WI_REV,
				tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
		else
		mvwprintw(top_win, 1, 2, " %02d:%02d         Watchit - BETA VERSION (NOT FOR RESALE)  %s%c     %02d/%02d/%02d ",
				tim->tm_hour, tim->tm_min, WI_VERSION, WI_REV,
				tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
#	else

		if (demo_ver)
			mvwprintw(top_win, 1, 2, " %02d:%02d   DEMO  Watchit - The UNIX Performance Monitor.    DEMO    %02d/%02d/%02d "
		    		, tim->tm_hour, tim->tm_min, tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
		    else
			mvwprintw(top_win, 1, 2, " %02d:%02d         Watchit - The UNIX Performance Monitor.     %s%c  %02d/%02d/%02d " ,
				tim->tm_hour, tim->tm_min, WI_VERSION, WI_REV,
				tim->tm_mday, (tim->tm_mon + 1), tim->tm_year);
#	endif
#endif
		Set_Colour(top_win, Normal);
		break;

	case 2:
		mvwaddstr(main_win, 0, 2, "Buf");
		mvwaddstr(main_win, 0, 7, "Cpu");
		mvwaddstr(main_win, 0, 12, "File");
		mvwaddstr(main_win, 0, 18, "Ino");
		mvwaddstr(main_win, 0, 23, "Mnt");
		mvwaddstr(main_win, 0, 28, "I/O");
		mvwaddstr(main_win, 0, 33, "Proc");
		mvwaddstr(main_win, 0, 39, "Reg");
		mvwaddstr(main_win, 0, 44, "Stat");
		mvwaddstr(main_win, 0, 50, "User");
		mvwaddstr(main_win, 0, 56, "Var");
		mvwaddstr(main_win, 0, 61, "sYs");

		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "B");
		mvwaddstr(main_win, 0, 7, "C");
		mvwaddstr(main_win, 0, 12, "F");
		mvwaddstr(main_win, 0, 18, "I");
		mvwaddstr(main_win, 0, 23, "M");
		mvwaddstr(main_win, 0, 30, "O");
		mvwaddstr(main_win, 0, 33, "P");
		mvwaddstr(main_win, 0, 39, "R");
		mvwaddstr(main_win, 0, 44, "S");
		mvwaddstr(main_win, 0, 50, "U");
		mvwaddstr(main_win, 0, 56, "V");
		mvwaddstr(main_win, 0, 62, "Y");
		mvwaddstr(main_win, 0, 72, "+");
		mvwaddstr(main_win, 0, 75, "-");
		Set_Colour(main_win, Normal);
		break;

	case 3:
		mvwaddstr(main_win, 0, 2, "Colours");
		mvwaddstr(main_win, 0, 11, "Defaults");
		mvwaddstr(main_win, 0, 21, "Screen_Size");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "C");
		mvwaddstr(main_win, 0, 11, "D");
		mvwaddstr(main_win, 0, 21, "S");
		mvwaddstr(main_win, 0, 72, "+");
		mvwaddstr(main_win, 0, 75, "-");
		Set_Colour(main_win, Normal);
		break;

	case 4:
		mvwaddstr(main_win, 0, 29, "Watchit");
		mvwaddstr(main_win, 0, 38, "Cache");
		mvwaddstr(main_win, 0, 45, "Display");
		mvwaddstr(main_win, 0, 54, "Pg_up");
		mvwaddstr(main_win, 0, 61, "Pg_down");
		mvwaddstr(main_win, 0, 70, "Search");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Buffer Table");
		mvwaddstr(main_win, 0, 29, "W");
		mvwaddstr(main_win, 0, 38, "C");
		mvwaddstr(main_win, 0, 45, "D");
		mvwaddstr(main_win, 0, 70, "S");
		Set_Colour(main_win, Normal);
		break;

	case 5:
		if (!(size_flag))
			mvwaddstr(main_win, 0, 45, "Display");
		mvwaddstr(main_win, 0, 54, "Reset");
		mvwaddstr(main_win, 0, 61, "Update");
		mvwaddstr(main_win, 0, 69, "Watchit");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "CPU");
		if (!(size_flag))
			mvwaddstr(main_win, 0, 45, "D");
		mvwaddstr(main_win, 0, 54, "R");
		mvwaddstr(main_win, 0, 61, "U");
		mvwaddstr(main_win, 0, 69, "W");
		Set_Colour(main_win, Normal);
		break;

	case 6:
		mvwaddstr(main_win, 0, 37, "Watchit");
		mvwaddstr(main_win, 0, 46, "Update");
		mvwaddstr(main_win, 0, 54, "Pg_up");
		mvwaddstr(main_win, 0, 61, "Pg_down");
		mvwaddstr(main_win, 0, 70, "Search");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "File Table");
		mvwaddstr(main_win, 0, 37, "W");
		mvwaddstr(main_win, 0, 46, "U");
		mvwaddstr(main_win, 0, 70, "S");
		Set_Colour(main_win, Normal);
		break;

	case 7:
		mvwaddstr(main_win, 0, 28, "Watchit");
		mvwaddstr(main_win, 0, 37, "Update");
		mvwaddstr(main_win, 0, 45, "Display");
		mvwaddstr(main_win, 0, 54, "Pg_up");
		mvwaddstr(main_win, 0, 61, "Pg_down");
		mvwaddstr(main_win, 0, 70, "Search");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Inode Table");
		mvwaddstr(main_win, 0, 28, "W");
		mvwaddstr(main_win, 0, 37, "U");
		mvwaddstr(main_win, 0, 45, "D");
		mvwaddstr(main_win, 0, 70, "S");
		Set_Colour(main_win, Normal);
		break;

	case 8:
		mvwaddstr(main_win, 0, 47, "Type");
		mvwaddstr(main_win, 0, 53, "Device");
		mvwaddstr(main_win, 0, 61, "Update");
		mvwaddstr(main_win, 0, 69, "Watchit");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Mount Table");
		mvwaddstr(main_win, 0, 47, "T");
		mvwaddstr(main_win, 0, 53, "D");
		mvwaddstr(main_win, 0, 61, "U");
		mvwaddstr(main_win, 0, 69, "W");
		Set_Colour(main_win, Normal);
		break;

	case 9:
		mvwaddstr(main_win, 0, 29, "Watchit");
		mvwaddstr(main_win, 0, 38, "Update");
		mvwaddstr(main_win, 0, 46, "Detail");
		mvwaddstr(main_win, 0, 54, "Pg_up");
		mvwaddstr(main_win, 0, 61, "Pg_down");
		mvwaddstr(main_win, 0, 70, "Search");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Process Table");
		mvwaddstr(main_win, 0, 29, "W");
		mvwaddstr(main_win, 0, 38, "U");
		mvwaddstr(main_win, 0, 46, "D");
		mvwaddstr(main_win, 0, 70, "S");
		Set_Colour(main_win, Normal);
		break;

	case 10:
		mvwaddstr(main_win, 0, 37, "Watchit");
		mvwaddstr(main_win, 0, 46, "Update");
		mvwaddstr(main_win, 0, 54, "Pg_up");
		mvwaddstr(main_win, 0, 61, "Pg_down");
		mvwaddstr(main_win, 0, 70, "Search");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Region Table");
		mvwaddstr(main_win, 0, 37, "W");
		mvwaddstr(main_win, 0, 46, "U");
		mvwaddstr(main_win, 0, 70, "S");
		Set_Colour(main_win, Normal);
		break;

	case 11:
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Statistics");
		Set_Colour(main_win, Normal);
		break;

	case 12:
		mvwaddstr(main_win, 0, 37, "Watchit");
		mvwaddstr(main_win, 0, 46, "Update");
		mvwaddstr(main_win, 0, 54, "Pg_up");
		mvwaddstr(main_win, 0, 61, "Pg_down");
		mvwaddstr(main_win, 0, 70, "Search");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "User Area");
		mvwaddstr(main_win, 0, 37, "W");
		mvwaddstr(main_win, 0, 46, "U");
		mvwaddstr(main_win, 0, 70, "S");
		Set_Colour(main_win, Normal);
		break;

	case 13:
		mvwaddstr(main_win, 0, 55, "Type");
		mvwaddstr(main_win, 0, 61, "Update");
		mvwaddstr(main_win, 0, 69, "Watchit");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Variables");
		mvwaddstr(main_win, 0, 47, "1");
		mvwaddstr(main_win, 0, 49, "->");
		mvwaddstr(main_win, 0, 52, "7");
		mvwaddstr(main_win, 0, 55, "T");
		mvwaddstr(main_win, 0, 61, "U");
		mvwaddstr(main_win, 0, 69, "W");
		Set_Colour(main_win, Normal);
		break;

	case 14:
		mvwaddstr(main_win, 0, 55, "Type");
		mvwaddstr(main_win, 0, 61, "Update");
		mvwaddstr(main_win, 0, 69, "Watchit");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "System Info");
		mvwaddstr(main_win, 0, 55, "T");
		mvwaddstr(main_win, 0, 61, "U");
		mvwaddstr(main_win, 0, 69, "W");
		Set_Colour(main_win, Normal);
		break;

	case 15:
		mvwaddstr(main_win, 0, 49, "Filter");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Processes");
		mvwaddstr(main_win, 0, 51, "l");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 16:
		mvwaddstr(main_win, 0, 49, "Lookup");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "User Area");
		mvwaddstr(main_win, 0, 49, "L");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 17:
		if ((access("/etc/watchit/watch.port", F_OK)) == 0)
			mvwaddstr(main_win, 0, 26, "Port");

		if ((access("/etc/watchit/watch.nfs", F_OK)) == 0)
			mvwaddstr(main_win, 0, 32, "NFS_Stats");

		if ((access("/etc/watchit/watch.one", F_OK)) == 0)
			mvwaddstr(main_win, 0, 43, "Oneac_UPS");

		mvwaddstr(main_win, 0, 54, "Disk");
		mvwaddstr(main_win, 0, 60, "Stream");
		mvwaddstr(main_win, 0, 68, "Terminal");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "I/O Statistics");
		if ((access("/etc/watchit/watch.port", F_OK)) == 0)
			mvwaddstr(main_win, 0, 26, "P");

		if ((access("/etc/watchit/watch.nfs", F_OK)) == 0)
			mvwaddstr(main_win, 0, 32, "N");

		if ((access("/etc/watchit/watch.one", F_OK)) == 0)
			mvwaddstr(main_win, 0, 43, "O");
/*
		if ((access(ONEAC_UPS, F_OK)) == 0)
			mvwaddstr(main_win, 0, 43, "O");
*/
		mvwaddstr(main_win, 0, 54, "D");
		mvwaddstr(main_win, 0, 60, "S");
		mvwaddstr(main_win, 0, 68, "T");
		Set_Colour(main_win, Normal);
		break;

	case 18:
		mvwaddstr(main_win, 0, 37, "Detail");
		mvwaddstr(main_win, 0, 45, "Update");
		mvwaddstr(main_win, 0, 53, "Pg_up");
		mvwaddstr(main_win, 0, 60, "Pg_down");
		mvwaddstr(main_win, 0, 69, "Watchit");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Terminal I/O");
		mvwaddstr(main_win, 0, 37, "D");
		mvwaddstr(main_win, 0, 45, "U");
		mvwaddstr(main_win, 0, 69, "W");
		Set_Colour(main_win, Normal);
		break;

	case 19:
		mvwaddstr(main_win, 0, 49, "Detail");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Terminal I/O");
		mvwaddstr(main_win, 0, 49, "D");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 20:
		mvwaddstr(main_win, 0, 54, "Type");
		mvwaddstr(main_win, 0, 60, "Update");
		mvwaddstr(main_win, 0, 68, "Watchit");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Disk I/O");
		mvwaddstr(main_win, 0, 54, "T");
		mvwaddstr(main_win, 0, 60, "U");
		mvwaddstr(main_win, 0, 68, "W");
		Set_Colour(main_win, Normal);
		break;

	case 21:
		mvwaddstr(main_win, 0, 51, "Type");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Disk I/O");
		mvwaddstr(main_win, 0, 51, "T");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 22:
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Stream Table");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 23:
		mvwaddstr(main_win, 0, 60, "Update");
		mvwaddstr(main_win, 0, 68, "Watchit");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Stream Table");
		mvwaddstr(main_win, 0, 60, "U");
		mvwaddstr(main_win, 0, 68, "W");
		Set_Colour(main_win, Normal);
		break;

	case 24:
		mvwaddstr(main_win, 0, 49, "Lookup");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "File Table");
		mvwaddstr(main_win, 0, 49, "L");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 25:
		mvwaddstr(main_win, 0, 41, "Device");
		mvwaddstr(main_win, 0, 49, "Lookup");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Inode Table");
		mvwaddstr(main_win, 0, 41, "D");
		mvwaddstr(main_win, 0, 49, "L");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 26:
		mvwaddstr(main_win, 0, 49, "Lookup");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Region Table");
		mvwaddstr(main_win, 0, 49, "L");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 27:
		mvwaddstr(main_win, 0, 49, "Lookup");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Process Table");
		mvwaddstr(main_win, 0, 49, "L");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 28:
		mvwaddstr(main_win, 0, 41, "Device");
		mvwaddstr(main_win, 0, 49, "Lookup");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Buffer Table");
		mvwaddstr(main_win, 0, 41, "D");
		mvwaddstr(main_win, 0, 49, "L");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 29:
		if (!(size_flag))
			mvwaddstr(main_win, 0, 41, "Display");
		mvwaddstr(main_win, 0, 50, "Reset");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "CPU");
		if (!(size_flag))
			mvwaddstr(main_win, 0, 41, "D");
		mvwaddstr(main_win, 0, 50, "R");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 30:
		mvwaddstr(main_win, 0, 43, "Type");
		mvwaddstr(main_win, 0, 49, "Device");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Mount Table");
		mvwaddstr(main_win, 0, 43, "T");
		mvwaddstr(main_win, 0, 49, "D");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 31:
		mvwaddstr(main_win, 0, 51, "Type");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "Variables");
		mvwaddstr(main_win, 0, 51, "T");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	case 32:
		mvwaddstr(main_win, 0, 51, "Type");
		mvwaddstr(main_win, 0, 57, "Time");
		mvwaddstr(main_win, 0, 63, "Slow");
		mvwaddstr(main_win, 0, 68, "Med");
		mvwaddstr(main_win, 0, 72, "Fast");
		Set_Colour(main_win, Colour_Banner);
		mvwaddstr(main_win, 0, 2, "System Info");
		mvwaddstr(main_win, 0, 51, "T");
		mvwaddstr(main_win, 0, 58, "i");
		mvwaddstr(main_win, 0, 63, "S");
		mvwaddstr(main_win, 0, 68, "M");
		mvwaddstr(main_win, 0, 72, "F");
		Set_Colour(main_win, Normal);
		break;

	default :
		break;
	}
	return;
}

/*
 * +--------------------------------------------------------------------+
 * |                      END OF THE PROGRAM                            |
 * +--------------------------------------------------------------------+
 */
