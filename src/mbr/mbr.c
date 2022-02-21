#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware.h"
#include "hwconfig.h"
#include "drive.h"
#include "init.h"
#include "volume.h"

int main() {

    volume_t *volume;
    	
    init_h();

    mbr = malloc(sizeof(mbr_t));


    mbr->nb_volume = 0;

    for (int i = 0; i < 0; i++) {
        volume = malloc(sizeof(volume_t));
        volume->debutCyl = i*2;
        volume->debutSect = 1;
        volume->nbBlocs = (i<7) ? 32 : 31;
        volume->type = BASE;
        mbr->volumes[i] = *volume;
    }

    save_mbr();
    free(mbr);
}