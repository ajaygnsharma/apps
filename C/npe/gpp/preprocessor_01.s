	.file	"preprocessor_01.c"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$0, 24(%esp)
	movl	$8, 20(%esp)
	movl	$0, 16(%esp)
	movl	$0, 12(%esp)
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	movl	$0, (%esp)
	call	i16WritePCF8562
	leave
	ret
	.size	main, .-main
.globl i16WritePCF8562
	.type	i16WritePCF8562, @function
i16WritePCF8562:
	pushl	%ebp
	movl	%esp, %ebp
	cmpl	$8, 28(%ebp)
	je	.L4
	movl	$-1, %eax
	jmp	.L5
.L4:
	movl	$0, %eax
.L5:
	popl	%ebp
	ret
	.size	i16WritePCF8562, .-i16WritePCF8562
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",@progbits
