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
// 		https://github.com/jblang/rc9918/blob/master/examples/ascii.asm

#include <stdio.h>
#include "rc9918lib.h"

#define CHAR_dblhorizontal	205
#define CHAR_dblvertical	186
#define CHAR_dbltopleft		201
#define CHAR_dbltopright	187
#define CHAR_dblbottomleft	200
#define CHAR_dblbottomright	188

void setBorderLineBuffer( char* buffer, char leftChar, char midChar, char rightChar )
{
	buffer[0] = leftChar;	
	for (int i = 1; i < 39; i++ ) {
		buffer[i] = midChar;
	}
	buffer[39] = rightChar;
	buffer[40] = 0;
}

void main( void ) 
{
	char lineBuffer[41];
	const char* title_str = "ASCII Character Set";
	
	printf("Initing TMS9918A\n");
	void* context = vdpInitBoard( RC9918_DEFAULT_RAMPORT, RC9918_DEFAULT_REGISTER_PORT );
	
	if (context == 0) {
		printf("ERROR - could not allocated context!");
		return;
	}
	
	printf("initing text mode.\n");
	vdpSetTextMode( context );
	vdpSetTextModeBackgroundColor( context, VDPCOLOR_DARK_BLUE );
	
	debugPrintContext(context);
	
	printf("Printing border\n");
	setBorderLineBuffer( lineBuffer, CHAR_dbltopleft, CHAR_dblhorizontal, CHAR_dbltopright );
	vdpWriteText( context, 0, 0, lineBuffer );
	
	for ( int row = 1; row < 23; row++ ) {
		vdpWriteCharacter( context, 0, row, CHAR_dblvertical );
		vdpWriteCharacter( context, 39, row, CHAR_dblvertical );
	}

	setBorderLineBuffer( lineBuffer, CHAR_dblbottomleft, CHAR_dblhorizontal, CHAR_dblbottomright );
	vdpWriteText( context, 0, 23, lineBuffer );
	
	printf("Writing string to VRAM = %s\n", title_str);
	vdpWriteText( context, 11, 2, title_str );
	
	printf("Writing ASCII characters to VRAM\n");
	int padding = 4;
	int linelength = 40 - (2*padding);
	int ypos = 6;
	int i = 1;
	// the 0 character terminates a C string. Just print space, as character 0 is empty anyway.
	lineBuffer[0] = 0x20;
	for ( int c = 1; c <= 255; c++ ) {
		lineBuffer[i] = c;
		i++;
		if (i >= linelength) {
			lineBuffer[i] = 0;
			vdpWriteText( context, padding, ypos, lineBuffer );
			ypos += 2;			
			if ((ypos >= 22) && (c < 255)) {
				// too far. just stop. 
				printf("ERROR - Not enough space to print all characters. Adjust borders.");
				break;
			}
			i = 0;
		}
	}
	printf("Done!\n");
}
