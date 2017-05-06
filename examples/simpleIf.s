	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:                                ## %ifcont
	movl	$9, %eax
	retq
	.cfi_endproc


.subsections_via_symbols
