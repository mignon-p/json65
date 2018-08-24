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

        .macpack generic
        .include "zeropage.inc"

        .import _fprintf
        .import _fputc
        .import _fwrite
        .import popax
        .import pushax

        .export _j65_print_escaped

        fileptr = regbank
        strptr = regbank + 2
        startidx = regbank + 4
        saveidx = regbank + 5
        t1 = tmp1
        len = tmp2
        character = tmp3

;; pushes regbank (caller-saved registers) onto 6502 stack
.macro save_regbank
        .repeat 6, i
        lda regbank+i
        pha
        .endrep
.endmacro               ; save_regbank

;; pops regbank (caller-saved registers) off of 6502 stack
.macro restore_regbank
        .repeat 6, i
        pla
        sta regbank+5-i
        .endrep
.endmacro               ; restore_regbank

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                         j65_print_escaped                        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; void __fastcall__ j65_print_escaped (const char *str, FILE *f)
.proc _j65_print_escaped
        sta t1
        save_regbank
        lda t1
        sta fileptr
        stx fileptr+1
        jsr popax
        sta strptr
        stx strptr+1
        ldy #0
loop1:  sty startidx
loop2:  lda (strptr),y
        cmp #'#'                ; check whether character is special
        bge higher
        and #$fe
        cmp #' '
        bne special_char
okay_char:                      ; character does not need to be escaped
        iny
        jmp loop2
higher: cmp #$5c                ; backslash
        bne okay_char
special_char:                   ; character needs to be escaped
        sty saveidx
        tya
        sub startidx
        beq skip_fwrite         ; skip fwrite if "count" would be 0
        sta len
        ldx strptr+1            ; add strptr to startidx to make "buf" argument
        lda strptr
        add startidx
        bcc skip_inx
        inx
skip_inx:
        jsr pushax              ; push argument "buf"
        lda #1
        ldx #0
        jsr pushax              ; push argument "size"
        lda len
        ldx #0
        jsr pushax              ; push argument "count"
        lda fileptr             ; argument "f" passed in ax
        ldx fileptr+1
        jsr _fwrite
skip_fwrite:
        ldy saveidx
        lda (strptr),y
        beq done                ; terminating NUL character
        pha                     ; save character to be escaped
        lda #$5c                ; print backslash
        ldx #0
        jsr pushax              ; push argument "c"
        lda fileptr             ; argument "f" passed in ax
        ldx fileptr+1
        jsr _fputc
        pla                     ; restore character to be escaped
        sta character
        ldx #6                  ; see if there is a short escape for character
esc_loop:
        lda escaped_chars,x
        cmp character
        beq found_short_escape
        dex
        bpl esc_loop
        lda fileptr             ; there is not a short escape, use \uxxxx
        ldx fileptr+1
        jsr pushax              ; push argument "f"
        lda #<fmt_str
        ldx #>fmt_str
        jsr pushax              ; push argument "format"
        lda character
        ldx #0
        jsr pushax              ; push varargs argument (character as int)
        ldy #6                  ; bytes of arguments on the stack
        jsr _fprintf
continue:
        ldy saveidx
        iny
        jmp loop1
found_short_escape:
        lda escape_codes,x      ; print escape code character
        ldx #0
        jsr pushax              ; push argument "c"
        lda fileptr             ; argument "f" passed in ax
        ldx fileptr+1
        jsr _fputc
        jmp continue
done:   restore_regbank
        rts

        .rodata
escape_codes:
        .byte $22, $5c, "bfnrt"
escaped_chars:
        .byte $22, $5c, $08, $0c, $0a, $0d, $09
fmt_str:
        .asciiz "u%04x"

.endproc                ; _j65_print_escaped
