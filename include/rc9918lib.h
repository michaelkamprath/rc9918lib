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
#include <stdbool.h>

//
// General Defines
//

#define VDPCOLOR_TRANSPARENT	0
#define VDPCOLOR_BLACK			1
#define VDPCOLOR_MEDIUM_GREEN	2
#define VDPCOLOR_LIGHT_GREEN	3
#define VDPCOLOR_DARK_BLUE		4
#define VDPCOLOR_LIGHT_BLUE		5
#define VDPCOLOR_DARK_RED		6
#define VDPCOLOR_CYAN			7
#define VDPCOLOR_MEDIUM_RED		8
#define VDPCOLOR_LIGHT_RED		9
#define VDPCOLOR_DARK_YELLOW	0xA
#define VDPCOLOR_LIGHT_YELLOW	0xB
#define VDPCOLOR_DARK_GREEN		0xC
#define VDPCOLOR_MAGENTA		0xD
#define VDPCOLOR_GRAY			0xE
#define VDPCOLOR_WHITE			0xF 

#define RC9918_DEFAULT_RAMPORT			0xBE
#define RC9918_DEFAULT_REGISTER_PORT	0xBF


//
// Initialization and Utility
//

/*
 * returns a board context that will need to be passed to subsequent calls
 * in the library. Should be called only once per port configuration.
 */
void* vdpInitBoard(	unsigned char ramPort, unsigned char registerPort );

void vdpCopyToVRAM(	const void* context,
						const unsigned char *data,
						unsigned int byte_count,
						unsigned int vram_addr );	

void vdpCopyToGeneratorTable(	const void* context,
								const unsigned char* data,
								unsigned int byte_count );
void vdpWriteNGeneratorTableEntries( 	const void* context,
										const unsigned char* data,
										unsigned int startEntryIndex,
										unsigned int entryCount );
void vdpCopyToGeneratorTableEntry(	const void* context,
									const unsigned char* data,
									unsigned int entryIndex );

void vdpSetDefaultFont( void* context );
void vdpSetTextFont(	void* context,
						const unsigned char* font_data,
						unsigned int font_byte_count );
						
void vdpWriteText(	const void* context, 
					unsigned char xpos,
					unsigned char ypos,
					const unsigned char* str );
					
void vdpWriteCharacter( 	const void* context, 
							unsigned char xpos,
							unsigned char ypos,
							unsigned char value );
							
void vdpWriteNCharacters(	const void* context,
							unsigned char start_xpos,
							unsigned char start_ypos,
							unsigned char value,
							unsigned int repeat_count );


//
// Text Mode
//

void vdpSetTextMode( void* context );
void vdpSetTextModeBackgroundColor( void* context, unsigned char color );
void vdpSetTextModeForegroundColor( void* context, unsigned char color );

//
//  Graphics Mode (Mode 1)
//

void vdpSetGraphicsMode1( void* context );
void vdpSetGraphicsMode1ColorEntry( const void* context, unsigned char color, unsigned int entryIndex );

//
// Graphics Mode 2
// 

void vdpSetGraphicsMode2( void* context ) ;
void vdpCopyGraphicsMode2GeneratorTableEntries( 	
											const void* context,
											const unsigned char* data,
											unsigned int startBlockNum,
											unsigned int startEntryIndex,
											unsigned int entryCount );
void vdpCopyNGraphicModeColorTableEntries(
											const void* context,
											const unsigned char* data,
											unsigned int startBlockNum,
											unsigned int startEntryIndex,
											unsigned int entryCount );
void vdpSetNGraphicModeColorTableEntries( 
											const void* context,
											const unsigned char value,
											unsigned int startBlockNum,
											unsigned int startIndex,
											unsigned int count );
//
// Sprites
//

typedef struct {
	unsigned char vert_pos;
	unsigned char horiz_pos;
	unsigned char name;
	unsigned char properties;	// Early Clock and Color
} SpriteAttribute;

void vdpSetSpriteSizeAndMagnification(	void* context,
										bool is16x16Sprite,
										bool isMagnified );
void vdpSetSpritePatterns(	const void* context,
							unsigned char* pattern_data,
							unsigned int data_size );
								
void vdpWriteSpriteAttributes(	const void* context,
								unsigned int firstSpriteIndex,
								unsigned int countAttributes,
								const SpriteAttribute* spriteData );


//
// DEBUG
//

void debugPrintContext( const void* context );

#endif
