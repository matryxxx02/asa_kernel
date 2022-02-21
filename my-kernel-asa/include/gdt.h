/**
 * Global Descriptor Table structure and configuration
 */

#ifndef __GDT__
#define __GDT__

#include <stdint.h>

/**
 * gdt_init_default init the global descriptor table with to provide a complete access to the
 * memory. In this way segments are almost "invisibles". others gdt.h declarations are useless 
 * until you wanna to manage intel segmentation.   
 */
void gdt_init_default();

/**
 * struct gdt_entry_s is the data structure for a single gdt entry as specify by intel.
 */
struct gdt_entry_s {
	unsigned int limit_low   : 16;/* Lower 16bits of address limit */
	unsigned int base_low    : 16;/* Lower 16bits of base address */
	unsigned int base_middle : 8; /* Middle 8bits of base address */
	unsigned int access_ac   : 1; /* Access bit will be set when cpu actually use the entry */
	unsigned int access_RW   : 1; /* Readable segment for code/Writable segment for data (cfEx)*/
	unsigned int access_DC   : 1; /* Direction bit for data or Conforming bit for code */
	unsigned int access_Ex   : 1; /* Executable bit. 1 means code segment, 0 means data segment */
	unsigned int access_one  : 1; /* must be set to 1 */
	unsigned int access_Privl: 2; /* Ring level of the segment : from 0(kernel) to 3(user)*/
	unsigned int access_Pr   : 1; /* Present bit : set to one for a valid gdt entry */
	unsigned int limit_high  : 4; /* Higher 4bits of the address limit */
	unsigned int flags_zero  : 1; /* must be set to 0 */
	unsigned int flags_L     : 1; /* must be set for 64bits large gdt entry. */
	unsigned int flags_Sz    : 1; /* must be set for 32bits protected mod entry */
	unsigned int flags_Gr    : 1; /* when set, the limit is multiply by 4096 */
	unsigned int base_high   : 8; /* Higher bytes of base address */
} ;

/**
 * GDT_NULL_SEGMENT() define a struct gdt_entry initializer declaring a NULL segment gdt_entry_s.
 *   i.e. All flags init to zero. 
 */
#define GDT_NULL_SEGMENT() {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

/**
 * GDT_CODE_SEGMENT() define a struct gdt_entry initializer declaring a code segment gdt_entry_s.
 *  parameters description are described in the dynamic counterpart 'void gdt_set_codesegment_entry(...)'.
 */
#define GDT_CODE_SEGMENT(base,limit,limit_gr,accessible, conforming, ring_level) {(limit & 0xFFFF),(base & 0xFFFF),(base>>16)&0xFF,0,accessible,conforming,1,1,ring_level,1,((limit >> 16) & 0x0F),0,0,1,limit_gr,(base >> 24) & 0xFF }
/**
 * GDT_DATA_SEGMENT() define a struct gdt_entry initializer declaring a data segment gdt_entry_s.
 *  parameters description are described in the dynamic counterpart 'void gdt_set_datasegment_entry(...)'.
 */
#define GDT_DATA_SEGMENT(base, limit, limit_gr, accessible, direction, ring_level) {(limit & 0xFFFF),(base & 0xFFFF),(base >> 16) & 0xFF,0,accessible,direction,0,1,ring_level,1,((limit >> 16) & 0x0F),0,0,1,limit_gr,(base >> 24) & 0xFF }

/**
 * here after the gdt_entries used by the default_setup.
 */ 
#define DEFAULT_GDT_ENTRIES 3
extern struct gdt_entry_s default_gdt[DEFAULT_GDT_ENTRIES];

/**
 * gdt_activate setup the cpu gdt register and associated segments register
 *   gdt_base   : the address of the global descriptor table             (part of intel register gdt)
 *   gdt_limit  : the size, in bytes, of the global descriptor table - 1 (part of intel register gdt)
 *   cs         : offset of the code segment to use in this table        (intel register cs)
 *   ds         : offset of the 1st data segment to use in this table    (intel register ds)
 *   es         : offset of the 2nd data segment to use in this table    (intel register es) 
 *   fs         : offset of the 3rd data segment to use in this table    (intel register fs)
 *   gs         : offset of the 4th data segment to use in this table    (intel register gs)
 *   ss         : offset of the stack segment to use in this table       (intel register ss) 
 */
void gdt_activate(struct gdt_entry_s *gdt_base, uint16_t gdt_limit, uint16_t cs, uint16_t ds, uint16_t es, uint16_t fs, uint16_t gs, uint16_t ss);

/**
 * gdt_set_codesegment_entry is used to setup an entry of the gdt. 
 *  - entry : the entry address ;
 *  - base  : the base address for the segment ;
 *  - limit : the size of the segment ;
 *  - limit_granularity : the metric of 'limit' 
 *              0 means limit is give in bytes, 
 *              1 means limit is give in page of 4096 bytes ;
 *  - accessible :      for code segment, if accessible, reading is allowed,
 *                      for data segment, if accessible, writing is allowed (reading allways allowed)
 *  - conforming : 
 *              0 means the segment can only be call/jump form a segment of the same ring level
 *              1 means the segment can be call/jump from a segment of higher ring level
 *  - ring_level : the ring_level give to the segment ;
 */
void gdt_set_codesegment_entry(struct gdt_entry_s *entry, uint32_t base, uint32_t limit, int limit_granularity, int accessible, int conforming, int ring_level );

/**
 * gdt_set_datasegment_entry is used to setup an entry of the gdt. 
 *  - entry : the entry address ;
 *  - base  : the base address for the segment ;
 *  - limit : the size of the segment ;
 *  - limit_granularity : the metric of 'limit' 
 *              0 means limit is give in bytes, 
 *              1 means limit is give in page of 4096 bytes ;
 *  - accessible : if accessible(==1), writing is allowed (reading allways allowed)
 *  - direction : 
 *              0 means segment is growing (limit is after base address)
 *              1 means segment is decreasing (limit is before base address, like a stack) 
 *  - ring_level : the ring_level give to the segment ;
 */
void gdt_set_datasegment_entry(struct gdt_entry_s *entry, uint32_t base, uint32_t limit, int limit_granularity, int accessible, int direction, int ring_level );

/**
 * gdt_set_null_entry is used to setup an entry of the gdt. 
 *  - entry : the entry address ;
 */
void gdt_set_null_entry(struct gdt_entry_s *entry);
#endif
