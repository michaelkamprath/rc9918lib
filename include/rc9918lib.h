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

#ifndef RC9918LIB_H_
#define RC9918LIB_H_

//
// General Defines
//

#define TMSCOLOR_TRANSPARENT	0
#define TMSCOLOR_BLACK			1
#define TMSCOLOR_MEDIUM_GREEN	2
#define TMSCOLOR_LIGHT_GREEN	3
#define TMSCOLOR_DARK_BLUE		4
#define TMSCOLOR_LIGHT_BLUE		5
#define TMSCOLOR_DARK_RED		6
#define TMSCOLOR_CYAN			7
#define TMSCOLOR_MEDIUM_RED		8
#define TMSCOLOR_LIGHT_RED		9
#define TMSCOLOR_DARK_YELLOW	0xA
#define TMSCOLOR_LIGHT_YELLOW	0xB
#define TMSCOLOR_DARK_GREEN		0xC
#define TMSCOLOR_MAGENTA		0xD
#define TMSCOLOR_GRAY			0xE
#define TMSCOLOR_WHITE			0xF 

#define RC9918_DEFAULT_RAMPORT			0x98
#define RC9918_DEFAULT_REGISTER_PORT	0x99


//
// Initialization and Utility
//

/*
 * returns a board context that will need to be passed to subsequent calls
 * in the library. Should be called only once per port configuration.
 */
void* tmsInitBoard(	unsigned char ramPort, unsigned char registerPort );

void tmsWriteToVRAM(	const void* context,
						const unsigned char *data,
						unsigned int byte_count,
						unsigned int vram_addr );	

void tmsWriteGeneratorTable(	const void* context,
								const unsigned char* data,
								unsigned int byte_count );
void tmsWriteGeneratorTableEntry(	const void* context,
									const unsigned char* data,
									unsigned int entryIndex );

void tmsSetDefaultFont( void* context );
void tmsSetTextFont(	void* context,
						const unsigned char* font_data,
						unsigned int font_byte_count );
void tmsWriteText( const void* context, unsigned char xpos, unsigned char ypos, const unsigned char* str );
void tmsWriteCharacter( const void* context, unsigned char xpos, unsigned char ypos, unsigned char value );



//
// Text Mode
//

void tmsSetTextMode( void* context );
void tmsSetTextModeBackgroundColor( void* context, unsigned char color );
void tmsSetTextModeForegroundColor( void* context, unsigned char color );

//
//  Graphics Mode (Mode 1)
//

void tmsSetGraphicsMode( void* context );
void tmsSetGraphicsModeColorEntry( const void* context, unsigned char color, unsigned int entryIndex );

//
// Sprites
//

typedef struct {
	unsigned char vert_pos;
	unsigned char horiz_pos;
	unsigned char name;
	unsigned char properties;	// Early Clock and Color
} SpriteAttribute;

void tmsWriteSpriteAttributes(	const void* context,
								unsigned int firstSpriteIndex,
								unsigned int countAttributes,
								const SpriteAttribute* spriteData );

void tmsSetSpritePatterns(	const void* context,
							unsigned char* pattern_data,
							unsigned int data_size );

//
// DEBUG
//

void debugPrintContext( const void* context );

#endif
