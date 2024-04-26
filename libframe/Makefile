root=..
INCLUDES=$(root)/include
XTRACFLAGS=-g -I$(INCLUDES)
LIB=libframe.a
OBJS=frbox.o frdelete.o frdraw.o frinit.o frinsert.o frptofchar.o \
	frselect.o frstr.o frutil.o misc.o
include $(INCLUDES)/Makelib
$(OBJS):	$(INCLUDES)/u.h $(INCLUDES)/libc.h $(INCLUDES)/frame.h
