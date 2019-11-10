/*	example of MMC3 for cc65
 *	Doug Fraker 2019
 */	
 
#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 
#include "MMC3/mmc3_code.h"
#include "MMC3/mmc3_code.c"


 
#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES

unsigned char arg1;
unsigned char arg2;
unsigned char pad1;
unsigned char pad1_new;
unsigned char char_state;
unsigned char song;
unsigned char sound;
unsigned char pauze;

#pragma bss-name(pop)


#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff
unsigned char wram_array[0x2000];

#pragma bss-name(pop)




#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30

const unsigned char palette[]={
BLACK, DK_GY, LT_GY, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 


// test putting things in other banks
#pragma rodata-name ("BANK0")
#pragma code-name ("BANK0")
const unsigned char TEXT0[]="BANK0";

void function_bank0(void){
	ppu_off();
	vram_adr(NTADR_A(1,4));
	vram_write(TEXT0,sizeof(TEXT0));
	ppu_on_all();
}


#pragma rodata-name ("BANK1")
#pragma code-name ("BANK1")
const unsigned char TEXT1[]="BANK1";

void function_bank2(void);	// a prototype can be anywhere
							// as long as it's above the code that uses it

void function_bank1(void){
	ppu_off();
	vram_adr(NTADR_A(1,6));
	vram_write(TEXT1,sizeof(TEXT1));
	ppu_on_all();
	
	banked_call(BANK_2, function_bank2);
}


#pragma rodata-name ("BANK2")
#pragma code-name ("BANK2")
const unsigned char TEXT2[]="BANK2";

void function_bank2(void){
	ppu_off();
	vram_adr(NTADR_A(1,8));
	vram_write(TEXT2,sizeof(TEXT2));
	ppu_on_all();
}


#pragma rodata-name ("BANK3")
#pragma code-name ("BANK3")
const unsigned char TEXT3[]="BANK3";

void function_bank3(void){
	 
	ppu_off();
	vram_adr(NTADR_A(1,10));
	vram_write(TEXT3,sizeof(TEXT3));
	
	vram_put(0);
	vram_put(arg1); // these args were passed via globals
	vram_put(arg2);
	ppu_on_all();
}


#pragma rodata-name ("BANK6")
#pragma code-name ("BANK6")
const unsigned char TEXT6[]="BANK6";

void function_bank6(void){
	 
	ppu_off();
	vram_adr(NTADR_A(1,16));
	vram_write(TEXT6,sizeof(TEXT6));
	
	vram_put(0);
	vram_put(wram_array[0]); // testing the $6000-7fff area
	vram_put(wram_array[2]); // should print A, C
	ppu_on_all();
}
	
	
	
	
// the fixed bank, bank 7
	
#pragma rodata-name ("CODE")
#pragma code-name ("CODE")	

const unsigned char text[]="BACK IN FIXED BANK, 7";

void draw_sprites (void);




void main (void) {
	
	bank_spr(1);

	wram_array[0] = 'A'; // put some values at $6000-7fff
	wram_array[2] = 'C'; // for later testing
	
	ppu_off(); // screen off
	pal_bg(palette); //	load the BG palette
	pal_spr(palette); // load the sprite palette
	ppu_on_all(); // turn on screen
	
	
	// calling functions in other banks
	
	banked_call(0, function_bank0);
	banked_call(1, function_bank1);
	// banked_call(2, function_bank2); // moved to bank 1
	
	arg1 = 'G'; // must pass arguments with globals
	arg2 = '4';
	banked_call(3, function_bank3);
	banked_call(6, function_bank6);

	 
	ppu_off(); // screen off
	vram_adr(NTADR_A(1,18));
	vram_write(text,sizeof(text)); // code running from the fixed bank
	
	
	// set the bank 0 at $8000-bfff
	// with a swappable bank in place, we can read it from the fixed bank
	set_prg_bank(0); 
	vram_adr(NTADR_A(1,20));
	vram_write(TEXT0,sizeof(TEXT0)); // this array is in bank 0
	
	ppu_on_all(); //	turn on screen
	
	while (1){ // infinite loop
		ppu_wait_nmi();
		
		pad1 = pad_poll(0);
		pad1_new = get_pad_new(0);
		
		
	}
}

	
// testing sprites using the second tileset	
void draw_sprites (void) { 
//	bank_spr(1);
	oam_clear();
//	oam_spr(0x50,0x10,0,0);
//	oam_spr(0x58,0x10,1,0);
//	oam_spr(0x50,0x18,0x10,0);
//	oam_spr(0x58,0x18,0x11,0);
}

