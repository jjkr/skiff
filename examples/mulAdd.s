	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_moduleMain
	.p2align	4, 0x90
_moduleMain:                            ## @moduleMain
	.cfi_startproc
## BB#0:
	xorl	%eax, %eax
	retq
	.cfi_endproc

	.globl	_mulAdd
	.p2align	4, 0x90
_mulAdd:                                ## @mulAdd
	.cfi_startproc
## BB#0:                                ## %entry
                                        ## kill: %EDX<def> %EDX<kill> %RDX<def>
                                        ## kill: %EDI<def> %EDI<kill> %RDI<def>
	imull	%esi, %edi
	leal	(%rdi,%rdx), %eax
	retq
	.cfi_endproc


.subsections_via_symbols
