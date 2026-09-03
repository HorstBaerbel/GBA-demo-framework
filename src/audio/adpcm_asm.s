#include "resampler_structs.h"
#include "adpcm_structs.h"

 .global ADPCM_DeltaTable_4bit
 .global ADPCM_IndexTable_4bit
 .global ADPCM_DitherState

//#define ADPCM_DITHER
#define ADPCM_DITHER_SHIFT 24
//#define ADPCM_ROUNDING

// baseline: Audio avg. decode: 6.31 ms (max. 7.81 ms)
// unrolled: Audio avg. decode: 5.65 ms (max. 6.83 ms)

// Resampling
// C++: Audio avg. decode: 35.25 ms (max. 37.10 ms)
// ASM: Audio avg. decode: 12.84 ms (max. 13.67 ms)

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
    @ Decode one channel of a frame of ADPCM data w/o resampling
    @ ------------------------------
    @ Input:
    @ r0: pointer to ADPCM frame data (afterwards points to start of next channel)
    @ r1: pointer to the 8bit sample buffer (trashed)
    @ r2: number of samples to decode (trashed)
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
    sub     r2, #1 @ r2 = nr of nibbles to decode (nr of samples - 1)
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
    @ clamp PCM data in r3 to [-32768, 32767]
    mov     r7, r3, lsl #16     @ r7 = r3 << 16
    cmp     r3, r7, asr #16     @ shift back and sign-extend r7 and compare with r3. check if r3 fits into signed 16-bit
    eorne   r3, r3, r8, asr #31 @ extract sign bit of r3. xor with r8 and apply to saturate
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
.adpcm_channel_decode_sample_loop:
    @ load two ADPCM nibbles to r5
    ldrb    r5, [r0], #1
    @ decode first nibble
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
    @ clamp PCM data in r3 to [-32768, 32767]
    mov     r7, r3, lsl #16     @ r7 = r3 << 16
    cmp     r3, r7, asr #16     @ shift back and sign-extend r7 and compare with r3. check if r3 fits into signed 16-bit
    eorne   r3, r3, r8, asr #31 @ extract sign bit of r3. xor with r8 and apply to saturate
#ifdef ADPCM_ROUNDING
    add     r7, r3, #128 @ r7 = pcmData + 128
    mov     r7, r7, asr #8 @ r7 = (pcmData + 128) >> 8
#else
    mov     r7, r3, asr #8 @ r7 = pcmData >> 8
#endif
    strb    r7, [r1], #1   @ store nibble / 8-bit PCM sample
    subs    r2, #1 @ more nibbles?
    beq     .adpcm_channel_decode_sample_end
    mov     r5, r5, lsr #4 @ r5 = (nibble >> 4)
    @ decode second nibble
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
    @ clamp PCM data in r3 to [-32768, 32767]
    mov     r7, r3, lsl #16     @ r7 = r3 << 16
    cmp     r3, r7, asr #16     @ shift back and sign-extend r7 and compare with r3. check if r3 fits into signed 16-bit
    eorne   r3, r3, r8, asr #31 @ extract sign bit of r3. xor with r8 and apply to saturate
#ifdef ADPCM_ROUNDING
    add     r7, r3, #128 @ r7 = pcmData + 128
    mov     r7, r7, asr #8 @ r7 = (pcmData + 128) >> 8
#else
    mov     r7, r3, asr #8 @ r7 = pcmData >> 8
#endif
    strb    r7, [r1], #1   @ store nibble / 8-bit PCM sample
    subs    r2, #1 @ more nibbles?
    bne     .adpcm_channel_decode_sample_loop
.adpcm_channel_decode_sample_end:
    bx      lr

 .arm
 .align
 .global adpcm_ucw8_8_channel_resample
 .type adpcm_ucw8_8_channel_resample,function
#ifdef __NDS__
 .section .itcm, "ax", %progbits
#else
 .section .iwram, "ax", %progbits
#endif
.adpcm_ucw8_8_channel_resample:
    @ Decode one channel of a frame of ADPCM data with resampling
    @ ------------------------------
    @ Input:
    @ r0: pointer to ADPCM frame data (afterwards points to start of next channel)
    @ r1: pointer to the 8bit sample buffer (trashed)
    @ r2: resampler channel data (preserved)
    @ r3: number of samples to decode (trashed)
    @ r9: adress of ADPCM_DeltaTable_4bit
    @ r10: adress of ADPCM_IndexTable_4bit
    @ ------------------------------
    @ In function:
    @ r4 = PCM data
    @ r5 = Current ADPCM index
    @ r6 = Current ADPCM byte (2*4 bit data)
    @ r7 = resampler position
    @ r8 = nr of samples generated
    @ r11,r12,r14 are scratch registers
    push    {lr}
    sub     r3, #1 @ r3 = nr of nibbles to decode (nr of samples - 1)
    @ load first verbatim PCM sample to r4
    ldrb    r4, [r0], #1        @ load lower byte
    ldrsb   r12, [r0], #1       @ load higher byte and sign-extend
    orr     r4, r4, r12, lsl #8 @ combine the two
    @ load first index to r5
    ldrb    r5, [r0], #1        @ load lower byte
    ldrsb   r12, [r0], #1       @ load higher byte and sign-extend
    orr     r5, r5, r12, lsl #8 @ combine the two
    @ we can have history from previous calls, but at most one sample,
    @ because we must have output data until resamplerData.position >= RESAMPLER_POSITION_ONE
    @ we can also have no history at all for a first call
    @ this means we need at least one new sample here
    ldr     r7, [r2, #RESAMPLER_OFFSET_POSITION]
    cmp     r7, #RESAMPLER_POSITION_TWO
    blt     .adpcm_channel_resample_start_below_two
    @ resampler position >= RESAMPLER_POSITION_TWO (initial frame / first call)
    @ we have no samples in history
    strh    r4, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_0]
    strh    r4, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_1]
    sub     r7, #RESAMPLER_POSITION_ONE
    b       .adpcm_channel_resample_loop_setup
.adpcm_channel_resample_start_below_two:
    cmp     r7, #RESAMPLER_POSITION_ONE
    blt     .adpcm_channel_resample_loop_setup
    @ resampler position >= RESAMPLER_POSITION_ONE (regular frames)
    @ we have one samples in history. add new PCM sample
    ldrsh   r12, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_1]
    strh    r12, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_0]
    strh    r4, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_1]
    sub     r7, #RESAMPLER_POSITION_ONE
.adpcm_channel_resample_loop_setup:
    @ build byte load operation for patching depending on nr of nibbles
    add     r14, pc, #.adpcm_channel_resample_byteload - . - 8
    mov     r12, #0x04D00000      @ r12 = 0x04D00000
    orr     r12, r12, #0x6000     @ r12 = 0x04D06000
    orr     r12, r12, #0x0001     @ r12 = 0x04D06001 (ldreqb r6, [r0], #1)
    tst     r3, #1                @ check if nr of nibbles is even
    orrne   r12, r12, #0x10000000 @ r12 = 0x14D06001 (ldrneb r6, [r0], #1)
    str     r12, [r14]            @ patch instruction
    @ clear nr of samples generated
    mov     r8, #0
.adpcm_channel_resample_loop:
    @ check if we need to decode more input samples or emit output samples
    cmp     r7, #RESAMPLER_POSITION_ONE
    blt     .adpcm_channel_resample_output
    @ resampler position >= RESAMPLER_POSITION_ONE
    @ load two ADPCM nibbles to r6 if needed
    tst     r3, #1
.adpcm_channel_resample_byteload:
    nop @ <---- will be patched to:
        @ ldrneb r6, [r0], #1 for odd r3
        @ ldreqb r6, [r0], #1 for even r3
    @ decode nibble
    and     r14, r6, #0x07   @ r14 = nibble & 7 
    mov     r12, r5, lsl #4  @ r12 = index * 2 * 8, because uint16_t and 8 entries per index
    add     r12, r14, lsl #1 @ r12 = index * 2 * 8 + (nibble & 7) * 2
    ldrh    r12, [r9, r12]   @ load delta to r12
    tst     r6, #0x08        @ ADPCM value & 8?
    subne   r4, r4, r12      @ true  -> pcmData -= delta
    addeq   r4, r4, r12      @ false -> pcmData += delta
    ldrsb   r12, [r10, r14]  @ load index into r12 and 
    adds    r5, r5, r12      @ add to old index in r5. sets flags
    movmi   r5, #0           @ index = index < 0 ? 0 : index
    cmp     r5, #88          @ index > 88 ?
    movgt   r5, #88          @ index = index > 88 ? 88 : index
    @ clamp PCM data in r4 to [-32768, 32767]
    mov     r12, r4, lsl #16     @ r12 = r4 << 16
    cmp     r4, r12, asr #16     @ shift back and sign-extend r12 and compare with r4. check if r4 fits into signed 16-bit
    mvn     r11, #0x80000000     @ r11 = 0x7FFFFFFF
    eorne   r4, r4, r11, asr #31 @ extract sign bit of r4. xor with r11 and apply to saturate
    @ move new PCM data to resampler history
    ldrsh   r12, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_1]
    strh    r12, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_0]
    strh    r4, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_1]
    sub     r7, #RESAMPLER_POSITION_ONE
    mov     r6, r6, lsr #4 @ r6 = (nibble >> 4)
    sub     r3, #1 @ we processed one nibble
.adpcm_channel_resample_output:
    @ resampler position < RESAMPLER_POSITION_ONE
    ldrsh   r14, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_0]
    ldrsh   r12, [r2, #RESAMPLER_OFFSET_PCM_HISTORY_1]
    sub     r12, r14 @ r12 = diff = resampler.history[1] - resampler.history[0]
.adpcm_channel_resample_sample_output:
    @ calculate: resampler.history[0] - (((resampler.position >> (RESAMPLER_PRECISION - 8)) * diff) >> 8)
    asr     r11, r7, #RESAMPLER_PRECISION - 8
    mul     r11, r12
    add     r11, r14, r11, asr #8
    mov     r11, r11, asr #8 @ sample >> 8
    strb    r11, [r1], #1    @ store PCM sample
    add     r8, #1           @ increase nr of samples generated
    ldr     r11, [r2, #RESAMPLER_OFFSET_STEP]
    add     r7, r11
    cmp     r7, #RESAMPLER_POSITION_ONE
    blt     .adpcm_channel_resample_sample_output
    cmp     r3, #0 @ more nibbles?
    bgt     .adpcm_channel_resample_loop
    str     r7, [r2, #RESAMPLER_OFFSET_POSITION]
    pop     {lr}
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
    @ returns: number of generated samples in r0
    push    {r4 - r10, lr}
    @ store constants
    ldr     r8, =#0x7FFFFFFF @ used for clamping PCM data
    ldr     r9, =ADPCM_DeltaTable_4bit
    ldr     r10, =ADPCM_IndexTable_4bit
    @ load first header half-word to r4
    ldrb    r6, [r0], #1       @ load lower byte of flags
    ldrb    r7, [r0], #1       @ load higher byte of flags
    orr     r4, r6, r7, lsl #8 @ combine the two
    and     r4, r4, #0x60      @ get number of channels into r4
    mov     r4, r4, lsr #5     @ r4 = number of channnels
    @ load number of samples into r3
    ldrb    r6, [r0], #1       @ load lower byte of uncompressed size
    ldrb    r7, [r0], #1       @ load higher byte of uncompressed size
    orr     r3, r6, r7, lsl #8 @ combine the two
    lsr     r3, r3, #1         @ halve count, as we're outputting 8 bits instead of 16
    cmp     r2, #0             @ check if we want resampling or not
    beq     .adpcm_ucw8_8_normal
.adpcm_ucw8_8_resample:
    cmp     r4, #1 @ mono or stereo?
    bgt     .adpcm_ucw8_8_resample_stereo
    push    {r11}
    ldr     r1, [r1, #0] @ get first output buffer
    bl      .adpcm_ucw8_8_channel_resample @ decode first channel
    pop     {r11}
    b       .adpcm_ucw8_8_finish
.adpcm_ucw8_8_resample_stereo:
    mov     r3, r3, lsr #1 @ divide size of data by two for stereo data
    push    {r1, r2, r3, r11}
    ldr     r1, [r1, #0] @ get first output buffer
    bl      .adpcm_ucw8_8_channel_resample @ decode first channel
    pop     {r1 - r3}    @ restore r1, r2, r3
    ldr     r1, [r1, #4] @ get second output buffer
    add     r2, #RESAMPLER_SIZEOF_CHANNEL_DATA @ get second resampler data
    bl      .adpcm_ucw8_8_channel_resample @ decode second channel
    pop     {r11}
    mov     r0, r8, lsl #1 @ number of samples generated returned in r8
    b       .adpcm_ucw8_8_finish
.adpcm_ucw8_8_normal:
#ifdef ADPCM_DITHER
    @ load dither state
    push    {r11}
    ldr     r5, =ADPCM_DitherState
    ldmia   r5, {r11, r12} @ load r11 = last_dither, r12 = dither
#endif
    cmp     r4, #1 @ mono or stereo?
    bgt     .adpcm_ucw8_8_normal_stereo
    mov     r2, r3 @ move sample count to r2
    push    {r2}
    ldr     r1, [r1, #0] @ get first output buffer
    bl      .adpcm_ucw8_8_channel_decode @ decode first channel
    pop     {r0}
    b       .adpcm_ucw8_8_normal_finish
.adpcm_ucw8_8_normal_stereo:
    mov     r2, r3, lsr #1 @ divide size of data by two for stereo data and move to r2
    push    {r1, r2}
    ldr     r1, [r1, #0] @ get first output buffer
    bl      .adpcm_ucw8_8_channel_decode @ decode first channel
    ldr     r1, [sp, #0] @ restore r1
    ldr     r2, [sp, #4] @ restore number of samples
    ldr     r1, [r1, #4] @ get second output buffer
    bl      .adpcm_ucw8_8_channel_decode @ decode second channel
    pop     {r1}
    pop     {r0}
    mov     r0, r0, lsl #1
.adpcm_ucw8_8_normal_finish:
#ifdef ADPCM_DITHER
    @ store dither state
    ldr     r5, =ADPCM_DitherState
    stmia   r5, {r11, r12} @ store r11 = last_dither, r12 = dither
    pop     {r11}
#endif
.adpcm_ucw8_8_finish:
    pop     {r4 - r10, lr}
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
