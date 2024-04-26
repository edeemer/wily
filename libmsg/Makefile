root=..
INCLUDES=$(root)/include
XTRACFLAGS=-g -I$(INCLUDES)
LIB=libmsg.a
OBJS=connect.o msg.o util.o rpc.o
include $(INCLUDES)/Makelib
$(OBJS):	$(INCLUDES)/msg.h
