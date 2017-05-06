	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	pushq	%rax
Lcfi0:
	.cfi_def_cfa_offset 16
	movl	$5, %edi
	callq	_recurse
	popq	%rcx
	retq
	.cfi_endproc

	.globl	_recurse
	.p2align	4, 0x90
_recurse:                               ## @recurse
	.cfi_startproc
## BB#0:                                ## %entry
	pushq	%rax
Lcfi1:
	.cfi_def_cfa_offset 16
	callq	_recurse
	popq	%rcx
	retq
	.cfi_endproc


.subsections_via_symbols
