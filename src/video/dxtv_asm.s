#include "dxtv_constants.h"
#include "dxtv_structs.h"

 .global DXT_C2_ModeHalf_5bit
 .global DXT_C2C3_ModeThird_5bit
 .global DXT_BlockColors

 .arm
 .align
 .global DecodeBlock4x4
 .type DecodeBlock4x4,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
DecodeBlock4x4:
    @ Decode a 4x4 block of DXTV data
    @ r0: pointer to DXTV source data, must be 2-byte-aligned (will point to the next DXTV data block on return)
    @ r1: pointer to the destination pixel buffer, must be 4-byte-aligned (trashed)
    @ r2: pointer to the previous pixel buffer, must be 4-byte-aligned (trashed)
    @ r3: line stride in bytes of previous buffer (remains unchanged)
    @ r4: line stride in bytes of destination buffer (remains unchanged)
    @ r5, r6, r7, r8, r9: scratch registers (trashed)
    ldrh r5, [r0], #2 @ load block type / color 0
    ands r6, r5, #DXTV_CONSTANTS_BLOCK_IS_REF @ check if block is a motion compensated or DXT block
    beq .db4x4_dxt
.db4x4_mc:
    // ----- copy motion compensated block -----
    ands r6, r5, #DXTV_CONSTANTS_BLOCK_FROM_PREV @ check if block is from the previous frame
    moveq r2, r1 @ r2 = same frame (r1) or previous frame (r2)
    mov r9, r3 @ r9 = r3
    moveq r9, r4 @ r9 = same frame (r4) or previous frame (r3)
    // get x-offset of the source block
    and r6, r5, #DXTV_CONSTANTS_BLOCK_MOTION_MASK
    sub r6, r6, #DXTV_CONSTANTS_BLOCK_HALF_RANGE
    mov r6, r6, lsl #1 @ multiply x-offset by 2, because pixels are 2 bytes wide
    // get y-offset of the source block
    mov r5, r5, lsr #DXTV_CONSTANTS_BLOCK_MOTION_Y_SHIFT
    and r5, r5, #DXTV_CONSTANTS_BLOCK_MOTION_MASK
    sub r5, r5, #DXTV_CONSTANTS_BLOCK_HALF_RANGE
    // calculate offset to source block
    mla r7, r5, r9, r6 @ multiply y-offset by line stride and add x-offset
    add r2, r2, r7 @ calculate source address
    // check if block is word-aligned
    ands r5, r2, #0x03 @ check if source pointer is aligned to 4 bytes / 2 pixels
    beq .mc4x4_aligned
.mc4x4_half_aligned:
    // half-word aligned block copy
    // line 0
    ldrh r5, [r2, #0]        @ r2 is half-word aligned, read pixel 0
    ldr  r6, [r2, #2]        @ r2+2 is now word-aligned, read pixel 1,2
    ldrh r7, [r2, #6]        @ read pixel 3
    orr  r5, r5, r6, lsl #16 @ recombine pixel 0,1
    str  r5, [r1, #0]        @ r1 is always word-aligned, write pixel 0,1
    mov  r6, r6, lsr #16     @ recombine pixel 2,3
    orr  r6, r6, r7, lsl #16 @ recombine pixel 2,3
    str  r6, [r1, #4]        @ write pixel 2,3
    // line 1-3
    .rept 3
    ldrh r5, [r2, r9]!       @ pre-indexed advance r2
    ldr  r6, [r2, #2]
    ldrh r7, [r2, #6]
    orr  r5, r5, r6, lsl #16
    str  r5, [r1, r4]!       @ pre-indexed advance r1
    mov  r6, r6, lsr #16
    orr  r6, r6, r7, lsl #16
    str  r6, [r1, #4]
    .endr
    bx lr
.mc4x4_aligned:
    // word-aligned block copy
    // line 0-2
    .rept 3
    ldmia r2, {r5, r6}
    stmia r1, {r5, r6}
    add r2, r2, r9
    add r1, r1, r4
    .endr
    // line 3
    ldmia r2, {r5, r6}
    stmia r1, {r5, r6}
    bx lr
.db4x4_dxt:
    // ----- uncompress DXT block -----
    ldrh r6, [r0], #2 @ load c1 to r6
    // get blue components of c0 and c1 to r7
    and r2, r5, #0x7c00
    and r9, r6, #0x7c00
    mov r2, r2, lsr #5
    orr r7, r2, r9, lsr #10
    // get green components of c0 and c1 to r8
    and r2, r5, #0x03E0
    and r9, r6, #0x03E0
    orr r8, r2, r9, lsr #5
    // get red components of c0 and c1 to r9
    and r2, r5, #0x001F
    and r9, r6, #0x001F
    orr r9, r9, r2, lsl #5
    // check which intermediate colors mode we're using
    cmp r5, r6
    bgt .dxt4x4_third
.dxt4x4_half:
    // calculate intermediate color c2 at 1/2 of c0 and c1 with rounding and set c3 to black
    ldr r2, =DXT_C2_ModeHalf_5bit
    ldrb r7, [r2, r7]
    ldrb r8, [r2, r8]
    ldrb r9, [r2, r9]
    orr r7, r9, r7, lsl #10
    orr r7, r7, r8, lsl #5
    mov r8, #0x0000 @ r8 is c3, which is always black
    b .dxt4x4_store_colors
.dxt4x4_third:
    // calculate intermediate colors c2 and c3 at 1/3 and 2/3 of c0 and c1
    ldr r2, =DXT_C2C3_ModeThird_5bit
    ldr r7, [r2, r7, lsl #2]
    ldr r8, [r2, r8, lsl #2]
    ldr r9, [r2, r9, lsl #2]
    orr r8, r9, r8, lsl #5
    orr r8, r8, r7, lsl #10
    mov r7, r8, lsl #16 @ move c3 from the lower part of r8 to r7
    mov r7, r7, lsr #16
    mov r8, r8, lsr #16 @ move c2 to the lower part of r8
.dxt4x4_store_colors:
    // we now have c0, c1, c2 and c3 in r5, r6, r7 and r8. store them in the LUT
    ldr r2, =DXT_BlockColors
    strh r5, [r2, #0] @ store c0
    strh r6, [r2, #2] @ store c1
    strh r7, [r2, #4] @ store c2
    strh r8, [r2, #6] @ store c3
.dxt4x4_decode:
    mov r9, #0x06 @ = index mask 0x03 << 1 because of halfword-indexing for ldrh
    // line 0
    ldrh r5, [r0], #2 @ load indices to r5
    and r6, r9, r5, lsl #1
    and r7, r9, r5, lsr #1
    ldrh r6, [r2, r6]
    ldrh r7, [r2, r7]
    orr r6, r6, r7, lsl #16
    and r7, r9, r5, lsr #3
    and r8, r9, r5, lsr #5
    ldrh r7, [r2, r7]
    ldrh r8, [r2, r8]
    orr r7, r7, r8, lsl #16
    stmia r1, {r6, r7}
    add r1, r1, r4 @ increment destination pointer by line stride
    // line 1
    and r6, r9, r5, lsr #7
    and r7, r9, r5, lsr #9
    ldrh r6, [r2, r6]
    ldrh r7, [r2, r7]
    orr r6, r6, r7, lsl #16
    and r7, r9, r5, lsr #11
    and r8, r9, r5, lsr #13
    ldrh r7, [r2, r7]
    ldrh r8, [r2, r8]
    orr r7, r7, r8, lsl #16
    stmia r1, {r6, r7}
    add r1, r1, r4 @ increment destination pointer by line stride
    // line 2
    ldrh r5, [r0], #2 @ load indices to r5
    and r6, r9, r5, lsl #1
    and r7, r9, r5, lsr #1
    ldrh r6, [r2, r6]
    ldrh r7, [r2, r7]
    orr r6, r6, r7, lsl #16
    and r7, r9, r5, lsr #3
    and r8, r9, r5, lsr #5
    ldrh r7, [r2, r7]
    ldrh r8, [r2, r8]
    orr r7, r7, r8, lsl #16
    stmia r1, {r6, r7}
    add r1, r1, r4 @ increment destination pointer by line stride
    // line 3
    and r6, r9, r5, lsr #7
    and r7, r9, r5, lsr #9
    ldrh r6, [r2, r6]
    ldrh r7, [r2, r7]
    orr r6, r6, r7, lsl #16
    and r7, r9, r5, lsr #11
    and r8, r9, r5, lsr #13
    ldrh r7, [r2, r7]
    ldrh r8, [r2, r8]
    orr r7, r7, r8, lsl #16
    stmia r1, {r6, r7}
    bx lr

.arm
 .align
 .global DecodeBlock8x8
 .type DecodeBlock8x8,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
DecodeBlock8x8:
    @ Decode a 8x8 block of DXTV data
    @ r0: pointer to DXTV source data, must be 2-byte-aligned (will point to the next DXTV data block on return)
    @ r1: pointer to the destination pixel buffer, must be 4-byte-aligned (trashed)
    @ r2: pointer to the previous pixel buffer, must be 4-byte-aligned (trashed)
    @ r3: line stride in bytes of previous buffer (remains unchanged)
    @ r4: line stride in bytes of destination buffer (remains unchanged)
    @ r5, r6, r7, r8, r9, r10, r11: scratch registers (trashed)
    ldrh r5, [r0], #2 @ load block type / color 0
    ands r6, r5, #DXTV_CONSTANTS_BLOCK_IS_REF @ check if block is a motion compensated or DXT block
    beq .db8x8_dxt
.db8x8_mc:
    // ----- copy motion compensated block -----
    ands r6, r5, #DXTV_CONSTANTS_BLOCK_FROM_PREV @ check if block is from the previous frame
    moveq r2, r1 @ r2 = same frame (r1) or previous frame (r2)
    mov r11, r3
    moveq r11, r4 @ r11 = same frame (r4) or previous frame (r3)
    // get x-offset of the source block
    and r6, r5, #DXTV_CONSTANTS_BLOCK_MOTION_MASK
    sub r6, r6, #DXTV_CONSTANTS_BLOCK_HALF_RANGE
    mov r6, r6, lsl #1 @ multiply x-offset by 2, because pixels are 2 bytes wide
    // get y-offset of the source block
    mov r5, r5, lsr #DXTV_CONSTANTS_BLOCK_MOTION_Y_SHIFT
    and r5, r5, #DXTV_CONSTANTS_BLOCK_MOTION_MASK
    sub r5, r5, #DXTV_CONSTANTS_BLOCK_HALF_RANGE
    // calculate offset to source block
    mla r7, r5, r11, r6 @ multiply y-offset by line stride and add x-offset
    add r2, r2, r7 @ calculate source address
    // check if block is word-aligned
    ands r5, r2, #0x03 @ check if source pointer is aligned to 4 bytes / 2 pixels
    beq .mc8x8_aligned
.mc8x8_half_aligned:
    // half-word aligned block copy
    mov r10, #8 @ number of lines to copy
    subs r2, r2, r11 @ adjust source pointer for the first line
.mc8x8_half_aligned_loop:
    ldrh r5, [r2, r11]! @ r5 = hw0 (bits 15:0)
    ldr r6, [r2, #2] @ load 3 consecutive aligned words into r6, r7, r8
    ldr r7, [r2, #6]
    ldr r8, [r2, #10]
    ldrh r9, [r2, #14] @ r9 = hw7 (bits 15:0)
    orr r5, r5, r6, lsl #16 @ r5 now holds (hw1 << 16) | hw0 (word0)
    mov r6, r6, lsr #16     @ r6 now holds hw2
    orr r6, r6, r7, lsl #16 @ r6 now holds (hw3 << 16) | hw2 (word1)
    mov r7, r7, lsr #16     @ r7 now holds hw4
    orr r7, r7, r8, lsl #16 @ r7 now holds (hw5 << 16) | hw4 (word2)
    mov r8, r8, lsr #16     @ r8 now holds hw6
    orr r8, r8, r9, lsl #16 @ r8 now holds (hw7 << 16) | hw6 (word3)
    stmia r1, {r5 - r8} @ store the 4 reassembled words from to destination
    add r1, r1, r4 @ increment destination pointer by line stride
    subs r10, r10, #1 @ decrement line counter
    bne .mc8x8_half_aligned_loop @ repeat for the next line
    bx lr
.mc8x8_aligned:
    // word-aligned block copy
    .rept 7
    ldmia r2, {r5 - r8}
    stmia r1, {r5 - r8}
    add r2, r2, r11
    add r1, r1, r4
    .endr
    ldmia r2, {r5 - r8}
    stmia r1, {r5 - r8}
    bx lr
.db8x8_dxt:
    // ----- uncompress DXT block -----
    ldrh r6, [r0], #2 @ load c1 to r6
    // get blue components of c0 and c1 to r7
    and r2, r5, #0x7c00
    and r9, r6, #0x7c00
    mov r2, r2, lsr #5
    orr r7, r2, r9, lsr #10
    // get green components of c0 and c1 to r8
    and r2, r5, #0x03E0
    and r9, r6, #0x03E0
    orr r8, r2, r9, lsr #5
    // get red components of c0 and c1 to r9
    and r2, r5, #0x001F
    and r9, r6, #0x001F
    orr r9, r9, r2, lsl #5
    // check which intermediate colors mode we're using
    cmp r5, r6
    bgt .dxt8x8_third
.dxt8x8_half:
    // calculate intermediate color c2 at 1/2 of c0 and c1 with rounding and set c3 to black
    ldr r2, =DXT_C2_ModeHalf_5bit
    ldrb r7, [r2, r7]
    ldrb r8, [r2, r8]
    ldrb r9, [r2, r9]
    orr r7, r9, r7, lsl #10
    orr r7, r7, r8, lsl #5
    mov r8, #0x0 @ r8 is c3, which is always black
    b .dxt8x8_store_colors
.dxt8x8_third:
    // calculate intermediate colors c2 and c3 at 1/3 and 2/3 of c0 and c1
    ldr r2, =DXT_C2C3_ModeThird_5bit
    ldr r7, [r2, r7, lsl #2]
    ldr r8, [r2, r8, lsl #2]
    ldr r9, [r2, r9, lsl #2]
    orr r8, r9, r8, lsl #5
    orr r8, r8, r7, lsl #10
    mov r7, r8, lsl #16 @ move c3 from the lower part of r8 to r7
    mov r7, r7, lsr #16
    mov r8, r8, lsr #16 @ move c2 to the lower part of r8
.dxt8x8_store_colors:
    // we now have c0, c1, c2 and c3 in r5, r6, r7 and r8. store them in the LUT
    push {r12}
    ldr   r2, =DXT_BlockColors
    @ --- Block 0: High Color = c0 (r5) ---
    mov   r12, r5, lsl #16
    orr   r9,  r12, r5          @ (c0 << 16) | c0  [Index 0x0]
    orr   r10, r12, r6          @ (c0 << 16) | c1  [Index 0x1]
    orr   r11, r12, r7          @ (c0 << 16) | c2  [Index 0x2]
    orr   r12, r12, r8          @ (c0 << 16) | c3  [Index 0x3]
    stmia r2!, {r9 - r12}
    @ --- Block 1: High Color = c1 (r6) ---
    mov   r12, r6, lsl #16
    orr   r9,  r12, r5          @ (c1 << 16) | c0  [Index 0x4]
    orr   r10, r12, r6          @ (c1 << 16) | c1  [Index 0x5]
    orr   r11, r12, r7          @ (c1 << 16) | c2  [Index 0x6]
    orr   r12, r12, r8          @ (c1 << 16) | c3  [Index 0x7]
    stmia r2!, {r9 - r12}
    @ --- Block 2: High Color = c2 (r7) ---
    mov   r12, r7, lsl #16
    orr   r9,  r12, r5          @ (c2 << 16) | c0  [Index 0x8]
    orr   r10, r12, r6          @ (c2 << 16) | c1  [Index 0x9]
    orr   r11, r12, r7          @ (c2 << 16) | c2  [Index 0xA]
    orr   r12, r12, r8          @ (c2 << 16) | c3  [Index 0xB]
    stmia r2!, {r9 - r12}
    @ --- Block 3: High Color = c3 (r8) ---
    mov   r12, r8, lsl #16
    orr   r9,  r12, r5          @ (c3 << 16) | c0  [Index 0xC]
    orr   r10, r12, r6          @ (c3 << 16) | c1  [Index 0xD]
    orr   r11, r12, r7          @ (c3 << 16) | c2  [Index 0xE]
    orr   r12, r12, r8          @ (c3 << 16) | c3  [Index 0xF]
    stmia r2!, {r9 - r12}
    sub   r2, r2, #64           @ Restore r2 pointer to start of LUT
    pop   {r12}
.dxt8x8_decode:
    mov   r10, #8               @ line counter, 8 lines to decode
    mov   r11, #0x3C            @ Byte-Offset Mask (4 bits shifted by 2)
.dxt8x8_line:
    ldrh  r5, [r0], #2          @ Load 16 index bits (8 pixels)
    and   r6, r11, r5, lsl #2   @ Offset Pair 0 (Pixel 0, 1)
    and   r7, r11, r5, lsr #2   @ Offset Pair 1 (Pixel 2, 3)
    and   r8, r11, r5, lsr #6   @ Offset Pair 2 (Pixel 4, 5)
    and   r9, r11, r5, lsr #10  @ Offset Pair 3 (Pixel 6, 7)
    ldr   r6, [r2, r6]          @ Direct 32-bit read Pair 0
    ldr   r7, [r2, r7]          @ Direct 32-bit read Pair 1
    ldr   r8, [r2, r8]          @ Direct 32-bit read Pair 2
    ldr   r9, [r2, r9]          @ Direct 32-bit read Pair 3
    stmia r1, {r6 - r9}         @ Store entire 8-pixel line
    add   r1, r1, r4            @ Advance to next line
    subs  r10, r10, #1 @ decrement line counter
    bne   .dxt8x8_line @ repeat for the next line
    bx    lr

.arm
 .align
 .global DXTVUnCompWrite16bit
 .type DXTVUnCompWrite16bit,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
DXTVUnCompWrite16bit:
    @ Decode an image frame in DXTV format
    @ ------------------------------
    @ Input:
    @ r0: pointer to DXTV source data, must be 2-byte-aligned (trashed)
    @ r1: pointer to the destination pixel buffer, must be 4-byte-aligned (trashed)
    @ r2: pointer to the previous pixel buffer, must be 4-byte-aligned (trashed)
    @ r3: line stride in bytes of previous pixel buffer (2-byte pixels) (trashed)
    @ on stack: width of image frame in pixels
    @ on stack: height of image frame in pixels
    @ ------------------------------
    @ In function:
    @ r4: line stride in bytes of destination buffer * 2 (2-byte pixels)
    @ r5, r6, r7, r8, r9, r10, r11: scratch registers
    @ r12: block flags
    @ on stack: x-loop counter
    @ on stack: y-loop counter
    push {r4 - r12, lr}
    @ read frame header and early-out if this is a repeated frame
    ldr r5, [r0], #DXTV_FRAMEHEADER_SIZE
    ands r5, r5, #DXTV_CONSTANTS_FRAME_KEEP
    bne .ucw16_end
    @ r4: line stride in bytes of destination buffer * 2 (2-byte pixels)
    ldr r4, [sp, #40] @ load width from stack
    mov r4, r4, lsl #1 @ r4 *= 2
    @ r6 / on stack: block-y counter max
    ldr r6, [sp, #44] @ load height from stack
    mov r6, r6, lsr #DXTV_CONSTANTS_BLOCK_MAX_SHIFT @ on stack = height >> DXTV_CONSTANTS_BLOCK_MAX_SHIFT
.ucw16_block_y_loop:
    @ r12: block flags
    mov r12, #0x0 @ clear flags
    @ r5 / on stack: block-x counter max
    ldr r5, [sp, #40] @ load width from stack
    mov r5, r5, lsr #DXTV_CONSTANTS_BLOCK_MAX_SHIFT @ r5 = width >> DXTV_CONSTANTS_BLOCK_MAX_SHIFT
    @ save y-counter, destination and previous source pointer
    push {r1, r2, r6}
.ucw16_block_x_loop:
    @ save x-counter, destination and previous source pointer
    push {r1, r2, r5}
    @ load some new flags if we've run out
    tst     r12, #65536 @ as long as bit 16 is 1 we have some flags in the lower 16 bits
    ldreq   r5, =#0xFFFF0000 @ set upper 16 bits in r5
    ldreqh  r12, [r0], #2 @ r12 = 16 bits worth of flags from DXTV data
    orreq   r12, r12, r5 @ r12 = 0xFFFF|flags
    tst     r12, #1 @ check if block split flag is set
    mov     r12, r12, lsr #1 @ move next flag into position
    @ block not split? decode 8x8 block
    bleq    DecodeBlock8x8
    beq     .ucw16_block_x_end
    @ block split? decode four 4x4 blocks
    @ decode block A - upper-left
    bl DecodeBlock4x4
    ldmia sp, {r1, r2}
    @ decode block B - upper-right
    add r1, r1, #8 @ move destination pointer 4 pixels right
    add r2, r2, #8 @ move previous source pointer 4 pixels right
    bl DecodeBlock4x4
    ldmia sp, {r1, r2}
    @ decode block C - lower-left
    add r1, r1, r4, lsl #DXTV_CONSTANTS_BLOCK_MIN_SHIFT @ move destination pointer 4 lines down
    add r2, r2, r3, lsl #DXTV_CONSTANTS_BLOCK_MIN_SHIFT @ move previous source pointer 4 lines down
    bl DecodeBlock4x4
    ldmia sp, {r1, r2}
    @ decode block D - lower-right
    add r1, r1, r4, lsl #DXTV_CONSTANTS_BLOCK_MIN_SHIFT @ move destination pointer 4 lines down
    add r2, r2, r3, lsl #DXTV_CONSTANTS_BLOCK_MIN_SHIFT @ move previous source pointer 4 lines down
    add r1, r1, #8 @ move destination pointer 4 pixels right
    add r2, r2, #8 @ move previous source pointer 4 pixels right
    bl DecodeBlock4x4
.ucw16_block_x_end:
    @ move to next 8x8 block horizontally
    pop {r1, r2, r5}
    add r1, r1, #16 @ move destination pointer 8 pixels right
    add r2, r2, #16 @ move previous source pointer 8 pixels right
    subs r5, r5, #1 @ check if there are blocks remaining horizontally
    bne .ucw16_block_x_loop
    @ move to next 8x8 block line vertically
    pop {r1, r2, r6}
    add r1, r1, r4, lsl #DXTV_CONSTANTS_BLOCK_MAX_SHIFT @ move destination pointer 8 lines down
    add r2, r2, r3, lsl #DXTV_CONSTANTS_BLOCK_MAX_SHIFT @ move previous source pointer 8 lines down
    subs r6, r6, #1 @ check if there are blocks remaining vertically
    bne .ucw16_block_y_loop
.ucw16_end:
    pop {r4 - r12, lr}
    bx lr


.arm
 .align
 .global DXTVUnCompGetSize
 .type DXTVUnCompGetSize,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
DXTVUnCompGetSize:
    @ Calculate decompressed frame data size
    @ r0: pointer to DXTV frame data, must be 4-byte-aligned (trashed for return value)
    ldr r0, [r0] @ read frame header
    mov r0, r0, lsr #8
    bx lr
