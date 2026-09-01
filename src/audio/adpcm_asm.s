#include "adpcm_structs.h"

 .global ADPCM_DeltaTable_4bit
 .global ADPCM_IndexTable_4bit
 .global ADPCM_DitherState

 //#define ADPCM_DITHER
 #define ADPCM_DITHER_SHIFT 24
 //#define ADPCM_ROUNDING
 #define ADPCM_CLAMP

 // baseline: Audio avg. decode: 6.31 ms (max. 7.81 ms)
 // smc1      Audio avg. decode: 6.14 ms (max. 6.83 ms)

 .arm
 .align
 .global adpcm_ucw8_8_channel_decode
 .type adpcm_ucw8_8_channel_decode,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
.adpcm_ucw8_8_channel_decode:
    @ Decode one channel of a frame of ADPCM data
    @ ------------------------------
    @ Input:
    @ r0: pointer to ADPCM frame data (afterwards points to start of next channel)
    @ r1: pointer to the 8bit sample buffer (trashed)
    @ r2: number of nibbles to decode (output samples - 1) (trashed)
    @ r8: 0x7FFFFFFF
    @ r9: adress of ADPCM_DeltaTable_4bit
    @ r10: adress of ADPCM_IndexTable_4bit
    @ r11: last_dither (if dithering enabled)
    @ r12: dither (if dithering enabled)
    @ ------------------------------
    @ In function:
    @ r3 = PCM data
    @ r4 = Current ADPCM index
    @ r5 = Current ADPCM byte (2*4 bit data)
    @ r6,r7 are scratch registers

    @ load first verbatim PCM sample to r3
    ldrb    r3, [r0], #1       @ load lower byte
    ldrsb   r6, [r0], #1       @ load higher byte and sign-extend
    orr     r3, r3, r6, lsl #8 @ combine the two
#ifdef ADPCM_DITHER
    @ dither PCM data in r3
    sub     r3, r3, r11 @ pcmData -= last_dither
    mov     r11, r12 @ r11 = dither
    rsb     r12, r11, r12, lsl #4 @ r12 = (dither << 4) - dither
    eor     r12, r12, #1 @ r12 ^= 1
    mov     r11, r11, lsr #ADPCM_DITHER_SHIFT @ r11 = dither >> ADPCM_DITHER_SHIFT
    add     r3, r3, r11 @ pcmData += dither >> ADPCM_DITHER_SHIFT
#endif
#ifdef ADPCM_CLAMP
    @ clamp PCM data in r3 to [-32768, 32767]
    mov     r7, r3, lsl #16 @ r7 = r3 << 16
    cmp     r3, r7, asr #16 @ shift back and sign-extend r7 and compare with r3. check if r3 fits into signed 16-bit
    eorne   r7, r3, r8, asr #31 @ extract sign bit of r3. xor with r8 and apply to saturate
#endif
#ifdef ADPCM_ROUNDING
    add     r7, r3, #128 @ r7 = pcmData + 128
    mov     r7, r7, asr #8 @ r7 = (pcmData + 128) >> 8
#else
    mov     r7, r3, asr #8 @ r7 = pcmData >> 8
#endif
    strb    r7, [r1], #1 @ store first 8-bit PCM sample
    @ load first index to r4
    ldrb    r4, [r0], #1       @ load lower byte
    ldrsb   r6, [r0], #1       @ load higher byte and sign-extend
    orr     r4, r4, r6, lsl #8 @ combine the two
    @ build byte load operation for patching depending on nr of nibbles
    add     r7, pc, #.adpcm_channel_decode_sample_byteload - . - 8
    mov     r6, #0x04D00000     @ r6 = 0x04D00000
    orr     r6, r6, #0x5000     @ r6 = 0x04D05000
    orr     r6, r6, #0x0001     @ r6 = 0x04D05001 (ldreqb r5, [r0], #1)
    tst     r2, #1              @ check if nr of nibbles is even
    orrne   r6, r6, #0x10000000 @ r6 = 0x14D05001 (ldrneb r5, [r0], #1)
    str     r6, [r7]            @ patch instruction
.adpcm_channel_decode_sample_loop:
    @ load two ADPCM nibbles to r5 if we need them
    tst     r2, #1
.adpcm_channel_decode_sample_byteload:
    nop                    @ <---- will be patched to:
                           @ "ldrneb r5, [r0], #1" for odd r2
                           @ "ldreqb r5, [r0], #1" for even r2
    @ decode nibble
    and     r6, r5, #0x07  @ r6 = nibble & 7 
    mov     r7, r4, lsl #4 @ r7 = index * 2 * 8, because uint16_t and 8 entries per index
    add     r7, r6, lsl #1 @ r7 = index * 2 * 8 + (nibble & 7) * 2
    ldrh    r7, [r9, r7]   @ load delta to r7
    tst     r5, #0x08      @ ADPCM value & 8?
    subne   r3, r3, r7     @ true  -> pcmData -= delta
    addeq   r3, r3, r7     @ false -> pcmData += delta
    ldrsb   r7, [r10, r6]  @ load index into r7 and 
    adds    r4, r4, r7     @ add to old index in r4. sets flags
    movmi   r4, #0         @ index = index < 0 ? 0 : index
    cmp     r4, #88        @ index > 88 ?
    movgt   r4, #88        @ index = index > 88 ? 88 : index
#ifdef ADPCM_DITHER
    @ dither PCM data in r3
    sub     r3, r3, r11 @ pcmData -= last_dither
    mov     r11, r12 @ r11 = dither
    rsb     r12, r11, r12, lsl #4 @ r12 = (dither << 4) - dither
    eor     r12, r12, #1 @ r12 ^= 1
    mov     r11, r11, lsr #ADPCM_DITHER_SHIFT @ r11 = dither >> ADPCM_DITHER_SHIFT
    add     r3, r3, r11 @ pcmData += dither >> ADPCM_DITHER_SHIFT
#endif
#ifdef ADPCM_CLAMP
    @ clamp PCM data in r3 to [-32768, 32767]
    mov     r7, r3, lsl #16 @ r7 = r3 << 16
    cmp     r3, r7, asr #16 @ shift back and sign-extend r7 and compare with r3. check if r3 fits into signed 16-bit
    eorne   r7, r3, r8, asr #31 @ extract sign bit of r3. xor with r8 and apply to saturate
#endif
#ifdef ADPCM_ROUNDING
    add     r7, r3, #128 @ r7 = pcmData + 128
    mov     r7, r7, asr #8 @ r7 = (pcmData + 128) >> 8
#else
    mov     r7, r3, asr #8 @ r7 = pcmData >> 8
#endif
    strb    r7, [r1], #1   @ store nibble / 8-bit PCM sample
    @ loop finish
    mov     r5, r5, lsr #4 @ r5 = (nibble >> 4) 
    subs    r2, #1 @ less than number of nibbles?
    bne     .adpcm_channel_decode_sample_loop
    bx      lr

 .arm
 .align
 .global ADPCMUnCompWrite8bit_8bit
 .type ADPCMUnCompWrite8bit_8bit,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
ADPCMUnCompWrite8bit_8bit:
    @ Decode a frame of ADPCM data
    @ r0: pointer to ADPCM frame data (trashed)
    @ r1: pointer to the 8bit sample buffer(s) (trashed)
    @ r2: pointer to resampler data, if resampling enabled, or nullptr
    @ r3,r12 trashed, r4-r11 and r14 used and saved / restored
    @ r6,r7 are scratch registers

    push    {r4 - r12, lr}
    @ store constants
    ldr     r8, =#0x7FFFFFFF @ used for clamping PCM data
    ldr     r9, =ADPCM_DeltaTable_4bit
    ldr     r10, =ADPCM_IndexTable_4bit
#ifdef ADPCM_DITHER
    @ load dither state
    ldr     r5, =ADPCM_DitherState
    ldmia   r5, {r11, r12} @ load r11 = last_dither, r12 = dither
#endif
    @ load first header half-word to r3
    ldrb    r6, [r0], #1       @ load lower byte
    ldrb    r7, [r0], #1       @ load higher byte
    orr     r3, r6, r7, lsl #8 @ combine the two
    and     r3, r3, #0x60      @ get number of channels into r3
    mov     r3, r3, lsr #5
    cmp     r3, #1
    @ load uncompressed size half-word to r3
    ldrb    r6, [r0], #1       @ load lower byte
    ldrb    r7, [r0], #1       @ load higher byte
    orr     r3, r6, r7, lsl #8 @ combine the two
    lsr     r3, r3, #1         @ halve count, as we're outputting 8 bits instead of 16
    movgt   r3, r3, lsr #1     @ divide size of data by two for stereo data
    sub     r3, #1             @ r3 = nr of nibbles (nr of samples - 1)
    bgt     .adpcm_ucw8_8_channel_stereo
.adpcm_ucw8_8_channel_mono:
    cmp     r2, #0       @ check if we want resampling or not
    ldrne   r2, [r2, #0] @ get first resampler data (if resampling)
@    blne    .adpcm_ucw8_8_channel_resample
@    mov     r6, ...
@    b       .adpcm_ucw8_8_channel_end
    mov     r2, r3
    push    {r2}
    ldr     r1, [r1, #0] @ get first output buffer
    bleq    .adpcm_ucw8_8_channel_decode
    pop     {r2}
    add     r2, #1
    b       .adpcm_ucw8_8_channel_channel_end
.adpcm_ucw8_8_channel_stereo:
    cmp     r2, #0       @ check if we want resampling or not
    bne     .adpcm_ucw8_8_channel_stereo_resampling
    mov     r2, r3
    push    {r1, r2}
    ldr     r1, [r1, #0] @ get first output buffer
    bl      .adpcm_ucw8_8_channel_decode
    ldr     r1, [sp, #0]
    ldr     r2, [sp, #4]
    ldr     r1, [r1, #4] @ get second output buffer
    bl      .adpcm_ucw8_8_channel_decode
    pop     {r1, r2}
    mov     r2, r2, lsl #1
    add     r2, #2
    b       .adpcm_ucw8_8_channel_channel_end
.adpcm_ucw8_8_channel_stereo_resampling:
@    ldr     r2, [r2, #0] @ get first resampler data (if resampling)
@    blne    .adpcm_ucw8_8_channel_resample
@    mov     r6, ...
@    b       .adpcm_ucw8_8_channel_end
.adpcm_ucw8_8_channel_channel_end:
#ifdef ADPCM_DITHER
    @ store dither state
    ldr     r5, =ADPCM_DitherState
    stmia   r5, {r11, r12} @ store r11 = last_dither, r12 = dither
#endif
    mov     r0, r2 @ return number of samples generated
    pop     {r4 - r12, lr}
    bx      lr

 .arm
 .align
 .global ADPCMUnCompGetSize_8bit
 .type ADPCMUnCompGetSize_8bit,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
ADPCMUnCompGetSize_8bit:
    @ Calculate decompressed PCM data size
    @ r0: pointer to ADPCM frame data, must be 4-byte-aligned
    ldrh r1, [r0] @ load first header half-word header into r1
    ldrh r0, [r0, #2] @ load uncompressed size into r0
    lsl r0, r0, #3
    add r0, r0, #7
    and r1, r1, #0x1F80 @ get PCM bits per sample into r1
    mov r1, r1, lsr #7
    cmp r1, #8 @ 8 bits / sample
    moveq r0, r0, lsr #3
    beq .adpcm_ucgs_8_end
    cmp r1, #16 @ 16 bits / sample
    moveq r0, r0, lsr #4
    beq .adpcm_ucgs_8_end
    cmp r1, #24 @ 24 bits / sample
    ldr r1, =2863311531
    muleq r1, r0, r1
    moveq r0, r1, lsr #4
    beq .adpcm_ucgs_8_end
    cmp r1, #32 @ 32 bits / sample
    moveq r0, r0, lsr #5
    beq .adpcm_ucgs_8_end
    eor r0, r0, r0
.adpcm_ucgs_8_end:
    bx lr
