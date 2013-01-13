CC = gcc 
#CC= i586-mingw32msvc-cc
BINDIR= ./bin
DATADIR= ./dat
SOUNDDIR= $(DATADIR)/sounds
SRCPATH= ./src/

CONFIGDIR=$(HOME)/.spacezero
RECORDFILE=$(CONFIGDIR)/record


# The binary executable will be copied in the next directory.
INSTALL_DIR=/usr/local/bin

# All data files will be copied in the next directory.
# tar.gz 
INSTALL_DATA_DIR=/usr/local/share/spacezero
# DEB pkg 
#INSTALL_DATA_DIR=/usr/share/spacezero


OPENALFLAGS= -lopenal -lalut -lvorbisfile

GTKFLAGS=`pkg-config --cflags gtk+-2.0`
GTKLIBS=`pkg-config --libs gtk+-2.0`

GTKLIBS12=`gtk-config --cflags`
GTKFLAGS12=`gtk-config --libs`


LDFLAGS=  -lm -lpthread $(OPENALFLAGS) $(GTKLIBS)
LDFLAGS12=  -lm -lpthread $(OPENALFLAGS) $(GTKLIBS12)
CFLAGS=  -Wall -O3  -I./include -DDATADIR=\"$(DATADIR)\" -DINSTALL_DATA_DIR=\"$(INSTALL_DATA_DIR)\"
CFLAGS12=  -Wall -O3 -I./include -DGTK12 -DDATADIR=\"$(DATADIR)\" -DINSTALL_DATA_DIR=\"$(INSTALL_DATA_DIR)\" 
PROGFLAGS=$(CFLAGS) -pg 
PROFCC=  $(CC) -pg 
PROGRAM=$(BINDIR)/spacezero


SOUND_OBJS= $(SRCPATH)sound.o  $(SRCPATH)streamsound.o
SOUND_SRCS= $(SRCPATH)sound.c  $(SRCPATH)streamsound.c


PROGRAM_OBJS=$(SRCPATH)spacezero.o $(SRCPATH)objects.o $(SRCPATH)ai.o $(SRCPATH)save.o $(SRCPATH)shell.o $(SRCPATH)spacecomm.o $(SRCPATH)help.o $(SRCPATH)comm.o $(SRCPATH)graphics.o  $(SRCPATH)functions.o $(SRCPATH)data.o $(SRCPATH)menu.o $(SRCPATH)sectors.o $(SRCPATH)clock.o  $(SRCPATH)statistics.o $(SRCPATH)randomnamegen.o $(SOUND_OBJS)
PROGRAM_SRCS=$(SRCPATH)spacezero.c $(SRCPATH)objects.c $(SRCPATH)ai.c $(SRCPATH)save.c $(SRCPATH)shell.c $(SRCPATH)spacecomm.c $(SRCPATH)help.c $(SRCPATH)comm.c $(SRCPATH)graphics.c  $(SRCPATH)functions.c $(SRCPATH)data.c $(SRCPATH)menu.c $(SRCPATH)sectors.c $(SRCPATH)clock.c$ $(SRCPATH)statistics.c $(SRCPATH)randomnamegen.c $(SOUND_SRCS) 



all: dirs spacezero

spacezero: $(PROGRAM_OBJS) 
	$(CC) $(PROGRAM_OBJS)  $(LDFLAGS) $(CFLAGS) -g -o $(PROGRAM)

gtk12: $(PROGRAM_SRCS) 
	$(CC) `gtk-config --cflags` $(PROGRAM_SRCS)  $(LDFLAGS12) $(CFLAGS12) -g -o $(PROGRAM) `gtk-config --libs`

static: $(PROGRAM_OBJS) 
	$(CC) -static $(GTKFLAGS) $(PROGRAM_OBJS)  $(LDFLAGS) $(CFLAGS) -o $(PROGRAM) 

install:
	-if [ ! -d $(INSTALL_DIR) ] ; then mkdir -p $(INSTALL_DIR) ; fi
	-if [ ! -d $(INSTALL_DATA_DIR) ] ; then mkdir -p $(INSTALL_DATA_DIR); fi
	-if [ ! -d $(INSTALL_DATA_DIR)/sounds/ ] ; then mkdir -p $(INSTALL_DATA_DIR)/sounds/; fi
	cp $(SOUNDDIR)/* $(INSTALL_DATA_DIR)/sounds/
	cp $(DATADIR)/letterfrequencytable $(INSTALL_DATA_DIR)
	cp $(PROGRAM) $(INSTALL_DIR)

uninstall:
	-rm $(INSTALL_DIR)/spacezero
	-if [ ! -d $(INSTALL_DATA_DIR) ] ; then rm -r $(INSTALL_DATA_DIR); fi

debug: 	$(PROGRAM_OBJS) 
	$(CC) $(GTKFLAGS) $(PROGRAM_OBJS)  $(LDFLAGS) $(CFLAGS) -g -o $(PROGRAM) 

prof: $(PROGRAM_OBJS)
	$(CC) $(GTKFLAGS) $(PROGRAM_OBJS) $(LDFLAGS) $(CFLAGS) -pg -o $(PROGRAM)

prof2: 
	$(CC) $(PROGRAM_SRCS) $(GTKFLAGS) $(PROGFLAGS)  -c
	$(CC) $(PROGRAM_OBJS) $(GTKFLAGS) $(PROGFLAGS)  $(GTKLIBS)

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
#		$(CC)  $(CFLAGS)   $(LDFLAGS) $(GTKFLAGS) -c $< -o  $@ 
# add -pg for profile
#		$(CC) -pg  $(CFLAGS)  $(GTKFLAGS) -c $< -o  $@ 
# add -g for debug
		$(CC) -g $(CFLAGS)  $(GTKFLAGS) -c $< -o  $@ 
