SUBDIRS = libXg libframe libmsg wily

all:	wily/wily

wily/wily: libXg/libXg.a libframe/libframe.a libmsg/libmsg.a
	cd wily && $(MAKE) $(MFLAGS)

libXg/libXg.a:
	cd libXg && $(MAKE) $(MFLAGS)

libframe/libframe.a:
	cd libframe && $(MAKE) $(MFLAGS) all

libmsg/libmsg.a:
	cd libmsg && $(MAKE) $(MFLAGS) all

install:	all
	cd wily && $(MAKE) $(MFLAGS) install

clean:
	for i in $(SUBDIRS); do \
		(cd $$i && $(MAKE) $(MFLAGS) $@); \
	done

nuke:
	for i in $(SUBDIRS); do \
		(cd $$i && $(MAKE) $(MFLAGS) $@); \
	done
