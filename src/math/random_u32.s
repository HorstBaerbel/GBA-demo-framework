.section .iwram, "ax", %progbits
.cpu arm7tdmi
.arm
.align 2

.global random_u32
.type random_u32_arm STT_FUNC
@ === uint32_t random_u32(void) IWRAM_FUNC; =============
@ Based on the random number generator by Dooby
random_u32:
	ldr	a3, =random_seed    @ Point at seeds
	ldmia	a3, {a1, a2}    @ Load them
	tst	a2, a2, lsr #1      @ Top bit into carry
	movs	a4, a1, rrx     @ 33 bit rotate right
	adc	a2, a2, a2          @ Carry into lsb of Rb
	eor	a4, a4, a1, lsl #12	@ (involved!)
	eor	a1, a4, a4, lsr #20	@ (similarly involved!)
	stmia	a3, {a1, a2}    @ Use these next time
	bx	lr

.global srandom
.type srandom_arm STT_FUNC
@ === void srandom(uint32_t seed) IWRAM_FUNC; =============
@ Seed random number generator
srandom:
	ldr	r1, =random_seed @ Point at seeds
	str	r0, [r1, #2]     @ Store seed in the middle
	bx	lr

.pool

random_seed:
	.word	0x159A55A0
	.word	0x29A65EAD @ Some values from the KISS generator

.end
