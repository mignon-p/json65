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
        .endmacro               ; restore_regs

        .macro print_str str
        .local S
        .data
S:      .asciiz str
        .code
        sta save0a
        stx save0x
        lda #<S
        ldx #>S
        jsr debug_str
        ldx save0x
        lda save0a
        .endmacro               ; print_str

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
