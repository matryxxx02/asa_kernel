#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware.h"
#include "hwconfig.h"
#include "drive.h"
#include "init.h"
#include "volume.h"

void load_mbr() {
    void *buffer;
    buffer = malloc(HDA_SECTORSIZE);

    //on se lit le premier secteur
    read_sector(0, 0, buffer);

    memcpy((void *) mbr, (void *) buffer, sizeof(mbr_t));
    free(buffer);
}

void save_mbr() {
    void *buffer;
    buffer = malloc(HDA_SECTORSIZE);

    memcpy((void *) buffer, (void *) mbr, sizeof(mbr_t));

    write_sector(0, 0, buffer);

    free(buffer);
}

int* cytToSect(unsigned int vol, unsigned int nbloc) {
    int *res = malloc(2*sizeof(int));
    volume_t *volume = &mbr->volumes[vol];

    res[0] = volume->debutCyl;
    res[1] = volume->debutSect + nbloc;

    if (res [1] >= HDA_MAXSECTOR) {
        res[0] += res[1] / HDA_MAXSECTOR;
        res[1] = res[1] % HDA_MAXSECTOR;
    }
    return res;
}

void read_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer) {
    int *cylSect = cytToSect(vol, nbloc);

    read_sector(cylSect[0], cylSect[1], buffer);

    free(cylSect);
}

void write_bloc(unsigned int vol, unsigned int nbloc, const unsigned char *buffer) {
    int *cylSect = cytToSect(vol, nbloc);

    write_sector(cylSect[0], cylSect[1], buffer);

    free(cylSect);
}

void format_vol(unsigned int vol, unsigned int value) {
    volume_t *volume = &mbr->volumes[vol];
    int size = volume->nbBlocs;
    int debutCyl = volume->debutCyl;
    int debutSect = volume->debutSect;

    format_sector(debutCyl, debutSect, size, value);
}

void display_bloc(unsigned int vol, unsigned int nbloc) {
    void *buffer = malloc(HDA_SECTORSIZE);
	read_bloc(vol, nbloc, buffer);
	for (int i = 0; i < HDA_SECTORSIZE; i++)
		printf("%x", ((char*) buffer)[i]);
	free(buffer);
}