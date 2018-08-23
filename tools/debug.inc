;; JSON65 - A JSON parser for the 6502 microprocessor.
;;
;; https://github.com/ppelleti/json65
;;
;; Copyright © 2018 Patrick Pelletier
;;
;; This software is provided 'as-is', without any express or implied
;; warranty.  In no event will the authors be held liable for any damages
;; arising from the use of this software.
;;
;; Permission is granted to anyone to use this software for any purpose,
;; including commercial applications, and to alter it and redistribute it
;; freely, subject to the following restrictions:
;;
;; 1. The origin of this software must not be misrepresented; you must not
;;    claim that you wrote the original software. If you use this software
;;    in a product, an acknowledgment in the product documentation would be
;;    appreciated but is not required.
;; 2. Altered source versions must be plainly marked as such, and must not be
;;    misrepresented as being the original software.
;; 3. This notice may not be removed or altered from any source distribution.

        .import pusha0
        .import pushax
        .import _printf

        zp_bytes = 24

        .data
save0a: .byte 0
save0x: .byte 0
save_a: .byte 0
save_x: .byte 0
save_y: .byte 0
save_zp:
        .res zp_bytes
        .code

.macro save_regs
        .local L
        php
        sta save_a
        stx save_x
        sty save_y
        ldy #zp_bytes-1
L:      lda sreg,y
        sta save_zp,y
        dey
        bpl L
.endmacro               ; save_regs

.macro restore_regs
        .local L
        ldy #zp_bytes-1
L:      lda save_zp,y
        sta sreg,y
        dey
        bpl L
        ldy save_y
        ldx save_x
        lda save_a
        plp
.endmacro               ; restore_regs

.macro print_str str
        .local S
        .data
S:      .asciiz str
        .code
        php
        sta save0a
        stx save0x
        lda #<S
        ldx #>S
        jsr debug_str
        ldx save0x
        lda save0a
        plp
.endmacro               ; print_str

.macro print_hex
        jsr debug_hex
.endmacro               ; print_hex

.macro print_str_nl str
        print_str str
        jsr debug_nl
.endmacro               ; print_str_nl

.macro print_str_hex str
        print_str str
        jsr debug_hex
.endmacro               ; print_str_hex

.macro print_hex_nl
        jsr debug_hex
        jsr debug_nl
.endmacro               ; print_hex_nl

.macro print_str_hex_nl str
        print_str str
        jsr debug_hex
        jsr debug_nl
.endmacro               ; print_str_hex_nl

.macro print_nl
        jsr debug_nl
.endmacro               ; print_nl

.macro print_long arg
        save_regs
        lda #<percent_08lx
        ldx #>percent_08lx
        jsr pushax
        lda arg+2
        sta sreg
        lda arg+3
        sta sreg+1
        lda arg
        ldx arg+1
        jsr pusheax
        ldy #6
        jsr _printf
        restore_regs
.endmacro               ; print_log

        .data
percent_08lx:
        .asciiz "[%08lx]"
        .code

.macro print_word arg
        save_regs
        lda #<percent_04x
        ldx #>percent_04x
        jsr pushax
        lda arg
        ldx arg+1
        jsr pushax
        ldy #4
        jsr _printf
        restore_regs
.endmacro               ; print_log

        .data
percent_04x:
        .asciiz "[%04x]"
        .code

.proc debug_str
        save_regs
        lda #<percent_s
        ldx #>percent_s
        jsr pushax
        lda save_a
        ldx save_x
        jsr pushax
        ldy #4
        jsr _printf
        restore_regs
        rts

        .data
percent_s:
        .asciiz "%s"
        .code

.endproc                ; debug_str

.proc debug_hex
        save_regs
        lda #<percent_x
        ldx #>percent_x
        jsr pushax
        lda save_a
        jsr pusha0
        ldy #4
        jsr _printf
        restore_regs
        rts

        .data
percent_x:
        .asciiz "%02X"
        .code

.endproc                ; debug_hex

.proc debug_nl
        save_regs
        lda #<newline
        ldx #>newline
        jsr pushax
        ldy #2
        jsr _printf
        restore_regs
        rts

        .data
newline:
        .byte $0a, $00
        .code

.endproc                ; debug_nl
