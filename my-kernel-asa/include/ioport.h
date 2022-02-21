#ifndef _IOPORT_H_
#define _IOPORT_H_

/* read a byte from the ioport 'port' */
static inline unsigned char _inb(unsigned short port) {
  int ret; 
  asm("inb %1,%0": "=a" (ret) : "Nd" (port) :);
  /***
   * N.B. in inb %0,%1 :  
   *   %0 must be the register al => "=a" 
   *   %1 must be a 8bits imediate value (N) or the register dx (d) =>"Nd"
   *      %1 constraint also limit the var. port to a 16bits value => "[unsigned] short".
   */
  return ret;
} 

/* write a byte value in the ioport 'port' */
static inline void _outb(unsigned short port, unsigned char value) {
  asm("outb %0,%1":: "a" (value), "Nd" (port) :);
  /***
   * N.B. in outb %0,%1 :
   *   %0 must be the register al => "a"
   *   %1 must be a 8bits imediate value (N) or the register dx =>"Nd"
   *      %1 constraint also limit the var. port to a 16bits value => "[unsigned] short".
   */
}

#endif
