# $Sunshine$

PROGS= svwaitup svwaitdown svwaitdone utmpset fghack
SRCS.svwaitup= svwaitup.c
SRCS.svwaitdown= svwaitdown.c
SRCS.svwaitdone= svwaitdone.c
SRCS.utmpset= utmpset.c
SRCS.fghack= fghack.c
MAN.utmpset=utmpset.8

BINDIR= /sbin

DPADD= ${LIBTIME} ${LIBUNIX} ${LIBBYTE} 
LDADD= ${LIBTIME} ${LIBUNIX} ${LIBBYTE}

clean:
	rm ${PROGS} *.o

.include <bsd.progs.mk>
.include <../runit.prog.mk>
