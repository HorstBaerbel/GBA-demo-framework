#define LZ4_CONSTANTS_MIN_MATCH_LENGTH 4     // A match needs at least 3 bytes to encode, thus 4 is the minimum match length
#define LZ4_CONSTANTS_LITERAL_LENGTH_SHIFT 4 // Left-shift of literal length in token byte
#define LZ4_CONSTANTS_LENGTH_MASK 0x0F       // Used for masking literal and match lengths
 
 #define LZ4_OVERRUN_PROTECTION // If turned off can overrun the buffer max. 3 bytes. Very minor performance impact.

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
    @ ------------------------------
    @ In function:
    @ r5-r7 trashed

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
    @ check how many bytes are left
    cmp     r4, #2
    blo     .lz4_mc16_tail_write_byte @ only [0,1] byte left
    @ >= 2 bytes left. check for an overlapping copy with a distance <= 2
    subs    r5, r1, r0  @ r5 = r1 - r0
    rsbmi   r5, r5, #0 @ r5 = (r1 - r0) < 0 ? 0-r5 : r5
    cmp     r5, #2
    blt     .lz4_mc16_repeat_byte     @ if |r1 - r0| < 2, do repeating byte copy
    @ check if src aligned to halfwords
    tst     r0, #1
    beq     .lz4_mc16_src_halfword_aligned_loop
    @ src unaligned so read bytes and assemble halfwords
    ldrb    r5, [r0], #1 @ pre-read 1 byte
.lz4_mc16_src_unaligned_loop:
    @ now src is halfword aligned
    ldrh    r6, [r0], #2
    orr     r5, r5, r6, lsl #8
    strh    r5, [r1], #2
    mov     r5, r5, lsr #16
    sub     r4, r4, #2
    cmp     r4, #2
    bhs     .lz4_mc16_src_unaligned_loop
    sub     r0, r0, #1 @ we have read one byte too much from src, so decrement src once
    b       .lz4_mc16_tail_write_byte
.lz4_mc16_src_halfword_aligned_loop:
    @ src halfword aligned, so copy halfwords
    ldrh    r5, [r0], #2
    strh    r5, [r1], #2
    sub     r4, r4, #2
    cmp     r4, #2
    bhs     .lz4_mc16_src_halfword_aligned_loop
    b       .lz4_mc16_tail_write_byte
.lz4_mc16_repeat_byte:
    @ overlapping copy with distance == 1 that repeats >= 2 times. the value is in r6 already
    ldrb    r6, [r0], #1       @ r6 = src[0]
    orr     r6, r6, lsl #8
.lz4_mc16_repeat_loop:
    strh    r6, [r1], #2
    sub     r4, r4, #2
    add     r0, r0, #2
    cmp     r4, #2
    bhs     .lz4_mc16_repeat_loop
.lz4_mc16_tail_write_byte:
    @ check if we have a byte left to copy
    cmp     r4, #1
    @ r4 == 1, dst halfword aligned, so we read-modify-write a word to update only the low byte
    ldreqb  r5, [r1, #1]        @ r4 == 1, r5 = dst[last+1]
    ldreqb  r6, [r0], #1        @ r4 == 1, r6 = src[last]
    orreq   r5, r6, r5, lsl #8  @ r4 == 1, r5 = (dst[last+1] << 8) | src[last]
    streqh  r5, [r1], #1        @ r4 == 1, write src[last], dst[last+1] to dst
    bx      lr

 .arm
 .align
 .global LZ4_MemCopy
 .type LZ4_MemCopy,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
LZ4_MemCopy:
    @ Copy data from r0 to r1, possibly in 4, 2 or 1 byte chunks
    @ ------------------------------
    @ Input:
    @ r0: source pointer (will point to r0 + r4)
    @ r1: destination pointer (will point to r1 + r4)
    @ r4: number of bytes to copy (trashed)
    @ ------------------------------
    @ In function:
    @ r5-r6 trashed
    
    @ if we're copying less than 8 bytes, just do a byte copy
    cmp r4, #8
    blo .lz4_mc_byte_loop
    @ check if both pointers have the same offset in regards to word alignment, so 0/0, 1/1, 2/2 or 3/3
    and r5, r0, #3
    and r6, r1, #3
    cmp r5, r6
    beq .lz4_mc_word_aligned
    @ check if both pointers have the same offset in regards to halfword alignment, so 0/2, 2/0, 1/3 or 3/1
    and r5, r0, #1
    and r6, r1, #1
    cmp r5, r6
    beq .lz4_mc_halfword_aligned
.lz4_mc_byte_loop:
    ldrb r5, [r0], #1
    strb r5, [r1], #1
    subs r4, r4, #1
    bne .lz4_mc_byte_loop
    bx lr
.lz4_mc_halfword_aligned:
    @ check for an overlapping copy with a distance smaller than 2
    subs r5, r1, r0 @ r5 = r1 - r0
    rsbmi r5, r5, #0 @ r5 = (r1 - r0) < 0 ? 0-r5 : r5
    cmp r5, #2
    blt .lz4_mc_byte_loop @ if |r1 -r0| < 2, do byte copy
    @ check how many bytes we must pre-copy for halfword alignment
    @ offset == 1: pre-copy 1 byte
    cmp r6, #1
    subeq r4, r4, #1 @ offset from halfword == 1, copy length -= 1
    ldreqb r5, [r0], #1 @ offset from halfword == 1, do byte pre-copy
    streqb r5, [r1], #1
.lz4_mc_halfword_loop: @ here we have halfword alignment
    ldrh r5, [r0], #2
    strh r5, [r1], #2
#ifdef LZ4_OVERRUN_PROTECTION
    sub r4, r4, #2
    cmp r4, #2
    bhs .lz4_mc_halfword_loop
    tst r4, #1 @ check for byte tail
    ldrneb r5, [r0], #1 @ bytes left > 0, do byte post-copy
    strneb r5, [r1], #1
#else
    subs r4, #2
    bgt .lz4_mc_halfword_loop
    adds r0, r4
    adds r1, r4
#endif
    bx lr
.lz4_mc_word_aligned:
    @ check for an overlapping copy with a distance smaller than 4
    subs r5, r1, r0 @ r5 = r1 - r0
    rsbmi r5, r5, #0 @ r5 = (r1 - r0) < 0 ? 0-r5 : r5
    cmp r5, #4
    blt .lz4_mc_byte_loop @ if |r1 -r0| < 4, do byte copy
    @ check how many bytes we must pre-copy for word alignment
    @ offset == 1: pre-copy 1 byte + 1 halfword
    @ offset == 2: pre-copy 1 halfword
    @ offset == 3: pre-copy 1 byte
    cmp r6, #0
    beq .lz4_mc_word_loop @ offset from word == 0, do word copy
    cmp r6, #2
    ldrneb r5, [r0], #1 @ offset from word == 1 or 3, do byte pre-copy
    strneb r5, [r1], #1
    subne r4, r4, #1 @ offset from word == 1 or 3, copy length -= 1
    ldrlsh r5, [r0], #2 @ offset from word <= 2, do halfword pre-copy
    strlsh r5, [r1], #2
    subls r4, r4, #2 @ offset from word <= 2, copy length -= 2
.lz4_mc_word_loop:
    ldr r5, [r0], #4
    str r5, [r1], #4
#ifdef LZ4_OVERRUN_PROTECTION
    sub r4, r4, #4
    cmp r4, #4
    bhs .lz4_mc_word_loop
    tst r4, #2 @ check for halfword tail
    ldrneh r5, [r0], #2 @ bytes left >= 2, do halfword tail copy
    strneh r5, [r1], #2
    tst r4, #1 @ check for byte tail
    ldrneb r5, [r0], #1 @ bytes left > 0, do byte tail copy
    strneb r5, [r1], #1
#else
    subs r4, #4
    bgt .lz4_mc_word_loop
    adds r0, r4
    adds r1, r4
#endif
    bx lr


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
    push {r4 - r6, lr}

    @ read header word:
    @ Bit 0-7: Compressed type (40h for LZ4)
    @ Bit 8-31: Size of uncompressed data
    ldrb r2, [r0, #0]
    @ stop if this isn't LZ4
    cmp r2, #0x40
    bne .lz4_ucw8_end

    @ r2 = uncompressed size
    ldrb r2, [r0, #1]
    ldrb r3, [r0, #2]
    lsl r3, r3, #8
    orr r2, r2, r3
    ldrb r3, [r0, #3]
    lsl r3, r3, #16
    orrs r2, r2, r3
    @ stop if uncompressed size is 0
    beq .lz4_ucw8_end

    @ move input pointer past header
    add r0, r0, #4
.lz4_ucw8_decode_loop:
    @ r3 = token
    ldrb r3, [r0], #1
    @ ----- literal decoding -----
    @ r4 = literal length
    mov r4, r3, lsr #LZ4_CONSTANTS_LITERAL_LENGTH_SHIFT
    ands r4, r4, #LZ4_CONSTANTS_LENGTH_MASK
    beq .lz4_ucw8_literals_end
    @ read extra literal length if initial length == 15
    cmp r4, #15
    blo .lz4_ucw8_copy_literals
.lz4_ucw8_read_literals_length:
    ldrb r5, [r0], #1
    add r4, r4, r5
    cmp r5, #255
    beq .lz4_ucw8_read_literals_length
.lz4_ucw8_copy_literals:
    sub r2, r2, r4 @ uncompressed size -= length of literals
    bl LZ4_MemCopy16
.lz4_ucw8_literals_end:
    @ ----- match decoding -----
    @ r4 = match length
    ands r4, r3, #LZ4_CONSTANTS_LENGTH_MASK
    beq .lz4_ucw8_match_end
    @ r6 = 16-bit match offset
    ldrb r6, [r0], #1
    ldrb r5, [r0], #1
    orr r6, r5, r6, lsl #8
    @ read extra match length if initial length == 15
    cmp r4, #15
    blo .lz4_ucw8_copy_match
.lz4_ucw8_read_match_length:
    ldrb r5, [r0], #1
    add r4, r4, r5
    cmp r5, #255
    beq .lz4_ucw8_read_match_length
.lz4_ucw8_copy_match:
    add r4, r4, #LZ4_CONSTANTS_MIN_MATCH_LENGTH - 1
    sub r2, r2, r4 @ uncompressed size -= length of match
    mov r3, r0 @ save r0
    sub r0, r1, r6 @ src pointer = dst pointer - match offset
    bl LZ4_MemCopy16
    mov r0, r3 @ restore r0
.lz4_ucw8_match_end:
    cmp r2, #0 @ still data left to decompress?
    bne .lz4_ucw8_decode_loop
.lz4_ucw8_end:
    pop {r4 - r6, lr}
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
