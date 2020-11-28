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
#include "rc9918lib_Internal.h"
#include "DefaultFonts.h"

// private defines

#define vdpREGBIT 0x80

#define vdp_REGISTER_VDP_OPTIONS	1
#define vdp_REGISTER_COLOR			7




// calculate dimension properties

unsigned int SCREEN_COLUMNS_BY_MODE[5] = {0, 32, 32, 32, 40};


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

const unsigned char vdp_BLANK_REGISTER_TABLE[8] = { 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//const unsigned char vdp_BITMAP_REGISTER_TABLE[8] = { 0x02, 0xC2, 0x0E, 0xFF, 0x00, 0x76, 0x03, 0x01 };


#pragma mark - Private Methods

void _vdpInitContextVRAMAddresses( RC9918Context* c )
{
	if ( c->graphicsMode == GMODE_GRAPHICS_2 ) {
		c->colorTableAddr = (((c->registerTable[3]&0x80) == 0) ? 0 : 0x2000);
		c->generatorTableAddr = (((c->registerTable[4]&0x80) == 0) ? 0 : 0x2000);
	} else {
		c->colorTableAddr = c->registerTable[3]*0x0040;
		c->generatorTableAddr = c->registerTable[4]*0x0800;
	}
	
	c->nameTableAddr = c->registerTable[2]*0x0400;
	c->spriteAttributesAddr = c->registerTable[5]*0x0080;
	c->spritePatternsAddr = c->registerTable[6]*0x0800;
}

void _vdpWriteRegister( const RC9918Context* c, unsigned char value, unsigned char register_num )
{
	unsigned char reg_value = register_num + 0x80;
	
	outp(c->registerPort,value);	
	for (int nop = 0; nop < 2; nop++ ) {;}
	outp(c->registerPort,reg_value);
}

void _vdpSetRegister( RC9918Context* c, unsigned char value, unsigned char register_num )
{
	c->registerTable[register_num] = value;
	
	_vdpWriteRegister( c, value, register_num );
}


void _vdpApplyRegisterTable( const RC9918Context* c )
{
	for (unsigned char i = 0; i < 8; i++ ) {
		_vdpWriteRegister( c, c->registerTable[i], i);
	}
}

#pragma mark - Public Methods

void* vdpInitBoard( unsigned char ramPort, unsigned char registerPort ) 
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
		c->registerTable[i] = vdp_BLANK_REGISTER_TABLE[i];
	}
	_vdpApplyRegisterTable( c );
	_vdpInitContextVRAMAddresses( c );
	
	// write 0's to VRAM
	_vdpWriteValueToVRAM( c->ramPort, c->registerPort, 0, 0x4000, 0x0 );
	return c;
}

void vdpCopyToVRAM(
	const void* context,
	const unsigned char *data,
	unsigned int byte_count,
	unsigned int vram_addr
) {
	const RC9918Context* c = (const RC9918Context*)context;
	
	_vdpSetVRAMAddress( c->registerPort, vram_addr );
	const unsigned char *p = data;
	while (p < data+byte_count) {
		outp(c->ramPort, *p );
		p++;
	}
}

void vdpSetDefaultFont( void* context )
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
	
	if ( c->graphicsMode == GMODE_GRAPHICS_2 ) {
		_vdpCopyDataToVRAM(
				c->ramPort,
				c->registerPort,
				c->font,
				c->fontMemSize,
				c->generatorTableAddr
			);
		_vdpCopyDataToVRAM(
				c->ramPort,
				c->registerPort,
				c->font,
				c->fontMemSize,
				c->generatorTableAddr + FONT_MEMORY_SIZE
			);
		_vdpCopyDataToVRAM(
				c->ramPort,
				c->registerPort,
				c->font,
				c->fontMemSize,
				c->generatorTableAddr + (2*FONT_MEMORY_SIZE)
			);	
	} else {
		vdpCopyToGeneratorTable( context, c->font, c->fontMemSize );
	}
	
}

void vdpSetTextFont(
	void* context,
	const unsigned char* font_data,
	unsigned int font_byte_count
) {
	RC9918Context* c = (RC9918Context*)context;

	c->font = font_data;
	c->fontMemSize = font_byte_count;
	
	vdpCopyToGeneratorTable( context, c->font, c->fontMemSize );
}

void vdpCopyToGeneratorTable(
	const void* context,
	const unsigned char* data,
	unsigned int byte_count
) {
	RC9918Context* c = (RC9918Context*)context;
	_vdpCopyDataToVRAM(
			c->ramPort,
			c->registerPort,
			data,
			byte_count,
			c->generatorTableAddr
		);

//	vdpCopyToVRAM( context, data, byte_count, c->generatorTableAddr );
}

void vdpWriteNGeneratorTableEntries( 
	const void* context,
	const unsigned char* data,
	unsigned int startEntryIndex,
	unsigned int entryCount
) {
	RC9918Context* c = (RC9918Context*)context;
	_vdpCopyDataToVRAM(
			c->ramPort,
			c->registerPort,
			data,
			8*entryCount,
			c->generatorTableAddr + startEntryIndex*8
		);

// 	vdpCopyToVRAM(
// 			context,
// 			data,
// 			8*entryCount,
// 			c->generatorTableAddr + startEntryIndex*8
// 		);
}

void vdpCopyToGeneratorTableEntry( 
	const void* context,
	const unsigned char* data,
	unsigned int entryIndex
) {
	vdpWriteNGeneratorTableEntries( context, data, entryIndex, 1 );
}

void vdpWriteText( const void* context, unsigned char xpos, unsigned char ypos, const unsigned char* str )
{
	const RC9918Context* c = (const RC9918Context*)context;
	unsigned int vram_offset = c->nameTableAddr + (ypos*SCREEN_COLUMNS(c) + xpos);
	
	_vdpCopyDataToVRAM( c->ramPort, c->registerPort, str, strlen(str), vram_offset );
}

void vdpWriteNCharacters(
	const void* context,
	unsigned char start_xpos,
	unsigned char start_ypos,
	unsigned char value,
	unsigned int repeat_count
) {
	const RC9918Context* c = (const RC9918Context*)context;
	unsigned int vram_offset = c->nameTableAddr 
								+ (start_ypos*SCREEN_COLUMNS(c) + start_xpos);
	
	_vdpWriteValueToVRAM(
			c->ramPort,
			c->registerPort,
			value,
			repeat_count,
			vram_offset
		);
}

void vdpWriteCharacter( const void* context, unsigned char xpos, unsigned char ypos, unsigned char value )
{
	vdpWriteNCharacters( context, xpos, ypos, value, 1 );
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
	
	printf("\n   Name table loc = 0x%X\n", c->nameTableAddr );
	printf("   Color table loc = 0x%X\n", c->colorTableAddr );
	printf("   Generator table loc = 0x%X\n", c->generatorTableAddr );
	printf("   Sprite attributes loc = 0x%X\n", c->spriteAttributesAddr );
	printf("   Sprite patterns loc = 0x%X\n", c->spritePatternsAddr );
	printf("   Column count = %d\n", SCREEN_COLUMNS(c) );
	printf("\n");
}

#pragma mark - Text Mode

const unsigned char vdp_TEXTMODE_REGISTER_TABLE[8] = { 0x00, 0xD0, 0x00, 0x00, 0x01, 0x00, 0x00, 0xF1 };

void vdpSetTextMode( void* context ) 
{
	RC9918Context* c = (RC9918Context*)context;
	c->graphicsMode = GMODE_TEXT;

	for (int i = 0; i < 8; i++ ) {
		c->registerTable[i] = vdp_TEXTMODE_REGISTER_TABLE[i];
	}
	_vdpApplyRegisterTable( c );
	_vdpInitContextVRAMAddresses( c );

	vdpSetDefaultFont( context );
		
	// blank the screen
	_vdpWriteValueToVRAM(
			c->ramPort,
			c->registerPort,
			0x20,
			SCREEN_COLUMNS(c)*24,
			c->nameTableAddr
		);
}

void vdpSetTextModeBackgroundColor( void* context, unsigned char color )
{
	RC9918Context* c = (RC9918Context*)context;
	unsigned char cur_color = c->registerTable[vdp_REGISTER_COLOR];
	unsigned char value = (cur_color&0xF0) + (color&0x0F);
	
	_vdpSetRegister( c, value, vdp_REGISTER_COLOR );	
}
void vdpSetTextModeForegroundColor( void* context, unsigned char color )
{
	RC9918Context* c = (RC9918Context*)context;
	unsigned char cur_color = c->registerTable[vdp_REGISTER_COLOR];
	unsigned char value = ((color<<4)&0xF0) + (cur_color&0x0F);
	
	_vdpSetRegister( c, value, vdp_REGISTER_COLOR );	
}

#pragma mark - Graphics Mode 1

const unsigned char vdp_GMODE1_REGISTER_TABLE[8] = { 0x00, 0xC3, 0x00, 0x0C, 0x02, 0x08, 0x01, 0x01 };

void vdpSetGraphicsMode1( void* context ) 
{
	RC9918Context* c = (RC9918Context*)context;

	c->graphicsMode = GMODE_GRAPHICS_1;
	
	for (int i = 0; i < 8; i++ ) {
		c->registerTable[i] = vdp_GMODE1_REGISTER_TABLE[i];
	}
	_vdpApplyRegisterTable( c );
	_vdpInitContextVRAMAddresses( c );
	
	vdpSetDefaultFont( context );
	
	// blank the screen
	_vdpWriteValueToVRAM(
			c->ramPort,
			c->registerPort,
			0x20,
			SCREEN_COLUMNS(c)*24,
			c->nameTableAddr
		);
	
	// set colors to white on black. 
	_vdpWriteValueToVRAM(
			c->ramPort,
			c->registerPort,
			0xF1,
			SCREEN_COLUMNS(c),
			c->colorTableAddr
		);
}

void vdpSetGraphicsMode1ColorEntry( const void* context, unsigned char color, unsigned int entryIndex )
{
	const RC9918Context* c = (const RC9918Context*)context;
	
	_vdpSetVRAMAddress( c->registerPort, c->colorTableAddr + entryIndex );
	outp( c->ramPort, color );
}

#pragma mark - Graphics Mode 2

const unsigned char VDP_GMODE2_REGISTER_TABLE[8] = { 0x02, 0xC0, 0x06, 0xFF, 0x03, 0x36, 0x07, 0x01 };

void vdpSetGraphicsMode2( void* context ) 
{
	RC9918Context* c = (RC9918Context*)context;

	c->graphicsMode = GMODE_GRAPHICS_2;
	for (int i = 0; i < 8; i++ ) {
		c->registerTable[i] = VDP_GMODE2_REGISTER_TABLE[i];
	}
	_vdpApplyRegisterTable( c );
	_vdpInitContextVRAMAddresses( c );

	// blank the screen
	_vdpSetVRAMAddress( c->registerPort, c->nameTableAddr );
	for (int i = 0; i < 32*24; i++ ) {
		outp( c->ramPort, 0 );
	}

	// set colors to white on black. 
	_vdpSetVRAMAddress( c->registerPort, c->colorTableAddr );
	for (int i = 0; i < 0x800; i++ ) {
		outp( c->ramPort, 0xF1 );
	}
}

void vdpCopyGraphicsMode2GeneratorTableEntries( 	
	const void* context,
	const unsigned char* data,
	unsigned int startBlockNum,
	unsigned int startEntryIndex,
	unsigned int entryCount
) {
	const RC9918Context* c = (const RC9918Context*)context;
	vdpCopyToVRAM(
			context,
			data,
			8*entryCount,
			c->generatorTableAddr + startBlockNum*0x800 + startEntryIndex*8
		);
}

void vdpCopyNGraphicModeColorTableEntries( 
	const void* context,
	const unsigned char* data,
	unsigned int startBlockNum,
	unsigned int startEntryIndex,
	unsigned int entryCount
) {
	const RC9918Context* c = (const RC9918Context*)context;
	vdpCopyToVRAM(
			context,
			data,
			8*entryCount,
			c->colorTableAddr + startBlockNum*0x800 + startEntryIndex*8
		);
}

void vdpSetNGraphicModeColorTableEntries( 
	const void* context,
	const unsigned char value,
	unsigned int startBlockNum,
	unsigned int startIndex,
	unsigned int count
) {
	const RC9918Context* c = (const RC9918Context*)context;
	
	_vdpWriteValueToVRAM(
			c->ramPort,
			c->registerPort,
			value,
			count,
			c->colorTableAddr + startBlockNum*0x800 + startIndex
		);
}

#pragma mark - Sprites

void vdpSetSpriteSizeAndMagnification(
	void* context,
	bool is16x16Sprite,
	bool isMagnified
) {
	RC9918Context* c = (RC9918Context*)context;
	
	unsigned char curReg1 = c->registerTable[1];
	
	unsigned char newReg1 = (curReg1&0xFC);	// keep other values
	if (is16x16Sprite) {
		newReg1 += 0x1;
	}
	if (isMagnified) {
		newReg1 += 0x2;
	}
	
	_vdpSetRegister( c, newReg1, vdp_REGISTER_VDP_OPTIONS );
}

void vdpSetSpritePatterns(
	const void* context,
	unsigned char* pattern_data,
	unsigned int data_size
) {
	const RC9918Context* c = (const RC9918Context*)context;

	vdpCopyToVRAM( context,  pattern_data, data_size,  c->spritePatternsAddr );
}

void vdpWriteSpriteAttributes(
	const void* context,
	unsigned int firstSpriteIndex,
	unsigned int countAttributes,
	const SpriteAttribute* spriteData
) {
	const RC9918Context* c = (const RC9918Context*)context;

	unsigned int vram_addr = c->spriteAttributesAddr + firstSpriteIndex*sizeof(SpriteAttribute);
	
	vdpCopyToVRAM( context,  spriteData, countAttributes*sizeof(SpriteAttribute),  vram_addr);
}

