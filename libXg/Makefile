root=..
PKG_CONFIG=pkg-config
INCLUDES=$(root)/include
X11FLAGS=`$(PKG_CONFIG) --cflags x11`
XTRACFLAGS=-g $(X11FLAGS) -I$(INCLUDES)
LIB=libXg.a
OBJS=arc.o arith.o balloc.o bitblt.o bitbltclip.o border.o bscreenrect.o\
	circle.o clipline.o clipr.o copymasked.o cursorset.o cursorswitch.o\
	disc.o ellipse.o font.o gcs.o getrect.o gwin.o ldconvert.o latin1.o\
	mkfont.o menuhit.o point.o polysegment.o rdbitmap.o rdbitmapfile.o\
	rdfontfile.o rectclip.o rune.o segment.o string.o strwidth.o texture.o\
	wrbitmap.o wrbitmapfile.o xtbinit.o
include $(INCLUDES)/Makelib
$(OBJS):	$(INCLUDES)/u.h $(INCLUDES)/libc.h $(INCLUDES)/libg.h

gwin.o:	keysymtab.h
gwin.o latin1.o: latin1.h
