	.file	"asm.c"
	.comm	array,32768,32
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
.L2:
	movl	array(%rip)     , %eax
	movl	array+64(%rip)  , %eax
	movl	array+128(%rip) , %eax
	movl	array+192(%rip) , %eax
	movl	array+256(%rip) , %eax
	movl	array+320(%rip) , %eax
	movl	array+384(%rip) , %eax
	movl	array+448(%rip) , %eax
	movl	array+512(%rip) , %eax
	movl	array+576(%rip) , %eax
	movl	array+640(%rip) , %eax
	movl	array+704(%rip) , %eax
	movl	array+768(%rip) , %eax
	movl	array+832(%rip) , %eax
	movl	array+896(%rip) , %eax
	movl	array+960(%rip) , %eax
	movl	array+1024(%rip), %eax
	movl	array+1088(%rip), %eax
	movl	array+1152(%rip), %eax
	movl	array+1216(%rip), %eax
	movl	array+1280(%rip), %eax
	movl	array+1344(%rip), %eax
	movl	array+1408(%rip), %eax
	movl	array+1472(%rip), %eax
	movl	array+1536(%rip), %eax
	movl	array+1600(%rip), %eax
	movl	array+1664(%rip), %eax
	movl	array+1728(%rip), %eax
	movl	array+1792(%rip), %eax
	movl	array+1856(%rip), %eax
	movl	array+1920(%rip), %eax
	movl	array+1984(%rip), %eax
	movl	array+2048(%rip), %eax
	movl	array+2112(%rip), %eax
	movl	array+2176(%rip), %eax
	movl	array+2240(%rip), %eax
	movl	array+2304(%rip), %eax
	movl	array+2368(%rip), %eax
	movl	array+2432(%rip), %eax
	movl	array+2496(%rip), %eax
	movl	array+2560(%rip), %eax
	movl	array+2624(%rip), %eax
	movl	array+2688(%rip), %eax
	movl	array+2752(%rip), %eax
	movl	array+2816(%rip), %eax
	movl	array+2880(%rip), %eax
	movl	array+2944(%rip), %eax
	movl	array+3008(%rip), %eax
	movl	array+3072(%rip), %eax
	movl	array+3136(%rip), %eax
	movl	array+3200(%rip), %eax
	movl	array+3264(%rip), %eax
	movl	array+3328(%rip), %eax
	movl	array+3392(%rip), %eax
	movl	array+3456(%rip), %eax
	movl	array+3520(%rip), %eax
	movl	array+3584(%rip), %eax
	movl	array+3648(%rip), %eax
	movl	array+3712(%rip), %eax
	movl	array+3776(%rip), %eax
	movl	array+3840(%rip), %eax
	movl	array+3904(%rip), %eax
	movl	array+3968(%rip), %eax
	movl	array+4032(%rip), %eax
	movl	array+4096(%rip), %eax
	movl	array+4160(%rip), %eax
	movl	array+4224(%rip), %eax
	movl	array+4288(%rip), %eax
	movl	array+4352(%rip), %eax
	movl	array+4416(%rip), %eax
	movl	array+4480(%rip), %eax
	movl	array+4544(%rip), %eax
	movl	array+4608(%rip), %eax
	movl	array+4672(%rip), %eax
	movl	array+4736(%rip), %eax
	movl	array+4800(%rip), %eax
	movl	array+4864(%rip), %eax
	movl	array+4928(%rip), %eax
	movl	array+4992(%rip), %eax
	movl	array+5056(%rip), %eax
	movl	array+5120(%rip), %eax
	movl	array+5184(%rip), %eax
	movl	array+5248(%rip), %eax
	movl	array+5312(%rip), %eax
	movl	array+5376(%rip), %eax
	movl	array+5440(%rip), %eax
	movl	array+5504(%rip), %eax
	movl	array+5568(%rip), %eax
	movl	array+5632(%rip), %eax
	movl	array+5696(%rip), %eax
	movl	array+5760(%rip), %eax
	movl	array+5824(%rip), %eax
	movl	array+5888(%rip), %eax
	movl	array+5952(%rip), %eax
	movl	array+6016(%rip), %eax
	movl	array+6080(%rip), %eax
	movl	array+6144(%rip), %eax
	movl	array+6208(%rip), %eax
	movl	array+6272(%rip), %eax
	movl	array+6336(%rip), %eax
	movl	array+6400(%rip), %eax
	movl	array+6464(%rip), %eax
	movl	array+6528(%rip), %eax
	movl	array+6592(%rip), %eax
	movl	array+6656(%rip), %eax
	movl	array+6720(%rip), %eax
	movl	array+6784(%rip), %eax
	movl	array+6848(%rip), %eax
	movl	array+6912(%rip), %eax
	movl	array+6976(%rip), %eax
	movl	array+7040(%rip), %eax
	movl	array+7104(%rip), %eax
	movl	array+7168(%rip), %eax
	movl	array+7232(%rip), %eax
	movl	array+7296(%rip), %eax
	movl	array+7360(%rip), %eax
	movl	array+7424(%rip), %eax
	movl	array+7488(%rip), %eax
	movl	array+7552(%rip), %eax
	movl	array+7616(%rip), %eax
	movl	array+7680(%rip), %eax
	movl	array+7744(%rip), %eax
	movl	array+7808(%rip), %eax
	movl	array+7872(%rip), %eax
	movl	array+7936(%rip), %eax
	movl	array+8000(%rip), %eax
	movl	array+8064(%rip), %eax
	movl	array+8128(%rip), %eax
	jmp	.L2
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
