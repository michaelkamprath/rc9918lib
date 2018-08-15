
#ifndef RC9918LIB_INTERNAL_H_
#define RC9918LIB_INTERNAL_H_

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
	unsigned int	nameTableAddr;
	unsigned int	generatorTableAddr;
	unsigned int	colorTableAddr;
	unsigned int	spriteAttributesAddr;
	unsigned int	spritePatternsAddr;
} RC9918Context;

// Graphic mode values

#define GMODE_UNSET 		0
#define GMODE_GRAPHICS_1	1
#define GMODE_GRAPHICS_2	2
#define GMODE_MULTICOLOR	3
#define GMODE_TEXT			4

extern unsigned int SCREEN_COLUMNS_BY_MODE[];
#define SCREEN_COLUMNS(c)	SCREEN_COLUMNS_BY_MODE[c->graphicsMode]



// 
// private methods
//

void _vdpInitContextVRAMAddresses( RC9918Context* c );
void _vdpWriteRegister( const RC9918Context* c, unsigned char value, unsigned char register_num );
void _vdpSetRegister( RC9918Context* c, unsigned char value, unsigned char register_num );
void _vdpApplyRegisterTable( const RC9918Context* c );
// void _vdpSetVRAMAddress( const RC9918Context* c, unsigned int addr );

// 
// ASM implmented methods
//
extern void _vdpSetVRAMAddress( unsigned char registerPort, unsigned int address ) __smallc;
extern void _vdpCopyDataToVRAM( unsigned char ramPort,
								unsigned char registerPort,
								unsigned char *data_ptr,
								unsigned int data_size,
								unsigned int vram_start_address
							) __smallc;
void _vdpWriteValueToVRAM(	unsigned char ramPort,
							unsigned char registerPort,
							unsigned char value,
							unsigned int repeat_count,
							unsigned int vram_start_address
						) __smallc;
						
						
void _vdpCopyAdjustedSceneSectionsToVRAM(
			unsigned char ramPort,
			unsigned char registerPort,
			const unsigned char* scene_data,
			unsigned char data_value_increment,	
			unsigned int start_vram_addr,
			const unsigned int* scene_sections,
			unsigned char scene_sections_count
		) __smallc;
#endif
