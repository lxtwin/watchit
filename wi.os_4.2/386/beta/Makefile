#
#	Copyright(C) Paul Ready 1992 - 1994.
#	This Module contains Proprietary Information of Dapix
#	Technologies and should be treated as Confidential.
#
#	Makefile for watchit
#

#
#ifdef __STDC__
#	pragma comment(exestr, "@(#) Makefile		Version 1.2 93/06/15 - Dapix")
#else
#	ident "@(#) Makefile		Version 1.2 93/06/15 - Dapix"
#endif /* __STDC__ */
#

VERSION	= 1.2

FILES	= Makefile \
		admmenu.c buf_cache.c bufs.c colour.c cpu.c disk.c \
		drawdisp.c files.c get_tune.c inodes.c interface.c \
		mon_proc.c mounts.c odds_and_sods.c print_dev.c \
		print_tty.c procs.c regions.c stats.c stream.c \
		sysinfo.c ttys.c user.c vars.c wi.c

OBJECTS	= 	admmenu.o buf_cache.o bufs.o cpu.o colour.o disk.o \
		drawdisp.o files.o get_tune.o inodes.o interface.o \
		mounts.o mon_proc.o odds_and_sods.o print_tty.o \
		print_dev.o procs.o regions.o stats.o stream.o \
		sysinfo.o ttys.o user.o vars.o wi.o

HEADERS	= wi.h

SHARFILES	= wi
COMPRESS	= gzip
COMPFLAGS	= -v -9

# 486/586 Flags and switches
#	CC	= icc
#	CFLAGS	= -g
#	CCFLAGS	= -c
#	LDFLAGS	=
#	LIBS	= -lcurses -lx -lmalloc -lc_s
#	DEFS	= -DM_TERMINFO -D_INKERNEL -D_ALPHA -D_WATCHIT -D_486

# 386 Flags and switches
CC	= cc
# CFLAGS	= -g -Zd -Zi
# CFLAGS	= -Ox
CFLAGS	= -Ox
CCFLAGS	= -c -W1
LDFLAGS	= -Ox
LIBS	= -lcurses -lx -lmalloc -lc_s
DEFS	= -DM_TERMINFO -D_INKERNEL -D_ALPHA -D_WATCHIT

# DEFS	= -DM_TERMINFO -D_INKERNEL -D_ALPHA -D_WATCHIT -D_PROTECTED -D_OLD_OS

SHELL	= /bin/sh
CMDS	= wi
INSDIR	= $(ROOT)/etc
MODE	= 755
OWNER	= root
GROUP	= bin

#	all the programs
all:	$(CMDS)

#	call cc to link the *.o files
$(CMDS):	$(OBJECTS)
	@echo "\n\tLinking \"$(CMDS)\""
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
#	strip $(CMDS)
	@size  $(CMDS)

#	Build all the *.o files.
$(OBJECTS):	$(HEADERS)
	@echo "\n\tBuilding \"$*.o\""
	$(CC) $(CCFLAGS) $(CFLAGS) $(DEFS) $<

shar:	$(CMDS)
	@echo "\n\tStriping \"$(CMDS)\""
	strip $(CMDS)
	@echo "\n\tMaking sharfile \"$(CMDS).shar\""
	shar $(FILES) $(SHARFILES) $(HEADERS) > $(CMDS).shar

sharall:	shar
	$(COMPRESS) $(COMPFLAGS) $(CMDS).shar

tar:	sharall
	tar -cv6 $(CMDS).shar.gz

tarall:	sharall
	$(COMPRESS) $(COMPFLAGS) $(OBJECTS)
	tar -cv6 $(CMDS).shar.gz *.o.gz
	gzip -d -v *.o.gz

domhome:	sharall
	uucp -r -x9 $(CMDS).shar.gz domhome!~
	/usr/lib/uucp/uutry -r domhome
	
domg:	$(CMDS)
	strip $(CMDS)
	$(COMPRESS) $(COMPFLAGS) $(CMDS)
	uucp -r -x9 $(CMDS).gz domg!~
	/usr/lib/uucp/uutry -r domg
	
install: $(CMDS)
	chmod $(MODE)  $(CMDS)
	chgrp $(GROUP) $(CMDS)
	chown $(OWNER) $(CMDS)
	strip $(CMDS)

clean:
	rm -f $(CMDS) *.o a.out

clobber:	clean
	rm -f core

touch:
	touch *.h
