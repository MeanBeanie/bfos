[BITS 16]
[ORG 0x7C00]

global _start

_start:
	mov si, boot_msg
	call print16
	call switch_to_protected_mode

print16:
	lodsb
	test al,al
	jz .done
	mov ah, 0x0E
	int 0x10
	jmp print16
.done:
	ret

switch_to_protected_mode:
	cli

	lgdt [gdt32_descriptor]

	mov eax, cr0
	or eax, 0x1
	mov cr0, eax

	jmp GDT32_CODE:protected_mode

[BITS 32]

protected_mode:
	mov ax, GDT32_DATA ; initialize all registers to data segment offset

	mov ds, ax ; data segment reg
	mov ss, ax ; stack segment reg
	mov es, ax ; gp reg
	mov fs, ax ; gp reg
	mov gs, ax ; gp reg

	mov ebp, 0x7C00 ; update stack position
	mov esp, ebp

	mov edi, PML4T_ADDR
	mov cr3, edi

	xor eax, eax
	mov ecx, SIZEOF_PAGE_TABLE
	rep stosd ; writes 4 * SIZEOF, enough for 4 tables

	mov edi, cr3 ; reset di back to beginning of table
	mov DWORD [edi], PDPT_ADDR & PT_ADDR_MASK | PT_PRESENT | PT_READABLE

	mov edi, PDPT_ADDR
	mov DWORD [edi], PDT_ADDR & PT_ADDR_MASK | PT_PRESENT | PT_READABLE
	
	mov edi, PDT_ADDR
	mov DWORD [edi], PT_ADDR & PT_ADDR_MASK | PT_PRESENT | PT_READABLE

	mov edi, PT_ADDR
	mov ebx, PT_PRESENT | PT_READABLE
	mov ecx, ENTRIES_PER_PT

.set_entry:
	mov DWORD[edi], ebx
	add ebx, PAGE_SIZE
	add edi, SIZEOF_PT_ENTRY
	loop .set_entry

	mov eax, cr4
	or eax, CR4_PAE_ENABLE
	mov cr4, eax

	; set the LM bit

	mov ecx, 0xC0000080
	rdmsr
	or eax, 0x100
	wrmsr

	; enable paging
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	lgdt [gdt64_descriptor]
	jmp GDT64_CODE:long_mode_start

[BITS 64]

long_mode_start:
	mov ax, GDT64_DATA

	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov rdi, 0xB8000
	mov rax, 0
	mov rcx, VGA_TEXT_BUFFER_SIZE / 8

	rep stosq
	hlt

gdt32_start:

gdt32_null: ; must start with null segment
	dd 0x0
	dd 0x0

gdt32_code:
	dw 0xFFFF     ; 0-1: segment limit                       15:00
	dw 0x0        ; 2-3: base address                        15:00
	db 0x0        ; 4:   base address                        23:16
	db 10011010b  ; 5:   1st flags and type flags
	db 11001111b  ; 6:   2nd flags, upper nibble of limit
	db 0x0        ; 7:   base address                        31:24

gdt32_data: ; same as code segment but changed type flags
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0

gdt32_end:

gdt32_descriptor:
	dw gdt32_end - gdt32_start - 1
	dd gdt32_start

GDT32_CODE equ gdt32_code - gdt32_start ; 0x08
GDT32_DATA equ gdt32_data - gdt32_start ; 0x10

gdt64_start:
gdt64_null:
	dd 0
	dd 0
gdt64_code:
	dw 0xFFFF
	dw 0
	db 0
	db 10011010b
	db 10101111b
	db 0
gdt64_data:
	dw 0xFFFF
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0
gdt64_end:
gdt64_descriptor:
	dw gdt64_end - gdt32_start - 1
	dd gdt64_start

GDT64_CODE equ gdt64_code - gdt64_start
GDT64_DATA equ gdt64_data - gdt64_start

PML4T_ADDR equ 0x1000
SIZEOF_PAGE_TABLE equ 4096
PDPT_ADDR equ 0x2000
PDT_ADDR equ 0x3000
PT_ADDR equ 0x4000

PT_ADDR_MASK equ 0xffffffffff000
PT_PRESENT equ 1
PT_READABLE equ 1

ENTRIES_PER_PT equ 512
SIZEOF_PT_ENTRY equ 8
PAGE_SIZE equ 0x1000

CR4_PAE_ENABLE equ 1 << 5

COLS equ 80
ROWS equ 25
BPC equ 2
VGA_TEXT_BUFFER_SIZE equ BPC * COLS * ROWS

boot_msg: db "[Loaded in 16 bit mode]", 0
lm_msg:   db "[Switched to 64 bit long mode]", 0

times 510-($-$$) db 0
dw 0xAA55
