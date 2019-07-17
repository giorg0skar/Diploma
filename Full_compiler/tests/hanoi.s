	.text
	.file	"dana program"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rdi, 8(%rsp)
	movl	$.L__unnamed_1, %edi
	callq	writeString
	callq	readInteger
	movl	%eax, 16(%rsp)
	leaq	8(%rsp), %rdi
	movl	$.L__unnamed_2, %edx
	movl	$.L__unnamed_3, %ecx
	movl	$.L__unnamed_4, %r8d
	movl	%eax, %esi
	callq	hanoi
	addq	$24, %rsp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.globl	hanoi                   # -- Begin function hanoi
	.p2align	4, 0x90
	.type	hanoi,@function
hanoi:                                  # @hanoi
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movq	%rdi, (%rsp)
	movl	%esi, 8(%rsp)
	movq	%rdx, 16(%rsp)
	movq	%rcx, 24(%rsp)
	movq	%r8, 32(%rsp)
	testl	%esi, %esi
	jle	.LBB1_2
# %bb.1:                                # %then
	movq	(%rsp), %rdi
	movl	8(%rsp), %esi
	decl	%esi
	movq	16(%rsp), %rdx
	movq	32(%rsp), %rcx
	movq	24(%rsp), %r8
	callq	hanoi
	movq	16(%rsp), %rsi
	movq	24(%rsp), %rdx
	movq	%rsp, %rdi
	callq	move
	movq	(%rsp), %rdi
	movl	8(%rsp), %esi
	decl	%esi
	movq	32(%rsp), %rdx
	movq	24(%rsp), %rcx
	movq	16(%rsp), %r8
	callq	hanoi
.LBB1_2:                                # %return
	addq	$40, %rsp
	retq
.Lfunc_end1:
	.size	hanoi, .Lfunc_end1-hanoi
	.cfi_endproc
                                        # -- End function
	.globl	move                    # -- Begin function move
	.p2align	4, 0x90
	.type	move,@function
move:                                   # @move
	.cfi_startproc
# %bb.0:                                # %return
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rdi, (%rsp)
	movq	%rsi, 8(%rsp)
	movq	%rdx, 16(%rsp)
	movl	$.L__unnamed_5, %edi
	callq	writeString
	movq	8(%rsp), %rdi
	callq	writeString
	movl	$.L__unnamed_6, %edi
	callq	writeString
	movq	16(%rsp), %rdi
	callq	writeString
	movl	$.L__unnamed_7, %edi
	callq	writeString
	addq	$24, %rsp
	retq
.Lfunc_end2:
	.size	move, .Lfunc_end2-move
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_5,@object   # @0
	.section	.rodata,"a",@progbits
.L__unnamed_5:
	.asciz	"Moving from "
	.size	.L__unnamed_5, 13

	.type	.L__unnamed_6,@object   # @1
.L__unnamed_6:
	.asciz	" to "
	.size	.L__unnamed_6, 5

	.type	.L__unnamed_7,@object   # @2
.L__unnamed_7:
	.asciz	".\n"
	.size	.L__unnamed_7, 3

	.type	.L__unnamed_1,@object   # @3
.L__unnamed_1:
	.asciz	"Rings: "
	.size	.L__unnamed_1, 8

	.type	.L__unnamed_2,@object   # @4
.L__unnamed_2:
	.asciz	"left"
	.size	.L__unnamed_2, 5

	.type	.L__unnamed_3,@object   # @5
.L__unnamed_3:
	.asciz	"right"
	.size	.L__unnamed_3, 6

	.type	.L__unnamed_4,@object   # @6
.L__unnamed_4:
	.asciz	"middle"
	.size	.L__unnamed_4, 7


	.section	".note.GNU-stack","",@progbits
