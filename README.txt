README - MMC3 code for neslib based C (cc65)
(version 1)

DEMO CONTROLS
Up/Down/Right/Left - move sprites
A or B - scroll the screen


 
In this configuration, A000-FFFF is fixed and only 8000-9FFF 
is swappable.Use the banked_call() system that cppchriscpp 
designed. If you are calling a function in a swappable bank, 
use a banked_call().

CHR (graphics) is set to "INVERTED", so that we can cycle through 
BG tiles using the smallest amount of tile banks.
You can change this (top of mmc3_code.asm)...A12_INVERT...to zero.

The A12_INVERT is a CONSTANT value in this code. I suppose you could 
make it variable...with many changes to the asm code.

;if invert bit is 0 - this is if you want tiny sprite banks
;mode 0 changes $0000-$07FF
;mode 1 changes $0800-$0FFF
;mode 2 changes $1000-$13FF
;mode 3 changes $1400-$17FF
;mode 4 changes $1800-$1BFF
;mode 5 changes $1C00-$1FFF

;if invert bit is $80 - this is if you want tiny BG banks
;mode 0 changes $1000-$17FF
;mode 1 changes $1800-$1FFF
;mode 2 changes $0000-$03FF
;mode 3 changes $0400-$07FF
;mode 4 changes $0800-$0BFF
;mode 5 changes $0C00-$0FFF

For the scanline IRQ to work properly, you must set BG to use set 
#0 and sprites to use #1. And the screen needs to be ON.

I have it planned so that the main code only does PRG bank changes
and the IRQ code only does CHR bank changes. So they don't conflict.
(they share the same register pair, $8000/8001)
You could disregard this plan, and have the main code do CHR changes,
but make sure the IRQ system isn't doing any CHR changes. But, even
if you don't want split screens, the IRQ system can be told to change
CHR banks, at the end of the NMI code it does an obligatory call to
the IRQ parser.

The scanline IRQ uses an automated system that I developed. It uses 
a char array that describes how the screen will set scanline counts, 
and gives you some options about what happens at that scanline.

IRQ SYSTEM FORMAT = 
value < 0xf0, it's a scanline count
zero is valid, it triggers an IRQ at the end of the current line

if >= 0xf0...
f0 = 2000 write, next byte is write value
f1 = 2001 write, next byte is write value
f2-f4 unused - future TODO ?
f5 = 2005 write, next byte is H Scroll value
f6 = 2006 write, next 2 bytes are write values

f7 = change CHR mode 0, next byte is write value
f8 = change CHR mode 1, next byte is write value
f9 = change CHR mode 2, next byte is write value
fa = change CHR mode 3, next byte is write value
fb = change CHR mode 4, next byte is write value
fc = change CHR mode 5, next byte is write value

fd = very short wait, no following byte 
fe = short wait, next byte is quick loop value
(for fine tuning timing of things)

ff = end of data set
Once it sees a scanline value or an 0xff, it exits the parser.

So, if you want a CHR swap to affect the entire screen, put it first,
then the first IRQ scanline count (or 0xff end).
The next thing after a scanline count won't happen until the next
IRQ fires. You can then change scroll, or whatever.

Small example...
irq_array[0] = 47; // wait 48 lines
...
irq_array[1] = 0xf5; // 2005, H scroll change
irq_array[2] = 0; // change it to zero
irq_array[3] = 0xff; // end of data

Note: the very top of the screen will use the H scroll value passed to
the neslib scroll() function (or my set_scroll_x function).

Passing the array address to set_irq_ptr() turns on the system. Try
not to extend your game logic past 1 frame and only have the array
half written... something bad could happen. The system can't swap
PRG banks, so, it shouldn't crash the game.

Note: use extreme caution writing to the $2000 register. I suppose you
could change the nametable selection, but don't change the other bits
or something bad will happen.




Other functions
---------------
set_prg_8000() change the swapped bank
get_prg_8000() get the bank #
set_mirroring() change the mirroring
disable_irq() turn off the irq system
set_irq_ptr() pass the address of an array to the system, turn it on

set_chr_mode_0() change a chr bank
set_chr_mode_1() etc
set_chr_mode_2()
set_chr_mode_3()
set_chr_mode_4()
set_chr_mode_5()


I wouldn't recommend using these functions, but they are available...

set_prg_a000() !! could crash with the current cfg
set_wram_mode() you shouldn't need to touch this


Just to repeat. To call a function in a swapped bank, use banked_call().
It will automatically swap the correct bank in, and call the function.
If you just need to access data stored in a swappable bank, then you
can call set_prg_8000() to put it in place, first, before reading the 
data.

To swap CHR banks, either use the irq system, or call one of the CHR
functions, but make sure the irq system isn't also changing CHR banks.



IRQ SYSTEM WAIT TIMINGS
 
fd short wait 1/10 of scanline (36 pixels)
fe, wait 0 = 100 pixels, or 1/3 scanline
fe, wait 5 = 1/2 scanline
fe, wait 16 = 1 scanline
fe, wait 39 ($27) = 2 scanlines
wait x scanlines = ((x-1)*23) + 15
fe, wait 255 ($ff) = 11 1/2 scanlines

These could be used to better time writes to 2005 or 2006


A hardware note
$8000 register = CP-- -RRR	
; P = 0, set PRG $8000 swappable and $c000 fixed to the next to last bank
; P = 1, set PRG $c000 swappable and $8000 fixed to the next to last bank
; C = 0, set CHR two 2 KB banks at $0000-$0FFF, four 1 KB banks at $1000-$1FFF
; C = 1, set CHR two 2 KB banks at $1000-$1FFF, four 1 KB banks at $0000-$0FFF
; C called "A12_INVERT" at top of mmc3_code.asm
; RRR = mode

MODES
0: Select 2 KB CHR bank at PPU $0000-$07FF (or $1000-$17FF)*
1: Select 2 KB CHR bank at PPU $0800-$0FFF (or $1800-$1FFF)*
2: Select 1 KB CHR bank at PPU $1000-$13FF (or $0000-$03FF)*
3: Select 1 KB CHR bank at PPU $1400-$17FF (or $0400-$07FF)*
4: Select 1 KB CHR bank at PPU $1800-$1BFF (or $0800-$0BFF)*
5: Select 1 KB CHR bank at PPU $1C00-$1FFF (or $0C00-$0FFF)*

6: Select 8 KB PRG ROM bank at $8000-$9FFF (or $C000-$DFFF)**
7: Select 8 KB PRG ROM bank at $A000-$BFFF

*depending on C value "A12_INVERT"
**depending on P value, the other will be fixed to the next to last bank



