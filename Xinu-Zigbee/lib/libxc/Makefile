#
#  Make the Xinu version of the C run-time support library
#

#INCLUDE	= -I../../include
#hacked for platforms library.  Ugh.
INCLUDE	= -I../../include/MC1322x-include -I../../include
LIBNAME = libxc

# Make sure the library is optimized for size, if we want any optimization
CFLAGS := ${CFLAGS:-O%=-Os}

# Update the include flag
CFLAGS := ${CFLAGS:-I%=${INCLUDE}}

CFLAGS += -c

.s.o:
		${AS} -o $@ ${AFLAGS} $<

.c.o:
		${CC} ${CFLAGS} $<

CFILES	= abs.c atoi.c atol.c bzero.c ctype_.c doprnt.c doscan.c	\
		fgetc.c	fdoprnt.c fgets.c fprintf.c fputc.c fputs.c	\
		fscanf.c getchar.c labs.c memchr.c memcmp.c memcpy.c	\
		memset.c printf.c putchar.c qsort.c rand.c sprintf.c	\
		sscanf.c strchr.c strrchr.c strstr.c strncat.c		\
		strncmp.c strncpy.c strnlen.c strcmp.c strcpy.c		\
		strlen.c

OFILE2 = ${CFILES:%.c=%.o}
OFILES = ${OFILE2:%.s=%.o}

all:		${LIBNAME}.a

${LIBNAME}.a:	${OFILES}
		rm -f ${LIBNAME}.a
		${AR} cr ${LIBNAME}.a ${OFILES}
		${RANLIB} libxc.a

install:	${LIBNAME}.a
		cp ${LIBNAME}.a ${LIBDIR}/${LIBNAME}.a

clean:
		rm -f ${OFILES} ${LIBNAME}.a ${LIBDIR}/${LIBNAME}.a
