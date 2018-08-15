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

PUBLIC _vdpSetVRAMAddress
PUBLIC __vdpSetVRAMAddress


;
; void _vdpSetVRAMAddress( unsigned char registerPort, unsigned int address ) __smallc
;

_vdpSetVRAMAddress:
__vdpSetVRAMAddress:
  	ld		hl,2
	add		hl,sp			; skip over return address on stack
	ld		e,(hl)
	inc		hl
	ld		d,(hl)			; de = desired VRAM address
	inc		hl
	ld		c,(hl)			; no need to load MSB because LSB is the char, so c = registerPort
	ld		a, e			; send lsb
	out		(c), a
	ld		a, d			; mask off msb to max of 16KB
	and		$3F
	or		$40				; set second highest bit to indicate write
	out		(c), a			; send msb
	ret
	
	
	
