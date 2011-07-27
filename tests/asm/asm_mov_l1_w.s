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
	movl	%eax, array(%rip)     
	movl	%eax, array+64(%rip)  
	movl	%eax, array+128(%rip) 
	movl	%eax, array+192(%rip) 
	movl	%eax, array+256(%rip) 
	movl	%eax, array+320(%rip) 
	movl	%eax, array+384(%rip) 
	movl	%eax, array+448(%rip) 
	movl	%eax, array+512(%rip) 
	movl	%eax, array+576(%rip) 
	movl	%eax, array+640(%rip) 
	movl	%eax, array+704(%rip) 
	movl	%eax, array+768(%rip) 
	movl	%eax, array+832(%rip) 
	movl	%eax, array+896(%rip) 
	movl	%eax, array+960(%rip) 
	movl	%eax, array+1024(%rip)
	movl	%eax, array+1088(%rip)
	movl	%eax, array+1152(%rip)
	movl	%eax, array+1216(%rip)
	movl	%eax, array+1280(%rip)
	movl	%eax, array+1344(%rip)
	movl	%eax, array+1408(%rip)
	movl	%eax, array+1472(%rip)
	movl	%eax, array+1536(%rip)
	movl	%eax, array+1600(%rip)
	movl	%eax, array+1664(%rip)
	movl	%eax, array+1728(%rip)
	movl	%eax, array+1792(%rip)
	movl	%eax, array+1856(%rip)
	movl	%eax, array+1920(%rip)
	movl	%eax, array+1984(%rip)
	movl	%eax, array+2048(%rip)
	movl	%eax, array+2112(%rip)
	movl	%eax, array+2176(%rip)
	movl	%eax, array+2240(%rip)
	movl	%eax, array+2304(%rip)
	movl	%eax, array+2368(%rip)
	movl	%eax, array+2432(%rip)
	movl	%eax, array+2496(%rip)
	movl	%eax, array+2560(%rip)
	movl	%eax, array+2624(%rip)
	movl	%eax, array+2688(%rip)
	movl	%eax, array+2752(%rip)
	movl	%eax, array+2816(%rip)
	movl	%eax, array+2880(%rip)
	movl	%eax, array+2944(%rip)
	movl	%eax, array+3008(%rip)
	movl	%eax, array+3072(%rip)
	movl	%eax, array+3136(%rip)
	movl	%eax, array+3200(%rip)
	movl	%eax, array+3264(%rip)
	movl	%eax, array+3328(%rip)
	movl	%eax, array+3392(%rip)
	movl	%eax, array+3456(%rip)
	movl	%eax, array+3520(%rip)
	movl	%eax, array+3584(%rip)
	movl	%eax, array+3648(%rip)
	movl	%eax, array+3712(%rip)
	movl	%eax, array+3776(%rip)
	movl	%eax, array+3840(%rip)
	movl	%eax, array+3904(%rip)
	movl	%eax, array+3968(%rip)
	movl	%eax, array+4032(%rip)
	movl	%eax, array+4096(%rip)
	movl	%eax, array+4160(%rip)
	movl	%eax, array+4224(%rip)
	movl	%eax, array+4288(%rip)
	movl	%eax, array+4352(%rip)
	movl	%eax, array+4416(%rip)
	movl	%eax, array+4480(%rip)
	movl	%eax, array+4544(%rip)
	movl	%eax, array+4608(%rip)
	movl	%eax, array+4672(%rip)
	movl	%eax, array+4736(%rip)
	movl	%eax, array+4800(%rip)
	movl	%eax, array+4864(%rip)
	movl	%eax, array+4928(%rip)
	movl	%eax, array+4992(%rip)
	movl	%eax, array+5056(%rip)
	movl	%eax, array+5120(%rip)
	movl	%eax, array+5184(%rip)
	movl	%eax, array+5248(%rip)
	movl	%eax, array+5312(%rip)
	movl	%eax, array+5376(%rip)
	movl	%eax, array+5440(%rip)
	movl	%eax, array+5504(%rip)
	movl	%eax, array+5568(%rip)
	movl	%eax, array+5632(%rip)
	movl	%eax, array+5696(%rip)
	movl	%eax, array+5760(%rip)
	movl	%eax, array+5824(%rip)
	movl	%eax, array+5888(%rip)
	movl	%eax, array+5952(%rip)
	movl	%eax, array+6016(%rip)
	movl	%eax, array+6080(%rip)
	movl	%eax, array+6144(%rip)
	movl	%eax, array+6208(%rip)
	movl	%eax, array+6272(%rip)
	movl	%eax, array+6336(%rip)
	movl	%eax, array+6400(%rip)
	movl	%eax, array+6464(%rip)
	movl	%eax, array+6528(%rip)
	movl	%eax, array+6592(%rip)
	movl	%eax, array+6656(%rip)
	movl	%eax, array+6720(%rip)
	movl	%eax, array+6784(%rip)
	movl	%eax, array+6848(%rip)
	movl	%eax, array+6912(%rip)
	movl	%eax, array+6976(%rip)
	movl	%eax, array+7040(%rip)
	movl	%eax, array+7104(%rip)
	movl	%eax, array+7168(%rip)
	movl	%eax, array+7232(%rip)
	movl	%eax, array+7296(%rip)
	movl	%eax, array+7360(%rip)
	movl	%eax, array+7424(%rip)
	movl	%eax, array+7488(%rip)
	movl	%eax, array+7552(%rip)
	movl	%eax, array+7616(%rip)
	movl	%eax, array+7680(%rip)
	movl	%eax, array+7744(%rip)
	movl	%eax, array+7808(%rip)
	movl	%eax, array+7872(%rip)
	movl	%eax, array+7936(%rip)
	movl	%eax, array+8000(%rip)
	movl	%eax, array+8064(%rip)
	movl	%eax, array+8128(%rip)
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
