#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware.h"
#include "hwconfig.h"
#include "drive.h"

void do_seek(unsigned int cyl, unsigned int sect) {
    //printf(">> Seek: %d %d\n", cyl, sect);
    _out(HDA_DATAREGS, (cyl >> 8) & 0xFF);  
    _out(HDA_DATAREGS + 1, cyl & 0xFF);
    _out(HDA_DATAREGS + 2 , (sect >> 8) & 0xFF);
    _out(HDA_DATAREGS + 3, sect & 0xFF);
    
    _out(HDA_CMDREG, CMD_SEEK); 
    
    _sleep(HDA_IRQ);
    //printf(">> Seek reussi\n");
}

void do_read(unsigned short nbSec){
    //printf(">> Read: %d\n", nbSec);

    _out(HDA_DATAREGS, nbSec >> 8);
    _out(HDA_DATAREGS + 1, nbSec & 0xFF);

    _out(HDA_CMDREG, CMD_READ);

    _sleep(HDA_IRQ);
    //printf(">> Read reussi\n");
}

void read_sector(unsigned short ncyl, unsigned short nsect, unsigned char *buffer){

    do_seek(ncyl, nsect);
    do_read(nsect);
    memcpy((void *) buffer, (void *) MASTERBUFFER, HDA_SECTORSIZE);
}

void do_write(unsigned short nbSec){
    //printf(">> Write: %d\n", nbSec);

    _out(HDA_DATAREGS, nbSec >> 8);
    _out(HDA_DATAREGS + 1, nbSec & 0xFF);

    _out(HDA_CMDREG, CMD_WRITE);

    _sleep(HDA_IRQ);
    //printf(">> Write reussi\n");
}


void write_sector(unsigned short ncyl, unsigned short nsect, unsigned char *buffer){

    do_seek(ncyl, nsect);
    memcpy((void *) MASTERBUFFER, (void *) buffer, HDA_SECTORSIZE);
    do_write(nsect);

}

void do_format(unsigned short sec, unsigned int val){
    //printf(">> Formatage de %d secteur de valeurs %d\n", sec, val);

    _out(HDA_DATAREGS, sec >> 8);
    _out(HDA_DATAREGS + 1, sec & 0xFF);

    _out(HDA_DATAREGS + 2, val >> 24);
    _out(HDA_DATAREGS + 3, (val >> 16) & 0xFF);
    _out(HDA_DATAREGS + 4, (val >> 8) && 0xFF);
    _out(HDA_DATAREGS + 5, val & 0xFF);

    _out(HDA_CMDREG, CMD_FORMAT);

    _sleep(HDA_IRQ);
    //printf(">> Formatage reussi\n");
}


void format_sector(unsigned int cylinder, unsigned int sector, unsigned int nsector, unsigned int value) {
	unsigned int newcyl, newsect, i;
	newsect = sector;
	newcyl = cylinder;
	
	for ( i = 0; i < nsector; i++) {
		// on se positionne
		do_seek(newcyl, newsect);

		// on  entre dans la value de formatage dans les ports 2 3 4 et 5
		_out(HDA_DATAREGS + 2, (value >> 24) & 0xFF);
		_out(HDA_DATAREGS + 3, (value >> 16) & 0xFF);
		_out(HDA_DATAREGS + 4, (value >> 8) & 0xFF);
		_out(HDA_DATAREGS + 5, value & 0xFF);

		// ainsi que le nombre de secteurs dans les ports 0 et 1
		_out(HDA_DATAREGS, 0);
		_out(HDA_DATAREGS + 1, 1);

		//on envoie la commande et on attends
		_out(HDA_CMDREG, CMD_FORMAT);
		_sleep(HDA_IRQ);
		newsect = (newsect + 1) % HDA_MAXSECTOR;
		newcyl = (newsect == 0) ? newcyl + 1 : newcyl;
	}
	
}

void display_sector(unsigned int cyl, unsigned int sect){
	void *buffer = malloc(HDA_SECTORSIZE);
	read_sector(cyl, sect, buffer);
	for (int i = 0; i < HDA_SECTORSIZE; i++)
		printf("%x", ((char*) buffer)[i]);
	free(buffer);
}

