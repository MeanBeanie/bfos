[BITS 64]

global set_gdt
global reload_segments

gdtr:
	DW 0
	DQ 0

set_gdt:
	mov [gdtr], di
	mov [gdtr+2], rsi
	lgdt [gdtr]
	ret

; basically "returns" to the reload_cs label
; this is b/c far jumps don't work in long mode
reload_segments:
	push 0x08
	lea rax, [rel .reload_cs]
	push rax
	retfq
.reload_cs:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
