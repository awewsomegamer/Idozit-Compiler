	.file	"library.c"
	.intel_syntax noprefix
	.text
	.section	.rodata
.LC0:
	.string	"End Of File"
.LC1:
	.string	"Addition"
.LC2:
	.string	"Subtraction"
.LC3:
	.string	"Multiplication"
.LC4:
	.string	"Division"
.LC5:
	.string	"Left Parenthesis"
.LC6:
	.string	"Exponent"
.LC7:
	.string	"Number"
.LC8:
	.string	"Variable"
.LC9:
	.string	"Identifier"
	.section	.data.rel.local,"aw"
	.align 32
	.type	TOKEN_NAMES, @object
	.size	TOKEN_NAMES, 96
TOKEN_NAMES:
	.quad	.LC0
	.quad	.LC1
	.quad	.LC2
	.quad	.LC3
	.quad	.LC4
	.quad	.LC5
	.quad	.LC5
	.quad	.LC6
	.zero	8
	.quad	.LC7
	.quad	.LC8
	.quad	.LC9
	.section	.rodata
.LC10:
	.string	"INTEGRAL"
.LC11:
	.string	"DERIVATIVE"
	.section	.data.rel.local
	.align 16
	.type	T_FUNC_NAMES, @object
	.size	T_FUNC_NAMES, 16
T_FUNC_NAMES:
	.quad	.LC10
	.quad	.LC11
	.section	.rodata
.LC12:
	.string	"FATAL ERROR"
.LC13:
	.string	"ERROR"
.LC14:
	.string	"WARNING"
.LC15:
	.string	"DEBUG"
	.section	.data.rel.local
	.align 32
	.type	MESSAGE_NAMES, @object
	.size	MESSAGE_NAMES, 32
MESSAGE_NAMES:
	.quad	.LC12
	.quad	.LC13
	.quad	.LC14
	.quad	.LC15
	.section	.rodata
.LC16:
	.string	"%s = %f\n"
	.text
	.globl	expression
	.type	expression, @function
expression:
.LFB6:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 208
	mov	QWORD PTR -200[rbp], rdi
	mov	QWORD PTR -168[rbp], rsi
	mov	QWORD PTR -160[rbp], rdx
	mov	QWORD PTR -152[rbp], rcx
	mov	QWORD PTR -144[rbp], r8
	mov	QWORD PTR -136[rbp], r9
	test	al, al
	je	.L4
	movaps	XMMWORD PTR -128[rbp], xmm0
	movaps	XMMWORD PTR -112[rbp], xmm1
	movaps	XMMWORD PTR -96[rbp], xmm2
	movaps	XMMWORD PTR -80[rbp], xmm3
	movaps	XMMWORD PTR -64[rbp], xmm4
	movaps	XMMWORD PTR -48[rbp], xmm5
	movaps	XMMWORD PTR -32[rbp], xmm6
	movaps	XMMWORD PTR -16[rbp], xmm7
.L4:
	mov	rax, QWORD PTR -200[rbp]
	mov	QWORD PTR expression_string[rip], rax
	mov	QWORD PTR variables_list[rip], 0
	mov	DWORD PTR variable_count[rip], 0
	mov	DWORD PTR expression_ptr[rip], 0
	mov	eax, 0
	call	build_tree@PLT
	mov	QWORD PTR -192[rbp], rax
	mov	rax, QWORD PTR -192[rbp]
	mov	rdi, rax
	call	validate@PLT
	mov	rax, QWORD PTR -192[rbp]
	mov	rdi, rax
	call	evaluate_tree@PLT
	movq	rax, xmm0
	mov	rdx, QWORD PTR -200[rbp]
	movq	xmm0, rax
	mov	rsi, rdx
	lea	rax, .LC16[rip]
	mov	rdi, rax
	mov	eax, 1
	call	printf@PLT
	mov	edi, 16
	call	malloc@PLT
	mov	QWORD PTR -184[rbp], rax
	mov	rax, QWORD PTR -184[rbp]
	mov	rdx, QWORD PTR -192[rbp]
	mov	QWORD PTR [rax], rdx
	mov	rax, QWORD PTR -184[rbp]
	mov	rdx, QWORD PTR 8[rax]
	mov	rax, QWORD PTR [rax]
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	expression, .-expression
	.globl	compile
	.type	compile, @function
compile:
.LFB7:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 48
	mov	QWORD PTR -24[rbp], rdi
	mov	rcx, rdx
	mov	rax, rsi
	mov	rdx, rdi
	mov	rdx, rcx
	mov	QWORD PTR -48[rbp], rax
	mov	QWORD PTR -40[rbp], rdx
	mov	rax, QWORD PTR fs:40
	mov	QWORD PTR -8[rbp], rax
	xor	eax, eax
	mov	rdx, QWORD PTR -48[rbp]
	mov	rax, QWORD PTR -24[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	default_x86_64_generator@PLT
	mov	rax, QWORD PTR -8[rbp]
	sub	rax, QWORD PTR fs:40
	je	.L7
	call	__stack_chk_fail@PLT
.L7:
	mov	rax, QWORD PTR -24[rbp]
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	compile, .-compile
	.section	.rodata
.LC17:
	.string	"%X\n"
	.text
	.globl	run
	.type	run, @function
run:
.LFB8:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 208
	mov	QWORD PTR -176[rbp], rdi
	mov	QWORD PTR -168[rbp], rsi
	mov	QWORD PTR -160[rbp], rdx
	mov	QWORD PTR -152[rbp], rcx
	mov	QWORD PTR -144[rbp], r8
	mov	QWORD PTR -136[rbp], r9
	test	al, al
	je	.L11
	movaps	XMMWORD PTR -128[rbp], xmm0
	movaps	XMMWORD PTR -112[rbp], xmm1
	movaps	XMMWORD PTR -96[rbp], xmm2
	movaps	XMMWORD PTR -80[rbp], xmm3
	movaps	XMMWORD PTR -64[rbp], xmm4
	movaps	XMMWORD PTR -48[rbp], xmm5
	movaps	XMMWORD PTR -32[rbp], xmm6
	movaps	XMMWORD PTR -16[rbp], xmm7
.L11:
	mov	rax, QWORD PTR 32[rbp]
	mov	r9d, 0
	mov	r8d, -1
	mov	ecx, 34
	mov	edx, 7
	mov	rsi, rax
	mov	edi, 0
	call	mmap@PLT
	mov	QWORD PTR -200[rbp], rax
	mov	rdx, QWORD PTR 32[rbp]
	mov	rcx, QWORD PTR 16[rbp]
	mov	rax, QWORD PTR -200[rbp]
	mov	rsi, rcx
	mov	rdi, rax
	call	memcpy@PLT
	mov	QWORD PTR -192[rbp], 0
	lea	rax, 24[rbp]
#APP
# 52 "src/library.c" 1
	push rcx;                 mov rcx, rax;
# 0 "" 2
#NO_APP
	mov	rax, QWORD PTR -192[rbp]
	mov	rsi, rax
	lea	rax, .LC17[rip]
	mov	rdi, rax
	mov	eax, 0
	call	printf@PLT
	mov	rax, QWORD PTR -200[rbp]
	call	rax
	movq	rax, xmm0
	mov	QWORD PTR -184[rbp], rax
	movsd	xmm0, QWORD PTR -184[rbp]
	movq	rax, xmm0
	movq	xmm0, rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	run, .-run
	.globl	set_message_handler
	.type	set_message_handler, @function
set_message_handler:
.LFB9:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	QWORD PTR -8[rbp], rdi
	mov	rax, QWORD PTR -8[rbp]
	mov	rdi, rax
	call	_set_message_handler@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	set_message_handler, .-set_message_handler
	.globl	default_message_handler
	.type	default_message_handler, @function
default_message_handler:
.LFB10:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	edi, 0
	call	_set_message_handler@PLT
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	default_message_handler, .-default_message_handler
	.ident	"GCC: (GNU) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
