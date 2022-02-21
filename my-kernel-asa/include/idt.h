/**
 * Interrupt Descriptor Table structure and configuration
 */

#ifndef __IDT__
#define __IDT__

#include <stdint.h>

struct idt_entry_struct
{
	uint16_t base_lo;
	uint16_t sel;
	uint8_t  always0; 	
	uint8_t  flags; 	
	uint16_t base_hi; 
} __attribute__((packed));

struct idt_ptr_struct
{
	uint16_t limit; //!< Address limit
	uint32_t base; //!< IDT pointer base
} __attribute__((packed));

typedef const struct int_regs
{
    uint32_t ds;
	uint32_t edi; 
	uint32_t esi; 
	uint32_t ebp; 
	uint32_t esp; 
	uint32_t ebx; 
	uint32_t edx; 
	uint32_t ecx; 
	uint32_t eax; 
    uint32_t int_no; 
	uint32_t err_code; 
	uint32_t eip; 
	uint32_t cs; 
	uint32_t eflags; 
	uint32_t useresp; 
	uint32_t ss; 
} int_regs_t ;

void setup_idt();
void idt_setup_descriptor(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_setup_handler(int irq, void (*handler)(int_regs_t *r));
extern void zero_idt(struct idt_entry_struct *entries);

#endif
