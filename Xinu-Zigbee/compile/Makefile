#
#  Makefile for the mc1322 version of the Xinu operating system
#

XINU_ROOT = ..
DEV_PLATFORM=LINUX

LIB	= 	../lib/libxc
XINU_ELF =	./xinu.elf
XINU	=	./xinu.bin

MAKEDEP	= gcc -M -MG

DEFS	= 
CFLAGS_EXTRA =

INCLUDE = -I../include/MC1322x-include -I../include

CFLAGS =  ${DEFS} ${INCLUDE} ${CFLAGS_EXTRA}

#include ../system/Makefile.include
# Set up cross compiler and toolchain definitions.

CROSS_COMPILE ?= arm-none-eabi-
AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CC) -E
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB	= $(CROSS_COMPILE)ranlib
OBJSIZE = $(CROSS_COMPILE)size

# Build CFLAGS and prepend it to user-supplied CFLAGS
CFLAGS_PLAT  ?= -march=armv4t -mtune=arm7tdmi-s -mlong-calls -msoft-float \
		-mthumb-interwork -fno-strict-aliasing -fno-common -ffixed-r8 \
		-ffunction-sections -ffreestanding -fno-builtin -nodefaultlibs
CFLAGS_WARN  ?= -Wcast-align -Wall -Wstrict-prototypes -Wextra 
CFLAGS_OPT   ?= -Os
# CFLAGS_DEBUG ?= -g -DDEBUG -Werror
CFLAGS_DEBUG ?= -g
CFLAGS_MISC  ?= -pipe
CFLAGS := $(CFLAGS_PLAT) $(CFLAGS_WARN) $(CFLAGS_OPT) $(CFLAGS_DEBUG) $(CFLAGS_MISC) $(CFLAGS)

# Thumb flags, used for building most objects
CFLAGS_THUMB ?= -mthumb -mcallee-super-interworking

# Linker flags
LINKERSCRIPT ?= ../compile/mc1322x.lds
LDFLAGS      ?= -T $(LINKERSCRIPT) -nostartfiles -static -export-dynamic -u_start -Wl,-Map=$(@:.elf=.map)

# Assembler flags
AFLAGS       ?= -Wa,-gstabs $(CFLAGS)

# Misc tool options
OBJCOPYFLAGS ?= --gap-fill=0xff
ARFLAGS      = cr

%.o: %.S
	$(CC) $(AFLAGS) -MMD -c -o $@ $<

TOPDIR	= ..

# Components (files for each are listed below) 

COMPS	=	system 							\
		device/ttyMC1322x					\
		device/radio						\
#		device/nam						\
#		device/eth						\
		net 							\
#		shell 							\

# Start with empty source file list and add files for each directory
#

SRC_FILES =

#------------------------------------------------------------------------
# Files for ../system							#
#------------------------------------------------------------------------

SYSTEM_SFILES =								\
	start.S intr.S ctxsw.S irq.S userstart.S

SYSTEM_CFILES =								\
	bufinit.c	chprio.c	panic.c		\
	close.c		conf.c		control.c	\
	freebuf.c	freemem.c	getbuf.c	\
	getc.c		getdev.c	getitem.c	getmem.c	\
	getpid.c	getprio.c 	getstk.c	\
			insert.c	insertd.c	ioerr.c		\
	ionull.c	kill.c		main.c		\
	mkbufpool.c	newqueue.c	open.c				\
	putc.c		queue.c		read.c		ready.c		\
	receive.c	recvclr.c	recvtime.c	resched.c	\
	resume.c	sched_cntl.c 	seek.c		semcount.c	\
	semcreate.c	semdelete.c	semreset.c	send.c		\
	signal.c	signaln.c	sleep.c		suspend.c	\
	unsleep.c	userret.c	wait.c		wakeup.c	\
	write.c		xdone.c		yield.c				\
	isr.c		kprintf.c	\
	initialize.c	blink_red.c \
	clkinit.c 	gpio.c		blink_green.c	blink_red_once.c \
	create.c	blink_green_once.c		clkint.c	

SYSTEM_SFULL = ${SYSTEM_SFILES:%=../system/%}
SYSTEM_CFULL = ${SYSTEM_CFILES:%=../system/%}

SRC_FILES += $(SYSTEM_SFULL)
SRC_FILES += $(SYSTEM_CFULL)

#------------------------------------------------------------------------
# Files for ../device/ttyMC1322x					#
#------------------------------------------------------------------------

# TTY_SFILES =								\
# 	ttyDispatch.S


TTY_CFILES =								\
	ttyControl.c	ttyGetc.c	ttyInit.c	ttyInter_in.c	\
	ttyInter_out.c	ttyInterrupt.c	ttyKickOut.c	ttyPutc.c	\
	ttyRead.c	ttyWrite.c	ttyDispatch.c

TTY_SFULL = ${TTY_SFILES:%=../device/ttyMC1322x/%}
TTY_CFULL = ${TTY_CFILES:%=../device/ttyMC1322x/%}

SRC_FILES += $(TTY_SFULL)
SRC_FILES += $(TTY_CFULL)

#------------------------------------------------------------------------
# Files for ../device/radio
#------------------------------------------------------------------------

RADIO_CFILES =								\
	radioInit.c	radioIsr.c	radioRead.c	radioWrite.c	\
	maca.c		nvm.c		radioIn.c	radioOut.c

RADIO_SFULL = ${RADIO_SFILES:%=../device/radio/%}
RADIO_CFULL = ${RADIO_CFILES:%=../device/radio/%}

SRC_FILES += $(RADIO_SFULL)
SRC_FILES += $(RADIO_CFULL)

#------------------------------------------------------------------------
# Files for ../device/nam                                                                                                                                                                        
#------------------------------------------------------------------------

#NAM_CFILES =                                                            \
#	mount.c         namInit.c       namOpen.c       nammap.c
#	
#NAM_CFULL  = ${NAM_CFILES:%=../device/nam/%}
#
#SRC_FILES += $(NAM_CFULL)

#------------------------------------------------------------------------
# Files in directory ../device/eth
#------------------------------------------------------------------------

#ETH_SFILES =								\
#	ethDispatch.S
#
#ETH_CFILES =								\
#	ethInit.c 	ethInterrupt.c 	ethControl.c	ethRead.c 	\
#	ethWrite.c 	e1000e_init.c 	e1000e_open.c 	e1000e_read.c 	\
#	e1000e_write.c 	e1000e_cntl.c 	e1000e_intr.c 	e1000e_cmd.c 	\
#	3c905C_init.c 	3c905C_open.c 	3c905C_read.c 	3c905C_write.c 	\
#	3c905C_cntl.c 	3c905C_intr.c 	3c905C_cmd.c	e1000e_rar_set.c
#
#ETH_SFULL = ${ETH_SFILES:%=../device/eth/%}
#ETH_CFULL = ${ETH_CFILES:%=../device/eth/%}
#
#SRC_FILES += $(ETH_SFULL)
#SRC_FILES += $(ETH_CFULL)

#------------------------------------------------------------------------
# Files for ../net
#------------------------------------------------------------------------

NET_CFILES =								\
	net6.c		netiface6.c	pdump6.c	printhex.c
#	net.c	 	arp.c 		udp.c 		pdump.c 	\
#	dhcp.c		ip.c		icmp.c		nat.c		\
#	netiface.c	netstart.c	dot2ip.c	hexdump.c
#
NET_CFULL  = ${NET_CFILES:%=../net/%}

SRC_FILES += $(NET_CFULL)

#------------------------------------------------------------------------
# Files for ../shell
#------------------------------------------------------------------------

#SHELL_CFILES =								\
#	addargs.c	lexan.c		shell.c
#
#SHELL_CFILES +=								\
#	xsh_argecho.c	xsh_cat.c	xsh_clear.c 	xsh_uptime.c	\
#	xsh_echo.c	xsh_exit.c	xsh_devdump.c	xsh_help.c	\
#	xsh_kill.c	xsh_led.c	xsh_memdump.c	xsh_ps.c 	\
#	xsh_ping.c	xsh_sleep.c 	xsh_memstat.c	xsh_date.c	\
#	xsh_arp.c	xsh_ipaddr.c 	xsh_udpecho.c	xsh_udpdump.c	\
#	xsh_bingid.c	xsh_udpeserver.c
#
#SHELL_CFULL  = ${SHELL_CFILES:%=../shell/%}
#
#SRC_FILES += $(SHELL_CFULL)

#------------------------------------------------------------------------
# Files for ../platforms/MC1322x
#------------------------------------------------------------------------

#MC1322x_SFILES = start.S intr.S ctxsw.S irq.S userstart.S

#MC1322x_CFILES = isr.c	kprintf.c	initialize.c	blink_red.c \
#		 clkinit.c 	gpio.c	blink_green.c	blink_red_once.c \
#		 create.c	blink_green_once.c	clkint.c

#MC1322x_SFULL  = ${MC1322x_SFILES:%=../platforms/MC1322x/%}
#MC1322x_CFULL  = ${MC1322x_CFILES:%=../platforms/MC1322x/%}

#SRC_FILES += $(MC1322x_SFULL)
#SRC_FILES += $(MC1322x_CFULL)


########################################
# Generate a list of all object files
#######################################

OBJ_TMP	= $(patsubst %.c,%.o,$(SRC_FILES)) 	# substitute .c => .o
OBJ_FILES  = $(patsubst %.S,%.o,$(OBJ_TMP)) 	# substitute .S => .o


# Export variables for recursive make calls (such as the library)
export

#------------------------------------------------------------------------
# make targets
#------------------------------------------------------------------------

all:	xinu

xinu: 	Makefile ../config/Configuration configure 	\
		$(OBJ_FILES) $(LIB)/libxc.a
		@echo;echo Now loading all object files to produce xinu.elf
		$(CC) $(LDFLAGS) ${OBJ_FILES} ${LIB}/libxc.a -o ${XINU_ELF}
		@echo;echo Produce xinu.bin from xinu.elf
		${OBJCOPY} --gap-fill=0xff -O binary xinu.elf xinu.bin
		@echo
		${OBJSIZE} xinu.elf
		@echo

${LIB}/libxc.a:
		@echo;echo Making libxc.a
		@(cd ${LIB}; make libxc.a)

objects: $(OBJ_FILES)

configure: ../include/conf.h 	../system/conf.c	

../include/conf.h ../system/conf.c: ../config/config ../config/Configuration
	@echo making conf.h and conf.c
	@(cd ../config; make install)

../config/config:
	make -C ../config/ -f ../config/Makefile clean all install

clean:
	@echo removing .o files in directories
	@echo $(COMPS)
	@rm -f ${OBJ_FILES} ${LIB}/*.a ${LIB}/*.o
	@echo removing lib ...
	@(cd ${LIB}; make clean)
	@echo   removing configuration files ...
	@rm -f ../include/conf.h ../system/conf.c
	@(cd ../config; make clean)
	@echo removing xinu ...
	@rm -f xinu xinu.bin xinu.elf


depend:
	@echo;echo Making all dependencies in .deps
	@$(MAKEDEP) ${INCLUDE} ${SRC_FILES} > .deps
	@echo;echo Finished making dependencies

depclean:
	rm -f .deps
	touch .deps

###################
# Special targets #
###################
${COMPS}:
	$(MAKE) COMPS="$@" objects

../system/initialize.o: ../system/initialize.c
	sh mkvers.sh
	${CC} -c ${CFLAGS} -DVERSION=\""`cat version`"\" 		\
		../system/initialize.c -o ../system/initialize.o

tar:
	(cd ../..; tar cf XINU_MC1322_`date +"%C%y%m%d"`.droms.tar.gz -z ./MC1322)

include .deps
