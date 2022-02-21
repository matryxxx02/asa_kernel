#include <stdlib.h>
#include <stdio.h>

#include "hardware.h"
#include "hwconfig.h"

static void empty_it() {
    return;
}

int main() {
	/* init hardware */
	if(init_hardware("hwconfig.ini") == 0) {
	fprintf(stderr, "Error in hardware initialization\n");
	exit(EXIT_FAILURE);
	}

	/* Interrupt handlers */
	for(unsigned i = 0; i < 16; i++) {
	IRQVECTOR[i] = empty_it;
	}

	/* Allows all IT */
	_mask(1);

    do_seek(3,4);
    do_read(4);

	/* and exit! */
  	exit(EXIT_SUCCESS);
}
