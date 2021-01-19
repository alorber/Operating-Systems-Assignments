# Andrew Lorber
# Assembly file
# Updated for MacOS

.text				        # Declare we are in text region
.globl _tas			        # Declare tas to be a global symbol
_tas:                       # Label marking start of tas function
	pushq	%rbp            # Save base pointer on stack
	movq	%rsp, %rbp      # Establish new base pointer
	movq	$1, %rax        # Fetch arg#1 to edx
	lock;xchgb %al,(%rdi)   # The actual atomic instruction
	movsbq	%al,%rax	    # sign-extend result into rax
	pop	%rbp                # Restore base pointer
	ret				        # Return (retval in eax)
.Lfe1:
