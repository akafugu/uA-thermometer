/* 
Access Dallas 1-Wire Devices with ATMEL AVRs
Author of the initial code: Peter Dannegger (danni(at)specs.de)
modified by Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 9/2004 - use of delay.h, optional bus configuration at runtime
10/2009 - additional delay in ow_bit_io for recovery
 5/2010 - timing modifcations, additonal config-values and comments,
          use of atomic.h macros, internal pull-up support
 7/2010 - added method to skip recovery time after last bit transfered
          via ow_command_skip_last_recovery
*/

#ifndef ONEWIRE_H__
#define ONEWIRE_H__

#define W1_PIN	PB3
#define W1_IN	PINB
#define W1_OUT	PORTB
#define W1_DDR	DDRB

#define MATCH_ROM	0x55
#define SKIP_ROM	0xCC
#define	SEARCH_ROM	0xF0

#define CONVERT_T	0x44		// DS1820 commands
#define READ		0xBE
#define WRITE		0x4E
#define EE_WRITE	0x48
#define EE_RECALL	0xB8

#define	SEARCH_FIRST	0xFF		// start new search
#define	PRESENCE_ERR	0xFF
#define	DATA_ERR	0xFE
#define LAST_DEVICE	0x00		// last device found
//			0x01 ... 0x40: continue searching

uint8_t w1_reset(void);

uint8_t w1_byte_wr(uint8_t b);
uint8_t w1_byte_rd(void);

uint8_t w1_rom_search(uint8_t diff, uint8_t* id);

void w1_command(uint8_t command, uint8_t* id);


// temperature
void start_meas(void);
uint16_t read_meas(void);


#endif // ONEWIRE_H__
