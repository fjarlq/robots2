# just -lcurses for SYS V
LIBS	= -lcurses -ltermcap
# add -DBSD42 for performance wins on 4.2 or better
IFDEF	= -DBSD42
CFLAGS  = $(IFDEF) #-g
LINT	= lint
LLFLAGS = -haxbc
SHAR	= shar
SHFLGS  = -v -c
HEADERS = robots.h
SRC	= good.c main.c opt.c robot.c score.c user.c
OBJS	= good.o main.o opt.o robot.o score.o user.o
MAN	= robots.6
MISC	= READ_ME Makefile
DIST	= $(MISC) $(SRC) $(MAN) $(HEADERS)
MANUAL  = /usr/man/man6/robots.6	# where the manual page goes
BINARY  = /usr/sheriff/jpo/games			# Where the binary goes

robots: ${OBJS}
	$(CC) $(CFLAGS) -o $@ ${OBJS} $(LIBS)

install: robots robots.6
	install -s robots $(BINARY)	# or strip robots && mv robots $(BINARY)
#	install -c robots.6 $(MANUAL)	# or cp robots.6 $(MANUAL)

tidy: clean
clean:
	rm -f ${OBJS} core a.out lint.out shar.out

clobber: clean
	rm -f robots

lint: ${SRC} $(HEADERS)
	$(LINT) $(LLFLAGS) $(IFDEF) ${SRC} $(LIBS)

shar: shar.out
shar.out: $(DIST)
	$(SHAR) $(SHFLGS) $(DIST) > $@

${OBJS}: ${HEADERS}
