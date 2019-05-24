CC = /usr/bin/gcc 
#CC=gcc

BINDIR= ./bin
DATADIR= ./dat
SOUNDDIR= $(DATADIR)/sounds
SRCPATH= ./src/

CONFIGDIR=$(HOME)/.spacezero
RECORDFILE=$(CONFIGDIR)/record


# The binary executable will be copied in the next directory.
INSTALL_DIR=/usr/local/games

# All data files will be copied in the next directory.
# tar.gz 
INSTALL_DATA_DIR=/usr/local/share/spacezero

# DEB pkg 
#INSTALL_DIR=/usr/games
#INSTALL_DATA_DIR=/usr/share/spacezero

PKG_CONFIG ?= pkg-config
OPENALFLAGS= -lopenal -lalut
SOUNDFLAGS= $(OPENALFLAGS) -lvorbisfile

GTKFLAGS=`$(PKG_CONFIG) --cflags gtk+-2.0`
GTKLIBS=`$(PKG_CONFIG) --libs gtk+-2.0`

GTKLIBS12=`gtk-config --cflags`
GTKFLAGS12=`gtk-config --libs`

CFLAGS=  -Wall --pedantic -I./include -DDATADIR=\"$(DATADIR)\" -DINSTALL_DATA_DIR=\"$(INSTALL_DATA_DIR)\"
LDFLAGS=  $(SOUNDFLAGS) $(GTKLIBS) -lX11 -lm -lpthread

CFLAGS12=  -Wall -I./include -DGTK12 -DDATADIR=\"$(DATADIR)\" -DINSTALL_DATA_DIR=\"$(INSTALL_DATA_DIR)\" 
LDFLAGS12= $(SOUNDFLAGS) $(GTKLIBS12) -lX11 -lm -lpthread

PROGRAM=$(BINDIR)/spacezero


SOUND_OBJS= $(SRCPATH)sound.o  $(SRCPATH)streamsound.o
SOUND_SRCS= $(SRCPATH)sound.c  $(SRCPATH)streamsound.c

PROGRAM_OBJS=$(SRCPATH)spacezero.o $(SRCPATH)objects.o $(SRCPATH)ai.o $(SRCPATH)save.o $(SRCPATH)shell.o $(SRCPATH)spacecomm.o $(SRCPATH)help.o $(SRCPATH)comm.o $(SRCPATH)graphics.o  $(SRCPATH)functions.o $(SRCPATH)data.o $(SRCPATH)menu.o $(SRCPATH)sectors.o $(SRCPATH)clock.o  $(SRCPATH)statistics.o $(SRCPATH)randomnamegen.o $(SRCPATH)locales.o $(SRCPATH)players.o $(SRCPATH)snow.o $(SOUND_OBJS) 
PROGRAM_SRCS=$(SRCPATH)spacezero.c $(SRCPATH)objects.c $(SRCPATH)ai.c $(SRCPATH)save.c $(SRCPATH)shell.c $(SRCPATH)spacecomm.c $(SRCPATH)help.c $(SRCPATH)comm.c $(SRCPATH)graphics.c  $(SRCPATH)functions.c $(SRCPATH)data.c $(SRCPATH)menu.c $(SRCPATH)sectors.c $(SRCPATH)clock.c $(SRCPATH)statistics.c $(SRCPATH)randomnamegen.c $(SRCPATH)locales.c $(SRCPATH)players.c $(SRCPATH)snow.c $(SOUND_SRCS) 



all: dirs spacezero

spacezero: $(PROGRAM_OBJS) 
	$(CC) $(PROGRAM_OBJS)  $(LDFLAGS) $(CFLAGS) -g -o $(PROGRAM)

gtk12: $(PROGRAM_SRCS) 
	$(CC) `gtk-config --cflags` $(PROGRAM_SRCS)  $(LDFLAGS12) $(CFLAGS12) -g -o $(PROGRAM) `gtk-config --libs`

static: $(PROGRAM_OBJS) 
	$(CC) -static $(PROGRAM_OBJS)  $(LDFLAGS) $(CFLAGS) -o $(PROGRAM) 

install:
	-if [ ! -d $(INSTALL_DIR) ] ; then mkdir -p $(INSTALL_DIR) ; fi
	-if [ ! -d $(INSTALL_DATA_DIR) ] ; then mkdir -p $(INSTALL_DATA_DIR); fi
	-if [ ! -d $(INSTALL_DATA_DIR)/sounds/ ] ; then mkdir -p $(INSTALL_DATA_DIR)/sounds/; fi
	-if [ ! -d $(INSTALL_DATA_DIR)/locales/ ] ; then mkdir -p $(INSTALL_DATA_DIR)/locales/; fi
	cp $(SOUNDDIR)/*.ogg $(INSTALL_DATA_DIR)/sounds/
	cp $(DATADIR)/letterfrequencytable $(INSTALL_DATA_DIR)
	cp $(DATADIR)/locales/*.txt $(INSTALL_DATA_DIR)/locales/
	cp $(PROGRAM) $(INSTALL_DIR)
	chmod -x $(INSTALL_DATA_DIR)/sounds/*
	chmod -x $(INSTALL_DATA_DIR)/locales/*
	chmod -x $(INSTALL_DATA_DIR)/letterfrequencytable
	@echo executable files copied to: $(INSTALL_DIR)
	@echo data files copied to: $(INSTALL_DATA_DIR)

uninstall:
	-rm $(INSTALL_DIR)/spacezero
	-if [ -d $(INSTALL_DATA_DIR) ] ; then rm -r $(INSTALL_DATA_DIR); fi

debug: 	$(PROGRAM_OBJS) 
	$(CC) $(GTKFLAGS) $(PROGRAM_OBJS)  $(LDFLAGS) $(CFLAGS) -g -o $(PROGRAM) 

prof: $(PROGRAM_OBJS)
	$(CC) $(GTKFLAGS) $(PROGRAM_OBJS) $(LDFLAGS) $(CFLAGS) -pg -o $(PROGRAM)

dirs:
	-if [ ! -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi
	-if [ ! -d $(CONFIGDIR) ] ; then mkdir $(CONFIGDIR) ; fi
	-if [ ! -f $(RECORDFILE) ] ; then touch $(RECORDFILE) ; fi

clean: 
	rm -f src/*.o bin/spacezero
	rm -f core

mrproper: clean
	rm -f src/*~ include/*~

.c.o:	
#		$(CC) $(CFLAGS) $(GTKFLAGS) -c $< -o  $@ 
# add -pg for profile
#		$(CC) -pg  $(CFLAGS)  $(GTKFLAGS) -c $< -o  $@ 
# add -g for debug
		$(CC) -g $(CFLAGS)  $(GTKFLAGS) -c $< -o  $@ 
