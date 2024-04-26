root=../..
CC?=cc
W=$(TMPDIR)/wily-0.13.19/
INC=-I$(root)/include
WLD=-L$W/sam/libXg -L$W/libmsg -lmsg -lXg
CFLAGS+=-Wall $(INC)
LIBS+=$(WLD) -ltk -ltcl -lX11 -lm
T=wtcl wtk
S=wtcl.c wtk.c wily.c
O=$(S:.c=.o)

all : $T

wtcl : wtcl.o wily.o
	$(CC) $(LDFLAGS) -o $@ wtcl.o wily.o $(LIBS)
	
wtk : wtk.o wily.o
	$(CC) $(LDFLAGS) -o $@ wtk.o wily.o $(LIBS)
	
clean :
	rm -f $T $O core
