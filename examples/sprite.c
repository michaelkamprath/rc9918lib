/*
    This file is part of rc9918lib - https://github.com/michaelkamprath/rc9918lib

    rc9918lib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    rc9918lib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with rc9918lib.  If not, see <https://www.gnu.org/licenses/>.
*/

// Original assembly version by J. B. Langston
// 		https://github.com/jblang/rc9918/blob/master/examples/sprite.asm

#include <stdio.h>
#include <stdlib.h>
#include <im2.h>

#include "rc9918lib.h"

const unsigned char worldSpritePatterns[] = {
// WORLD0:
//         ; Sprite world0 pattern 1,
        7,28,56,112,120,92,14,15,
        15,31,127,99,115,61,31,7,
        224,248,124,102,242,190,220,252,
        248,160,192,192,226,244,248,224,
//         ; Sprite world0 pattern 2,
        0,3,7,15,7,163,241,240,
        240,224,128,28,12,2,0,0,
        0,0,128,152,12,65,35,3,
        7,95,63,62,28,8,0,0,
// WORLD1:
//         ; Sprite world1 pattern 1,
        3,31,62,124,126,151,3,3,
        3,7,31,120,124,63,31,7,
        224,56,28,24,60,47,183,255,
        254,232,240,240,248,124,248,224,
//         ; Sprite world1 pattern 2,
        0,0,1,3,1,104,252,252,
        252,248,224,7,3,0,0,0,
        0,192,224,230,194,208,72,0,
        1,23,15,14,6,128,0,0,
// WORLD2:
//         ; Sprite world2 pattern 1,
        7,31,63,127,63,229,192,192,
        128,1,7,30,63,63,31,7,
        224,200,132,6,142,203,237,255,
        255,250,252,60,62,220,248,224,
//         ; Sprite world2 pattern 2,
        0,0,0,0,64,26,63,63,
        127,254,248,97,64,0,0,0,
        0,48,120,248,112,52,18,0,
        0,5,3,194,192,32,0,0,
// WORLD3:
//         ; Sprite world3 pattern 1,
        7,31,63,31,79,249,112,240,
        224,128,1,7,15,31,31,7,
        224,240,224,194,226,114,59,63,
        63,126,255,142,206,244,248,224,
//         ; Sprite world3 pattern 2,
        0,0,0,96,48,6,143,15,
        31,127,254,120,112,32,0,0,
        0,8,28,60,28,141,196,192,
        192,129,0,112,48,8,0,0,
// WORLD4:
//         ; Sprite world4 pattern 1,
        7,31,63,103,115,190,220,252,
        248,160,192,65,99,55,31,7,
        224,248,248,240,248,92,14,15,
        15,31,127,226,242,252,248,224,
//         ; Sprite world4 pattern 2,
        0,0,0,24,12,65,35,3,
        7,95,63,62,28,8,0,0,
        0,0,4,14,6,163,241,240,
        240,224,128,28,12,0,0,0,
// WORLD5:
//         ; Sprite world5 pattern 1,
        7,31,31,25,60,47,183,255,
        254,232,240,112,120,61,31,7,
        224,248,252,252,254,151,3,3,
        3,7,31,120,252,252,248,224,
//        ; Sprite world5 pattern 2,
        0,0,32,102,67,208,72,0,
        1,23,15,15,7,2,0,0,
        0,0,0,2,0,104,252,252,
        252,248,224,134,2,0,0,0,
// WORLD6:
//         ; Sprite world6 pattern 1,
        7,15,7,6,15,203,237,255,
        255,250,252,60,62,31,31,7,
        224,248,252,126,62,229,192,192,
        128,1,7,30,62,124,248,224,
//         ; Sprite world6 pattern 2,
        0,16,56,121,112,52,18,0,
        0,5,3,67,65,32,0,0,
        0,0,0,128,192,26,63,63,
        127,254,248,224,192,128,0,0,
// WORLD7:
//         ; Sprite world7 pattern 1,
        7,19,33,65,99,114,59,63,
        63,126,255,15,79,55,31,7,
        224,248,252,158,206,249,112,240,
        224,128,1,6,142,220,248,224,
//         ; Sprite world7 pattern 2,
        0,12,30,62,28,141,196,192,
        192,129,0,112,48,8,0,0,
        0,0,0,96,48,6,143,15,
        31,127,254,248,112,32,0,0
};

#define STAR_1A_CHAR	240
#define STAR_1B_CHAR	248
#define STAR_2A_CHAR	241
#define STAR_2B_CHAR	249

#define PATTERN_A_COLOR	0xB1
#define PATTERN_B_COLOR	0XE1

#define PATTERN_A_TABLE_INDEX	30
#define PATTERN_B_TABLE_INDEX	31

const unsigned char PATTERN_STAR_1A[8] = { 0, 0, 0x38, 0x38, 0x38, 0, 0, 0 };
const unsigned char PATTERN_STAR_1B[8] = { 0x10, 0x10, 0x38, 0xFE, 0x38, 0x10, 0x10, 0 };

const unsigned char PATTERN_STAR_2A[8] = { 0, 0x04, 0x0E, 0x04, 0, 0, 0, 0 };
const unsigned char PATTERN_STAR_2B[8] = { 0x04, 0x04, 0x1F, 0x04, 0x04, 0, 0, 0 };

#define STAR_LOCATION_COUNT	12
const unsigned char STAR_LOCATIONS[STAR_LOCATION_COUNT*3] = {
//		x,	y,	char
		4,	10, STAR_1A_CHAR,
		8,	18, STAR_1A_CHAR,
		20,	13, STAR_1A_CHAR,
		30,	12, STAR_1A_CHAR,
		2,	9,	STAR_2A_CHAR,
		3,	18,	STAR_2A_CHAR,
		7,	15,	STAR_2A_CHAR,
		15,	17,	STAR_2A_CHAR,
		16,	11,	STAR_2A_CHAR,
		22,	13,	STAR_2A_CHAR,
		24, 18,	STAR_2A_CHAR,
		29, 15,	STAR_2A_CHAR
	};
	
SpriteAttribute spriteTable[32];

void setUpStaryBackground( void * context ) 
{
	// update generator patterns
	vdpCopyToGeneratorTableEntry( context, PATTERN_STAR_1A, STAR_1A_CHAR );
	vdpCopyToGeneratorTableEntry( context, PATTERN_STAR_1B, STAR_1B_CHAR );
	vdpCopyToGeneratorTableEntry( context, PATTERN_STAR_2A, STAR_2A_CHAR );
	vdpCopyToGeneratorTableEntry( context, PATTERN_STAR_2B, STAR_2B_CHAR );
	
	vdpSetGraphicsMode1ColorEntry( context, PATTERN_A_COLOR, PATTERN_A_TABLE_INDEX );
	vdpSetGraphicsMode1ColorEntry( context, PATTERN_B_COLOR, PATTERN_B_TABLE_INDEX );
	
	// place star on screen
	
	for (int i = 0; i < STAR_LOCATION_COUNT; i++ ) {
		vdpWriteCharacter(
			context, 
			STAR_LOCATIONS[i*3],
			STAR_LOCATIONS[i*3+1],
			STAR_LOCATIONS[i*3+2]
		);
	}
}

void twinkleOnStarAtIndex( void * context, unsigned int starIndex ) {
	vdpWriteCharacter(
			context, 
			STAR_LOCATIONS[starIndex*3],
			STAR_LOCATIONS[starIndex*3+1],
			STAR_LOCATIONS[starIndex*3+2] + 8
		);
}

void twinkleOffStarAtIndex( void * context, unsigned int starIndex ) {
	vdpWriteCharacter(
			context, 
			STAR_LOCATIONS[starIndex*3],
			STAR_LOCATIONS[starIndex*3+1],
			STAR_LOCATIONS[starIndex*3+2]
		);
}

void main( void ) 
{
	srand(time(0));
	
	printf("Initing TMS9918A\n");
	void* context = vdpInitBoard( RC9918_DEFAULT_RAMPORT, RC9918_DEFAULT_REGISTER_PORT );
	
	if (context == 0) {
		printf("ERROR - could not allocated context!");
		return;
	}
	
	printf("initing graphics mode.\n");
	vdpSetGraphicsMode1( context );
	vdpSetSpriteSizeAndMagnification( context, 1, 1 );
	
	// write out column and row headers
	
	unsigned char buffer[33];
	
	sprintf( buffer,"01234567890123456789012345678901");
	vdpWriteText( context, 0, 0, buffer );
	setUpStaryBackground( context );
	
	vdpSetSpritePatterns( context, worldSpritePatterns, 512 );

	for (int i = 0; i < 32; i++ ) {
		spriteTable[i].vert_pos = 0xFF,
		spriteTable[i].horiz_pos = 0;
		spriteTable[i].name = 0;
		spriteTable[i].properties = 0;
		
	}	
	
	vdpWriteSpriteAttributes( context, 0, 32, spriteTable);
	
	int idx = 0;
	int vel = -1;
	int xpos = 100;
	unsigned int twinkleStar = rand()%STAR_LOCATION_COUNT;
	char isStarTwinkling = 0;
	long twinkleTickCounter = 0;
	long nextStarTwinkleTick = rand()%100 + 50;
	
	spriteTable[0].vert_pos = 100,
	spriteTable[0].properties = 0x03;

	spriteTable[1].vert_pos = 100,
	spriteTable[1].properties = 0x05;
	
	vdpWriteText( context, 0, 2, "frame = " );
	
	for (long i = 0; i < 500000; i++ ) {
	
		if (i%50 == 0) {
			spriteTable[0].horiz_pos = xpos;
			spriteTable[0].name = idx*8;
	
			spriteTable[1].horiz_pos = xpos;
			spriteTable[1].name = idx*8 + 4;
			
			sprintf( buffer, "%d", idx );
			vdpWriteText( context, 8, 2, buffer );
			vdpWriteSpriteAttributes( context, 0, 2, &spriteTable[0]);
			
			if (i%200 == 0 ) {
				idx++;
				if (idx == 8) {
					idx = 0;
				}
			}
			xpos += vel;
			if (xpos == 0) {
				vel = 1;
			}
			if (xpos == 255-32) {
				vel = -1;
			}
			twinkleTickCounter++;
			if (twinkleTickCounter == nextStarTwinkleTick )	{
				if (isStarTwinkling) {
					twinkleOffStarAtIndex( context, twinkleStar );
					isStarTwinkling = 0;
				
					twinkleStar = rand()%STAR_LOCATION_COUNT;
				}
				else {
					twinkleOnStarAtIndex( context, twinkleStar );
					isStarTwinkling = 1;
				}
				
				twinkleTickCounter = 0;
				nextStarTwinkleTick = rand()%100 + 50;
			}
		}		
	}
	
	sprintf( buffer, "Done!                    ", idx);
	vdpWriteText( context, 0, 2, buffer );
}
