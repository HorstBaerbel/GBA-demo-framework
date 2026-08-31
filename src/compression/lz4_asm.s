@ ARM v4/v5 LZ4 decompressor
@ Author: HorstBaerbel / https://github.com/HorstBaerbel
@ Improvements: aikku93 / https://github.com/Aikku93

#define LZ4_CONSTANTS_MIN_MATCH_LENGTH 4     // A match needs at least 3 bytes to encode, thus 4 is the minimum match length
#define LZ4_CONSTANTS_LITERAL_LENGTH_SHIFT 4 // Left-shift of literal length in token byte
#define LZ4_CONSTANTS_LENGTH_MASK 0x0F       // Used for masking literal and match lengths

@ #define USE_DMA3 @ Faster copying of long matches, but uses DMA3. Only useful for copying, not setting RLE runs

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
    @ r0: source pointer (will point to r0 + r4 on return)
    @ r1: destination pointer (will point to r1 + r4 on return)
    @ r4: number of bytes to copy (trashed)
    @ r6: DMA3 register address, if USE_DMA3 defined
    @ r7: DMA control, if USE_DMA3 defined
    @ ------------------------------
    @ In function:
    @ r5,r12 trashed
    @ RLE uses temporary src, so we can trash r0 if needed

    @ check if dst aligned to halfwords
    tst     r1, #1
    beq     .lz4_mc16_dst_halfword_aligned
    @ dst is odd, read-modify-write a halfword to update only the high byte
    ldrb    r5, [r1, #-1]!      @ r5 = dst[-1], r1 -= 1
    ldrb    r12, [r0], #1       @ r12 = src[0]
    subs    r4, #1
    orr     r5, r12, lsl #8     @ r5 = (src[0] << 8) | dst[-1]
    strh    r5, [r1], #2        @ write dst[-1], src[0] to dst
    bxeq    lr                  @ exit if r4 == 0
.lz4_mc16_dst_halfword_aligned:
    @ check how many bytes are left
    @ pre-decrement r4 by the minimum copy count to save some instructions in the copy loops
    subs    r4, #2
    blo     .lz4_mc16_tail_fixup_odd @ only 1 byte left

    @ >= 2 bytes left. check for an overlapping copy with distance == 1
    @ this happens when src and dst are one byte apart and is basically a RLE run
    sub     r5, r1, r0
    cmp     r5, #1
    beq     .lz4_mc16_repeat_byte

    @ check if src aligned to halfwords
    tst     r0, #1
    beq     .lz4_mc16_src_halfword_aligned
.lz4_mc16_src_halfword_unaligned:
    @ src unaligned so read bytes and assemble halfwords
    ldrb    r12, [r0], #1            @ pre-read 1 byte
    @ now src is halfword aligned. r4 is actually -2
1:  ldrh    r5, [r0], #2             @ copy 2 bytes, next byte to r12
    subs    r4, #2
    orr     r12, r5, lsl #8
    strh    r12, [r1], #2
    lsr     r12, r5, #8
    bhs     1b
    tst     r4, #1                   @ if we needed one more byte, we already have it in r12
    bne     .lz4_mc16_tail_fixup_odd_r12src_ready
    sub     r0, #1                   @ we have read one byte too much from src, so decrement src once
    bx      lr

.lz4_mc16_repeat_byte:
    @ overlapping RLE run with distance == 1 that repeats >= 2 bytes. r4 is actually -2
    @ keep repeat byte in r12 for storing the tail byte if needed
    ldrb    r12, [r0]                @ r12 = src[last]
    ands    r5, r1, #2               @ prepare for 4-byte loop
    add     r5, #4-2
    orr     r0, r12, r12, lsl #8     @ r0 = src[last] * 0x01010101
    orr     r0, r0, lsl #16
    strneh  r0, [r1], #2             @ copy 2 bytes to align to words as needed
    subs    r4, r5
1:  strhs   r0, [r1], #4             @ copy 4 bytes
    subhss  r4, #4
    bhs     1b
    lsls    r5, r4, #31              @ copy 2 bytes, then tail byte if needed
    strcsh  r0, [r1], #2
    bmi     .lz4_mc16_tail_fixup_odd_r12src_ready
    bx      lr

.lz4_mc16_src_halfword_aligned:
#ifdef USE_DMA3
    @ set up DMA3 to copy halfwords
    add     r5, r7, r4, lsr #1       @ r5 = DMA_COUNT((count-2)/2 + 1) | DMA_ENABLE
    stmia   r6, {r0, r1, r5}
    add     r0, r5, lsl #1           @ src += hwords*2
    add     r1, r5, lsl #1           @ dst += hwords*2
    tst     r4, #1                   @ copy tail byte as needed
#else
    @ because src and dst are hword aligned now, then
    @ the distance between them must be n*2 (n > 0).
    @ because we read/write in multiples of 4 bytes, we
    @ must check for all cases of dist that are < 4,
    @ which happens to only be dist == 2.
    lsrs    r5, #2
    beq     .lz4_mc16_repeat_hword
    @ if src and dst share word alignment, copy words directly
    bcc     .lz4_mc16_copy_normal_congruent

.lz4_mc16_copy_normal:
    @ src or dst unaligned so read hword and assemble words
    ands    r5, r0, #2
    ldrneh  r12, [r0], #2            @ pre-read 2 bytes, aligning to words as needed
    ldreqh  r12, [r1, #-2]!
    subs    r4, r5                   @ prepare for 4-byte loop
    blo     2f
1:  ldr     r5, [r0], #4             @ copy 4 bytes, next hword to r12
    subs    r4, #4
    orr     r12, r5, lsl #16
    str     r12, [r1], #4
    lsr     r12, r5, #16
    bhs     1b
2:  lsls    r5, r4, #31              @ copy 2 bytes, tail byte as needed
    strcsh  r12, [r1], #2
    subcc   r0, #2                   @ we have read 2 bytes too much from src, so decrement src
    bmi     .lz4_mc16_tail_fixup_odd
    bx      lr

.lz4_mc16_repeat_hword:
    @ overlapping RLE run with distance == 2 that repeats >= 2 bytes. r4 is actually -2
    ldrh    r12, [r0]                @ r12 = src[last] (hword)
    ands    r5, r1, #2               @ prepare for 4-byte loop
    add     r5, #4-2
    orr     r12, r12, lsl #16        @ r12 = src[last] * 0x00010001
    strneh  r12, [r1], #2            @ copy 2 bytes to align to words as needed
    subs    r4, r5
1:  strhs   r12, [r1], #4            @ copy 4 bytes
    subhss  r4, #4
    bhs     1b
    lsls    r5, r4, #31              @ copy 2 bytes, tail byte as needed
    strcsh  r12, [r1], #2
    bxpl    lr
    and     r12, #0xFF
    b       .lz4_mc16_tail_fixup_odd_r12src_ready

.lz4_mc16_copy_normal_congruent:
    ands    r5, r0, #2
    ldrneh  r12, [r0], #2            @ copy 2 bytes to align to words as needed
    add     r5, #4-2                 @ prepare for 4-byte loop
    strneh  r12, [r1], #2
    subs    r4, r5
#if (__ARM_ARCH >= 5)
    blo     2f
1:  ldr     r5, [r0], #4             @ copy 4 bytes
    subs    r4, #4
    str     r5, [r1], #4
    bhs     1b
#else
1:  ldrhs   r5, [r0], #4
    strhs   r5, [r1], #4
    subhss  r4, #4
    bhs     1b
#endif
2:  lsls    r5, r4, #31              @ copy 2 bytes, tail byte as needed
    ldrcsh  r5, [r0], #2
    strcsh  r5, [r1], #2
#endif // #ifdef USE_DMA3

@ read-modify-write to update only the low byte
@ relies on Z=0 flag to signal if last byte is needed
.lz4_mc16_tail_fixup_odd:
    ldrneb  r12, [r0], #1            @ r12 = src[last]
.lz4_mc16_tail_fixup_odd_r12src_ready:
    ldrneb  r5, [r1, #1]             @ r5 = dst[last+1]
#if (__ARM_ARCH >= 5)
    @ use stall cycle for early exit
    bxeq    lr
#endif
    orrne   r5, r12, r5, lsl #8      @ r5 = (dst[last+1] << 8) | src[last]
    strneh  r5, [r1], #1             @ write src[last], dst[last+1] to dst
    bx      lr

 .arm
 .align
 .global LZ4UnCompWrite8bit
 .global LZ4UnCompWrite16bit
 .type LZ4UnCompWrite8bit,function
 .type LZ4UnCompWrite16bit,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
LZ4UnCompWrite8bit:
LZ4UnCompWrite16bit:
    @ Decode LZ4 data
    @ ------------------------------
    @ Input:
    @ r0: pointer to LZ4 data, must be 4-byte-aligned (trashed)
    @ r1: pointer to destination buffer, must be 4-byte-aligned (trashed)
    @ ------------------------------
    @ In function:
    @ r2: end of decompressed data in destination buffer (past the last byte)
    @ r3,r12 trashed, r4,r5 used and saved / restored
    @ r6,r7 used and saved / restore, if USE_DMA3 defined

    @ read header word:
    @ Bit 0-7: Compressed type (40h for LZ4)
    @ Bit 8-31: Size of uncompressed data
    ldrb    r3, [r0], #1
#if (__ARM_ARCH >= 5)
    ldrb    r2, [r0], #1 @ pre-load to avoid stall cycle
#endif
    @ stop if this isn't LZ4
    cmp     r3, #0x40
    bne     .lz4_ucw_return0
    @ r2 = uncompressed size
#if (__ARM_ARCH < 5)
    ldrb    r2, [r0], #1
#endif
    ldrb    r3, [r0], #1
    ldrb    r12, [r0], #1
    orr     r2, r3, lsl #8
    orrs    r2, r12, lsl #16
    @ stop if uncompressed size is 0
    beq     .lz4_ucw_return0
    @ r2 = end of decompressed data (past the last byte)
    add     r2, r1
#ifdef USE_DMA3
    push    {r4 - r7, lr}
    @ r6 = DMA3 register address
    @ r7 = DMA_COUNT(1) | DMA_ENABLE
    mov     r6, #0x04000000
    add     r6, #0x000000D4
    mov     r7, #0x80000001
#else
    push    {r4 - r5, lr}
#endif
.lz4_ucw_decode_loop:
    @ r3 = token
    ldrb    r3, [r0], #1
    @ ----- literal decoding -----
    @ r4 = literal length
    lsrs    r4, r3, #LZ4_CONSTANTS_LITERAL_LENGTH_SHIFT
#if (LZ4_CONSTANTS_LENGTH_MASK != (0xFF >> LZ4_CONSTANTS_LITERAL_LENGTH_SHIFT))
    ands    r4, #LZ4_CONSTANTS_LENGTH_MASK
#endif
    beq     .lz4_ucw_literals_end
    @ read extra literal length if initial length == 15
    cmp     r4, #15
.lz4_ucw_read_literals_length:
    ldreqb  r5, [r0], #1
    addeq   r4, r4, r5
    cmpeq   r5, #255
    beq     .lz4_ucw_read_literals_length
    @ now copy literals
    bl      LZ4_MemCopy16
.lz4_ucw_literals_end:
    @ ----- match decoding -----
    @ r4 = match length
    ands    r4, r3, #LZ4_CONSTANTS_LENGTH_MASK
    beq     .lz4_ucw_match_end
    @ r5,r12 = 16-bit match offset
    ldrb    r12, [r0], #1
    ldrb    r5, [r0], #1
    @ read extra match length if initial length == 15
    cmp     r4, #15
.lz4_ucw_read_match_length:
    ldreqb  r3, [r0], #1
#if (__ARM_ARCH >= 5)
    orr     r5, r5, r12, lsl #8 @ use stall cycle to assemble 16-bit match offset
#endif
    addeq   r4, r4, r3
    cmpeq   r3, #255
    beq     .lz4_ucw_read_match_length
    @ now copy match
#if (__ARM_ARCH < 5)
    orr     r5, r5, r12, lsl #8 @ assemble 16-bit match offset
#endif
    add     r4, #LZ4_CONSTANTS_MIN_MATCH_LENGTH - 1
    mov     r3, r0 @ save r0
    sub     r0, r1, r5 @ src pointer = dst pointer - match offset
    bl      LZ4_MemCopy16
    mov     r0, r3 @ restore r0
.lz4_ucw_match_end:
    cmp     r1, r2 @ still data left to decompress?
    blo     .lz4_ucw_decode_loop
#ifdef USE_DMA3
    pop     {r4 - r7, lr}
#else
    pop     {r4 - r5, lr}
#endif
    bx      lr
.lz4_ucw_return0:
    mov     r0, #0
    bx      lr

.arm
 .align
 .global LZ4UnCompGetSize
 .type LZ4UnCompGetSize,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
LZ4UnCompGetSize:
    @ Get uncompressed size from LZ4 data
    @ r0: pointer to LZ4 data, must be 4-byte-aligned (trashed)
    @ r1,r2,r3 are scratch registers (trashed)

    @ read header word:
    @ Bit 0-7: Compressed type (40h for LZ4)
    @ Bit 8-31: Size of uncompressed data
    ldrb    r3, [r0, #0]
#if (__ARM_ARCH >= 5)
    ldrb    r2, [r0, #1] @ pre-load to avoid stall cycle
#endif
    @ stop if this isn't LZ4 and return 0
    cmp     r3, #0x40
    @ read uncompressed size
#if (__ARM_ARCH < 5)
    ldreqb  r2, [r0, #1]
#endif
    ldreqb  r3, [r0, #2]
    ldreqb  r1, [r0, #3]
    orreq   r0, r2, r3, lsl #8
    orreq   r0, r1, lsl #16
    movne   r0, #0
    bx      lr