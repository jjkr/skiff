	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	xorl	%eax, %eax
	retq
	.cfi_endproc

	.globl	_addFive
	.p2align	4, 0x90
_addFive:                               ## @addFive
	.cfi_startproc
## BB#0:                                ## %entry
                                        ## kill: %EDI<def> %EDI<kill> %RDI<def>
	leal	5(%rdi), %eax
	retq
	.cfi_endproc


.subsections_via_symbols
