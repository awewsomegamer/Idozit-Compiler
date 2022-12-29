	.file	"test.c"
	.intel_syntax noprefix
# GNU C17 (GCC) version 12.2.0 (x86_64-pc-linux-gnu)
#	compiled by GNU C version 12.2.0, GMP version 6.2.1, MPFR version 4.1.0-p13, MPC version 1.2.1, isl version isl-0.25-GMP

# warning: MPFR header version 4.1.0-p13 differs from library version 4.1.1-p1.
# warning: MPC header version 1.2.1 differs from library version 1.3.1.
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -masm=intel -mtune=generic -march=x86-64
	.text
	.section	.rodata
.LC0:
	.string	"%X\n"
.LC1:
	.string	"%f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	push	rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp	#,
	.cfi_def_cfa_register 6
# test.c:8:         printf("%X\n", do_the_float());
	mov	eax, 0	#,
	call	do_the_float@PLT	#
	movq	rax, xmm0	# _1,
	movq	xmm0, rax	#, _1
	lea	rax, .LC0[rip]	# tmp86,
	mov	rdi, rax	#, tmp86
	mov	eax, 1	#,
	call	printf@PLT	#
# test.c:9:         printf("%f\n", do_the_float());
	mov	eax, 0	#,
	call	do_the_float@PLT	#
	movq	rax, xmm0	# _2,
	movq	xmm0, rax	#, _2
	lea	rax, .LC1[rip]	# tmp87,
	mov	rdi, rax	#, tmp87
	mov	eax, 1	#,
	call	printf@PLT	#
# test.c:11:         a(15.0);
	mov	rax, QWORD PTR .LC2[rip]	# tmp88,
	movq	xmm0, rax	#, tmp88
	call	a	#
# test.c:13:         return 0;
	mov	eax, 0	# _9,
# test.c:14: }
	pop	rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.globl	a
	.type	a, @function
a:
.LFB7:
	.cfi_startproc
	push	rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp	#,
	.cfi_def_cfa_register 6
	movsd	QWORD PTR -8[rbp], xmm0	# b, b
# test.c:17:         return 5.1 + b;
	movsd	xmm1, QWORD PTR -8[rbp]	# tmp84, b
	movsd	xmm0, QWORD PTR .LC3[rip]	# tmp85,
	addsd	xmm0, xmm1	# _2, tmp84
	movq	rax, xmm0	# <retval>, _2
# test.c:18: }
	movq	xmm0, rax	#, <retval>
	pop	rbp	#
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE7:
	.size	a, .-a
	.section	.rodata
	.align 8
.LC2:
	.long	0
	.long	1076756480
	.align 8
.LC3:
	.long	1717986918
	.long	1075078758
	.ident	"GCC: (GNU) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
