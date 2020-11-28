#include <stdio.h>
#include <im2.h>

#include "rc9918lib.h"
#include "rc9918lib_Internal.h"


extern void _initSpriteAndScrollHandler(
		unsigned char ramPort,					// 14
		unsigned char registerPort,				// 12
		unsigned int nameTableAddr,				// 10
		unsigned int generatorTableAddr,		// 8 
		unsigned int colorTableAddr,			// 6
		unsigned int spriteAttributesAddr,		// 4
		unsigned int spritePatternsAddr			// 2
	)	__smallc;

extern void _disableSpriteAndScrollHandler( void ) __smallc;

void main( void ) 
{
	void* context = vdpInitBoard( RC9918_DEFAULT_RAMPORT, RC9918_DEFAULT_REGISTER_PORT );
	
	if (context == 0) {
		printf("ERROR - could not allocated context!");
		return;
	}
	
	printf("initing graphics mode 2.\n");
	vdpSetGraphicsMode2( context );
	vdpSetSpriteSizeAndMagnification( context, 1, 1 );
	
	const unsigned char textColor = 0xF1;
	vdpSetNGraphicModeColorTableEntries( context, textColor, 0, 0, 0x1800 );
	vdpSetDefaultFont( context );

	RC9918Context* c = (RC9918Context*)context;

	_initSpriteAndScrollHandler(
			c->ramPort,
			c->registerPort,
			c->nameTableAddr,
			c->generatorTableAddr,
			c->colorTableAddr,
			c->spriteAttributesAddr,
			c->spritePatternsAddr			
		);
	printf("Activating interupt\n");
	_vdpWriteRegister( c, 0xE0, 1 );
	for ( long i = 0; i < 100000; i++ ) {
		if (i%100 == 0 ) {
		}
	}
	_vdpWriteRegister( c, 0xC0, 1 );
	printf("\nDone!\n");
	
	_disableSpriteAndScrollHandler();
}
