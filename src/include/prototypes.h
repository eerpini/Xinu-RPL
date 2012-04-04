/* in file addargs.c */
extern	status	addargs(pid32, int32, int32[], int32,char *, void *);

/* in file arp.c */
extern	int32	arp_alloc(void);
extern	void	arp_in(void);
extern	void	arp_init(void);
extern	status	arp_resolve(uint32, byte *);
extern 	void 	arp_ntoh(struct arppacket *);
extern 	void 	arp_hton(struct arppacket *);

/* in file ascdate.c */
extern	status	ascdate(uint32, char *);

/* in file bufinit.c */
extern	status	bufinit(void);

/* in file chprio.c */
extern	pri16	chprio(pid32, pri16);

/* in file clkupdate.S */
extern	uint32	clkcount(void);

/* in file clkhandler.c */
extern	interrupt clkhandler(void);

/* in file clkinit.c */
extern	void	clkinit(void);

/* in file clkint.S */
extern	void	clkint(void);

/* in file close.c */
extern	syscall	close(did32);

/* in file control.c */
extern	syscall	control(did32, int32, int32, int32);

/* in file create.c */
extern	pid32	create(void *, uint32, pri16, char *, uint32, ...);

/* in file ctxsw.S */
extern	void	ctxsw(void *, void *);

/* in file dot2ip.c */
extern	uint32	dot2ip(char *, uint32 *);

/* in file queue.c */
extern	pid32	enqueue(pid32, qid16);

/* in file intutils.S */
extern	intmask	disable(void);

/* in file intutils.S */
extern	void	enable(void);

/* in file e1000e_cmd.c */
extern 	void 	e1000e_irq_enable(struct ether 	*ethptr);
extern 	void 	e1000e_irq_disable(struct ether *ethptr);

/* in file e1000e_cntl.c */
extern 	devcall e1000e_cntl(struct ether *, int32, int32, int32);

/* in file e1000e_init.c */
extern 	void 	e1000e_init(struct ether *ethptr);

/* in file e1000e_intr.c */
extern  interrupt 	e1000e_intr(struct ether *ethptr);

/* in file e1000e_open.c */
extern 	status 	e1000e_open(struct ether *ethptr);

/* in file e1000e_read.c */
extern  devcall e1000e_read(struct ether *ethptr, void *buf, uint32 len);

/* in file e1000e_write.c */
extern 	devcall e1000e_write(struct ether *ethptr, void *buf, uint32 len);

/* in file i8255x_cntl.c */
extern 	devcall i8255x_cntl(struct ether *, int32, int32, int32);

/* in file i8255x_cmd.c  */
extern 	void 	i8255x_irq_enable(struct ether 	*ethptr);
extern 	void 	i8255x_irq_disable(struct ether *ethptr);

/* in file i8255x_init.c  */
extern  void    i8255x_init(struct ether *); 

/* in file i8255x_intr.c */
extern  interrupt 	i8255x_intr(struct ether *ethptr);

/* in file i8255x_rom.c */
extern unsigned short i8255x_romread(short , short);

/* in file i8255x_open.c */
extern status i8255x_open(struct ether *);

/* in file i8255x_utils.c */
extern byte i8255x_get_cu_status(struct ether *);
extern byte i8255x_get_ru_status(struct ether *);
extern status i8255x_exec_cu(struct ether *, uint32);
extern status i8255x_exec_ru(struct ether *, uint32);
extern status i8255x_resume_ru(struct ether *);
extern status i8255x_cmd_accept(struct ether *);
extern status i8255x_dump_stats(struct ether *);
extern status i8255x_dump_stats_reset(struct ether *);
extern uint32	i8255x_stats(struct ether *, uint32 , bool8);
extern uint32	i8255x_rx_count(struct ether *, bool8);
extern uint32	i8255x_tx_count(struct ether *, bool8);
extern status	i8255x_print_stats(struct ether *, bool8);

/* in file i8255x_write.c */
extern 	devcall i8255x_write(struct ether *, void *, uint32);

/* in file i8255x_read.c        */
extern devcall  i8255x_read(struct ether *, void *, uint32); 

/* in file ethControl.c */
extern	devcall	ethControl(struct dentry *, int32, int32, int32);

/* in file ethDispatch.S */
extern	interrupt	ethDispatch(void);

/* in file ethInit.c */
extern	devcall	ethInit(struct dentry *);

/* in file etherInterupt.c */
extern	interrupt	ethInterrupt(void);

/* in file ethRead.c */
extern	devcall ethRead(struct dentry *, void *, uint32);

/* in file ethStat.c */
extern	void	ethStat(uint16);

/* in file ethWrite.c */
extern	devcall	ethWrite(struct dentry *, void *, uint32);


/* in file evec.c */
extern	int32	initevec(void);
extern	int32	set_evec(uint32, uint32);
extern	void	trap(int32);

/* in file exception.c */
extern  void exception(int32, int32*);

/* in file freebuf.c */
extern	syscall	freebuf(char *);

/* in file freemem.c */
extern	syscall	freemem(char *, uint32);

/* in file getbuf.c */
extern	char	*getbuf(bpid32);

/* in file getc.c */
extern	syscall	getc(did32);

/* in file getitem.c */
extern	pid32	getfirst(qid16);

/* in file dhcp.c */
extern	uint32	getlocalip(void);

/* in file getmem.c */
extern	char	*getmem(uint32);

/* in file getpid.c */
extern	pid32	getpid(void);

/* in file getprio.c */
extern	syscall	getprio(pid32);

/* in file getstk.c */
extern	char	*getstk(uint32);

/* in file gettime.c */
extern	status	gettime(uint32 *);

/* in file getutime.c */
extern	status	getutime(uint32 *);

/* in file halt.S */
extern	void	halt(void);

/* in file icmp.c */
extern	void	icmp_init (void);
extern	status	icmp_in (void);
extern	process	icmp_out (void);
extern	int32	icmp_register (uint32);
extern	int32	icmp_recv (int32, char *, int32, uint32);
extern	status	icmp_send (uint32, uint16, uint16, uint16, char	*, int32);
extern	status	icmp_release (int32);
extern	uint16	icmp_cksum (char *, int32);
extern 	void 	icmp_ntoh(struct netpacket *);
extern 	void 	icmp_hton(struct netpacket *);


/* in file init.c */
extern	syscall	init(did32);

/* in file initialize.c */
extern	int32	sizmem(void);

/* in file insert.c */
extern	status	insert(pid32, qid16, int32);

/* in file insertd.c */
extern	status	insertd(pid32, qid16, int32);

/* in file intr.S */
extern	uint16	getirmask(void);

/* in file ioerr.c */
extern	devcall	ioerr(void);

/* in file ionull.c */
extern	devcall	ionull(void);

/* in file netin.c */
extern	uint16	ipcksum(struct netpacket *);
extern 	void 	eth_ntoh(struct netpacket *);
extern 	void 	eth_hton(struct netpacket *);
extern 	void 	ip_ntoh(struct netpacket *);
extern 	void 	ip_hton(struct netpacket *);


/* in file kill.c */
extern	syscall	kill(pid32);

/* in file lexan.c */
extern	int32	lexan(char *, int32, char *, int32 *, int32 [], int32 []);

/* in file lpgetc.c */
extern	devcall	lpgetc(struct dentry *);

/* in file lpinit.c */
extern	devcall	lpinit(struct dentry *);

/* in file lpopen.c */
extern	devcall	lpopen(struct dentry *, char *, char *);

/* in file lpputc.c */
extern	devcall	lpputc(struct dentry *, char);

/* in file lpread.c */
extern	devcall	lpread(struct dentry *, char *, int32);

/* in file lpwrite.c */
extern	devcall	lpwrite(struct dentry *, char *, int32);

/* in file mark.c */
extern	void	_mkinit(void);

/* in file memcpy.c */
extern	void	*memcpy(void *, const void *, int32);

/* in file memcpy.c */
extern	int32	*memcmp(void *, const void *, int32);

/* in file memset.c */
extern  void    *memset(void *, const int, int32);

/* in file mkbufpool.c */
extern	bpid32	mkbufpool(int32, int32);

/* in file mount.c */
extern	syscall	mount(char *, char *, did32);
extern	int32	namlen(char *, int32);

/* in file namInit.c */
extern	status	namInit(void);

/* in file nammap.c */
extern	devcall	nammap(char *, char[], did32);
extern	did32	namrepl(char *, char[]);
extern	status	namcpy(char *, char *, int32);

/* in file namOpen.c */
extern	devcall	namOpen(struct dentry *, char *, char *);

/* in file netin.c */
extern	process	netin(void);


/* in file newqueue.c */
extern	qid16	newqueue(void);

/* in file open.c */
extern	syscall	open(did32, char *, char *);

/* in file panic.c */
extern	void	panic(char *);

/* in file pci.c */
extern	int32	pci_init(void);

/* in file pdump.c */
extern	void	pdump(struct netpacket *);

/*in file printbytes.c*/
extern void printbytes(void *, uint32);

/* in file ptclear.c */
extern	void	_ptclear(struct ptentry *, uint16, int32);

/* in file ptcount.c */
extern	int32	ptcount(int32);

/* in file ptcreate.c */
extern	syscall	ptcreate(int32);

/* in file ptdelete.c */
extern	syscall	ptdelete(int32, int32);

/* in file ptinit.c */
extern	syscall	ptinit(int32);

/* in file ptrecv.c */
extern	uint32	ptrecv(int32);

/* in file ptreset.c */
extern	syscall	ptreset(int32, int32);

/* in file ptsend.c */
extern	syscall	ptsend(int32, umsg32);

/* in file putc.c */
extern	syscall	putc(did32, char);

/* in file rdsClose.c */
extern	devcall	rdsClose(struct dentry *);

/* in file rdsControl.c */
extern	devcall	rdsControl(struct dentry *, int32, int32, int32);

/* in file rdsInit.c */
extern	devcall	rdsInit(struct dentry *);

/* in file rdsOpen.c */
extern	devcall	rdsOpen(struct dentry *, char *, char *);

/* in file rdsRead.c */
extern	devcall	rdsRead(struct dentry *, char *, int32);

/* in file rdsWrite.c */
extern	devcall	rdsWrite(struct dentry *, char *, int32);

/* in file rdsbufalloc.c */
extern	struct	rdbuff * rdsbufalloc(struct rdscblk *);

/* in file rdscomm.c */
extern	status	rdscomm(struct rd_msg_hdr *, int32, struct rd_msg_hdr *,
		int32, struct rdscblk *);

/* in file rdsprocess.c */
extern	void	rdsprocess(struct rdscblk *);

/* in file read.c */
extern	syscall	read(did32, char *, uint32);

/* in file ready.c */
extern	status	ready(pid32, bool8);

/* in file receive.c */
extern	umsg32	receive(void);

/* in file recvclr.c */
extern	umsg32	recvclr(void);

/* in file recvtime.c */
extern	umsg32	recvtime(int32);

/* in file resched.c */
extern	void	resched(void);

/* in file intutils.S */
extern	void	restore(intmask);

/* in file resume.c */
extern	pri16	resume(pid32);

/* in file sched_cntl.c */
extern	status	sched_cntl(int32);

/* in file seek.c */
extern	syscall	seek(did32, uint32);

/* in file semcount.c */
extern	syscall	semcount(sid32);

/* in file semcreate.c */
extern	sid32	semcreate(int32);

/* in file semdelete.c */
extern	syscall	semdelete(sid32);

/* in file semreset.c */
extern	syscall	semreset(sid32, int32);

/* in file send.c */
extern	syscall	send(pid32, umsg32);

/* in file shell.c */
extern 	process shell(did32);

/* in file signal.c */
extern	syscall	signal(sid32);

/* in file signaln.c */
extern	syscall	signaln(sid32, int32);

/* in file sleep.c */
extern	syscall	sleepms(uint32);
extern	syscall	sleep(uint32);

/* in file start.S */
extern	int32	inb(int32);
extern	int32	inw(int32);
extern	int32	inl(int32);
extern	int32	outb(int32, int32);
extern	int32	outw(int32, int32);
extern	int32	outl(int32, int32);
extern	int32	outsw(int32, int32, int32);
extern	int32	insw(int32, int32 ,int32);

/* in file suspend.c */
extern	syscall	suspend(pid32);

/* in file ttyControl.c */
extern	devcall	ttyControl(struct dentry *, int32, int32, int32);

/* in file ttyDispatch.c */
extern	interrupt	ttyDispatch(void);

/* in file ttyGetc.c */
extern	devcall	ttyGetc(struct dentry *);

/* in file ttyInter_in.c */
extern	void	ttyInter_in(struct ttycblk *, struct uart_csreg *);

/* in file ttyInter_out.c */
extern	void	ttyInter_out(struct ttycblk *, struct uart_csreg *);

/* in file ttyInterrupt.c */
extern	void	ttyInterrupt(void);

/* in file ttyInit.c */
extern	devcall	ttyInit(struct dentry *);

/* in file ttyKickOut.c */
extern	void	ttyKickOut(struct ttycblk *, struct uart_csreg *);

/* in file ttyPutc.c */
extern	devcall	ttyPutc(struct dentry *, char);

/* in file ttyRead.c */
extern	devcall	ttyRead(struct dentry *, char *, int32);

/* in file ttyWrite.c */
extern	devcall	ttyWrite(struct dentry *, char *, int32);

/* in file udp.c */
extern  uint16  udpcksum(struct netpacket *);
extern	void	udp_in(void);
extern	void	udp_init(void);
extern	int32	udp_recv(uint32, uint16, uint16, char *, int32, uint32);
extern	int32	udp_recvaddr(uint32 *, uint16*, uint16, char *, int32, uint32);
extern	status	udp_register(uint32, uint16, uint16);
extern	status	udp_send(uint32, uint16, uint32, uint16, char *, int32);
extern	status	udp_release(uint32, uint16, uint16);
extern 	void 	udp_ntoh(struct netpacket *);
extern 	void 	udp_hton(struct netpacket *);



/* in file udpeserver.c */
extern	shellcmd  udpeserver (void);


/* in file unsleep.c */
extern	syscall	unsleep(pid32);

/* in file userret.c */
extern	void	userret(void);

/* in file wait.c */
extern	syscall	wait(sid32);

/* in file wakeup.c */
extern	void	wakeup(void);

/* in file write.c */
extern	syscall	write(did32, char *, uint32);

/* in file xdone.c */
extern	void	xdone(void);

/* in file yield.c */
extern	syscall	yield(void);

/* in stacktrace.c */
extern  syscall  stacktrace(int pid);

/* NETWORK BYTE ORDER CONVERSION NOT NEEDED ON A BIG-ENDIAN COMPUTER */
#define	htons(x)   ( ( 0xff & ((x)>>8) ) | ( (0xff & (x)) << 8 ) )
#define	htonl(x)   (  (((x)>>24) & 0x000000ff) | (((x)>> 8) & 0x0000ff00) | \
		      (((x)<< 8) & 0x00ff0000) | (((x)<<24) & 0xff000000) )
#define	ntohs(x)   ( ( 0xff & ((x)>>8) ) | ( (0xff & (x)) << 8 ) )
#define	ntohl(x)   (  (((x)>>24) & 0x000000ff) | (((x)>> 8) & 0x0000ff00) | \
		      (((x)<< 8) & 0x00ff0000) | (((x)<<24) & 0xff000000) )


/* Functions for virtual memory related stuff   */
extern  int psinit();
/* given calls for dealing with backing store */

extern syscall get_bs(bsd_t, unsigned int);
extern syscall release_bs(bsd_t);
extern syscall read_bs(char *, bsd_t, int);
extern syscall write_bs(char *, bsd_t, int);

/* Prototypes for required backing-store API */
extern bsd_t allocate_bs(unsigned int npages);
extern bsd_t deallocate_bs(bsd_t store);
extern bsd_t open_bs(bsd_t store);
extern bsd_t close_bs(bsd_t store);

/* Prototypes for new system calls required */
extern syscall xmmap(int, int, unsigned int, bsd_t);
extern syscall xmunmap(int);
extern syscall srpolicy(int);


