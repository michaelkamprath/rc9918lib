

SECTION code_user

PUBLIC __initSpriteAndScrollHandler
PUBLIC __disableSpriteAndScrollHandler

;
;
	
	
	
defc im1vect = $38                 		; location of interrupt mode 1 vector

defc tmsclkdiv = 3	 					; Z80 clock divider
                                        ; 1 for <= 10 MHz
                                        ; 2 for <= 5 MHz
                                        ; 3 for <= 3.33 MHz

; 
; Global Variables
;

.v_ramPort				defs	1, 0		; VDP ram port 
.v_registerPort			defs	1, 0		; VDP register port
.v_nameTableAddr		defs	2, 0		; VRAM Address for name table
.v_generatorTableAddr	defs	2, 0		; VRAM Address for generator/pattern table
.v_colorTableAddr		defs	2, 0		; VRAM Address for color table
.v_spriteAttributesAddr	defs	2, 0		; VRAM Address for sprite attributes block
.v_spritePatternsAddr	defs	2, 0		; VRAM Address for spatterns data

.b_origInterruptOpCodes	defs	8,0			; buffer for original $38 interrupt opcodes

;
; _initSpriteAndScrollHandler:
;	sets up sprite and scroll handler
;	
; _initSpriteAndScrollHandler(
;		unsigned char ramPort,					// 14
;		unsigned char registerPort,				// 12
;		unsigned int nameTableAddr,				// 10
;		unsigned int generatorTableAddr,		// 8 
;		unsigned int colorTableAddr,			// 6
;		unsigned int spriteAttributesAddr,		// 4
;		unsigned int spritePatternsAddr,		// 2
;	)	__smallc
;

.hello_world			defm			"Hello World!"
defc hello_world_len = 12

_initSpriteAndScrollHandler:
__initSpriteAndScrollHandler:
	ld		ix,0
	add		ix,sp
	ld		c,(ix+12)				; c = registerPort
	ld		hl, v_registerPort
	ld		(hl),c					; save register port to variable
	ld		c,(ix+14)				; c = ramPort
	ld		hl, v_ramPort
	ld		(hl),c					; save ram port to variable
	ld		c,(ix+10)
	ld		b,(ix+11)				; bc = nameTableAddr
	ld		hl,v_nameTableAddr
	ld		(hl),b
	inc		hl
	ld		(hl),c					; save name table VRAM address
		
	; set up interupt
	ld		hl,interrupthandler
	call	setupinterrupt

	; test code
	di
	ld		a,(v_registerPort)
	ld		c,a
	ld		a,(v_nameTableAddr)
	ld		d,a
	ld		a,(v_nameTableAddr+1)
	ld		e,a
	call	set_vram_addr_write
	ld		a,(v_ramPort)
	ld		c,a
	ld		de,hello_world_len
	ld		hl,hello_world
	call 	copy_data_to_vram
	ei

	ret


_disableSpriteAndScrollHandler:
__disableSpriteAndScrollHandler:
	di									; disable interrupts
	; load original opcodes
	ld		a,(b_origInterruptOpCodes)
	ld      (im1vect),a	
	ld		a,(b_origInterruptOpCodes+1)		
	ld      (im1vect+1),a	
	ld		a,(b_origInterruptOpCodes+2)		
	ld      (im1vect+2),a	
	ret
	
; setupinterrupt:
; 	set up interrupt mode 1 vector
;       HL = interrupt handler
setupinterrupt:
	di									; disable interrupts
	push 	hl
	
	; first, copy current $38 interrupt opcodes to buffer
	ld		ix,b_origInterruptOpCodes
	ld		a,(im1vect)
	ld		(ix),a
	ld		a,(im1vect+1)
	ld		(ix+1),a
	ld		a,(im1vect+2)
	ld		(ix+2),a
	
	; add jp opcodes to the end of saved $38 interrupt 
	ld		a,$C3
	ld		(ix+3),a
	ld		hl,im1vect+3
	ld		(ix+4),h
	ld		(ix+5),l
	
	pop		hl
	ld      a, $C3                  	; prefix with jump instruction
	ld      (im1vect), a			
	ld      (im1vect+1), hl         	; load interrupt vector
	im      1                       	; enable interrupt mode 1
	ei									; reenable interrupts
	ret


; interrupthandler
;	Handle the interrupt
;		no args
;	
;	Storage
;		interrupt_previous_stack = value of sp at interrupt entry
;		interrupt_stack = block of memory to be used for interrupt stack
;		interrupt_stack_start = end of interrupt_stack block, which is stack start

.v_interruptCounter		defs	1,0
interrupt_previous_stack:
	defs 2,0 
interrupt_stack:
	defs 256,0
interrupt_stack_start:

interrupthandler:
	ld ( interrupt_previous_stack ), sp		; save current stack pointer
	ld sp, interrupt_stack_start 			; use our own stack for this interrupt
	push af									; save register state
	push bc
	push de
	push hl
	push ix
	push iy
	exx
	ex af, af'
	push af
	push bc
	push de
	push hl	
	
	; read VDP register status and test to see if this is a VSP interrupt
	ld		a,(v_registerPort)				; get saved register port
	ld		c,a								; c = register port
	in      a, (c)							; read VSP status register
	ld		b,0x80							; bit mask for bit 0
	and		b
	jp		nz,interrupt_handler_code		; if F flag is set, do the interrupt
	
	; retore state and jump to saved $38 vector
	pop hl									; restore registers
	pop de
	pop bc
	pop af
	ex af, af'
	exx
	pop iy
	pop ix
	pop hl
	pop de
	pop bc
	pop af
	ld sp, ( interrupt_previous_stack )		; restore stack pointer
	jp b_origInterruptOpCodes


	; start of interrupt routine
interrupt_handler_code:
	ld		a,(v_interruptCounter)
	ld		b,0x1E						; check to see if loop count is 60
	cp		b
	jr		nz,increment_counter_value
	call	display_text_counter
	ld		a,0
	jr 		save_updated_counter_value
increment_counter_value:
	inc		a
save_updated_counter_value:	
	ld		(v_interruptCounter),a
	ld		b,$F
	cp		b
	jr		nz,return_from_interrupt
	call	toggle_display_interrupt_heartbeat

return_from_interrupt:
	pop hl									; restore registers
	pop de
	pop bc
	pop af
	ex af, af'
	exx
	pop iy
	pop ix
	pop hl
	pop de
	pop bc
	pop af
	ld sp, ( interrupt_previous_stack )		; restore stack pointer
	ei										; re-enable interrupts
	reti									; return from interrupt
; toggle_display_interrupt_heartbeat
;	toggles the display of the heart character in the lower right corner of screen
;		no arguments
;	Storage
;		v_heartbeattoggle = boolean indicating heartbeat on/off state
;		v_heartbeatCharacterBuffer = memory location of current heartbeat char
defc heartbeat_char = 3
.v_heartbeattoggle			defs	1,0
.v_heartbeatCharacterBuffer	defs	1,0
toggle_display_interrupt_heartbeat:
	ld		a,(v_nameTableAddr)
	ld		h,a
	ld		a,(v_nameTableAddr+1)
	ld		l,a
	ld		de,$2FF
	add		hl,de
	ld		d,h
	ld		e,l
	call	set_vram_addr_write
	ld		a,(v_heartbeattoggle)
	ld		b,1
	cp		b
	jr		z,turn_heartbeat_off
turn_heartbeat_on:
	ld		a,1
	ld		(v_heartbeattoggle),a
	ld		a,heartbeat_char
	ld		(v_heartbeatCharacterBuffer),a
	jr		send_heartbeat_to_vram
turn_heartbeat_off:
	ld		a,0
	ld		(v_heartbeattoggle),a
	ld		a,0x20
	ld		(v_heartbeatCharacterBuffer),a
send_heartbeat_to_vram:
	ld		de,1
	ld		hl,v_heartbeatCharacterBuffer
	call 	copy_data_to_vram
	ret	

; display_text_counter
;	no arguments
;
;	Storage
;		v_interruptTextCounter = counter	
;		counter_text = string constant
;
.v_interruptTextCounter	defs 1,0
.counter_text			defm			"0123456789ABCDEF"

display_text_counter:
	ld		a,(v_nameTableAddr)
	ld		h,a
	ld		a,(v_nameTableAddr+1)
	ld		l,a
	ld		de,64
	add		hl,de
	ld		d,h
	ld		e,l
	call	set_vram_addr_write

	ld		a,(v_interruptTextCounter)
	ld		b,0xF
	cp		b
	jr		nz,inc_text_counter
	ld		a,0
	jr		save_text_counter
inc_text_counter:
	inc		a
save_text_counter:
	ld		(v_interruptTextCounter),a
	; now display the counter text
	
	ld		hl,counter_text
	ld		e,a
	ld		d,0
	add		hl,de
	ld		de,1
	call	copy_data_to_vram
	ret		
		
	

; set_vram_addr_write:
; 	sets the VDP VRAM address for writing
;		de	= desired VRAM address
set_vram_addr_write:
	ld		a,(v_registerPort)		; get saved register port
	ld		c,a						; c = register port
	ld		a, e					; send lsb of address
	out		(c), a
	ld		a, d					; mask off msb to max of 16KB
	and		$3F
	or		$40						; set second highest bit to indicate write
	out		(c), a					; send msb
	ret

; copy_data_to_vram:
; 	copies a block of data to VRAM. Assumes VRAM address is set.
;		hl	= address of block to copy
;		de	= size of block
copy_data_to_vram:
	ld		a,(v_ramPort)			; get saved ram port
	ld		c,a						; c = ram port
copy_data_to_vram_loop:
	ld		a,(hl)					; get the current byte from ram
	out		(c),a					; send it to vram
	defs	11/tmsclkdiv, 0			; nops to waste time
	inc		hl						; next byte
	dec		de 						; continue until count is zero
	ld		a, d
	or		e
	jr		nz, copy_data_to_vram_loop
	ret
	
