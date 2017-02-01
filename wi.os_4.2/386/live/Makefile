#
#ifdef __STDC__
#	pragma comment(exestr, "@(#) Makefile		Version 1.2 93/06/14 - Dapix ")
#else
#	ident "@(#) Makefile		Version 1.2 93/06/14 - Dapix "
#endif /* __STDC__ */
#
#	Copyright (C) Paul Ready 1992, 1993.
#	This Module contains Proprietary Information of PKR,
#	and should be treated as Confidential.
#
#	Makefile for watchit
#

SHELL=/bin/sh

CMDS = wi

OBJS =	admmenu.o buf_cache.o bufs.o cpu.o disk.o drawdisp.o \
		drawwindow.o files.o get_tune.o inodes.o interface.o \
		mkwindow.o mounts.o mon_proc.o odds_and_sods.o print_tty.o \
		print_dev.o procs.o regions.o stats.o stream.o sysinfo.o ttys.o \
		user.o vars.o wi.o

FILES = Makefile \
	admmenu.c buf_cache.c bufs.c cpu.c disk.c drawdisp.c \
	drawwindow.c files.c get_tune.c inodes.c interface.c \
	mkwindow.c mon_proc.c mounts.c odds_and_sods.c print_dev.c \
	print_tty.c procs.c regions.c stats.c stream.c sysinfo.c ttys.c \
	user.c vars.c wi.c

CC = cc

# Just for sdb. and codeview

# OS VER 3.2v4.1
# CFLAGS = -c -d -g -Zd -Zi -DM_TERMINFO -D_INKERNEL -D_OLD_OS -D_PROTECTED
# CFLAGS = -c -d -g -Zd -Zi -DM_TERMINFO -D_INKERNEL -D_OLD_OS -D_BETA

# OS VER 3.2v4.2 	-	(LATEST)
  CFLAGS = -c -d -g -Zd -Zi -DM_TERMINFO -D_INKERNEL -D_PROTECTED -D_WATCHIT
# CFLAGS = -c -d -g -Zd -Zi -DM_TERMINFO -D_INKERNEL -D_BETA -D_WATCHIT

LIBS = -lcurses -lx -lmalloc -lc_s

INSDIR = $(ROOT)/etc
MODE   = 6755
OWNER  = root
GROUP  = bin

wi:	$(OBJS)
	$(CC) -d -Zd -Zi -o wi $(OBJS) $(LIBS) $(DEFS)
	strip $(CMDS)

install: $(CMDS)
	chmod $(MODE)  $(CMDS)
	chgrp $(GROUP) $(CMDS)
	chown $(OWNER) $(CMDS)
	strip $(CMDS)

clean:
	rm -f *.o a.out

clobber:	clean
	rm -f wi core

admmenu.o: wi.h admmenu.c

buf_cache.o: wi.h buf_cache.c

bufs.o: wi.h bufs.c

cpu.o: wi.h cpu.c

disk.o:	wi.h	disk.c

drawdisp.o: wi.h drawdisp.c

drawwindow.o: wi.h drawwindow.c

files.o: wi.h files.c

get_tune.o: wi.h get_tune.c

inodes.o: wi.h inodes.c

interface.o: wi.h interface.c

mkwindow.o: wi.h mkwindow.c

mounts.o: wi.h mounts.c

mon_proc.o: wi.h mon_proc.c

odds_and_sods.o: wi.h odds_and_sods.c

print_dev.o: wi.h print_dev.c

print_tty.o: print_tty.c

procs.o: wi.h procs.c

regions.o: wi.h regions.c

stats.o: wi.h stats.c

stream.o: wi.h stream.c

sysinfo.o: wi.h sysinfo.c

ttys.o: wi.h ttys.c

user.o: wi.h user.c

vars.o: wi.h vars.c

wi.o: wi.h wi.c
