// /snap/bin/z88dk.zcc +zxn -vn -subtype=dot -startup=16 -clib=sdcc_iy -SO3 --max-allocs-per-node200000 --opt-code-size less64.c font_8x8_iso88592.asm -o less64 -create-app
// /snap/bin/z88dk.zcc +zx -vn -subtype=dot -startup=4 -clib=sdcc_iy -SO3 --opt-code-size less64.c -o less64s -create-app

#pragma output CLIB_EXIT_STACK_SIZE = 3

#if __ZXNEXT
#pragma redirect CRT_OTERM_FONT_8X8 = _font_8x8_iso8859_2
//#pragma output CRT_ENABLE_COMMANDLINE_EX = 0x80
#pragma output NEXTOS_VERSION = 0
#endif

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <input.h>
#include <z80.h>

#if __ZXNEXT
#include <arch/zxn/esxdos.h>
#else
#include <arch/zx/esxdos.h>
#endif

#define WIDTH 64
#define MAX_PAGE 14000

unsigned char in255;
unsigned char f;
unsigned char changed = 1;
unsigned char lasts = 1;
int current_page = 0;
unsigned char *bufor2 = (unsigned char *)32768;
unsigned long start = 0;
int *pages_offset = (int *)(32786 + 2048);

void
begin(void)
{
    current_page = 0;
    start = 0;
    changed = 1;
}

void
page_up(void)
{
    if (!current_page) {
        return;
    }
    start -= pages_offset[--current_page];
    changed = 1;
}

void
page_down(void)
{
    if (!pages_offset[current_page]) {
        return;
    }
    if (current_page >= MAX_PAGE) {
        return;
    }
    start += pages_offset[current_page++];
    changed = 1;
}

void
quit(void)
{
    ioctl(1, IOCTL_OTERM_CLS);
    lasts = 0;
}

int
draw_screen(void)
{
    unsigned char y;
    unsigned char *buf = bufor2;
    unsigned char was_new_page_or_zero = 0;

    ioctl(1, IOCTL_OTERM_SET_CURSOR_COORD, 0, 0);
    for (y = 0; y < 24; y++) {
        unsigned char was_enter = 0;
        unsigned char x;

        for (x = 0; x < WIDTH; x++) {
            if (was_enter || was_new_page_or_zero) {
                putchar(' ');
            } else {
                if (!*buf) {
                    was_new_page_or_zero = 1;
                    putchar(' ');
                    continue;
                }
                if (*buf == 12) {
                   was_new_page_or_zero = 1;
                   putchar(' ');
                   buf++;
                   continue;
                }
                if (*buf == '\n') {
                    was_enter = 1;
                    putchar(' ');
                    buf++;
                    continue;
                }
                if (*buf == 7 || *buf == 13) {
                    buf++;
                    x--;
                    continue;
                }
                putchar(*buf++);
            }
        }
    }

    return buf - bufor2;
}

void
help(void)
{
    ioctl(1, IOCTL_OTERM_CLS);
    puts("Keys:\nb Page up\nspace Page down\ng Go to begin of text\nh Help\nq Quit");
    in_wait_key();
}

void
see(void)
{
    int count;

    esxdos_f_seek(f, start, SEEK_SET);
    count = esxdos_f_read(f, bufor2, WIDTH * 24);
    bufor2[count] = '\0';

    pages_offset[current_page] = draw_screen();
    changed = 0;
}

int
main(int argc, char **argv)
{
#if __ZXNEXT
__asm
    in a,(255)
    ld (_in255),a
    and a,62
    or a,6
    out (255),a
__endasm;
    if (argc >= 1) {
        f = esxdos_f_open(argv[0], ESXDOS_MODE_OPEN_EXIST | ESXDOS_MODE_R);
#else
    if (argc > 1) {
        f = esxdos_f_open(argv[1], ESXDOS_MODE_OPEN_EXIST | ESXDOS_MODE_R);
#endif
        if (errno) {
            goto exit;
        }
        while (lasts) {
            if (changed) {
                see();
            }
            in_wait_key();

            switch (in_inkey()) {
            case 'q':
            case 'Q':
                quit();
                break;
            case ' ':
                page_down();
                break;
            case 'b':
            case 'B':
                page_up();
                break;
            case 'g':
            case 'G':
                begin();
                break;
            case 'h':
            case 'H':
                help();
                break;
            default:
                break;
            }
        }
        esxdos_f_close(f);
    }
exit:
#if __ZXNEXT
__asm
    ld a,(_in255)
    out (255),a
__endasm;
#endif
    return 0;
}
