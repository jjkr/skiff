	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_moduleMain
	.p2align	4, 0x90
_moduleMain:                            ## @moduleMain
	.cfi_startproc
## BB#0:
	movl	$32, %eax
	retq
	.cfi_endproc

	.globl	_foo
	.p2align	4, 0x90
_foo:                                   ## @foo
	.cfi_startproc
## BB#0:                                ## %entry
	movl	$32, %eax
	retq
	.cfi_endproc


.subsections_via_symbols
