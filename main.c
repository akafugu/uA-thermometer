/*
 * uA-thermometer for ATTiny13
 * (C) 2011 Akafugu Corporation
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "onewire.h"

// duty cycles for temperatures 0C ~ 50C calibrated for 5V with 100kohm resistor
extern const uint8_t temp[51] PROGMEM;
const uint8_t temp[51] PROGMEM = {   0,
				     0,   6,  11,  16,  21,  26,  32,  37,  43,  48, // 10
					54,  59,  65,  70,  75,  82,  86,  92,  98, 103, // 20
				   110, 115, 121, 126, 131, 137, 144, 148, 154, 159, // 30
				   163, 168, 173, 178, 184, 189, 194, 200, 205, 211, // 40
				   216, 222, 226, 232, 237, 242, 247, 255, 255, 255 }; // 50

void show_temp(uint8_t t)
{
	OCR0A = pgm_read_byte(temp + t);
}

void WaitLoop(void)
{
	for (uint8_t i = 0; i < 10; i++)
		_delay_ms(1000);
}

int main(void)
{
	// Initialize PWM
	// CS00 -> no prescaler for clock
	// COM0A1 -> Compare Match Output A Mode
	// WGM00 and WGM01 -> fast PWM mode
	TCCR0A |=(1<<WGM00)|(1<<WGM01)|(1<<COM0A1)|(1<<CS00);
	TCCR0B = (1<<CS00);

	// Set PWM pin as output
	DDRB |= (1<<PB0);

	// Throw away initial temperature measurement
	start_meas();
	read_meas();
	
	// Splash screen
	{
		OCR0A = 0;
		_delay_ms(1000);
		OCR0A = 103;
		_delay_ms(500);
		OCR0A = 48;
		_delay_ms(500);
		OCR0A = 159;
		_delay_ms(500);
		OCR0A = 103;
		_delay_ms(500);
		OCR0A = 211;
		_delay_ms(500);
		OCR0A = 159;
		_delay_ms(500);
		OCR0A = 255;
		_delay_ms(500);
		OCR0A = 211;
		_delay_ms(500);
		OCR0A = 255;
		_delay_ms(1000);
	}

	while (1) {
		start_meas();
		show_temp(read_meas() / 100);
		WaitLoop();

		/* test code
		for (temp =	1; temp <= 50; temp++) {
			show_temp(temp);
			_delay_ms(2000);
		}
		*/
   }
}

