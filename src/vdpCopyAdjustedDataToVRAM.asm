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

PUBLIC _vdpCopyAdjustedDataToVRAM
PUBLIC __vdpCopyAdjustedDataToVRAM

; void _vdpCopyAdjustedDataToVRAM( 
;				unsigned char ramPort,
;				unsigned char registerPort,
;				unsigned char *data_ptr,
;				unsigned int data_size,
;				unsigned char data_value_increment,
;				unsigned int vram_start_address
;			) __smallc;

_vdpCopyAdjustedDataToVRAM:
__vdpCopyAdjustedDataToVRAM:
	ld		ix,0
	add		ix,sp
	
	; first, set the vram address
	ld		c,(ix+10)		; c = registerPort
	ld		e,(ix+2)
	ld		d,(ix+3)		; de = vram_start_address
	ld		a,e
	out		(c),a			; send lsb of VRAM addr
	ld		a,d
	and		$3F
	or		$40				; set second highest bit to indicate write
	out		(c),a			; send msb of VRAM address

	; now write data to vram
	ld		l,(ix+8)
	ld		h,(ix+9)		; hl = data_ptr
	ld		e,(ix+6)
	ld		d,(ix+7)		; de = data_size
	ld		c,(ix+12)		; c = ramPort
	ld		b,(ix+4)		; b = data_value_increment

copyloop:
	ld		a,(hl)			; get the current byte from ram
	add		a, b			; increment a value by data_value_increment
	out		(c),a			; send it to vram
	defs	5, 0 			; nops to waste time
	inc		hl				; next byte
	dec		de 				; continue until count is zero
	ld		a, d
	or		e
	jr		nz, copyloop
	ret
