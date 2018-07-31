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
#include "rc9918lib.h"

#define CHAR_dblhorizontal	205
#define CHAR_dblvertical	186
#define CHAR_dbltopleft		201
#define CHAR_dbltopright	187
#define CHAR_dblbottomleft	200
#define CHAR_dblbottomright	188

void setBoarderLineBuffer( char* buffer, char leftChar, char midChar, char rightChar )
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
	
	printf("Initing TMS9918\n");
	void* context = tmsInitBoard( RC9918_DEFAULT_RAMPORT, RC9918_DEFAULT_REGISTER_PORT );
	
	if (context == 0) {
		printf("ERROR - could not allocated context!");
		return;
	}
	
	printf("initing text mode.\n");
	tmsSetTextMode( context );
	tmsSetTextModeBackgroundColor( context, TMSCOLOR_DARK_BLUE );
	
	printf("Printing border\n");
	setBoarderLineBuffer( lineBuffer, CHAR_dbltopleft, CHAR_dblhorizontal, CHAR_dbltopright );
	tmsWriteText( context, 0, 0, lineBuffer );
	
	for ( int row = 1; row < 23; row++ ) {
		tmsWriteCharacter( context, 0, row, CHAR_dblvertical );
		tmsWriteCharacter( context, 39, row, CHAR_dblvertical );
	}

	setBoarderLineBuffer( lineBuffer, CHAR_dblbottomleft, CHAR_dblhorizontal, CHAR_dblbottomright );
	tmsWriteText( context, 0, 23, lineBuffer );
	
	printf("Writing string to VRAM = %s\n", title_str);
	tmsWriteText( context, 11, 2, title_str );
	
	printf("Writing ASCII characters to VRAM\n");
	int padding = 4;
	int linelength = 40 - (2*padding);
	int ypos = 6;
	int i = 0;
	for ( int c = 0; c <= 255; c++ ) {
		lineBuffer[i] = c;		
		i++;
		if (i >= linelength) {
			lineBuffer[i] = 0;
			tmsWriteToVRAM( context, lineBuffer, linelength, ypos*40 + padding );
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
