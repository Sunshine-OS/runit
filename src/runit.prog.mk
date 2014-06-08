# $Sunshine$

# # # # # # # #
# Includes  # #
# # # # # # # #
CFLAGS+=-I${PWD}/../libbyte -I${PWD}/../libtime -I${PWD}/../libunix \
		-I ${PWD}/../h

# # # # # # # #
# Libraries # #
# # # # # # # #
LDFLAGS+=-L${PWD}/../libbyte -L${PWD}/../libtime -L${PWD}/../libunix 

LIBBYTE=${PWD}/../libbyte/libbyte.a
LIBTIME=${PWD}/../libtime/libtime.a
LIBUNIX=${PWD}/../libunix/libunix.a

libclean: 
	rm -f *.o *.a *.po

