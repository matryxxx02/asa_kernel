#include "gdt.h"

struct gdt_entry_s default_gdt[DEFAULT_GDT_ENTRIES] = {
	GDT_NULL_SEGMENT(), 			/* 0x00 is not present */ 
	GDT_CODE_SEGMENT(0,0xFFFFF,1,1,0,0),	/* 0x08 is code seg: base 0, limit 4Gb, ring lvl 0 */ 
	GDT_DATA_SEGMENT(0,0xFFFFF,1,1,0,0)	/* 0x10 is data seg: base 0, limit 4Gb, ring lvl 0 */
};

/**
 * struct gdt_register is the 48 bits data structure of the intel Gobal Descriptor Table register.
 */
struct gdt_register
{
        uint16_t limit : 16;	/* 16 bits for the size of the Global Descriptor Table (-1) */
        uint32_t base  : 32;	/* 32 bits for the address of the Global Descriptor Table */
} __attribute__((packed));	/* this gcc directive avoid fields padding, base will not be aligned */ 

/**
 * gdt_activate setup the cpu gdt register and associated segments register
 *   gdt_base	: the address of the global descriptor table             (part of intel register gdt)
 *   gdt_limit	: the size, in bytes, of the global descriptor table - 1 (part of intel register gdt)
 *   cs 	: offset of the code segment to use in this table        (intel register cs)
 *   ds		: offset of the 1st data segment to use in this table    (intel register ds)
 *   es		: offset of the 2nd data segment to use in this table    (intel register es) 
 *   fs		: offset of the 3rd data segment to use in this table    (intel register fs)
 *   gs		: offset of the 4th data segment to use in this table    (intel register gs)
 *   ss		: offset of the stack segment to use in this table       (intel register ss) 
 */
void gdt_activate(struct gdt_entry_s *gdt_base, uint16_t gdt_limit, uint16_t cs, uint16_t ds, uint16_t es, uint16_t fs, uint16_t gs, uint16_t ss) {
	struct gdt_register gr;
	gr.base  = (uint32_t)gdt_base;
	gr.limit = gdt_limit;
	/* set the gdt for the 48bits ptr */
	asm( "lgdt %0" : :"m" (gr) :);  
	/* set the data segment registers (ds,es,fs,gs & ss) */
	asm( "movw %0,%%ds" : :"r" (ds) :);
	asm( "movw %0,%%es" : :"r" (es) :);
	asm( "movw %0,%%fs" : :"r" (fs) :);
	asm( "movw %0,%%gs" : :"r" (gs) :);
	asm( "movw %0,%%ss" : :"r" (ss) :);
	/* set the code segment register (cs) using a fake retf */ 
	asm( 	"pushl %0     \n"
		"push $cs_set \n"
		"retf         \n" /* far jump to cs_set with %0 as cs */
		"cs_set:      \n"
		::"r" (cs):);
}

/**
 * gdt_set_codesegment_entry is used to set up an entry of the gdt used as a codesegment. 
 *  - entry : the entry address ;
 *  - base  : the base address for the segment ;
 *  - limit : the size of the segment ;
 *  - limit_granularity : the metric of 'limit' 
 *		0 means limit is give in bytes, 
 * 		1 means limit is give in page of 4096 bytes ;
 *  - accessible : 	for code segment, if accessible, reading is allowed,
 *			for data segment, if accessible, writing is allowed (reading allways allowed)
 *  - conforming : 
 *		0 means the segment can only be call/jump form a segment of the same ring level
 * 		1 means the segment can be call/jump from a segment of higher ring level
 *  - ring_level : the ring_level give to the segment ;
 */
 
void gdt_set_codesegment_entry(struct gdt_entry_s *entry, uint32_t base, uint32_t limit, int limit_granularity, int accessible, int conforming, int ring_level ) {
	entry->base_low    = (base & 0xFFFF);
	entry->base_middle = (base >> 16) & 0xFF;
	entry->base_high   = (base >> 24) & 0xFF;
	entry->limit_low   = (limit & 0xFFFF);
	entry->limit_high  = ((limit >> 16) & 0x0F);
	entry->flags_zero  = 0;
	entry->flags_L     = 0; /* here we assume 32bits protected mod and not 64 */
	entry->flags_Sz    = 1; /* here we assume 32bits protected mod and not 16 */
	entry->flags_Gr    = limit_granularity;
	entry->access_ac   = 0;
	entry->access_RW   = accessible;
	entry->access_DC   = conforming;
	entry->access_Ex   = 1; /* this is a code segment entry */
	entry->access_one  = 1;
	entry->access_Privl= ring_level;
	entry->access_Pr   = 1;
}

/**
 * gdt_set_datasegment_entry is used to set up an entry of the gdt used as datasegment. 
 *  - entry : the entry address ;
 *  - base  : the base address for the segment ;
 *  - limit : the size of the segment ;
 *  - limit_granularity : the metric of 'limit' 
 *		0 means limit is give in bytes, 
 * 		1 means limit is give in page of 4096 bytes ;
 *  - accessible : if accessible(==1), writing is allowed (reading allways allowed)
 *  - direction : 
 *		0 means segment is growing (limit is after base address)
 * 		1 means segment is decreasing (limit is before base address, like a stack) 
 *  - ring_level : the ring_level give to the segment ;
 */
void gdt_set_datasegment_entry(struct gdt_entry_s *entry, uint32_t base, uint32_t limit, int limit_granularity, int accessible, int direction, int ring_level ) {
	entry->base_low    = (base & 0xFFFF);
	entry->base_middle = (base >> 16) & 0xFF;
	entry->base_high   = (base >> 24) & 0xFF;
	entry->limit_low   = (limit & 0xFFFF);
	entry->limit_high  = ((limit >> 16) & 0x0F);
	entry->flags_zero  = 0;
	entry->flags_L     = 0; /* this is not a 64bits large entry */
	entry->flags_Sz    = 1; /* this is not a 16bits entry, but a 32 bits protected mod entry */
	entry->flags_Gr    = limit_granularity;
	entry->access_ac   = 0;
	entry->access_RW   = accessible;
	entry->access_DC   = direction;
	entry->access_Ex   = 0; /* this is a data segment entry */
	entry->access_one  = 1;
	entry->access_Privl= ring_level;
	entry->access_Pr   = 1;
}

/**
 * gdt_set_null_entry is used to set up an entry of the gdt unused (nul). 
 *  - entry : the entry address ;
 */
void gdt_set_null_entry(struct gdt_entry_s *entry) {
	entry->base_low    = 0;
	entry->base_middle = 0;
	entry->base_high   = 0;
	entry->limit_low   = 0;
	entry->limit_high  = 0;
	entry->flags_zero  = 0;
	entry->flags_L     = 0; 
	entry->flags_Sz    = 0; 
	entry->flags_Gr    = 0;
	entry->access_ac   = 0;
	entry->access_RW   = 0;
	entry->access_DC   = 0;
	entry->access_Ex   = 0;
	entry->access_one  = 0;
	entry->access_Privl= 0;
	entry->access_Pr   = 0;
}

/**
 * init a default global descriptor table. 
 */
void gdt_init_default()
{
	int gdt_size = (DEFAULT_GDT_ENTRIES*sizeof(struct gdt_entry_s))-1;
	gdt_activate(default_gdt,gdt_size,0x08,0x10,0x10,0x10,0x10,0x10);
}
