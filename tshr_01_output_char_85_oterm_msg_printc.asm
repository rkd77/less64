SECTION code_driver
SECTION code_driver_terminal_output

PUBLIC tshr_01_output_char_85_oterm_msg_printc

EXTERN asm_tshr_cxy2saddr

tshr_01_output_char_85_oterm_msg_printc:

   ;   enter  :  c = ascii code
   ;             l = absolute x coordinate
   ;             h = absolute y coordinate
   ;   can use:  af, bc, de, hl

   ld a,c
   cp 32
   jr nc, code_ok
   
   ld c,'?'

code_ok:

   push hl
   ld a,l
   add a,l
   add a,l
   srl a
   srl a
   ld l,a ; * 3 / 4

   call asm_tshr_cxy2saddr
   ex de,hl                    ; de = screen address
   
   ld l,c
   ld h,0
   
   add hl,hl
   add hl,hl
   add hl,hl                   ; hl = 8 * ascii code
   
   ld c,(iy+21)
   ld b,(iy+22)                ; bc = font address
   
   add hl,bc                   ; hl = & character definition
   
   ; print character pixels

   pop bc

   ld a, c
   and 3
   jr z, jump0
   dec a
   jr z, jump1
   dec a
   jr z, jump2

jump7:                                  ; second display bits 5-0
jump3:                                  ; first display bits 5-0
    ld b,8
loop3:
    ld a,(hl)
    rrca
    rrca
    and 63                          ; 00111111
    ld c,a
    ld a,(de)
    and 192
    or c
    ld (de),a
    inc d
    inc hl
    djnz loop3
    ret

jump4:                                  ; second display bits 7-2
jump0:                                  ; first display, bits 7-2
    ld b,8
loop0:
    ld a,(hl)
    and 252                         ; 11111100
    ld c,a
    ld a,(de)
    and 3
    or c
    ld (de), a
    inc d
    inc hl
    djnz loop0
    ret

jump5:                                  ; second display bits 1-0, first display next 7-4
jump1:                                  ; first screen 1-0, second screen 7-4
    push hl
    push de
    ld b,8
loop1a:
    ld a,(hl)
    rlca
    rlca
    and 3                           ; 00000011
    ld c,a
    ld a,(de)
    and 252
    or c
    ld (de),a
    inc d
    inc hl
    djnz loop1a

    pop de
    call increment
preloop1b:
    pop hl
    ld b,8
loop1b:
    ld a,(hl)
    rlca
    rlca
    and 240                         ; 11110000
    ld c,a
    ld a,(de)
    and 15
    or c
    ld (de),a
    inc d
    inc hl
    djnz loop1b
    ret

jump6:                                  ; first display bits 3-0; second display bits 7-6
jump2:                                  ; second display bits 3-0, first display next 7-6
    push hl
    push de
    ld b,8
loop2a:
    ld a, (hl)
    rrca
    rrca
    rrca
    rrca
    and 15                          ; 00001111
    ld c,a
    ld a,(de)
    and 240
    or c
    ld (de),a
    inc d
    inc hl
    djnz loop2a
    pop de
    call increment
preloop2b:
    ld b,8
    pop hl
loop2b:
    ld a,(hl)
    rrca
    rrca
    rrca
    rrca
    and 192                        ; 11000000
    ld c,a
    ld a,(de)
    and 63
    or c
    ld (de),a
    inc d
    inc hl
    djnz loop2b
    ret

increment:
    bit 5,d
    jr z,first_screen
    res 5,d
    inc de
    ret
first_screen:
    set 5,d
    ret
