#include "ioport.h"
#include "gdt.h"
#include "idt.h"
#include "context.h"

void clear_screen();				  /* clear screen */
void putc(char aChar);				/* print a single char on screen */
void puts(char *aString);			/* print a string on the screen */
void puthex(int aNumber);			/* print an Hex number on screen */

unsigned char keyboardEntry[64];

/* base address for the video output assume to be set as character oriented by the multiboot */
unsigned char *video_memory = (unsigned char *) 0xB8000;

/**
 * interruptions 
 */

void irq_timer(int_regs_t *r) {
	yield();
}

char read_keyboard_entry(unsigned char scancode) {
  static char codesMap[58] = { 0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
                0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's', 'd',
                'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
                'm', ',', '.','/', 0, 0, 0, ' '};
  
  unsigned char value = scancode & 0x7F;
  return codesMap[value];
}

void keyboard_irq(int_regs_t *r) {
  unsigned char in = _inb(0x60);
  if((in & 0x80) == 0) {
    putc(read_keyboard_entry(in));
  }
}


/**
 * affichage du chargement
 */
void loading(void *args)
{
	while (1)
	{
		video_memory[(70+80*5)*2]='-';
		video_memory[(70+80*5)*2]='/';
		video_memory[(70+80*5)*2]='|';
		video_memory[(70+80*5)*2]='\\';
	}
}

void timer_irq(int_regs_t *r) {
  // les interruptions desactivées (comme si on fait CLI)
  /*
  * pour reactiver io va falloir faire deux choses:
  *
  * - faire STI (activer les interruptions)
  * - faire CLI 
  */
  irq_enable();
  _outb(0x20, 0x20);
  puts("Timer!\n");
  // si une IRQ timer survient pendant le yield / switch to : probleme!
  yield();
  irq_disable();
}

/* multiboot entry-point with datastructure as arg. */
void main(unsigned int * mboot_info)
{

    /* clear the screen */
    clear_screen();
    puts("Early boot.\n"); 
    puts("\t-> Setting up the GDT... ");
    gdt_init_default();
    puts("done\n");

    puts("\t-> Setting up the IDT... ");
    setup_idt();
    puts("OK\n");

    puts("\n\n");

    puts("Hello World\n");

    ctx_t context1;
    ctx_t context2;

    uint32_t pile_ctx1[4096];
    uint32_t pile_ctx2[4096];

    idt_setup_handler(0, timer_irq);
    irq_enable();
    idt_setup_handler(1, keyboard_irq);
    /* minimal setup done ! */

    init_ctx(&context2, &keyboard_irq, &context1);
    init_ctx(&context1, &loading, &context2);

    for(;;) ; /* nothing more to do... really nothing ! */
}

/* clear screen */
void clear_screen() {
  int i;
  for(i=0;i<80*25;i++) { 			/* for each one of the 80 char by 25 lines */
    video_memory[i*2+1]=0x0F;			/* color is set to black background and white char */
    video_memory[i*2]=(unsigned char)' '; 	/* character shown is the space char */
  }
}

/* print a string on the screen */
void puts(char *aString) {
  char *current_char=aString;
  while(*current_char!=0) {
    putc(*current_char++);
  }
}

/* print an number in hexa */
char *hex_digit="0123456789ABCDEF";
void puthex(int aNumber) {
  int i;
  int started=0;
  for(i=28;i>=0;i-=4) {
    int k=(aNumber>>i)&0xF;
    if(k!=0 || started) {
      putc(hex_digit[k]);
      started=1;
    }
  }
}

/* print a char on the screen */
int cursor_x=0;					/* here is the cursor position on X [0..79] */
int cursor_y=0;					/* here is the cursor position on Y [0..24] */

void setCursor() {
  int cursor_offset = cursor_x+cursor_y*80;
  _outb(0x3d4,14);
  _outb(0x3d5,((cursor_offset>>8)&0xFF));
  _outb(0x3d4,15);
  _outb(0x3d5,(cursor_offset&0xFF));
}

void putc(char c) {
  if(cursor_x>79) {
    cursor_x=0;
    cursor_y++;
  }
  if(cursor_y>24) {
    cursor_y=0;
    clear_screen();
  }
  switch(c) {					/* deal with a special char */
    case '\r': cursor_x=0; break;		/* carriage return */
    case '\n': cursor_x=0; cursor_y++; break; 	/* new ligne */	
    case 0x8 : if(cursor_x>0) cursor_x--; break;/* backspace */
    case 0x9 : cursor_x=(cursor_x+8)&~7; break;	/* tabulation */
						/* or print a simple character */
    default  : 
      video_memory[(cursor_x+80*cursor_y)*2]=c;
      cursor_x++;
      break;
  }
  setCursor();
}


