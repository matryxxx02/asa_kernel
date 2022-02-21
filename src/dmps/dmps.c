#include <stdlib.h>
#include <stdio.h>

#include "hardware.h"
#include "hwconfig.h"
#include "init.h"
#include "drive.h"

static void display_buffer(int nbSect) {
	printf("0x");
	for (int i = 0; i < nbSect * HDA_SECTORSIZE; i++)
		printf("%x", MASTERBUFFER[i]);
    printf("\n");
}

int main(int argc, char **argv) {
    	
    init_h();

	if (argc != 3) {
        
        printf("Please give 2 arguments.\n");
        exit(EXIT_FAILURE);
        
	}
	else {

		int cyl, sec, nbSect;
		
		cyl = atoi(argv[1]);
		sec = atoi(argv[2]);
		nbSect = 1;

		read_sector( cyl, sec, nbSect);
	
		printf("Cylindre n°%d, secteur n°%d :\n", cyl, sec);

		display_buffer(nbSect);

    	/* and exit! */
  	    exit(EXIT_SUCCESS);
	}
}
