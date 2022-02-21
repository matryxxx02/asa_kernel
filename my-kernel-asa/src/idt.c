#include "idt.h"
#include "ioport.h"
#include <stdint.h>

#define MAX_IDT_ENTRIES 256

/* Use this to define a hardware interrupt entry into the IDT */
#define IRQ_IDT(X) { extern void irq ## X(); idt_setup_descriptor (32+X, (uint32_t) irq ## X, 0x08, 0x8E); }

/* Use this to define a kernel-only software interrupt into the IDT */
#define KERN_IDT(X) { extern void isr ## X(); idt_setup_descriptor(X, (uint32_t) isr ## X, 0x08, 0x8E); }

/* Use this to define a software interrupt triggerable by anyone into the IDT */
#define USER_IDT(X) { extern void isr ## X(); idt_setup_descriptor (X, (uint32_t) isr ## X, 0x08, 0xEE); }


struct idt_entry_struct idt_entries[MAX_IDT_ENTRIES];

/**
 * struct idt_register is the 48 bits data structure of the intel Gobal Descriptor Table register.
 */
struct idt_register {
    uint16_t limit :16; /* 16 bits for the size of the Interrupt Descriptor Table (-1) */
    uint32_t base : 32; /* 32 bits for the address of the Interrupt Descriptor Table */
} __attribute__((packed)); /* this gcc directive avoid fields padding, base will not be aligned */

void _lidt(struct idt_register *idt_register) {
    asm("lidt (%0)" :: "r"(idt_register) :);
}

void *irq_handlers[16] = 
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void idt_setup_descriptor(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
	idt_entries[num].sel = sel;
	idt_entries[num].always0 = 0;
	idt_entries[num].flags = flags;
}

void idt_setup_handler(int irq, void (*handler)(int_regs_t *r))
{
	irq_handlers[irq] = handler;
}

void null_handler(int_regs_t *r)
{
	return;
}

void remap_irq()
{
	_outb(0x20, 0x11);
	_outb(0xA0, 0x11);
	_outb(0x21, 0x20);
	_outb(0xA1, 0x28);
	_outb(0x21, 0x04);
	_outb(0xA1, 0x02);
	_outb(0x21, 0x01);
	_outb(0xA1, 0x01);
	_outb(0x21, 0x0);
	_outb(0xA1, 0x0);
}

void setup_irq()
{
	IRQ_IDT(0);
	IRQ_IDT(1);
	IRQ_IDT(2);
	IRQ_IDT(3);
	IRQ_IDT(4);
	IRQ_IDT(5);
	IRQ_IDT(6);
	IRQ_IDT(7);
	IRQ_IDT(8);
	IRQ_IDT(9);
	IRQ_IDT(10);
	IRQ_IDT(11);
	IRQ_IDT(12);
	IRQ_IDT(13);
	IRQ_IDT(14);
	IRQ_IDT(15);

	uint8_t irq_num;
	for(irq_num = 0; irq_num < 16; irq_num++)
		idt_setup_handler(irq_num, null_handler);

	remap_irq();
}

void setup_softint()
{
	KERN_IDT(0);
	KERN_IDT(1);
	KERN_IDT(2);
	KERN_IDT(3);
	KERN_IDT(4);
	KERN_IDT(5);
	KERN_IDT(6);
	KERN_IDT(7);
	KERN_IDT(8);
	KERN_IDT(9);
	KERN_IDT(10);
	KERN_IDT(11);
	KERN_IDT(12);
	KERN_IDT(13);
	KERN_IDT(14);
	KERN_IDT(15);
	KERN_IDT(16);
	KERN_IDT(17);
	KERN_IDT(18);
	KERN_IDT(19);
	KERN_IDT(20);
	KERN_IDT(21);
	KERN_IDT(22);
	KERN_IDT(23);
	KERN_IDT(24);
	KERN_IDT(25);
	KERN_IDT(26);
	KERN_IDT(27);
	KERN_IDT(28);
	KERN_IDT(29);
	KERN_IDT(30);
	KERN_IDT(31);

    /* In idt0.s we defined a catchable interrupt 87, and want it to be triggerable from userland */
    USER_IDT(87);
}

void setup_idt()
{
    struct idt_register idt_reg;
	idt_reg.limit = sizeof(struct idt_entry_struct) * MAX_IDT_ENTRIES - 1;
	idt_reg.base = (uint32_t)&idt_entries;

	setup_irq();
	setup_softint();
	_lidt(&idt_reg);
}

void irq_handler(int_regs_t* r)
{
	void (*handler)(int_regs_t* r);
	handler = irq_handlers[r->int_no - 32];
	if(handler)
		handler(r);

	if(r->int_no >= 40){
		_outb(0xA0, 0x20);
    }
	_outb(0x20, 0x20);
}

char *int_mesg[] = {
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",

	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",

	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void isr_handler(int_regs_t* r)
{
	extern void puts(char* str);
	extern void puthex(int d);
    if(r->int_no < 32)
	{
		puts("Caught fault, halting. ");
		puts(int_mesg[r->int_no]);
		for(;;);
	}
	else
	{
        puts("I caught fault 0x");
        puthex(r->int_no);
        puts(", resuming execution\n");
		// Process your software interrupts here...
	}
}
