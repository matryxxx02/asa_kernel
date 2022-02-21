#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "hardware.h"
#include "hwconfig.h"
#include "drive.h"
#include "init.h"
#include "volume.h"
#include "superbloc.h"

superbloc_t *create_superbloc() {
    superbloc_t *sb;

    sb = malloc(sizeof(superbloc_t));
    srand(time(NULL));

    sb->magic = MAGIC;
    sb->serial = rand();
    memcpy(sb->name, "PLACEHOLDER\0", 32);


    return sb;
}

list_bloc_t *create_list(unsigned int bloc) {
    list_bloc_t *list;
    list = malloc(sizeof(list_bloc_t));
    list->magic = MAGIC;
    list->pos_bloc = bloc;
    list->next = NULL;
    return list;
}

void init_volume(unsigned int vol) {
    list_bloc_t *first, *list, *tmp;
    void* buffer = malloc(HDA_SECTORSIZE);
    int size = (&mbr->volumes[vol])->nbBlocs;

    current_superbloc = create_superbloc();
    memcpy((void *) buffer, (void *) current_superbloc, sizeof(superbloc_t));
    write_bloc(vol, 0, buffer);

    first = create_list(1);
    list = first;
    for (int i = 2; i < size; i++){
        tmp = create_list(i);
        list->next = tmp;

        memcpy((void *) buffer, (void *) list, sizeof(list_bloc_t));
        write_bloc(vol, list->pos_bloc, buffer);

        list = tmp;
    }
    memcpy((void *) buffer, (void *) list, sizeof(list_bloc_t));
    write_bloc(vol, list->pos_bloc, buffer);

    current_superbloc->first = first;
    
    free(buffer);
}


int load_super(unsigned int vol){
    void *buffer;   

    buffer = malloc(HDA_SECTORSIZE);
    current_superbloc = malloc(sizeof(superbloc_t));

    read_bloc(vol, 0, buffer);

    memcpy((void *) current_superbloc, (void *) buffer, sizeof(superbloc_t));

    if (current_superbloc->magic != MAGIC){
        free(current_superbloc);
        init_volume(vol);
    }

    return vol;
}

void save_super(){
    void *buffer; 

    buffer = malloc(HDA_SECTORSIZE);

    memcpy((void *) buffer, (void *) current_superbloc, sizeof(superbloc_t));

    write_bloc(current_vol, 0, buffer);

    free(buffer);
    free(current_superbloc);
    current_superbloc = NULL;
}

unsigned int new_bloc(){
    list_bloc_t *new;
    int res;

    if (!current_superbloc)
        return -1;

    if (!(current_superbloc->first))
        return 0;

    new = current_superbloc->first;
    res = new->pos_bloc;

    current_superbloc->first = new->next;
    free(new);

    return res;
}

int free_bloc(unsigned int bloc){
    list_bloc_t *newbloc;
    int size;

    if (!current_superbloc)
        return -2;

    size = (&mbr->volumes[current_vol])->nbBlocs;
    if (bloc >= (unsigned int) size)
        return -1;

    newbloc = current_superbloc->first;
    while (newbloc) {
        if ((unsigned int) newbloc->pos_bloc == bloc) 
            return 0;
        newbloc = newbloc->next;
    }
    
    newbloc = create_list(bloc);
    newbloc->next = current_superbloc->first;

    current_superbloc->first = newbloc;

    return 1;
}

void display_current_vol() {
    if (current_superbloc) {
        list_bloc_t *tmp = current_superbloc->first;

        printf("N° Vol: %d\tSerial: %d\tName: %s\n", current_vol, current_superbloc->serial, current_superbloc->name);
        printf("Free blocs: ");

        while (tmp) {
            printf("%d - ", tmp->pos_bloc);
            tmp = tmp->next;
        }

        printf("END\n");
    }
    else {
        printf("No volum selected yet.\n");
    }
}