root=..
PKG_CONFIG = pkg-config
INCLUDES = $(root)/include
OPTS = -DNDEBUG
XTRACFLAGS = -g $(OPTS) -I$(INCLUDES)
MYLIBS = $(root)/libmsg/libmsg.a $(root)/libframe/libframe.a \
	$(root)/libXg/libXg.a
XLIBS = `$(PKG_CONFIG) --static --libs x11 xt` -lXt -lSM -lICE -lX11
ALLIBS = $(MYLIBS) $(LIBS) $(XLIBS)
TARGET = wily
OBJECTS = env.o include.o label.o file.o msg.o data.o line.o \
	vgeom.o vsearch.o vshow.o \
	tagmatch.o place.o event.o exec.o dir.o \
	point.o global.o cursor.o scroll.o path.o keyboard.o \
	wily.o tag.o view.o grow.o adjust.o win.o list.o col.o \
	undo.o builtins.o util.o select.o \
	mouse.o regexp.o text2.o \
	sam.o text.o click.o tile.o search.o complete.o
prefix = /usr/local
bindir = $(prefix)/bin

.SUFFIXES: .c .o

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MYLIBS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(ALLIBS)

.c.o:
	$(CC) $(CFLAGS) $(XTRACFLAGS) -c $<

install: $(TARGET)
	mkdir -p $(DESTDIR)$(bindir)
	cp $(TARGET) $(DESTDIR)$(bindir)

clean:
	rm -f *.o *.core core

nuke: clean
	rm -f $(TARGET)

$(OBJECTS): wily.h const.h proto.h ../include/msg.h
builtins.o keyboard.o mouse.o tag.o text.o text2.o tile.o view.o: view.h
keyboard.o search.o tag.o text.o text2.o undo.o view.o line.o click.o complete.o : text.h
tile.o grow.o adjust.o win.o col.o list.o:	tile.h
point.o:	tile.h view.h
sam.o regexp.o: sam.h
file.o msg.o data.o label.o complete.o keyboard.o:	data.h
adjust.o builtins.o complete.o keyboard.o mouse.o point.o select.o tag.o text2.o tile.o vgeom.o view.o vsearch.o vshow.o : view.h
