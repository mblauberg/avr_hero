/*
 * display.c
 *
 * Authors: Luke Kamols, Jarrod Bennett, Martin Ploschner, Cody Burnett,
 * Renee Nightingale
 */ 

#include "display.h"
#include <stdio.h>
#include <avr/io.h>
#include "pixel_colour.h"
#include "ledmatrix.h"
#include "game.h"
#include "timer0.h"

// constant value used to display 'AVR HERO' on launch
static const uint8_t pong_display[MATRIX_NUM_COLUMNS] = 
		{127, 164, 127, 0, 239, 29, 233, 0, 255, 170, 85, 0, 6, 9, 6, 0};

// Fonts for LED Matrix score display
// Stored as a 5 x 3 grid pattern going from Left-to-Right, Top-to-Bottom
// Padded with a leading zero so that it fits into a 16-bit value


void show_start_screen(void)
{
	MatrixColumn column_colour_data;
	uint8_t col_data;
		
	ledmatrix_clear(); // start by clearing the LED matrix
	for (uint8_t col = 0; col < MATRIX_NUM_COLUMNS; col++)
	{
		col_data = pong_display[col];
		// go through the top 7 bits (not the bottom one as that was our colour bit)
		// and set any to be the correct colour
		for(uint8_t row = 0; row < MATRIX_NUM_ROWS; row++)
		{
			// If the relevant font bit is set, we make this a coloured pixel, else blank
			if(col_data>>row & 1)
			{
				column_colour_data[row] = (row < 4 ? COLOUR_RED : COLOUR_GREEN);
			}
			else
			{
				column_colour_data[row] = 0;
			}
		}
		ledmatrix_update_column(col, column_colour_data);
	}
	update_start_screen(0);
}

// Update dynamic start screen based on the frame number (0-31)
// Note: this is hardcoded to PONG game.
// Purposefully obfuscated so functionality cannot be copied for movement tasks
void update_start_screen(uint8_t frame_number)
{
	PixelColour colour;
	for (uint8_t row = 4; row < 8; row++)
	{
		for (uint8_t col = 11; col < 16; col++)
		{
			colour = col == 14 ? COLOUR_YELLOW : COLOUR_BLACK;
			if (((32+col-frame_number) & ((1<<(row-2))-1)) == (1<<(row-3))-1)
			{
				colour = col < 14 ? COLOUR_RED : COLOUR_GREEN;
			}
			ledmatrix_update_pixel(col, row, colour);
		}
	}
}

// Display countdown timer "3", "2", "1", "GO"
void display_countdown(uint8_t timer)
{
	// Create new MatrixData array
	MatrixData data = {0};

	switch (timer)
	{
		case 0:
			// Display "3"
			data[5][1] = COLOUR_RED;
			data[4][2] = COLOUR_RED;
			data[4][3] = COLOUR_RED;
			data[4][4] = COLOUR_RED;
			data[4][5] = COLOUR_RED;
			data[5][6] = COLOUR_RED;
			data[6][6] = COLOUR_RED;
			data[7][5] = COLOUR_RED;
			data[8][5] = COLOUR_RED;
			data[7][4] = COLOUR_RED;
			data[8][4] = COLOUR_RED;
			data[9][6] = COLOUR_RED;
			data[10][6] = COLOUR_RED;
			data[11][5] = COLOUR_RED;
			data[11][4] = COLOUR_RED;
			data[11][3] = COLOUR_RED;
			data[11][2] = COLOUR_RED;
			data[10][1] = COLOUR_RED;
			break;
		case 1:
			// Display "2"
			data[5][1] = COLOUR_ORANGE;
			data[4][2] = COLOUR_ORANGE;
			data[4][3] = COLOUR_ORANGE;
			data[4][4] = COLOUR_ORANGE;
			data[4][5] = COLOUR_ORANGE;
			data[5][6] = COLOUR_ORANGE;
			data[6][6] = COLOUR_ORANGE;
			data[7][6] = COLOUR_ORANGE;
			data[7][5] = COLOUR_ORANGE;
			data[8][5] = COLOUR_ORANGE;
			data[8][4] = COLOUR_ORANGE;
			data[8][3] = COLOUR_ORANGE;
			data[9][3] = COLOUR_ORANGE;
			data[9][2] = COLOUR_ORANGE;
			data[10][2] = COLOUR_ORANGE;
			data[10][1] = COLOUR_ORANGE;
			data[11][1] = COLOUR_ORANGE;
			data[11][2] = COLOUR_ORANGE;
			data[11][3] = COLOUR_ORANGE;
			data[11][4] = COLOUR_ORANGE;
			data[11][5] = COLOUR_ORANGE;
			data[11][6] = COLOUR_ORANGE;
			break;
		case 2:
			// Display "1"
			data[5][1] = COLOUR_YELLOW;
			data[5][2] = COLOUR_YELLOW;
			data[6][1] = COLOUR_YELLOW;
			data[6][2] = COLOUR_YELLOW;
			data[4][3] = COLOUR_YELLOW;
			data[4][4] = COLOUR_YELLOW;
			data[5][3] = COLOUR_YELLOW;
			data[5][4] = COLOUR_YELLOW;
			data[6][3] = COLOUR_YELLOW;
			data[6][4] = COLOUR_YELLOW;
			data[7][3] = COLOUR_YELLOW;
			data[7][4] = COLOUR_YELLOW;
			data[8][3] = COLOUR_YELLOW;
			data[8][4] = COLOUR_YELLOW;
			data[9][3] = COLOUR_YELLOW;
			data[9][4] = COLOUR_YELLOW;
			data[10][1] = COLOUR_YELLOW;
			data[10][2] = COLOUR_YELLOW;
			data[10][3] = COLOUR_YELLOW;
			data[10][4] = COLOUR_YELLOW;
			data[10][5] = COLOUR_YELLOW;
			data[10][6] = COLOUR_YELLOW;
			data[11][1] = COLOUR_YELLOW;
			data[11][2] = COLOUR_YELLOW;
			data[11][3] = COLOUR_YELLOW;
			data[11][4] = COLOUR_YELLOW;
			data[11][5] = COLOUR_YELLOW;
			data[11][6] = COLOUR_YELLOW;
			break;
		case 3:
			// Display "GO"
			// "G"
			data[1][2] = COLOUR_GREEN;
			data[1][3] = COLOUR_GREEN;
			data[1][4] = COLOUR_GREEN;
			data[1][5] = COLOUR_GREEN;
			data[2][1] = COLOUR_GREEN;
			data[3][1] = COLOUR_GREEN;
			data[4][1] = COLOUR_GREEN;
			data[5][1] = COLOUR_GREEN;
			data[6][2] = COLOUR_GREEN;
			data[6][3] = COLOUR_GREEN;
			data[6][4] = COLOUR_GREEN;
			data[6][5] = COLOUR_GREEN;
			data[2][6] = COLOUR_GREEN;
			data[4][6] = COLOUR_GREEN;
			data[5][6] = COLOUR_GREEN;
			data[4][4] = COLOUR_GREEN;
			data[4][5] = COLOUR_GREEN;
			// "O"
			data[9][2] = COLOUR_GREEN;
			data[9][3] = COLOUR_GREEN;
			data[9][4] = COLOUR_GREEN;
			data[9][5] = COLOUR_GREEN;
			data[10][1] = COLOUR_GREEN;
			data[11][1] = COLOUR_GREEN;
			data[12][1] = COLOUR_GREEN;
			data[13][1] = COLOUR_GREEN;
			data[14][2] = COLOUR_GREEN;
			data[14][3] = COLOUR_GREEN;
			data[14][4] = COLOUR_GREEN;
			data[14][5] = COLOUR_GREEN;
			data[10][6] = COLOUR_GREEN;
			data[11][6] = COLOUR_GREEN;
			data[12][6] = COLOUR_GREEN;
			data[13][6] = COLOUR_GREEN;
			break;
		default:
			break;
	}

	// Update LED matrix
	ledmatrix_update_all(data);
	
}

// Initialise the display for the board, this creates the display
// for an empty board.
void default_grid(void)
{
	ledmatrix_clear();
	MatrixColumn colours;
	
	for (uint8_t row=0; row<MATRIX_NUM_ROWS; row++)
	{
		colours[row] = COLOUR_YELLOW;
	}
	ledmatrix_update_column(13, colours);
	
	for (uint8_t row=0; row<MATRIX_NUM_ROWS; row++)
	{
		colours[row] = COLOUR_HALF_YELLOW;
	}
	ledmatrix_update_column(12, colours);
	ledmatrix_update_column(14, colours);
	
	for (uint8_t row=0; row<MATRIX_NUM_ROWS; row++)
	{
		colours[row] = COLOUR_QUART_YELLOW;
	}
	ledmatrix_update_column(11, colours);
	ledmatrix_update_column(15, colours);
}

