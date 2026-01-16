#define LZ4_CONSTANTS_MIN_MATCH_LENGTH 4     // A match needs at least 3 bytes to encode, thus 4 is the minimum match length
#define LZ4_CONSTANTS_LITERAL_LENGTH_SHIFT 4 // Left-shift of literal length in token byte
#define LZ4_CONSTANTS_LENGTH_MASK 0x0F       // Used for masking literal and match lengths

//#define USE_DMA3 @ Faster copying of long matches, but uses DMA3. Only useful for copying, not setting RLE runs

.arm
 .align
 .global LZ4_MemCopy16
 .type LZ4_MemCopy16,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
LZ4_MemCopy16:
    @ Copy data from r0 to r1, possibly in 4, 2 byte chunks or
    @ by read-modify-writing halfwords to be VRAM-safe
    @ ------------------------------
    @ Input:
    @ r0: source pointer (will point to r0 + r4)
    @ r1: destination pointer (will point to r1 + r4)
    @ r4: number of bytes to copy (trashed)
    @ r7: DMA3 register address, if USE_DMA3 defined
    @ ------------------------------
    @ In function:
    @ r5-r6 trashed

    @ check if dst aligned to halfwords
    tst     r1, #1
    beq     .lz4_mc16_dst_halfword_aligned
    @ dst is odd, read-modify-write a halfword to update only the high byte
    sub     r1, r1, #1
    ldrb    r5, [r1]           @ r5 = dst[-1] 
    ldrb    r6, [r0], #1       @ r6 = src[0]
    orr     r5, r5, r6, lsl #8 @ r5 = (src[0] << 8) | dst[-1]
    strh    r5, [r1], #2       @ write dst[-1], src[0] to dst
    subs    r4, r4, #1
    bxeq    lr @ exit if r4 == 0
.lz4_mc16_dst_halfword_aligned:
    @ check how many bytes are left. pre-decrement r4 by the minimum copy count to save some instructions in the copy loops
    subs     r4, r4, #2
    bmi     .lz4_mc16_tail_fixup_r4 @ only [0,1] bytes left
    @ >= 2 bytes left. check for an overlapping copy with distance <= 2. this happens when src and dst are in the same memory area and is basically a RLE run
    subs    r5, r1, r0            @ r5 = r1 - r0
    rsbmi   r5, r5, #0            @ r5 = (r1 - r0) < 0 ? (0 - r5) : r5
    cmp     r5, #2                @ |r1 - r0| < 2?
    blt     .lz4_mc16_repeat_byte @ if |r1 - r0| < 2, do repeating byte copy
    @ check if src aligned to halfwords
    tst     r0, #1
    beq     .lz4_mc16_src_halfword_aligned
    @ src unaligned so read bytes and assemble halfwords
    ldrb    r5, [r0], #1 @ pre-read 1 byte
.lz4_mc16_src_unaligned_loop:
    @ now src is halfword aligned. r4 is actually -2
    ldrh    r6, [r0], #2
    orr     r5, r5, r6, lsl #8
    strh    r5, [r1], #2
    mov     r5, r5, lsr #16
    subs    r4, r4, #2
    bpl     .lz4_mc16_src_unaligned_loop
    sub     r0, r0, #1 @ we have read one byte too much from src, so decrement src once
    b       .lz4_mc16_tail_fixup_r4
.lz4_mc16_src_halfword_aligned:
#ifdef USE_DMA3
    @ set up DMA3 to copy halfwords
    mov     r6, r4, lsr #1
    add     r6, #1
    subs    r4, r6, lsl #1
    mov     r5, r6
    orr     r6, #0x80000000 @ enable DMA
    stmia   r7, {r0, r1, r6}
    add     r0, r5, lsl #1
    add     r1, r5, lsl #1
    b       .lz4_mc16_tail_fixup_r4
#else
    @ src halfword aligned, so copy halfwords. r4 is actually -2
    cmp     r5, #4                              @ |r1 - r0| < 4?
    blt     .lz4_mc16_src_halfword_aligned_loop @ if |r1 - r0| < 4, copy halfwords
.lz4_mc16_src_halfword_aligned_loop8:
    cmp     r4, #8
    blo     .lz4_mc16_src_halfword_aligned_loop
    ldrh    r5, [r0], #2
    ldrh    r6, [r0], #2
    strh    r5, [r1], #2
    strh    r6, [r1], #2
    ldrh    r5, [r0], #2
    ldrh    r6, [r0], #2
    strh    r5, [r1], #2
    strh    r6, [r1], #2
    sub     r4, r4, #8
    b      .lz4_mc16_src_halfword_aligned_loop8
.lz4_mc16_src_halfword_aligned_loop:
    ldrh    r5, [r0], #2
    strh    r5, [r1], #2
    subs    r4, r4, #2
    bpl     .lz4_mc16_src_halfword_aligned_loop
    b       .lz4_mc16_tail_fixup_r4
#endif
.lz4_mc16_repeat_byte:
    @ overlapping RLE run with distance == 1 that repeats >= 2 times. r4 is actually -2
    ldrb    r6, [r0], #1   @ r6 = src[last]
    orr     r6, r6, lsl #8 @ r6 = (src[last] << 8) | src[last]
    add     r0, r0, r4 @ we will not be reading src, so increment it here
.lz4_mc16_repeat_loop:
    strh    r6, [r1], #2
    subs    r4, r4, #2
    bpl     .lz4_mc16_repeat_loop
.lz4_mc16_tail_fixup_r4:
    adds    r4, r4, #2 @ post-incement r4, because we decremented it earlier and set flags
    @ if r4 > 0, dst halfword aligned, so we read-modify-write a word to update only the low byte
    ldrhib  r5, [r1, #1]        @ r4 == 1, r5 = dst[last+1]
    ldrhib  r6, [r0], #1        @ r4 == 1, r6 = src[last]
    orrhi   r5, r6, r5, lsl #8  @ r4 == 1, r5 = (dst[last+1] << 8) | src[last]
    strhih  r5, [r1], #1        @ r4 == 1, write src[last], dst[last+1] to dst
    bx      lr

 .arm
 .align
 .global LZ4UnCompWrite8bit_ASM
 .global LZ4UnCompWrite16bit_ASM
 .type LZ4UnCompWrite8bit_ASM,function
 .type LZ4UnCompWrite16bit_ASM,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
LZ4UnCompWrite8bit_ASM:
LZ4UnCompWrite16bit_ASM:
    @ Decode LZ4 data
    @ ------------------------------
    @ Input:
    @ r0: pointer to LZ4 data, must be 4-byte-aligned (trashed)
    @ r1: pointer to destination buffer, must be 4-byte-aligned (trashed)
    @ ------------------------------
    @ In function:
    @ r2: size of uncompressed data (trashed)
    @ r3 trashed, r4-r6 used and saved / restored
    @ r7 used and saved / restore, if USE_DMA3 defined
#ifdef USE_DMA3
    push {r4 - r7, lr}
#else
    push {r4 - r6, lr}
#endif

    @ read header word:
    @ Bit 0-7: Compressed type (40h for LZ4)
    @ Bit 8-31: Size of uncompressed data
    ldrb r2, [r0, #0]
    @ stop if this isn't LZ4
    cmp r2, #0x40
    bne .lz4_ucw_end

    @ r2 = uncompressed size
    ldrb r2, [r0, #1]
    ldrb r3, [r0, #2]
    lsl r3, r3, #8
    orr r2, r2, r3
    ldrb r3, [r0, #3]
    lsl r3, r3, #16
    orrs r2, r2, r3
    @ stop if uncompressed size is 0
    beq .lz4_ucw_end
#ifdef USE_DMA3
    @ r7 = DMA3 register address
    ldr r7, =#0x040000D4
#endif
    @ move input pointer past header
    add r0, r0, #4
.lz4_ucw_decode_loop:
    @ r3 = token
    ldrb r3, [r0], #1
    @ ----- literal decoding -----
    @ r4 = literal length
    mov r4, r3, lsr #LZ4_CONSTANTS_LITERAL_LENGTH_SHIFT
    ands r4, r4, #LZ4_CONSTANTS_LENGTH_MASK
    beq .lz4_ucw_literals_end
    @ read extra literal length if initial length == 15
    cmp r4, #15
    blo .lz4_ucw_copy_literals
.lz4_ucw_read_literals_length:
    ldrb r5, [r0], #1
    add r4, r4, r5
    cmp r5, #255
    beq .lz4_ucw_read_literals_length
.lz4_ucw_copy_literals:
    sub r2, r2, r4 @ uncompressed size -= length of literals
    mov lr, pc
    bl LZ4_MemCopy16
.lz4_ucw_literals_end:
    @ ----- match decoding -----
    @ r4 = match length
    ands r4, r3, #LZ4_CONSTANTS_LENGTH_MASK
    beq .lz4_ucw_match_end
    @ r6 = 16-bit match offset
    ldrb r6, [r0], #1
    ldrb r5, [r0], #1
    orr r6, r5, r6, lsl #8
    @ read extra match length if initial length == 15
    cmp r4, #15
    blo .lz4_ucw_copy_match
.lz4_ucw_read_match_length:
    ldrb r5, [r0], #1
    add r4, r4, r5
    cmp r5, #255
    beq .lz4_ucw_read_match_length
.lz4_ucw_copy_match:
    add r4, r4, #LZ4_CONSTANTS_MIN_MATCH_LENGTH - 1
    sub r2, r2, r4 @ uncompressed size -= length of match
    mov r3, r0 @ save r0
    sub r0, r1, r6 @ src pointer = dst pointer - match offset
    mov lr, pc
    bl LZ4_MemCopy16
    mov r0, r3 @ restore r0
.lz4_ucw_match_end:
    cmp r2, #0 @ still data left to decompress?
    bne .lz4_ucw_decode_loop
.lz4_ucw_end:
#ifdef USE_DMA3
    pop {r4 - r7, lr}
#else
    pop {r4 - r6, lr}
#endif
    bx lr

.arm
 .align
 .global LZ4UnCompGetSize_ASM
 .type LZ4UnCompGetSize_ASM,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
LZ4UnCompGetSize_ASM:
    @ Get uncompressed size from LZ4 data
    @ r0: pointer to LZ4 data, must be 4-byte-aligned (trashed)
    @ r2,r3 are scratch registers (trashed)

    @ read header word:
    @ Bit 0-7: Compressed type (40h for LZ4)
    @ Bit 8-31: Size of uncompressed data
    ldrb r2, [r0, #0]
    @ stop if this isn't LZ4 and return 0
    cmp r2, #0x40
    movne r0, #0
    bne .lz4_ugs_end
    @ read uncompressed size
    ldrb r2, [r0, #1]
    ldrb r3, [r0, #2]
    lsl r3, r3, #8
    orr r2, r2, r3
    ldrb r3, [r0, #3]
    add r0, r0, #4
    lsl r3, r3, #16
    orrs r2, r2, r3
    mov r0, r2
.lz4_ugs_end:
    bx lr
