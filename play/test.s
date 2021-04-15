	.file	"test.c"
	.text
	.section	.rodata
	.type	_ZStL19piecewise_construct, @object
	.size	_ZStL19piecewise_construct, 1
_ZStL19piecewise_construct:
	.zero	1
	.text
	.globl	main
	.type	main, @function
main:
.LFB899:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	leaq	-4144(%rsp), %rsp
	orq	$0, (%rsp)
	leaq	4128(%rsp), %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movb	$1, -10(%rbp)
	movb	$2, -9(%rbp)
	leaq	-9(%rbp), %rdx
	leaq	-10(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L3
	call	__stack_chk_fail@PLT
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE899:
	.size	main, .-main
	.section	.text._ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_,"axG",@progbits,_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_,comdat
	.weak	_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_
	.type	_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_, @function
_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_:
.LFB901:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE901:
	.size	_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_, .-_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_
	.section	.text._ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_,"axG",@progbits,_ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_,comdat
	.weak	_ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_
	.type	_ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_, @function
_ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_:
.LFB900:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	leaq	-4160(%rsp), %rsp
	orq	$0, (%rsp)
	leaq	4128(%rsp), %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_
	movzbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_
	movzbl	(%rax), %edx
	movq	-24(%rbp), %rax
	movb	%dl, (%rax)
	leaq	-9(%rbp), %rax
	movq	%rax, %rdi
	call	_ZSt4moveIRcEONSt16remove_referenceIT_E4typeEOS2_
	movzbl	(%rax), %edx
	movq	-32(%rbp), %rax
	movb	%dl, (%rax)
	nop
	movq	-8(%rbp), %rax
	xorq	%fs:40, %rax
	je	.L7
	call	__stack_chk_fail@PLT
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE900:
	.size	_ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_, .-_ZSt4swapIcENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS4_ESt18is_move_assignableIS4_EEE5valueEvE4typeERS4_SE_
	.ident	"GCC: (GNU) 9.1.0"
	.section	.note.GNU-stack,"",@progbits
