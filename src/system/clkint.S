/* clkint.s - _clkint */

#include <icu.s>
		.text
count1000:	.word	1000
		.globl	sltop
		.globl	clkint
		.globl	rpl_dao_timeout
		.globl	rpl_dis_timeout
clkint:
		pushal
		cli
		movb	$EOI,%al
		outb	%al,$OCW1_2

		incl	ctr1000
		subw	$1,count1000
		ja	cl1
		incl	clktime
		movw	$1000,count1000
cl1:
                movl    rpl_dis_timeout, %eax
                decl    %eax 
                movl     %eax, rpl_dis_timeout
                jg      pr_dao_tout 
                call    rpl_process_dis_timeout
pr_dao_tout:    movl    rpl_dao_timeout, %eax
                decl    %eax 
                movl     %eax, rpl_dao_timeout
                jg      clwakeup
                call    rpl_process_path_timeout
clwakeup:       cmpl	$0,slnonempty	# if no sleeping processes,
		je	clpreem		#   skip to preemption check
                movl	sltop,%eax	# decrement key of first
		decl	(%eax)		#   sleeping process
		jg	clpreem		# must use jg for signed int
		call	wakeup		# if zero, call wakeup
clpreem:	decl	preempt		# decrement preemption counter
		jg	clret		# must use jg for signed int
		call	resched		# if preemption, call resched
clret:					# return from interrupt
		sti
		popal
		iret
