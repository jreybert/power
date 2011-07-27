	.file	"asm.c"
	.comm	array,1048576,32
	.text
	.globl	loop
	.type	loop, @function
loop:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
.L4:
	movl	$511, -4(%rbp)
	jmp	.L2
.L3:
	movl	-4(%rbp), %eax
	sall	$6, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	cltq
	movl	array(,%rax,4), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	addl	$8192, %eax
	cltq
	movl	array(,%rax,4), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	addl	$16384, %eax
	cltq
	movl	array(,%rax,4), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	addl	$24576, %eax
	cltq
	movl	array(,%rax,4), %eax
	movl	%eax, -12(%rbp)
	subl	$1, -4(%rbp)
.L2:
	cmpl	$0, -4(%rbp)
	jns	.L3
	jmp	.L4
	.cfi_endproc
.LFE0:
	.size	loop, .-loop
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, %eax
	call	loop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Debian 4.6.1-1) 4.6.1"
	.section	.note.GNU-stack,"",@progbits
