#include "pic.h"

void outb(uint16_t port, uint8_t val){
	__asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port){
	uint8_t ret;
	__asm__ volatile (
		"inb %w1, %b0"
		: "=a"(ret)
		: "Nd"(port)
		: "memory"
	);

	return ret;
}

void io_wait(void){
	// can use any unused port, Linux defaults to 0x80 so i do too ig
	outb(0x80, 0);
}

void pic_send_eoi(uint8_t irq){
	// checks if the irq came from the slave PIC
	if(irq >= 8){
		outb(PIC2_COMMAND, PIC_EOI);
	}

	outb(PIC1_COMMAND, PIC_EOI);
}

// io wait calls used as older machines needed small delays for the PICs to react
void pic_remap(int offset1, int offset2){
	// starts init in cascade mode
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	// set the vector offsets
	outb(PIC1_DATA, offset1);
	io_wait();
	outb(PIC2_DATA, offset2);
	io_wait();

	// tells the master that a slave pic exists at IRQ2
	outb(PIC1_DATA, 1 << CASCADE_IRQ);
	io_wait();
	// give the slave its cascade id
	outb(PIC2_DATA, 2);
	io_wait();

	// have both pics use 8086 mode (as in x86)
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// unmasks the pics
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

void irq_set_mask(uint8_t irq_line){
	uint16_t port;
	if(irq_line < 8){
		port = PIC1_DATA;
	}
	else{
		port = PIC2_DATA;
		irq_line -= 8;
	}

	uint8_t value = inb(port) | (1 << irq_line);
	outb(port, value);
}

void irq_clear_mask(uint8_t irq_line){
	uint16_t port;
	if(irq_line < 8){
		port = PIC1_DATA;
	}
	else{
		port = PIC2_DATA;
		irq_line -= 8;
	}

	uint8_t value = inb(port) | ~(1 << irq_line);
	outb(port, value);
}

uint16_t __pic_get_irq_reg(int ocw3){
	outb(PIC1_COMMAND, ocw3);
	outb(PIC2_COMMAND, ocw3);

	return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t pic_get_isr(void){
	return __pic_get_irq_reg(PIC_READ_ISR);
}

uint16_t pic_get_irr(void){
	return __pic_get_irq_reg(PIC_READ_IRR);
}
