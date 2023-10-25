/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols, Jarrod Bennett, Cody Burnett
 * Modified by Michael Blauberg
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

uint16_t game_speed;
bool manual_mode = false;

/////////////////////////////// main //////////////////////////////////
int main(void)
{
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Loop forever and continuously play the game.
	while(1)
	{
		start_screen();
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void)
{
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200, 0);
	
	init_timer0();
	init_timer1();
	init_timer2();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void)
{
	// Clear terminal screen and output a message
	clear_terminal();
	show_cursor();
	clear_terminal();
	hide_cursor();
	set_display_attribute(FG_WHITE);
	move_terminal_cursor(10,4);
	printf_P(PSTR("  ______   __     __  _______         __    __"));
	move_terminal_cursor(10,5);
	printf_P(PSTR(" /      \\ |  \\   |  \\|       \\       |  \\  |  \\"));
	move_terminal_cursor(10,6);
	printf_P(PSTR("|  $$$$$$\\| $$   | $$| $$$$$$$\\      | $$  | $$  ______    ______    ______"));
	move_terminal_cursor(10,7);
	printf_P(PSTR("| $$__| $$| $$   | $$| $$__| $$      | $$__| $$ /      \\  /      \\  /      \\"));
	move_terminal_cursor(10,8);
	printf_P(PSTR("| $$    $$ \\$$\\ /  $$| $$    $$      | $$    $$|  $$$$$$\\|  $$$$$$\\|  $$$$$$\\"));
	move_terminal_cursor(10,9);
	printf_P(PSTR("| $$$$$$$$  \\$$\\  $$ | $$$$$$$\\      | $$$$$$$$| $$    $$| $$   \\$$| $$  | $$"));
	move_terminal_cursor(10,10);
	printf_P(PSTR("| $$  | $$   \\$$ $$  | $$  | $$      | $$  | $$| $$$$$$$$| $$      | $$__/ $$"));
	move_terminal_cursor(10,11);
	printf_P(PSTR("| $$  | $$    \\$$$   | $$  | $$      | $$  | $$ \\$$     \\| $$       \\$$    $$"));
	move_terminal_cursor(10,12);
	printf_P(PSTR(" \\$$   \\$$     \\$     \\$$   \\$$       \\$$   \\$$  \\$$$$$$$ \\$$        \\$$$$$$"));
	move_terminal_cursor(10,14);
	// Name and student number;
	printf_P(PSTR("CSSE2010/7201 A2 by Michael Blauberg - s4588982"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	show_start_screen();

	uint32_t last_screen_update, current_time;
	last_screen_update = get_current_time();
	
	uint8_t frame_number = 0;
	game_speed = 1000;

	// Game speed
	move_terminal_cursor(10,16);
	printf_P(PSTR("Game speed: %1d"), game_speed/100);
	// Selected track
	move_terminal_cursor(10,17);
	printf_P(PSTR("Track: "));

	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1)
	{
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S')
		{
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED)
		{
			break;
		}

		// every 200 ms, update the animation
		current_time = get_current_time();
		if (current_time - last_screen_update > game_speed/5)
		{
			update_start_screen(frame_number);
			frame_number = (frame_number + 1) % 32;
			last_screen_update = current_time;
		}
	}
}

void new_game(void)
{
	// Clear the serial terminal
	clear_terminal();
	
	// Display countdown
	uint32_t last_screen_update, current_time;
	last_screen_update = get_current_time();
	
	uint8_t timer = 0;
	while (timer < 5)
	{
		current_time = get_current_time();
		if (current_time >= last_screen_update + game_speed*2/5)
		{
			display_countdown(game_speed, timer);
			last_screen_update = current_time;
			timer++;
		}
	}

	// Initialise the game and display
	initialise_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void)
{
	
	uint32_t last_advance_time, current_time;
	uint8_t btn; // The button pushed
	
	last_advance_time = get_current_time();
	
	// We play the game until it's over
	while (!is_game_over())
	{
		// Update score on terminal
		move_terminal_cursor(10,4);
		printf_P(PSTR("Game Score: %3d"), score);


		// Check for button push
		btn = button_pushed();

		// Check for serial input
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
		}

		// Play note based on input
		if (btn == BUTTON0_PUSHED || serial_input == 'f' || serial_input == 'F')
		{
			// If button 0 or 'f'/'F' play the lowest note (right lane)
			play_note(0);
		}
		if (btn == BUTTON1_PUSHED || serial_input == 'd' || serial_input == 'D')
		{
			// If button 1 or 'd'/'D' play the second lowest note (second from right lane)
			play_note(1);
		}
		if (btn == BUTTON2_PUSHED || serial_input == 's' || serial_input == 'S')
		{
			// If button 2 or 's'/'S' play the second highest note (second from left lane)
			play_note(2);
		}
		if (btn == BUTTON3_PUSHED || serial_input == 'a' || serial_input == 'A')
		{
			// If button 3 or 'a'/'A' play the highest note (left lane)
			play_note(3);
		}

		// Check for serial manual mode input
		if (serial_input == 'm' || serial_input == 'M')
		{
			// If 'm'/'M' is pressed, toggle manual mode
			manual_mode = !manual_mode;
			if (manual_mode)
			{
				// If manual mode is on, update the display
				move_terminal_cursor(10,6);
				printf_P(PSTR("MANUAL MODE ACTIVE"));
			}
			else
			{
				// If manual mode is off, update the display
				move_terminal_cursor(10,6);
				printf_P(PSTR("                   "));
			}
		}
		
		current_time = get_current_time();
		if (current_time >= last_advance_time + game_speed/5 && 
		(!manual_mode || (manual_mode && (serial_input == 'n' || serial_input == 'N'))))
		{
			// 200ms (0.2 second) has passed since the last time we advance the
			// notes here, so update the advance the notes
			advance_note();
			
			// Update the most recent time the notes were advance
			last_advance_time = current_time;
		}
	}
	// We get here if the game is over.
}

void handle_game_over(void)
{
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button or 's'/'S' to start a new game"));
	
	// Do nothing until a button or 's'/'S' is pushed.
	while(1)
	{
		// Check for serial input
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the end screen
		if (serial_input == 's' || serial_input == 'S')
		{
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED)
		{
			break;
		}
	}
}
