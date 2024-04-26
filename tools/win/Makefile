root			= ../..

prefix			= /usr/local
bindir			= $(prefix)/bin
mandir			= $(prefix)/man

INSTALL			= install
INSTALL_DATA	= ${INSTALL} -m 644

RC			= $(bindir)/rc
# Use this macro to choose TTY handling type manually
#DEFS			= -DDEV_PTMY -UDEV_PTMX -UGETPTY
ALLCFLAGS		= $(CFLAGS) -g -I$(root)/include $(DEFS)
ALLIBS			= $(LIBS) $(root)/libmsg/libmsg.a $(root)/libXg/libXg.a

SRCS			= win.c wgoto.c wreplace.c Tag.rc mktags.rc Man.rc
BINS			= win   wgoto   wreplace   Tag    mktags    Man    
MANS			= win.1 wgoto.1 wreplace.1 Tag.1  mktags.1  Man.1

.SUFFIXES:
.SUFFIXES: .c .rc
.c:
	$(CC) $(ALLCFLAGS) $(DEFS) $(LDFLAGS) -o $* $*.c $(ALLIBS)

.rc:
	( echo '#!' $(RC) && cat $*.rc; ) >$* && \
	chmod a+x $*


all: $(BINS)

install: all installdirs
	for BIN in $(BINS) ; do \
		$(INSTALL) $$BIN $(DESTDIR)$(bindir)/$$BIN ; \
	done
	for MAN in $(MANS) ; do \
		$(INSTALL_DATA) $$MAN $(DESTDIR)$(mandir)/man1/$$MAN ; \
	done

installdirs: mkinstalldirs
	mkdir -p $(DESTDIR)$(bindir) $(DESTDIR)$(mandir)/man1

clean:
	rm -f $(BINS)
