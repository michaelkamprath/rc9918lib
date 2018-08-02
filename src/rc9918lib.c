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

#include <stdlib.h>
#include <stdio.h>

#include "rc9918lib.h"
#include "DefaultFonts.h"

// private defines

#define TMSREGBIT 0x80

#define TMS_REGISTER_COLOR	7

//
// RC9918Context Definition
//

typedef struct {
	const unsigned char*	font;
	unsigned int	fontMemSize;
	unsigned char	graphicsMode;		// see graphics mode values
	unsigned char 	ramPort;
	unsigned char 	registerPort;
	unsigned char 	registerTable[8];
} RC9918Context;

// Graphic mode values

#define GMODE_UNSET 		0
#define GMODE_GRAPHICS_1	1
#define GMODE_GRAPHICS_2	2
#define GMODE_MULTICOLOR	3
#define GMODE_TEXT			4

// Calculate memory locations based on register values

#define VRAM_ADDR_NAME_TABLE(c)			c->registerTable[2]*0x0400
#define VRAM_ADDR_COLOR_TABLE(c)		c->registerTable[3]*0x0040
#define VRAM_ADDR_GENERATOR_TABLE(c)	c->registerTable[4]*0x0800
#define VRAM_ADDR_SPRITE_ATTRIBUTES(c)	c->registerTable[5]*0x0080
#define VRAM_ADDR_SPRITE_PATTERN(c)		c->registerTable[6]*0x0800

// calculate dimension properties

unsigned int SCREEN_COLUMNS_BY_MODE[5] = {0, 32, 32, 32, 40};

#define SCREEN_COLUMNS(c)	SCREEN_COLUMNS_BY_MODE[c->graphicsMode]

//
// We use a stack allocated array of RC9918Context objects rather than requiring 
// heap allocation. Memory usage can be tuned by adjusting CONTEXT_ARRAY_SIZE
// at compile time.
//
#ifndef RC9918_CONTEXT_ARRAY_SIZE
#define RC9918_CONTEXT_ARRAY_SIZE	2
#endif
RC9918Context	gContextArray[RC9918_CONTEXT_ARRAY_SIZE];
int gAllocatedContextCount = 0;

const unsigned char TMS_BLANK_REGISTER_TABLE[8] = { 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char TMS_BITMAP_REGISTER_TABLE[8] = { 0x02, 0xC2, 0x0E, 0xFF, 0x00, 0x76, 0x03, 0x01 };


#pragma mark - Private Methods
// 
// private methods
//

void _tmsWriteRegister( const RC9918Context* c, unsigned char value, unsigned char register_num );
void _tmsSetRegister( RC9918Context* c, unsigned char value, unsigned char register_num );
void _tmsApplyRegisterTable( const RC9918Context* c );
void _tmsSetVRAMAddress( const RC9918Context* c, unsigned int addr );

void _tmsWriteRegister( const RC9918Context* c, unsigned char value, unsigned char register_num )
{
	unsigned char reg_value = register_num + 0x80;
	
	outp(c->registerPort,value);	
	for (int nop = 0; nop < 2; nop++ ) {;}
	outp(c->registerPort,reg_value);
}

void _tmsSetRegister( RC9918Context* c, unsigned char value, unsigned char register_num )
{
	c->registerTable[register_num] = value;
	
	_tmsWriteRegister( c, value, register_num );
}


void _tmsApplyRegisterTable( const RC9918Context* c )
{
	for (unsigned char i = 0; i < 8; i++ ) {
		_tmsWriteRegister( c, c->registerTable[i], i);
	}
}

void _tmsSetVRAMAddress( const RC9918Context* c, unsigned int addr )
{
	unsigned char lsb = addr&0x00FF;
	unsigned char msb = ((addr >> 8)&0x3F) + 0x40;
	
	outp(c->registerPort,lsb);
	for (int nop = 0; nop < 2; nop++ ) {;}
	outp(c->registerPort,msb);
}


#pragma mark - Public Methods

void* tmsInitBoard( unsigned char ramPort, unsigned char registerPort ) 
{
	gAllocatedContextCount++;
	if (gAllocatedContextCount >= RC9918_CONTEXT_ARRAY_SIZE ) {
		gAllocatedContextCount--;
		return 0;
	}
	
	RC9918Context *c = &gContextArray[gAllocatedContextCount-1];
	
	c->graphicsMode = GMODE_UNSET;
	c->font = 0;
	c->fontMemSize = 0;
	c->ramPort = ramPort;
	c->registerPort = registerPort;
	
	for (int i =0; i < 8; i++ ) {
		c->registerTable[i] = TMS_BLANK_REGISTER_TABLE[i];
	}
	_tmsApplyRegisterTable( c );
	
	// write 0's to VRAM
	_tmsSetVRAMAddress( c, 0 );
	for (int i = 0; i < 0x4000; i++ ) {
		outp(c->ramPort, 0);
	}
	return c;
}

void tmsWriteToVRAM(
	const void* context,
	const unsigned char *data,
	unsigned int byte_count,
	unsigned int vram_addr
) {
	const RC9918Context* c = (const RC9918Context*)context;
	
	_tmsSetVRAMAddress( c, vram_addr );
	for (int i = 0; i < byte_count; i++ ) {
		outp(c->ramPort, data[i]);
	}
}

void tmsSetDefaultFont( void* context )
{
	RC9918Context* c = (RC9918Context*)context;
	
	switch ( c->graphicsMode ) {
		case GMODE_TEXT:
			c->font = default_font_6x8;
			c->fontMemSize = FONT_MEMORY_SIZE;
			break;
		case GMODE_GRAPHICS_1:
		case GMODE_GRAPHICS_2:
			c->font = default_font_8x8;
			c->fontMemSize = FONT_MEMORY_SIZE;
			break;
		default:
			return;
	}
	
	tmsWriteGeneratorTable( context, c->font, c->fontMemSize );
}

void tmsSetTextFont(
	void* context,
	const unsigned char* font_data,
	unsigned int font_byte_count
) {
	RC9918Context* c = (RC9918Context*)context;

	c->font = font_data;
	c->fontMemSize = font_byte_count;
	
	tmsWriteGeneratorTable( context, c->font, c->fontMemSize );
}

void tmsWriteGeneratorTable(
	const void* context,
	const unsigned char* data,
	unsigned int byte_count
) {
	RC9918Context* c = (RC9918Context*)context;
	tmsWriteToVRAM( context, data, byte_count, VRAM_ADDR_GENERATOR_TABLE(c) );
}

void tmsWriteGeneratorTableEntry( 
	const void* context,
	const unsigned char* data,
	unsigned int entryIndex
) {
	RC9918Context* c = (RC9918Context*)context;
	tmsWriteToVRAM( context, data, 8, VRAM_ADDR_GENERATOR_TABLE(c) + entryIndex*8 );
}

void tmsWriteText( const void* context, unsigned char xpos, unsigned char ypos, const unsigned char* str )
{
	const RC9918Context* c = (const RC9918Context*)context;
	unsigned int vram_offset = VRAM_ADDR_NAME_TABLE(c) + (ypos*SCREEN_COLUMNS(c) + xpos);
	
	_tmsSetVRAMAddress( c, vram_offset );	
	int i = 0;	
	while ( str[i] != 0 ) {
		outp( c->ramPort, str[i]);
		i++;
	}
}

void tmsWriteCharacter( const void* context, unsigned char xpos, unsigned char ypos, unsigned char value )
{
	const RC9918Context* c = (const RC9918Context*)context;
	unsigned int vram_offset = VRAM_ADDR_NAME_TABLE(c) + (ypos*SCREEN_COLUMNS(c) + xpos);
	
	_tmsSetVRAMAddress( c, vram_offset );
	outp( c->ramPort, value);	
}



void debugPrintContext( const void* context )
{
	const RC9918Context* c = (const RC9918Context*)context;

	printf("context data:\n");
	printf("   RAM port = 0x%X\n", c->ramPort );
	printf("   Register port = 0x%X\n", c->registerPort );
	printf("   Graphics mode = %d\n", c->graphicsMode );	
	printf("   font ptr = 0x%X\n", c->font );
	printf("   font size = %d\n", c->fontMemSize );
	for (int i = 0; i < 8; i++ ) {
		printf("   register[%d] = 0x%X\n", i, c->registerTable[i]);
	}
	
	printf("\n   Name table loc = 0x%X\n", VRAM_ADDR_NAME_TABLE(c) );
	printf("   Color table loc = 0x%X\n", VRAM_ADDR_COLOR_TABLE(c) );
	printf("   Generator table loc = 0x%X\n", VRAM_ADDR_GENERATOR_TABLE(c) );
	printf("   Sprite attributes loc = 0x%X\n", VRAM_ADDR_SPRITE_ATTRIBUTES(c) );
	printf("   Sprite patterns loc = 0x%X\n", VRAM_ADDR_SPRITE_PATTERN(c) );
	printf("   Column count = %d\n", SCREEN_COLUMNS(c) );
	printf("\n");
}

#pragma mark - Text Mode

const unsigned char TMS_TEXTMODE_REGISTER_TABLE[8] = { 0x00, 0xD0, 0x00, 0x00, 0x01, 0x00, 0x00, 0xF1 };

void tmsSetTextMode( void* context ) 
{
	RC9918Context* c = (RC9918Context*)context;
	c->graphicsMode = GMODE_TEXT;

	for (int i = 0; i < 8; i++ ) {
		c->registerTable[i] = TMS_TEXTMODE_REGISTER_TABLE[i];
	}

	_tmsApplyRegisterTable( c );

	tmsSetDefaultFont( context );
		
	// blank the screen
	_tmsSetVRAMAddress( c, VRAM_ADDR_NAME_TABLE(c) );
	for (int i = 0; i < SCREEN_COLUMNS(c)*24; i++ ) {
		outp( c->ramPort, 0x20 );
	}	
}

void tmsSetTextModeBackgroundColor( void* context, unsigned char color )
{
	RC9918Context* c = (RC9918Context*)context;
	unsigned char cur_color = c->registerTable[TMS_REGISTER_COLOR];
	unsigned char value = (cur_color&0xF0) + (color&0x0F);
	
	_tmsSetRegister( c, value, TMS_REGISTER_COLOR );	
}
void tmsSetTextModeForegroundColor( void* context, unsigned char color )
{
	RC9918Context* c = (RC9918Context*)context;
	unsigned char cur_color = c->registerTable[TMS_REGISTER_COLOR];
	unsigned char value = ((color<<4)&0xF0) + (cur_color&0x0F);
	
	_tmsSetRegister( c, value, TMS_REGISTER_COLOR );	
}

#pragma mark - Bitmap Graphics Mode

const unsigned char TMS_GMODE1_REGISTER_TABLE[8] = { 0x00, 0xC3, 0x02, 0x2C, 0x00, 0x18, 0x04, 0x01 };

void tmsSetGraphicsMode( void* context ) 
{
	RC9918Context* c = (RC9918Context*)context;

	c->graphicsMode = GMODE_GRAPHICS_1;
	
	for (int i = 0; i < 8; i++ ) {
		c->registerTable[i] = TMS_GMODE1_REGISTER_TABLE[i];
	}
	_tmsApplyRegisterTable( c );
	
	tmsSetDefaultFont( context );
	
	// blank the screen
	_tmsSetVRAMAddress( c, VRAM_ADDR_NAME_TABLE(c) );
	for (int i = 0; i < 32*24; i++ ) {
		outp( c->ramPort, 0x20 );
		for (int nop = 0; nop < 2; nop++) {;}
	}
	
	// set colors to white on black. 
	_tmsSetVRAMAddress( c, VRAM_ADDR_COLOR_TABLE(c) );
	for (int i = 0; i < 32; i++ ) {
		outp( c->ramPort, 0xF1 );
	}
}

void tmsSetGraphicsModeColorEntry( const void* context, unsigned char color, unsigned int entryIndex )
{
	const RC9918Context* c = (const RC9918Context*)context;
	
	_tmsSetVRAMAddress( c, VRAM_ADDR_COLOR_TABLE(c) + entryIndex );
	outp( c->ramPort, color );
}

#pragma mark - Sprites

void tmsWriteSpriteAttributes(
	const void* context,
	unsigned int firstSpriteIndex,
	unsigned int countAttributes,
	const SpriteAttribute* spriteData
) {
	const RC9918Context* c = (const RC9918Context*)context;

	unsigned int vram_addr = VRAM_ADDR_SPRITE_ATTRIBUTES(c) + firstSpriteIndex*sizeof(SpriteAttribute);
	
	tmsWriteToVRAM( context,  spriteData, countAttributes*sizeof(SpriteAttribute),  vram_addr);
}

void tmsSetSpritePatterns(
	const void* context,
	unsigned char* pattern_data,
	unsigned int data_size
) {
	const RC9918Context* c = (const RC9918Context*)context;

	tmsWriteToVRAM( context,  pattern_data, data_size,  VRAM_ADDR_SPRITE_PATTERN(c) );
}
