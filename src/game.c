/*
 * game.c
 *
 * Functionality related to the game state and features.
 *
 * Author: Jarrod Bennett, Cody Burnett
 */ 

#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "display.h"
#include "ledmatrix.h"
#include "terminalio.h"

static const uint8_t track[TRACK_LENGTH] = {0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x80, 0x00, 0x00, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x12, 0x20, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x01, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00};

static uint8_t played_track[TRACK_LENGTH] = {0};
uint16_t score;
uint16_t beat;

// Initialise the game by resetting the grid and beat
void initialise_game(void)
{
	// initialise the display we are using.
	default_grid();
	beat = 0;
}

void award_points(uint8_t col)
{
	// award points based on the column
	if (col == 11 || col == 15)
	{
		score += 1;
	}
	else if (col == 12 || col == 14)
	{
		score += 2;
	}
	else if (col == 13)
	{
		score += 3;
	}
}

// Play a note in the given lane
void play_note(uint8_t lane)
{	
	// Change the value of lane so that they are ordered left to right
	lane = 3 - lane;
	// Check if there is a note in the scoring area
	for (uint8_t col = 11; col < MATRIX_NUM_COLUMNS; col++) 
	{
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col;
		uint8_t index = (future + beat) / 5;
		if ((future+beat) % 5)
		{
			continue;
		}
		if (index >= TRACK_LENGTH)
		{
			continue;
		}
		// Check if there's a note in the lane
		if (track[index] & (1<<lane))
		{	
			// Check if note has been played
			if (played_track[index] & (1<<lane))
			{
				score -= 1;
				continue;
			}
			// Mark the note as played
			played_track[index] |= (1<<lane);
			// Colour the two pixels green
			ledmatrix_update_pixel(col, 2*lane, COLOUR_GREEN);
			ledmatrix_update_pixel(col, 2*lane+1, COLOUR_GREEN);
			// Award points
			award_points(col);
		}
		else
		{
			score -= 1;
		}
	}
}

// Advance the notes oned row down the display
void advance_note(void)
{
	// remove all the current notes; reverse of below
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col;
		uint8_t index = (future + beat) / 5;
		if (index >= TRACK_LENGTH)
		{
			break;
		}
		if ((future+beat) % 5)
		{
			continue;
		}
		for (uint8_t lane = 0; lane < 4; lane++)
		{
			if (track[index] & (1<<lane))
			{
				PixelColour colour;
				// yellows in the scoring area
				if (col==11 || col == 15)
				{
					colour = COLOUR_QUART_YELLOW;
				}
				else if (col==12 || col == 14)
				{
					colour = COLOUR_HALF_YELLOW;
				}
				else if (col==13)
				{
					colour = COLOUR_YELLOW;
				}
				else
				{
					colour = COLOUR_BLACK;
				}
				ledmatrix_update_pixel(col, 2*lane, colour);
				ledmatrix_update_pixel(col, 2*lane+1, colour);
			}
		}
	}
	
	// increment the beat
	beat++;

	// draw the new notes
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		// col counts from one end, future from the other
		uint8_t future = MATRIX_NUM_COLUMNS-1-col;
		// notes are only drawn every five columns
		if ((future+beat)%5)
		{
			continue;
		}
		
		// index of which note in the track to play
		uint8_t index = (future+beat)/5;
		// if the index is beyond the end of the track,
		// no note can be drawn
		if (index >= TRACK_LENGTH)
		{
			continue;
		}
		// iterate over the four paths
		for (uint8_t lane=0; lane<4; lane++)
		{
			// check if there's a note in the specific path
			if (track[index] & (1<<lane))
			{
				// check if note has been played
				if (played_track[index] & (1<<lane))
				{
					ledmatrix_update_pixel(col, 2*lane, COLOUR_GREEN);
					ledmatrix_update_pixel(col, 2*lane+1, COLOUR_GREEN);
					continue;
				}
				// colour the note's two pixels red if not played
				ledmatrix_update_pixel(col, 2*lane, COLOUR_RED);
				ledmatrix_update_pixel(col, 2*lane+1, COLOUR_RED);
			}
		}
	}
}

// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void)
{
	// Detect if the game is over i.e. if a player has won.
	if (beat >= TRACK_LENGTH*5)
	{
		static uint8_t played_track[TRACK_LENGTH] = {0};
		return 1;
	}
	return 0;
}
