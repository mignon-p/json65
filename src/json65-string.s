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

        .import _free
        .import _malloc
        .import popax

        .export _j65_init_strings
        .export _j65_intern_string
        .export _j65_free_strings

        ;; take advantage of the fact that malloc and free don't
        ;; modify regsave or sreg
        loptr = regsave
        hiptr = regsave + 2
        linkptr = sreg
        ;; they don't modify tmp1 through tmp4, either
        t1 = tmp1
        t2 = tmp2
        hash_val = tmp3
        len = tmp4
        idx = tmp4
        ;; ptr4 is also preserved across malloc (but not free)
        strptr = ptr4
        ;; the following doesn't need to be preserved across
        ;; a malloc or free
        tmpptr = ptr3

        ;; bucket format:
        ;; lo byte of ptr to next bucket
        ;; hi byte of ptr to next bucket
        ;; length of string
        ;; 0-255 bytes of string
        ;; NUL byte

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                         j65_init_strings                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; void __fastcall__ j65_init_strings (j65_strings *strs);
.proc _j65_init_strings
        sta ptr1
        stx ptr1+1
        ldy #0
        tya
        jsr loop
        inc ptr1+1
loop:   sta (ptr1),y
        iny
        bne loop
        rts
.endproc                ; _j65_init_strings

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                         j65_intern_string                        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; const char *j65_intern_string (j65_strings *strs, const char *str);
.proc _j65_intern_string
        sta strptr
        stx strptr+1
        jsr hash_str
        sta hash_val
        sty len
        jsr popax               ; get pointer to j65_strings structure
        sta loptr
        stx loptr+1
        sta hiptr
        inx
        stx hiptr+1
        ldy hash_val            ; lookup hash bucket
        lda (loptr),y
        sta linkptr
        lda (hiptr),y
linkloop:                       ; traverse linked list
        sta linkptr+1
        ora linkptr
        beq not_found
        ldy #2
        lda (linkptr),y
        cmp len
        bne nextlink
        lda linkptr             ; compare string
        add #3
        sta tmpptr
        lda linkptr+1
        adc #0
        sta tmpptr+1
        ldy #0
strloop:
        lda (strptr),y
        cmp (tmpptr),y
        bne nextlink
        iny
        cpy len
        bne strloop
        lda tmpptr              ; return pointer to string from table
        ldx tmpptr+1
fail:   rts
nextlink:
        ldy #0
        lda (linkptr),y
        tax
        iny
        lda (linkptr),y
        stx linkptr
        jmp linkloop
not_found:                      ; so we need to add it to the hash table
        ldx #0
        lda len
        add #4
        bcc skip
        inx
skip:   jsr _malloc
        sta tmpptr
        stx tmpptr+1
        ora tmpptr+1
        beq fail                ; if malloc returned null, we return null
        ldy hash_val
        lda (hiptr),y           ; add new memory to linked list
        tax
        lda (loptr),y
        ldy #0
        sta (tmpptr),y
        iny
        txa
        sta (tmpptr),y
        ldy hash_val
        lda tmpptr
        sta (loptr),y
        lda tmpptr+1
        sta (hiptr),y
        ldy #2                  ; store length
        lda len
        sta (tmpptr),y
        lda tmpptr              ; copy string
        add #3
        sta tmpptr
        bcc skip2
        inc tmpptr+1
skip2:  ldy #0
copyloop:
        cpy len
        beq copydone
        lda (strptr),y
        sta (tmpptr),y
        iny
        jmp copyloop
copydone:
        lda #0
        sta (tmpptr),y
        lda tmpptr              ; return pointer to new string
        ldx tmpptr+1
        rts
.endproc                ; _j65_intern_string

;; rotate accumulator left by 1.
.macro rotate_left
        cmp #$80
        rol
.endmacro               ; rotate_left

;; hash the (NUL terminated) string pointed at by strptr.
;; return hash in a and length in y.
.proc hash_str
        lda #0
        tay
loop:   sta t1
        lda (strptr),y
        beq done
        iny
        beq done0
        add t1
        rotate_left
        rotate_left
        rotate_left
        jmp loop
done0:  dey
done:   lda t1
        rts
.endproc                ; hash_str

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                         j65_free_strings                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; void __fastcall__ j65_free_strings (j65_strings *strs);
.proc _j65_free_strings
        sta loptr
        stx loptr+1
        sta hiptr
        inx
        stx hiptr+1
        ldy #0
loop:   sty idx
        lda (hiptr),y
        tax
        lda (loptr),y
        jsr freelink
        ldy idx
        lda #0
        sta (loptr),y
        sta (hiptr),y
        iny
        bne loop
        rts
.endproc                ; _j65_free_strings

;; free the chain of buckets pointed to by ax
.proc freelink
        sta linkptr
        stx linkptr+1
        ora linkptr+1
        beq done
        ldy #0
        lda (linkptr),y
        sta t1
        iny
        lda (linkptr),y
        sta t2
        lda linkptr
        ldx linkptr+1
        jsr _free
        lda t1
        ldx t2
        jmp freelink
done:   rts
.endproc                ; freelink
