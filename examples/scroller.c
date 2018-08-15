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

#include <stdio.h>
#include <stdlib.h>

#include "rc9918lib.h"
#include "rc9918lib_Internal.h"

const unsigned char SOLID_BRICK_PATTERNS[64] = {
		0x00, 0xEF, 0xEF, 0xEF, 0x00, 0xFE, 0xFE, 0xFE,		// step 0
		0x00, 0xDF, 0xDF, 0xDF, 0x00, 0xFD, 0xFD, 0xFD, 	// step 1
		0x00, 0xBF, 0xBF, 0xBF, 0x00, 0xFB, 0xFB, 0xFB,		// step 2
		0x00, 0x7F, 0x7F, 0x7F, 0x00, 0xF7, 0xF7, 0xF7, 	// step 3
		0x00, 0xFE, 0xFE, 0xFE, 0x00, 0xEF, 0xEF, 0xEF,		// step 4
		0x00, 0xFD, 0xFD, 0xFD, 0x00, 0xDF, 0xDF, 0xDF,		// step 5
		0x00, 0xFB, 0xFB, 0xFB, 0x00, 0xBF, 0xBF, 0xBF,		// step 6
		0x00, 0xF7, 0xF7, 0xF7, 0x00, 0x7F, 0x7F, 0x7F		// step 7
	};

const unsigned char SOLID_BRICK_COLORS[64] = {
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E,		// step 0
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E,		// step 1
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 	// step 2
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E,		// step 3
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 	// step 4
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E,		// step 5
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E,		// step 6
		0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E		// step 8
	};
// const unsigned char SOLID_BRICK_COLORS[64] = {
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE,		// step 0
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE,		// step 1
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 	// step 2
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE,		// step 3
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 	// step 4
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE,		// step 5
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE,		// step 6
// 		0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE		// step 8
// 	};
	
const unsigned char UP_THEN_SOLID_BRICK_PATTERNS[64] = {
		0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFE, 0xFE, 0xFE,		// step 0
		0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFD, 0xFD, 0xFD, 	// step 1
		0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFB, 0xFB, 0xFB,		// step 2
		0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xF7, 0xF7, 0xF7, 	// step 3
		0x7F, 0xFE, 0xFE, 0xFE, 0xFF, 0xEF, 0xEF, 0xEF,		// step 4
		0xFF, 0xFD, 0xFD, 0xFD, 0xFF, 0xDF, 0xDF, 0xDF,		// step 5
		0xFF, 0xFB, 0xFB, 0xFB, 0xFF, 0xBF, 0xBF, 0xBF,		// step 6
		0xFF, 0xF7, 0xF7, 0xF7, 0xFF, 0x7F, 0x7F, 0x7F		// step 7
	};

const unsigned char UP_THEN_SOLID_BRICK_COLORS[64] = {
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 0
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 1
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 2
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 3
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 4
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 5
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 6
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E	 	// step 7
	};

const unsigned char AIR_THEN_UP_BRICK_PATTERNS[64] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 	// step 0
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03,  	// step 1
		0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 	// step 2
		0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0F, 	// step 3
		0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 	// step 4
		0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x3F, 	// step 5
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F, 	// step 6
		0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF	 	// step 7
	};

const unsigned char AIR_THEN_UP_BRICK_COLORS[64] = {
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 0
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 1
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 2
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 3
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 4
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 5
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 6
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61 		// step 7
	};		

const unsigned char SOLID_THEN_DOWN_BRICK_PATTERNS[64] = {
		0xFF, 0xEF, 0xEF, 0xEF, 0XFF, 0xFE, 0xFE, 0xFE,		// step 0
		0xFF, 0xDF, 0xDF, 0xDF, 0xFF, 0xFD, 0xFD, 0xFD,		// step 1
		0xFF, 0xBF, 0xBF, 0xBF, 0xFF, 0xFB, 0xFB, 0xFB,		// step 2
	 	0xFE, 0x7F, 0x7F, 0x7F, 0xFF, 0xF7, 0xF7, 0xF7,		// step 3
	 	0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xEF, 0xEF, 0xEF,		// step 4
	 	0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xDF, 0xDF, 0xDF,		// step 5
	 	0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xBF, 0xBF, 0xBF,		// step 6
	 	0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0x7F, 0x7F, 0x7F,		// step 7
	};

const unsigned char SOLID_THEN_DOWN_BRICK_COLORS[64] = {
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 0
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 1
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 2
		0xE1, 0x6E, 0x6E, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 3
		0xE1, 0x61, 0x61, 0x6E, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 4
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 5
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 6
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x6E, 0x6E, 0x6E, 	// step 7
	};

const unsigned char DOWN_THEN_AIR_BRICK_PATTERNS[64] = {
	 	0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF,		// step 0
		0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF,		// step 1
		0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE,		// step 2
		0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC,		// step 3
		0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8,		// step 4
		0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0,		// step 5
		0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0,		// step 6
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0		// step 7
	};

const unsigned char DOWN_THEN_AIR_BRICK_COLORS[64] = {
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 7
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 0
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 1
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 2
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 3
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 4
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 5
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 6
		0xE1, 0x61, 0x61, 0x61, 0xE1, 0x61, 0x61, 0x61, 	// step 7
	};

	
#define SOLID_BRICK_BASE_CHAR 128
#define UP_THEN_SOLID_BRICK_BASE_CHAR 136
#define AIR_THEN_UP_BRICK_BASE_CHAR 144
#define SOLID_THEN_DOWN_BRICK_BASE_CHAR 152
#define DOWN_THEN_AIR_BRICK_BASE_CHAR 160

#define AIR 32								// empty space
#define SOL SOLID_BRICK_BASE_CHAR			// solid bricks
#define UPS UP_THEN_SOLID_BRICK_BASE_CHAR	// up then solid bricks
#define	AUP	AIR_THEN_UP_BRICK_BASE_CHAR		// air than up
#define DOW SOLID_THEN_DOWN_BRICK_BASE_CHAR	// solid then down
#define DAR DOWN_THEN_AIR_BRICK_BASE_CHAR	// down then air

#define	BRICK_LOCATION_ENTRY 0x240			// row 18, column 0

#define TERRAIN_HEIGHT 6
#define TERRAIN_WIDTH 48

const unsigned char TERRAIN_MAP[TERRAIN_HEIGHT*TERRAIN_WIDTH] = {
	AIR, AIR, AUP, UPS, SOL, SOL, DOW, DAR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AUP, UPS, SOL, DOW, DAR, AIR, AIR, AIR, AIR, AIR,  
	AIR, AUP, UPS, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AUP, UPS, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AUP, UPS, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AIR, AIR, 
	SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AIR, AIR, AIR, AIR, AUP, UPS, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AIR, AIR, AIR, AIR, AUP, UPS, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL,
	SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AIR, AIR, AUP, UPS, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AIR, AIR, AUP, UPS, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL,
	SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, DOW, DAR, AIR, AUP, UPS, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL,
	SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL, SOL
};

#include "rc9918lib_Internal.h"

// sceneSectionsList is a list of a pair on unsigned ints. Each pair describes
// a section of the terrain that needs to be copied to VRAM. The first number in a 
// pair indicates the offset in the scene data to start at, and the second number 
// how many bytes to copy from the offset. 
//
// There are a maximum of 48 entries into this table, indicating all 24 rows of the 
// screen are scrolling over the end of the scene data and wrapping back to the starte.
//
unsigned int sceneSectionsList[48];

void blastSceneRowsToVRAM(
	void* context,
	const unsigned char *terrain,
	unsigned int terrain_width,
	unsigned int terrain_height,
	unsigned int scroll_step,
	unsigned int scroll_sub_step,
	unsigned int terrain_start_row
) {
	RC9918Context* c = (RC9918Context*)context;
	unsigned int screenColumns = SCREEN_COLUMNS(c);
	
	unsigned int initialColumnsFetched = screenColumns;
	if ( scroll_step + initialColumnsFetched >= terrain_width ) {
		initialColumnsFetched = terrain_width - scroll_step;
	}

	unsigned int remainingColumnsFetched = screenColumns - initialColumnsFetched;
	unsigned char sceneListIdx = 0;
	for (unsigned int r = 0; r <  terrain_height; r++ ) {
		sceneSectionsList[2*sceneListIdx] = r*terrain_width + scroll_step;
		sceneSectionsList[2*sceneListIdx+1] = initialColumnsFetched;
		sceneListIdx++;
		if (remainingColumnsFetched > 0) {
			sceneSectionsList[2*sceneListIdx] = r*terrain_width;
			sceneSectionsList[2*sceneListIdx+1] = remainingColumnsFetched;
			sceneListIdx++;
		}
	}
	
	_vdpCopyAdjustedSceneSectionsToVRAM(
			c->ramPort,
			c->registerPort,
			terrain,
			scroll_sub_step,	
			c->nameTableAddr + (terrain_start_row*screenColumns),
			sceneSectionsList,
			sceneListIdx
		);
}


void main( void ) 
{
	unsigned char screenBuffer[768];
	
	srand(time(0));
	
	printf("Smooth Scrolling Demo\n\n");
	printf("Initing TMS9918A\n");
	void* context = vdpInitBoard( RC9918_DEFAULT_RAMPORT, RC9918_DEFAULT_REGISTER_PORT );
	
	if (context == 0) {
		printf("ERROR - could not allocated context!");
		return;
	}
	
	printf("initing graphics mode 2.\n");
	vdpSetGraphicsMode2( context );
	vdpSetSpriteSizeAndMagnification( context, 1, 1 );

	const unsigned char textColor = 0xF1;
	vdpSetNGraphicModeColorTableEntries( context, textColor, 0, 0, 0x800 );
	vdpSetDefaultFont( context );
	
	// set up brick chars
	vdpCopyGraphicsMode2GeneratorTableEntries(
			context,
			SOLID_BRICK_PATTERNS,
			2,
			SOLID_BRICK_BASE_CHAR,
			8
		);
	vdpCopyNGraphicModeColorTableEntries(
			context,
			SOLID_BRICK_COLORS,
			2,
			SOLID_BRICK_BASE_CHAR,
			8
		);
		
	vdpCopyGraphicsMode2GeneratorTableEntries(
			context,
			UP_THEN_SOLID_BRICK_PATTERNS,
			2,
			UP_THEN_SOLID_BRICK_BASE_CHAR,
			8
		);
	vdpCopyNGraphicModeColorTableEntries(
			context,
			UP_THEN_SOLID_BRICK_COLORS,
			2,
			UP_THEN_SOLID_BRICK_BASE_CHAR,
			8
		);
		
	vdpCopyGraphicsMode2GeneratorTableEntries(
			context,
			AIR_THEN_UP_BRICK_PATTERNS,
			2,
			AIR_THEN_UP_BRICK_BASE_CHAR,
			8
		);
	vdpCopyNGraphicModeColorTableEntries(
			context,
			AIR_THEN_UP_BRICK_COLORS,
			2,
			AIR_THEN_UP_BRICK_BASE_CHAR,
			8
		);

	vdpCopyGraphicsMode2GeneratorTableEntries(
			context,
			SOLID_THEN_DOWN_BRICK_PATTERNS,
			2,
			SOLID_THEN_DOWN_BRICK_BASE_CHAR,
			8
		);
	vdpCopyNGraphicModeColorTableEntries(
			context,
			SOLID_THEN_DOWN_BRICK_COLORS,
			2,
			SOLID_THEN_DOWN_BRICK_BASE_CHAR,
			8
		);

	vdpCopyGraphicsMode2GeneratorTableEntries(
			context,
			DOWN_THEN_AIR_BRICK_PATTERNS,
			2,
			DOWN_THEN_AIR_BRICK_BASE_CHAR,
			8
		);
	vdpCopyNGraphicModeColorTableEntries(
			context,
			DOWN_THEN_AIR_BRICK_COLORS,
			2,
			DOWN_THEN_AIR_BRICK_BASE_CHAR,
			8
		);
	
	// start scrolling loop:
	unsigned int scrollSubStepIdx = 0;
	unsigned int scrollStep = 0;
	unsigned char lineBuffer[33];
	lineBuffer[32] = 0;
	
	vdpWriteText( context, 0, 0, "    Step = 0" );
	vdpWriteText( context, 0, 1, "   Frame = 0" );
	vdpWriteText( context, 0, 2, "Joystick = " );
	unsigned char numberTextBuffer[9];
	
	for (long int i = 0; i < 50000; i++ ) {
	
// 		unsigned char joystick = M_INP8( 0x1 );
// 		sprintf( numberTextBuffer, "%2X", joystick );
// 		vdpWriteText( context, 11, 2, numberTextBuffer );
// 		
// 		if ( (joystick&0x4) > 0 ) {
// 			// left
// 			scrollSubStepIdx++;
// 			if ( scrollSubStepIdx >= 8 ) {
// 				scrollSubStepIdx = 0;
// 				scrollStep++;
// 				if (scrollStep >= TERRAIN_WIDTH) {
// 					scrollStep = 0;
// 				}
// 			}
// 		}
// 		else if ( (joystick&0x8) > 0 ) {
// 			// right
// 			if ( scrollSubStepIdx == 0 ) {
// 				scrollSubStepIdx = 7;
// 				
// 				if ( scrollStep == 0 ) {
// 					scrollStep = TERRAIN_WIDTH - 1;
// 				}
// 				else {
// 					scrollStep--;
// 				}
// 			}
// 			else {
// 				scrollSubStepIdx--;
// 			}
// 		}
		
		sprintf( numberTextBuffer, "%2d", scrollStep );
		vdpWriteText( context, 11, 0, numberTextBuffer );
		sprintf( numberTextBuffer, "%2d", scrollSubStepIdx );
		vdpWriteText( context, 11, 1, numberTextBuffer );
		
		blastSceneRowsToVRAM(
				context,
				TERRAIN_MAP,
				TERRAIN_WIDTH,
				TERRAIN_HEIGHT,
				scrollStep,
				scrollSubStepIdx,
				18
			);

		scrollSubStepIdx++;
		if ( scrollSubStepIdx >= 8 ) {
			scrollSubStepIdx = 0;
			scrollStep++;
			if (scrollStep >= TERRAIN_WIDTH) {
				scrollStep = 0;
			}
		}
	
	}
}
