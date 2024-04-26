root = ../..
INCLUDES = $(root)/include
LDFLAGS = -L$(root)/libXg -L$(root)/libmsg
LDLIBS = -lmsg -lXg 
# gmake CFLAGS = -g -Wall -ansi
CFLAGS = -g -Wall -ansi -I$(INCLUDES) $(LDLIBS)
PROGS = wcat windows wreplace wattach wread

all: $(PROGS) 

install:
	cp $(PROGS) $(binpath)

clean:
	rm $(PROGS)
