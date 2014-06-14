# $Sunshine$

PROGS= svwaitup svwaitdown svwaitdone utmpset fghack chpst svlogd
SRCS.svwaitup= svwaitup.c
SRCS.svwaitdown= svwaitdown.c
SRCS.svwaitdone= svwaitdone.c
SRCS.utmpset= utmpset.c
SRCS.fghack= fghack.c
SRCS.chpst= chpst.c
SRCS.svlogd= svlogd.c pmatch.c
MAN.utmpset=utmpset.8
MAN.svlogd= svlogd.8
MAN.chpst= chpst.8

BINDIR= /libexec/svc

DPADD= ${LIBTIME} ${LIBUNIX} ${LIBBYTE} 
LDADD= ${LIBTIME} ${LIBUNIX} ${LIBBYTE}

clean:
	rm -f ${PROGS} *.o *.8.gz

.include <bsd.progs.mk>
.include <../runit.prog.mk>
