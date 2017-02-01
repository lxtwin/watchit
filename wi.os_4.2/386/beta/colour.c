/*
 * +--------------------------------------------------------------------+
 * | Function: colour.c                                  Date: 92/04/04 |
 * | Author: Paul Ready.                                                |
 * |                                                                    |
 * | Notes:                                                             |
 * |           All the new colour stuff will go in here.                |
 * +--------------------------------------------------------------------+
 *
 * Note:
 *		None Yet.
 *
 * Updates:   
 *          None yet.
 *
 * Bugs:
 *          None yet.
 */

#ifdef __STDC__
#	pragma comment(exestr, "%Z% %M%	Version %I% %D% - Dapix")
#else
#	ident "%Z% %M%	Version %I% %D% - Dapix"
#endif /* __STDC__ */

#include <curses.h>

enum states {
	Normal = 1, State_Normal, State_Warning, State_Alarm,
	Text_Normal, Text_Warning, Text_Alarm, Colour_Banner,
	Blink_Banner, Colour_Black, Colour_White, Colour_Bold,
	Colour_Blue, Colour_Magenta, Colour_Grey, Colour_Brown,
	Red_Banner, Red_Blink_Banner, Colour_Ltcyan, Colour_Ltblue
};

int colour[] = {
	COLOR_PAIR(0),						/*  0 */
	COLOR_PAIR(Normal),					/*  1 */
	COLOR_PAIR(State_Normal),				/*  2 */
	COLOR_PAIR(State_Warning) | A_BOLD,			/*  3 */
	COLOR_PAIR(State_Alarm),				/*  4 */
	COLOR_PAIR(State_Normal)   | A_BOLD,		/*  5 */
	COLOR_PAIR(State_Warning)  | A_BOLD,		/*  6 */
	COLOR_PAIR(State_Alarm),				/*  7 */
	COLOR_PAIR(Colour_Banner) | A_BOLD,			/*  8 */
	COLOR_PAIR(Colour_Banner) | A_BOLD | A_BLINK,	/*  9 */
	COLOR_PAIR(Normal),					/* 10 */
	COLOR_PAIR(Colour_White)  | A_BOLD,			/* 11 */
	COLOR_PAIR(Colour_Bold),				/* 12 */
	COLOR_PAIR(Colour_Blue),				/* 13 */
	COLOR_PAIR(Colour_Magenta),				/* 14 */
	COLOR_PAIR(Normal) | A_BOLD,				/* 15 */
	COLOR_PAIR(State_Warning),				/* 16 */
	COLOR_PAIR(Red_Banner) | A_BOLD,			/* 17 */
	COLOR_PAIR(Red_Banner) | A_BOLD | A_BLINK,	/* 18 */
	COLOR_PAIR(Colour_Ltcyan) | A_BOLD,			/* 19 */
	COLOR_PAIR(Colour_Ltblue) | A_BOLD			/* 20 */
};

int mono[] = {
	A_NORMAL,			/*  0 */
	A_NORMAL,			/*  1 */
	A_NORMAL,			/*  2 */
	A_BOLD,			/*  3 */
	A_REVERSE,			/*  4 */
	A_NORMAL,			/*  5 */
	A_BOLD,			/*  6 */
	A_BOLD | A_BLINK,		/*  7 */
	A_REVERSE,			/*  8 */
	A_REVERSE | A_BLINK,	/*  9 */
	A_NORMAL,			/* 10 */
	A_BOLD,			/* 11 */
	A_NORMAL,			/* 12 */
	A_NORMAL,			/* 13 */
	A_NORMAL,			/* 14 */
	A_BOLD,			/* 15 */
	A_BOLD,			/* 16 */
	A_REVERSE,			/* 17 */
	A_REVERSE | A_BLINK,	/* 18 */
	A_BOLD,			/* 19 */
	A_BOLD			/* 20 */
};

