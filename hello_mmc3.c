/*	example of MMC3 for cc65
 *	Doug Fraker 2019
 *	Feb 2023 version
 */	
 
#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 
#include "MMC3/mmc3_code.h"
#include "MMC3/mmc3_code.c"
#include "Sprites.h"

 
#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES

unsigned char arg1;
unsigned char arg2;
unsigned char pad1;
unsigned char pad1_new;
unsigned char char_state;

// fixed point 8.8
unsigned int scroll_top;
unsigned int scroll2;
unsigned int scroll3;
unsigned int scroll4;
unsigned char temp;

unsigned char sprite_x;
unsigned char sprite_y;
unsigned char dirLR;


#pragma bss-name(pop)
// should be in the regular 0x300 ram now

unsigned char irq_array[32];
unsigned char double_buffer[32];


#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff
unsigned char wram_array[0x2000];

#pragma bss-name(pop)






const unsigned char palette_bg[]={
0x0f, 0, 0x10, 0x30,
0x0f, 0, 0, 0,
0x0f, 0, 0, 0,
0x0f, 0, 0, 0
}; 

const unsigned char palette_spr[]={
0x0f, 0x09, 0x19, 0x29, // greens
0x0f, 0, 0, 0,
0x0f, 0, 0, 0,
0x0f, 0, 0, 0
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
	
	banked_call(2, function_bank2);
	// calling a function in another bank, use banked_call()
}


#pragma rodata-name ("BANK2")
#pragma code-name ("BANK2")
const unsigned char TEXT2[]="BANK2";

void function_same_bank(){
	vram_put(0);
	vram_put('H');
	vram_put('I');
}

void function_bank2(void){
	ppu_off();
	vram_adr(NTADR_A(1,8));
	vram_write(TEXT2,sizeof(TEXT2));

	function_same_bank();
	// calling a function in same bank, use regular function calls
	
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
	vram_adr(NTADR_A(1,14));
	vram_write(TEXT6,sizeof(TEXT6));
	
	vram_put(0);
	vram_put(wram_array[0]); // testing the $6000-7fff area
	vram_put(wram_array[2]); // should print A, C
	ppu_on_all();
}
	
	
	
	
// the fixed bank, bank 7
	
#pragma rodata-name ("CODE")
#pragma code-name ("CODE")	

const unsigned char text[]="BACK IN FIXED BANK";

void draw_sprites (void);




void main (void) {
	
	set_mirroring(MIRROR_HORIZONTAL);
	bank_spr(1);
	irq_array[0] = 0xff; // end of data
	set_irq_ptr(irq_array); // point to this array
	
	
	// clear the WRAM, not done by the init code
	// memfill(void *dst,unsigned char value,unsigned int len);
	memfill(wram_array,0,0x2000); 
	
	
	wram_array[0] = 'A'; // put some values at $6000-7fff
	wram_array[2] = 'C'; // for later testing

	
	ppu_off(); // screen off
	pal_bg(palette_bg); //	load the BG palette
	pal_spr(palette_spr); // load the sprite palette
	
	// draw some things
	vram_adr(NTADR_A(20,3)); // gear and squares
	vram_put(0xc0);
	vram_put(0xc1);
	vram_put(0xc2);
	vram_put(0xc3);
	vram_adr(NTADR_A(20,4));
	vram_put(0xd0);
	vram_put(0xd1);
	vram_put(0xd2);
	vram_put(0xd3);
	vram_adr(NTADR_A(20,7));
	vram_put(0xc0);
	vram_put(0xc1);
	vram_put(0xc2);
	vram_put(0xc3);
	vram_adr(NTADR_A(20,8));
	vram_put(0xd0);
	vram_put(0xd1);
	vram_put(0xd2);
	vram_put(0xd3);
	
	vram_adr(NTADR_A(20,5)); // blocks of color
	vram_put(0x2);
	vram_put(0x2);
	vram_put(0x2);
	vram_put(0x2);
	vram_adr(NTADR_A(20,9));
	vram_put(0x2);
	vram_put(0x2);
	vram_put(0x2);
	vram_put(0x2);
	vram_adr(NTADR_A(20,13));
	vram_put(0x2);
	vram_put(0x2);
	vram_put(0x2);
	vram_put(0x2);
    
	music_play(0);
	//ppu_on_all(); // turn on screen
	
	set_chr_mode_5(8); // make sure the gear tiles loaded
                       // for the first few frames
	
	
	// calling functions in other banks
	
	banked_call(0, function_bank0);
	banked_call(1, function_bank1);
	// banked_call(2, function_bank2); // moved to bank 1
	
	arg1 = 'G'; // must pass arguments with globals
	arg2 = '4';
	banked_call(3, function_bank3);
	banked_call(6, function_bank6);

	 
	ppu_off(); // screen off
	vram_adr(NTADR_A(1,16));
	vram_write(text,sizeof(text));

	sprite_x = 0x50;
	sprite_y = 0x30;
	draw_sprites();
	
	ppu_on_all(); //	turn on screen
	

	
	while (1){ // infinite loop
		ppu_wait_nmi();
		
		pad1 = pad_poll(0);
		pad1_new = get_pad_new(0);
		
		if(pad1 & PAD_A){ // shift screen right = subtract from scroll
			scroll_top -= 0x80; // sub pixel movement
			scroll2 -= 0x100; // 1 pixel
			scroll3 -= 0x180;
			scroll4 -= 0x200; 
		}
		
		if(pad1 & PAD_B){ // shift screen right = subtract from scroll
			scroll_top += 0x80; // sub pixel movement
			scroll2 += 0x100; // 1 pixel
			scroll3 += 0x180;
			scroll4 += 0x200;
		}
		
		temp = scroll_top >> 8;
		set_scroll_x(temp);
		
		if((get_frame_count() & 0x03) == 0){ // every 4th frame
			++char_state;
			if(char_state >=4) char_state = 0;
		}
		
		
		// load the irq array with values it can parse
		// ! CHANGED it, double buffered so we aren't editing the same
		// array that the irq system is reading from
		
		// top of the screen, probably read in v-blank
		// put whole screen things here, before the split
		double_buffer[0] = 0xfc; // CHR mode 5, change the 0xc00-0xfff tiles
		double_buffer[1] = 8; // top of the screen, static value
		double_buffer[2] = 47; // value < 0xf0 = scanline count, 1 less than #

		// after the first split
		double_buffer[3] = 0xf5; // H scroll change, do first for timing
		temp = scroll2 >> 8;
		double_buffer[4] = temp; // scroll value
		double_buffer[5] = 0xfc; // CHR mode 5, change the 0xc00-0xfff tiles
		double_buffer[6] = 8 + char_state; // value = 8,9,10,or 11
		double_buffer[7] = 30; // scanline count
		
		// after the 2nd split
		double_buffer[8] = 0xf5; // H scroll change
		temp = scroll3 >> 8;
		double_buffer[9] = temp; // scroll value
			double_buffer[10] = 0xf1; // $2001 test changing color emphasis
			double_buffer[11] = 0xfe; // value COL_EMP_DARK 0xe0 + 0x1e
			//double_buffer[11] = 0x1f; // alternate value for grayscale
		double_buffer[12] = 30; // scanline count
		
		// after the 3rd split
		double_buffer[13] = 0xf5; // H scroll change
		temp = scroll4 >> 8;
		double_buffer[14] = temp; // scroll value
		double_buffer[15] = 30; // scanline count
		
		double_buffer[16] = 0xf5; // H scroll change
		double_buffer[17] = 0; // to zero, to set the fine X scroll
		double_buffer[18] = 0xf6; // 2 writes to 2006 shifts screen
		double_buffer[19] = 0x20; // need 2 values...
		double_buffer[20] = 0x00; // PPU address $2000 = top of screen
		
		double_buffer[21] = 0xff; // end of data
		
		
		if(pad1 & PAD_LEFT){ // shift screen right = subtract from scroll
			sprite_x -= 1;
			dirLR = 0;
		}
		else if(pad1 & PAD_RIGHT){ // shift screen right = subtract from scroll
			sprite_x += 1;
			dirLR = 1;
		}
		if(pad1 & PAD_UP){ // shift screen right = subtract from scroll
			sprite_y -= 1;
		}
		else if(pad1 & PAD_DOWN){ // shift screen right = subtract from scroll
			sprite_y += 1;
		}
		
		draw_sprites();
		
		// wait till the irq system is done before changing it
		// this could waste a lot of CPU time, so we do it last
		while(!is_irq_done() ){ // have we reached the 0xff, end of data
							// is_irq_done() returns zero if not done
			// do nothing while we wait
		}
		
		// copy from double_buffer to the irq_array
		// memcpy(void *dst,void *src,unsigned int len);
		memcpy(irq_array, double_buffer, sizeof(irq_array)); 
	}
}



	
// testing sprites using the second tileset	
void draw_sprites (void) { 
	oam_clear();
	if(!dirLR) { // left
		oam_meta_spr(sprite_x, sprite_y, RoundSprL);
	}
	else {
		oam_meta_spr(sprite_x, sprite_y, RoundSprR);
	}
	
}

