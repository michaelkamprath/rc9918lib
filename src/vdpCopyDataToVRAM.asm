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

PUBLIC _vdpCopyDataToVRAM
PUBLIC __vdpCopyDataToVRAM

; void _vdpCopyDataToVRAM( 
;				unsigned char ramPort,
;				unsigned char registerPort,
;				unsigned char *data_ptr,
;				unsigned int data_size,
;				unsigned int vram_start_address
;			) __smallc;

_vdpCopyDataToVRAM:
__vdpCopyDataToVRAM:
	ld		ix,0
	add		ix,sp
	
	; first, set the vram address
	ld		c,(ix+8)		; c = registerPort
	ld		e,(ix+2)
	ld		d,(ix+3)		; de = vram_start_address
	ld		a,e
	out		(c),a			; send lsb of VRAM addr
	ld		a,d
	and     $3F
	or      $40				; set second highest bit to indicate write
	out		(c),a			; send msb of VRAM address

	; now write data to vram
	ld		l,(ix+6)
	ld		h,(ix+7)		; hl = data_ptr
	ld		e,(ix+4)
	ld		d,(ix+5)		; de = data_size
	ld		c,(ix+10)		; c = ramPort

copyloop:
	ld		a,(hl)			; get the current byte from ram
	out		(c),a			; send it to vram
	defs    5, 0 			; nops to waste time
	inc     hl				; next byte
	dec     de 				; continue until count is zero
	ld      a, d
	or      e
	jr      nz, copyloop
	ret
