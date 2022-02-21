#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware.h"
#include "hwconfig.h"
#include "drive.h"
#include "init.h"
#include "volume.h"

void display_all_vol() {
    volume_t *volume;
    printf("%d partition(s):\n", mbr->nb_volume);

    for (int i = 0; i < 8; i++){
        volume = (&mbr->volumes[i]);
        printf("%d\t%d\t%d\t%d\n", volume->debutCyl, volume->debutSect,volume->nbBlocs, volume->type);
    }
}

int main(int argc, char **argv) {

    int vol, nbloc;
    void *buffer;
    	
    init_h();

    mbr = malloc(sizeof(mbr_t));
    buffer = malloc(HDA_SECTORSIZE);
    vol = atoi(argv[1]);
    nbloc = atoi(argv[2]);

    memset(buffer, 1, HDA_SECTORSIZE);

    load_mbr();
    display_all_vol();
    printf("\n");

    printf("\nBloc = %d  | Volume = %d :\n0x", nbloc, vol);
    display_bloc(vol, nbloc);


    printf("\n>> Write \n");
    write_bloc(vol, nbloc, buffer);

    printf("\nBloc = %d | Volume = %d :\n0x", nbloc, vol);
    display_bloc(vol, nbloc);

    format_vol(vol+1, 4);
    printf("\n>> Formatage\n");

    for (int i = 0; i< 32; i++) {
        printf("\nBloc = %d | Volume = %d :\n0x", nbloc+i, vol+1);
        display_bloc(vol+1, nbloc+i);
    }

    printf("\n");

    free(mbr);

}