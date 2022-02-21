#include <stdlib.h>
#include <stdio.h>

#include "hardware.h"
#include "hwconfig.h"
#include "init.h"
#include "drive.h"


static void generic_wr_sector(int nbSect , int cmd) {

	// on met dans les ports de données qu'on veut lire un seul secteur
	_out(HDA_DATAREGS + 0, (nbSect << 8) & 0xFF );
	_out(HDA_DATAREGS + 1, nbSect & 0xFF);

	// et ion envoie la commande
	_out(HDA_CMDREG, cmd);

	_sleep(HDA_IRQ);
}

static void read_sector_frmt(int cyl, int sect, int nbSect) {
	do_seek(cyl, sect);

	generic_wr_sector(nbSect, CMD_READ);
}

static void formate_sector(int cyl, int sect, int nbSect, int value) {
	do_seek(cyl, sect);
	
	//on ecrit value dans P2 P3 P4 P5
	_out(HDA_DATAREGS + 2, (value >> 24) & 0xFF);
	_out(HDA_DATAREGS + 3, (value >> 16) & 0xFF);
	_out(HDA_DATAREGS + 4, (value >> 8) & 0xFF);
	_out(HDA_DATAREGS + 5, value & 0xFF);

	generic_wr_sector(nbSect, CMD_FORMAT);
}

static void format_all_sector(int value){
	int cyl, sect;
	for ( cyl = 0; cyl < HDA_MAXCYLINDER; cyl++)
		for (sect = 0; sect < HDA_MAXSECTOR; sect++)
			formate_sector(cyl, sect, 1, value);
}

static void display_buffer(int nbSect) {
	printf("0x");
	for (int i = 0; i < nbSect * HDA_SECTORSIZE; i++)
		printf("%x", MASTERBUFFER[i]);
    printf("\n");
}

int main(int argc, char **argv) {
    
    init_h();

	int cyl, sec, nbSect;
	int value;

	switch (argc) 
    {
        case 2:
            value = atoi(argv[1]);
		    format_all_sector(value);
            break;

        case 4:
            cyl = atoi(argv[1]);
            sec = atoi(argv[2]);
            nbSect = 1;
            value = atoi(argv[3]);

            formate_sector(cyl, sec, nbSect, value);

            printf("\n...... FORMATAGE ......\n\n");
        
            read_sector_frmt(cyl, sec, nbSect);
            printf("Cylindre n°%d, secteur n°%d :\n", cyl, sec);
            display_buffer(nbSect);
            break;

        default:
            printf("Please, give 1 or 3 arguments.\n");
            exit(EXIT_FAILURE);

	}
    /* and exit! */
  	exit(EXIT_SUCCESS);
}
