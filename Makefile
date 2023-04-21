CC =		gcc
CFLAGS =	$(FLAGS) -c -I$(INCLUDEPATH)

$(shell mkdir -p $(MACHTYPE)-$(OSTYPE))

OBJS =	standard.o

all:	$(OBJS)
	mv *.o $(MACHTYPE)-$(OSTYPE)
.KEEP_STATE:
