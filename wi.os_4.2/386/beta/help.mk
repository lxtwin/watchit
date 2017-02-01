#
#	@(#) help.mk	Version 1.2 93/06/15 - PKR.
#
#	Copyright (C) Paul Ready 1992.
#	This Module contains Proprietary Information of PKR,
#	and should be treated as Confidential.
#
#	Makefile for pm/watchit
#

SHELL=/bin/sh

CMDS = wi.hlp

OBJS =	drawdisp.o drawwindow.o help.o mkwindow.o


FILES = Makefile \
	drawdisp.c drawwindow.c help.c mkwindow.c

CC = cc

# Just for sdb. and codeview
CFLAGS = -d -Zd -Zi -DM_TERMINFO -D_INKERNEL -D_OLD_OS -D_BETA
# CFLAGS = -c -coff -Ox -s -d -DM_TERMINFO -D_INKERNEL -D_OLD_OS

LIBS = -lcurses -lx -lc

INSDIR = $(ROOT)/etc
MODE   = 6755
OWNER  = root
GROUP  = bin

DEFS	=	-D_BETA
wi:	$(OBJS)
	$(CC) -d -Zd -Zi -o wi.hlp $(OBJS) $(LIBS) $(DEFS)
	
install: $(CMDS)
	chmod $(MODE)  $(CMDS)
	chgrp $(GROUP) $(CMDS)
	chown $(OWNER) $(CMDS)
	strip $(CMDS)

clean:
	rm -f *.o a.out

clobber:	clean
	rm -f wi core

drawdisp.o: drawdisp.c

drawwindow.o: drawwindow.c

help.o.o: help.c

mkwindow.o: mkwindow.c
