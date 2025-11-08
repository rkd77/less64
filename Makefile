all: less85 less64 less64s

less85: less85.c tshr_01_output_char_85_oterm_msg_printc.asm font_6x8_.asm
	/snap/bin/z88dk.zcc +zxn -vn -subtype=dot -startup=16 -clib=sdcc_iy -SO3 --max-allocs-per-node200000 \
  --opt-code-size less85.c font_6x8_.asm tshr_01_output_char_85_oterm_msg_printc.asm -o less85 -create-app

less64: less64.c font_8x8_.asm
	/snap/bin/z88dk.zcc +zxn -vn -subtype=dot -startup=16 -clib=sdcc_iy -SO3 --max-allocs-per-node200000 --opt-code-size less64.c font_8x8_.asm -o less64 -create-app

less64s: less64.c
	/snap/bin/z88dk.zcc +zx -vn -subtype=dot -startup=4 -clib=sdcc_iy -SO3 --opt-code-size less64.c -o less64s -create-app
