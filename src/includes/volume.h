#ifndef _VOL_H_
#define _VOL_H_

enum type {BASE, ANNEXE, OTHER, UNUSED};
typedef enum type type_e;

struct volume_s { 
    type_e type;
    int debutCyl;
    int debutSect;
    int nbBlocs;
};
typedef struct volume_s volume_t;

struct mbr_s {
    unsigned int nb_volume;

    volume_t volumes[8];
};
typedef struct mbr_s mbr_t;

mbr_t *mbr;

void load_mbr();

void save_mbr();

int* cytToSect(unsigned int vol, unsigned int nbloc);

void read_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer);

void write_bloc(unsigned int vol, unsigned int nbloc, const unsigned char *buffer);

void format_vol(unsigned int vol, unsigned int value);

void display_bloc(unsigned int vol, unsigned int nbloc) ;

#endif