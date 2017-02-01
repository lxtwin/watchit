#
#	@(#)init.wi.mk	1.1 91/03/13 - PKR.
#
#	Makefile for init.wi

CMDS = install.wi
OBJS = init.wi.o
SRC  = init.wi.c

INSDIR = $(ROOT)/u/src/bin
MODE   = 0755
OWNER  = bin
GROUP  = bin

CFLAGS = -d -Olt
LIBS = -lcurses -lx -lcrypt_i
LDFLAGS = -DM_TERMINFO

all: $(CMDS)

$(CMDS): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(CMDS) $(LIBS) 
	strip $(CMDS)

install: $(CMDS)
	mv $(CMDS) $(INSDIR)
	chmod $(MODE)  $(INSDIR)/$(CMDS)
	chgrp $(GROUP) $(INSDIR)/$(CMDS)
	chown $(OWNER) $(INSDIR)/$(CMDS)
	strip $(CMDS)

clean:
	rm -f $(OBJS)

clobber: clean
	rm -f $(CMDS)
