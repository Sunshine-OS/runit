# $Sunshine$

SUBDIR=libbyte \
		libtime \
		libunix \
		runsv \
		runsvdir \
		pid1 \
		sv \
		telinit \
		svlogd \
		chpst \
		runsvchdir \
		utils

preclean:
	rm -rf SSOSrunit

clean: preclean

preinstall:
	mkdir -p ${DESTDIR}/usr/share/man/man8

package: all
	fakeroot make install DESTDIR=${PWD}/proto
	sh packageit


.include <bsd.subdir.mk>
