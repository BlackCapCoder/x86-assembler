	.file	"main.cpp"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	", "
	.text
	.globl	_Z9printIntsiii
	.type	_Z9printIntsiii, @function
_Z9printIntsiii:
.LFB2481:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	leaq	-4128(%rsp), %rsp
	orq	$0, (%rsp)
	leaq	4128(%rsp), %rsp
	movl	%esi, %r12d
	movl	%edx, %ebp
	movl	%edi, %esi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZNSolsEi@PLT
	movq	%rax, %rbx
	movl	$2, %edx
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	%r12d, %esi
	movq	%rbx, %rdi
	call	_ZNSolsEi@PLT
	movq	%rax, %rbx
	movl	$2, %edx
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	%ebp, %esi
	movq	%rbx, %rdi
	call	_ZNSolsEi@PLT
	movq	%rax, %rbx
	movq	(%rax), %rax
	movq	-24(%rax), %rax
	movq	240(%rbx,%rax), %rbp
	testq	%rbp, %rbp
	je	.L6
	cmpb	$0, 56(%rbp)
	je	.L3
	movzbl	67(%rbp), %esi
.L4:
	movsbl	%sil, %esi
	movq	%rbx, %rdi
	call	_ZNSo3putEc@PLT
	movq	%rax, %rdi
	call	_ZNSo5flushEv@PLT
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
.L6:
	.cfi_restore_state
	call	_ZSt16__throw_bad_castv@PLT
.L3:
	movq	%rbp, %rdi
	call	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	0(%rbp), %rax
	movl	$10, %esi
	movq	%rbp, %rdi
	call	*48(%rax)
	movl	%eax, %esi
	jmp	.L4
	.cfi_endproc
.LFE2481:
	.size	_Z9printIntsiii, .-_Z9printIntsiii
	.globl	main
	.type	main, @function
main:
.LFB2482:
	.cfi_startproc
	leaq	-4136(%rsp), %rsp
	orq	$0, (%rsp)
	leaq	4128(%rsp), %rsp
	.cfi_def_cfa_offset 16
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	_Z9printIntsiii
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	_Z9printIntsiii
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	_Z9printIntsiii
	movl	$0, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE2482:
	.size	main, .-main
	.type	_GLOBAL__sub_I__Z9printIntsiii, @function
_GLOBAL__sub_I__Z9printIntsiii:
.LFB3209:
	.cfi_startproc
	leaq	-4136(%rsp), %rsp
	orq	$0, (%rsp)
	leaq	4128(%rsp), %rsp
	.cfi_def_cfa_offset 16
	leaq	_ZStL8__ioinit(%rip), %rdi
	call	_ZNSt8ios_base4InitC1Ev@PLT
	leaq	__dso_handle(%rip), %rdx
	leaq	_ZStL8__ioinit(%rip), %rsi
	movq	_ZNSt8ios_base4InitD1Ev@GOTPCREL(%rip), %rdi
	call	__cxa_atexit@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE3209:
	.size	_GLOBAL__sub_I__Z9printIntsiii, .-_GLOBAL__sub_I__Z9printIntsiii
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I__Z9printIntsiii
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.hidden	__dso_handle
	.ident	"GCC: (GNU) 9.1.0"
	.section	.note.GNU-stack,"",@progbits
