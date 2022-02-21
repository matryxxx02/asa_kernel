#ifndef _SUPERBLOC_H_
#define _SUPERBLOC_H_

#define MAGIC 0xDEADBEEF

struct list_bloc_s {
    unsigned int magic;
    int pos_bloc;
    struct list_bloc_s *next;
};
typedef struct list_bloc_s list_bloc_t;

struct superbloc_s {
    unsigned int magic;
    int serial;
    char name[32];
    list_bloc_t *first;
};
typedef struct superbloc_s superbloc_t;

superbloc_t *current_superbloc;
int current_vol;

void init_volume(unsigned int vol);

int load_super(unsigned int vol);

void save_super();

unsigned int new_bloc();

int free_bloc(unsigned int bloc);

void display_current_vol();

#endif