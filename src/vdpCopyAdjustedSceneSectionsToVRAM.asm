; 
;     This file is part of rc9918lib - https://github.com/michaelkamprath/rc9918lib
; 
;     rc9918lib is free software: you can redistribute it and/or modify
;     it under the terms of the GNU Lesser General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version.
; 
;     rc9918lib is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with rc9918lib.  If not, see <https://www.gnu.org/licenses/>.
; 

SECTION code_user

PUBLIC _vdpCopyAdjustedSceneSectionsToVRAM
PUBLIC __vdpCopyAdjustedSceneSectionsToVRAM


; void _vdpCopyAdjustedSceneSectionsToVRAM(
;			unsigned char ramPort,					// 14
;			unsigned char registerPort,				// 12
;			const unsigned char* scene_data,		// 10
;			unsigned char data_value_increment,		// 8
;			unsigned int start_vram_addr,			// 6
;			const unsigned int* scene_sections,		// 4
;			unsigned char scene_sections_count		// 2
;		);
;
;  	This function will copy a list of non-contiguous sections of the scene_data to 
;  	a contiguous range of VRAM, ostensibly the name table. Furthermore, before copying
;  	each section os scene_data, the individual bytes are incremented by scroll_sub_step.
;
;	The scene_sections array is expected to be a array of unsigned int pairs where the
;	first unsigned int is the index into scene_data where the scene section starts
;	and the second unsigned int is for how many bytes from there to copy. It is assumed
;	that the scene sections will not overflow the scene_data buffer.
;
	

_vdpCopyAdjustedSceneSectionsToVRAM:
__vdpCopyAdjustedSceneSectionsToVRAM:	
	ld		ix,0
	add		ix,sp
	
	; first, set the vram address
	ld		c,(ix+12)				; c = registerPort
	ld		e,(ix+6)
	ld		d,(ix+7)				; de = vram_start_address
	ld		a,e
	out		(c),a					; send lsb of VRAM addr
	ld		a,d
	and		$3F
	or		$40						; set second highest bit to indicate write
	out		(c),a					; send msb of VRAM address

	; now write data to vram
	; This is done isn two loops: iterating though the sections listed 
	; in scene_sections, then transferring the identified scene data to VRAM.
	;
	; Set up section loop. de will be used to determine which 
	; two int entry in scene_sections is being processed.
	ld		de,0
sectionloop:
	ld		c,(ix+4)
	ld		b,(ix+5)		; bc = scene_sections
	ld		iy,0			; now set up iy to point to the current scene section entry
	add		iy,bc
	add		iy,de
	add		iy,de
	add		iy,de			
	add		iy,de			; adding de 4 times because each list entry is 4 bytes

	; iy is pointing at start of scene_sections entry. Now find the 
	; data in scene_data.
	push	de				; save sectionloop counter to the stack
	
	; get pointer to scene_data + section offset
	ld		l,(ix+10)
	ld		h,(ix+11)		; hl = scene_data
	ld		c,(iy)			; data in scene_sections is little endian
	ld		b,(iy+1)		; bc = offset to entry scene data index
	add		hl,bc			; hl = address to staring point in scene_data block
	
	; copy data to VRAM
	ld		c,(ix+14)		; c = ramPort
	ld		e,(iy+2)		; data in scene_sections is little endian
	ld		d,(iy+3)		; de = scene section character count
	ld		b,(ix+8)		; b = data_value_increment
copyloop:
	ld		a,(hl)			; get the current byte from scene_data
	add		a, b			; increment a value by data_value_increment
	out		(c),a			; send it to vram
	defs	5, 0 			; nops to waste time
	inc		hl				; next byte
	dec		de 				; continue until count is zero
	ld		a, d
	or		e
	jr		nz, copyloop
	
	; determine if we are done with scene section loop
	pop		de				; fetch sectionloop counter from the stack
	inc		de				; add 1
	ld		a,e				; the lower byte of de is our counter
	neg						; make a negative
	ld		b,(ix+2)		; get scene_sections_count value. 
	add		a,b				
	jr		nz,sectionloop	; loop back to sectionloop if needed.
	ret

	


