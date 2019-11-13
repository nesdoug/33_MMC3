// Contains functions to help with working with multiple PRG/CHR banks
// For MMC3 code.

// Maximum level of recursion to allow with banked_call and similar functions.
#define MAX_BANK_DEPTH 10

unsigned char bankLevel;
unsigned char bankBuffer[MAX_BANK_DEPTH];




// Switch to another bank and call this function.
// Note: Using banked_call to call a second function from within  
// another banked_call is safe. This will break if you nest more  
// than 10 calls deep.
void banked_call(unsigned char bankId, void (*method)(void));


// Internal function used by banked_call(), don't call this directly.
// Switch to the given bank, and keep track of the current bank, so 
// that we may jump back to it as needed.
void bank_push(unsigned char bankId);


// Internal function used by banked_call(), don't call this directly.
// Go back to the last bank pushed on using bank_push.
void bank_pop(void);






// Switch to the given bank (at $8000-9fff). Your prior bank is not saved.
// Can be used for reading data with a function in the fixed bank.
// bank_id: The bank to switch to.
void __fastcall__ set_prg_8000(unsigned char bank_id);


// Get the current PRG bank at $8000-9fff.
// returns: The current bank.
unsigned char __fastcall__ get_prg_8000(void);


// WARNING, DON'T USE THIS IN THE CURRENT CFG, unless you
// really know what you're doing. El Crasho.
// Switch to the given bank (at $a000-bfff). Your prior bank is not saved.
// bank_id: The bank to switch to.
void __fastcall__ set_prg_a000(unsigned char bank_id);


// Changes a portion of the tilsets
// The plan was to NOT use these. Use the irq system instead
// You can, but make sure the IRQ system isn't changing CHR.
// See notes in README
void __fastcall__ set_chr_mode_0(unsigned char chr_id);
void __fastcall__ set_chr_mode_1(unsigned char chr_id);
void __fastcall__ set_chr_mode_2(unsigned char chr_id);
void __fastcall__ set_chr_mode_3(unsigned char chr_id);
void __fastcall__ set_chr_mode_4(unsigned char chr_id);
void __fastcall__ set_chr_mode_5(unsigned char chr_id);





#define MIRROR_VERTICAL 0
#define MIRROR_HORIZONTAL 1

// Set the current mirroring mode. Your options are 
// MIRROR_HORIZONTAL, and MIRROR_VERTICAL.
void __fastcall__ set_mirroring(unsigned char mirroring);


#define WRAM_OFF 0x40
#define WRAM_ON 0x80
#define WRAM_READ_ONLY 0xC0

// Set the WRAM mode. Your options are 
// WRAM_OFF, WRAM_ON, and WRAM_READ_ONLY.
// May not work in some emulators. Init code turns it ON.
void __fastcall__ set_wram_mode(unsigned char mode);


// Turns off MMC3 irqs, and changes the array pointer
// to point to a default 0xff
void disable_irq(void);


// This points an array to the IRQ system 
// Also turns ON the system
void set_irq_ptr(char * address);


// Check if it's safe to write to the irq array
// returns 0xff if done, zero if not done
// if the irq pointer is pointing to 0xff it is
// safe to edit.
unsigned char is_irq_done(void);














