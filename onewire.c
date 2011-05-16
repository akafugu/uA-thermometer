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

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "onewire.h"

#define NULL 0

uint8_t w1_reset(void)
{
  uint8_t err;

  W1_OUT &= ~(1<<W1_PIN);
  W1_DDR |= 1<<W1_PIN;
  _delay_us(480);
  cli();
  W1_DDR &= ~(1<<W1_PIN);
  _delay_us(66);
  err = W1_IN & (1<<W1_PIN);			// no presence detect
  sei();
  _delay_us(414); // 480 - 66
  if( (W1_IN & (1<<W1_PIN)) == 0 )		// short circuit
    err = 1;
  return err;
}


uint8_t w1_bit_io(uint8_t b)
{
  cli();
  W1_DDR |= 1<<W1_PIN;
  _delay_us(1);
  if( b )
    W1_DDR &= ~(1<<W1_PIN);
  _delay_us(14); // 15 - 1
  if( (W1_IN & (1<<W1_PIN)) == 0 )
    b = 0;
  _delay_us(45); // 60 - 15
  W1_DDR &= ~(1<<W1_PIN);
  sei();
  return b;
}


uint8_t w1_byte_wr(uint8_t b)
{
  uint8_t i = 8, j;
  do{
    j = w1_bit_io( b & 1 );
    b >>= 1;
    if( j )
      b |= 0x80;
  }while( --i );
  return b;
}


uint8_t w1_byte_rd(void)
{
  return w1_byte_wr( 0xFF );
}


uint8_t w1_rom_search(uint8_t diff, uint8_t* id)
{
  uint8_t i, j, next_diff;
  uint8_t b;

  if( w1_reset() )
    return PRESENCE_ERR;			// error, no device found
  w1_byte_wr( SEARCH_ROM );			// ROM search command
  next_diff = LAST_DEVICE;			// unchanged on last device
  i = 8 * 8;					// 8 bytes
  do{
    j = 8;					// 8 bits
    do{
      b = w1_bit_io( 1 );			// read bit
      if( w1_bit_io( 1 ) ){			// read complement bit
	if( b )					// 11
	  return DATA_ERR;			// data error
      }else{
	if( !b ){				// 00 = 2 devices
	  if( diff > i ||
	    ((*id & 1) && diff != i) ){
	    b = 1;				// now 1
	    next_diff = i;			// next pass 0
	  }
	}
      }
      w1_bit_io( b );     			// write bit
      *id >>= 1;
      if( b )					// store bit
	*id |= 0x80;
      i--;
    }while( --j );
    id++;					// next byte
  }while( i );
  return next_diff;				// to continue search
}


void w1_command(uint8_t command, uint8_t* id)
{
  uint8_t i;
  w1_reset();
  if( id ){
    w1_byte_wr( MATCH_ROM );			// to a single device
    i = 8;
    do{
      w1_byte_wr( *id );
      id++;
    }while( --i );
  }else{
    w1_byte_wr( SKIP_ROM );			// to all devices
  }
  w1_byte_wr( command );
}


void start_meas(void)
{
  if (W1_IN & _BV(W1_PIN)) {
    w1_command( CONVERT_T, NULL );
    W1_OUT |= _BV(W1_PIN);
    W1_DDR |= _BV(W1_PIN); // parasite power on
  }
}


uint16_t read_meas(void)
{
  uint8_t id[8], diff;
  //uint8_t s[30];
  uint8_t i;
  uint16_t temp = 0;

  for( diff = SEARCH_FIRST; diff != LAST_DEVICE; ){
    diff = w1_rom_search( diff, id );

    if( diff == PRESENCE_ERR ){
      //uputsnl( "No Sensor found" );
      break;
    }
    if( diff == DATA_ERR ){
      //uputsnl( "Bus Error" );
      break;
    }
    if( id[0] == 0x28 || id[0] == 0x10 ){	// temperature sensor
      //uputs( "ID: " );
      for( i = 0; i < 8; i++ ){
	//sprintf( s, "%02X ", id[i] );
	//uputs( s );
      }
      w1_byte_wr( READ );			// read command
      temp = w1_byte_rd();			// low byte
      temp |= (uint16_t)w1_byte_rd() << 8;		// high byte
      if( id[0] == 0x10 )			// 9 -> 12 bit
        temp <<= 3;
      //sprintf( s, "  T: %04X = ", temp );	// hex value
      //uputs( s );
      //sprintf( s, "%4d.%01d¯C", temp >> 4, (temp << 12) / 6553 ); // 0.1¯C
      //uputsnl( s );
    }
  }
  //uputsnl( "" );
  
  return (temp >> 4) * 100 + ((temp << 12) / 6553) * 10;
  //return temp >> 4;
}
