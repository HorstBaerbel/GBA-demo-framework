.section .iwram, "ax", %progbits
.global sqrt
.arm
.cpu arm7tdmi
.align 2 
@ === int32_t sqrt(int32_t number) IWRAM_CODE; =============
sqrt:
	stmfd	sp!, {v1, lr}
	mov	lr, a1			@ Copy input somewhere.
	mov	a2, #1
sqrt1:
	cmp	a1, a2
	movhi	a1, a1, lsr #1
	movhi	a2, a2, lsl #1
	bhi	sqrt1
sqrt4:
	mov	a1, lr			@ Pull input back.
	mov	v1, a2
	mov	a4, #0
	mov	a3, a2
sqrt2:
	cmp	a3, a1, lsr #1
	movls	a3, a3, lsl #1
	bcc	sqrt2
sqrt3:
	cmp	a1, a3
	adc	a4, a4, a4
	subcs	a1, a1, a3
	teq	a3, a2
	movne	a3, a3, lsr #1
	bne	sqrt3
	add	a2, a2, a4
	movs	a2, a2, lsr #1
	cmp	a2, v1
	bcc	sqrt4
	mov	a1, v1
	ldmfd	sp!, {v1, lr}
	bx	lr

	.pool
.end

