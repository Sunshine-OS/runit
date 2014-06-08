# $Sunshine$

PROG=svlogd
SRCS=svlogd.c pmatch.c
MAN=svlogd.8

BINDIR= /sbin

DPADD= ${LIBTIME} ${LIBUNIX} ${LIBBYTE} 
LDADD= ${LIBTIME} ${LIBUNIX} ${LIBBYTE}

.include <../runit.prog.mk>
.include <bsd.prog.mk>
