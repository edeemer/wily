root=../..
ALLCFLAGS=$(CFLAGS) -g -Wall -I$(root)/include
ALLIBS=$(LIBS) $(root)/libmsg/libmsg.a

TARGETS=Wgoto Wexec Wins
OBJECTS=$(TARGETS:%=%.o)

.SUFFIXES: .c .o

all: $(TARGETS)

.c.o:
	$(CC) $(ALLCFLAGS) -c $<

Wins:	Wins.o
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o Wins Wins.o $(ALLIBS)
Wexec:	Wexec.o
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o Wexec Wexec.o $(ALLIBS)
Wgoto:	Wgoto.o
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o Wgoto Wgoto.o $(ALLIBS)
clean:
	rm -f $(OBJECTS)
nuke:
	rm -f $(OBJECTS) $(TARGETS)
