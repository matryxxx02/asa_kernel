#ifndef _DRIVE_H_
#define _DRIVE_H_


void do_seek(unsigned int cyl, unsigned int sect);

void do_read(unsigned short nbSec);

void read_sector(unsigned short ncyl, unsigned short nsect, unsigned char *buffer);

void do_write(unsigned short nbSec);

void write_sector(unsigned short ncyl, unsigned short nsect, unsigned char *buffer);

void do_format(unsigned short sec, unsigned int val);

void format_sector(unsigned int cylinder, unsigned int sector, unsigned int nsector, unsigned int value);

void display_sector(unsigned int cyl, unsigned int sect);

#endif