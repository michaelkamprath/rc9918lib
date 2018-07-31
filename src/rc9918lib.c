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

#define TEXT_MODE_FONT_ADDR		0x0800

#define TMSREGBIT 0x80

#define TMS_REGISTER_COLOR	7

typedef struct {
	const unsigned char*	font;
	unsigned int	fontMemSize;
	unsigned char 	ramPort;
	unsigned char 	registerPort;
	unsigned char 	registerTable[8];
} RC9918Context;

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
const unsigned char TMS_TEXTMODE_REGISTER_TABLE[8] = { 0x00, 0xD0, 0x00, 0x00, 0x01, 0x00, 0x00, 0xF1 };

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
	
	c->font = 0;
	c->fontMemSize = 0;
	c->ramPort = ramPort;
	c->registerPort = registerPort;
	
	for (int i =0; i < 8; i++ ) {
		c->registerTable[i] = TMS_BLANK_REGISTER_TABLE[i];
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
		// TODO - do I need some nops here?
		for (int nop = 0; nop < 2; nop++) {;}
	}
}

#pragma mark Text Mode

void tmsSetTextMode( void* context ) 
{
	RC9918Context* c = (RC9918Context*)context;

	tmsSetTextModeFont( context, default_font_6x8, FONT_MEMORY_SIZE );

	for (int i = 0; i < 8; i++ ) {
		c->registerTable[i] = TMS_TEXTMODE_REGISTER_TABLE[i];
	}

	_tmsApplyRegisterTable( c );
	
	// blank the screen
	_tmsSetVRAMAddress( c, 0 );
	for (int i = 0; i < 40*24; i++ ) {
		outp( c->ramPort, 0x20 );
	}	
}

void tmsSetTextModeFont(
	void* context,
	const unsigned char* font_data,
	unsigned int font_byte_count
) {
	RC9918Context* c = (RC9918Context*)context;

	c->font = font_data;
	c->fontMemSize = font_byte_count;
	
	tmsWriteToVRAM( context, c->font, c->fontMemSize, TEXT_MODE_FONT_ADDR );
	
}

void tmsSetTextModeBackgroundColor( void* context, unsigned char color )
{
	RC9918Context* c = (RC9918Context*)context;
	unsigned char cur_color = c->registerTable[TMS_REGISTER_COLOR];

	unsigned char value = (cur_color&0xF0) + (color&0x0F);
	
	_tmsSetRegister( c, value, TMS_REGISTER_COLOR );	
}

void tmsWriteText( const void* context, unsigned char xpos, unsigned char ypos, const unsigned char* str )
{
	const RC9918Context* c = (const RC9918Context*)context;
	unsigned int vram_offset = ypos*40 + xpos;
	
	_tmsSetVRAMAddress( c, vram_offset );	
	int i = 0;	
	while ( str[i] != 0 ) {
		outp( c->ramPort, str[i]);
		// TODO - do I need some nops here?
		for (int nop = 0; nop < 2; nop++) {;}
		i++;
	}

}

void tmsWriteCharacter( const void* context, unsigned char xpos, unsigned char ypos, unsigned char value )
{
	const RC9918Context* c = (const RC9918Context*)context;
	unsigned int vram_offset = ypos*40 + xpos;
	
	_tmsSetVRAMAddress( c, vram_offset );
	outp( c->ramPort, value);	
}

void debugPrintContext( const void* context )
{
	const RC9918Context* c = (const RC9918Context*)context;

	printf("context data:\n");
	printf("   RAM port = 0x%X\n", c->ramPort );
	printf("   Register port = 0x%X\n", c->registerPort );
	printf("   font ptr = 0x%X\n", c->font );
	printf("   font size = %d\n", c->fontMemSize );
	for (int i = 0; i < 8; i++ ) {
		printf("   register[%d] = 0x%X\n", i, c->registerTable[i]);
	}
}

