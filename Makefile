# $Sunshine$

SUBDIR=libbyte \
		libtime \
		libunix \
		runsv \
		runsvdir \
		pid1 \
		sv \
		telinit \
		runsvchdir \
		utils

preclean:
	rm -rf SSOSrunit
	rm -rf proto

clean: preclean

package: all
	mkdir -p proto
	cp -r distrib/* proto/
	mkdir -p proto/usr/share/man/man8
	fakeroot make install DESTDIR=${PWD}/proto
	sh packageit


.include <bsd.subdir.mk>
