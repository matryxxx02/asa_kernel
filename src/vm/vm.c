#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "hwconfig.h"
#include "drive.h"
#include "init.h"
#include "volume.h"
#include "superbloc.h"

/* ------------------------------
   command list
   ------------------------------------------------------------*/
struct _cmd {
    char *name;
    void (*fun) (struct _cmd *c);
    char *comment;
};

void *buffer;

static void list(struct _cmd *c);

static void read(struct _cmd *c);
static void write(struct _cmd *c);
static void format(struct _cmd *c);

static void load_vol(struct _cmd *c);
static void save_vol(struct _cmd *c);
static void list_vol(struct _cmd *c);
static void new_vol(struct _cmd *c);
static void del_vol(struct _cmd *c);

static void new(struct _cmd *c);
static void del(struct _cmd *c);
static void help(struct _cmd *c);
static void save(struct _cmd *c);
static void quit(struct _cmd *c);
static void xit(struct _cmd *c);
static void none(struct _cmd *c);

static struct _cmd commands [] = {
    {"list", list, 	"display the partition table"},
    {"read", read, 	"read specific partition's bloc"},
    {"write", write, 	"write specific partition's bloc"},
    {"format", format, 	"format a specific partition"},

    {"load_vol", load_vol, 	"load specific superbloc's volum"},
    {"save_vol", save_vol, 	"save specific superbloc's volum"},
    {"list_vol", list_vol, 	"display all free blocs from current volum"},
    {"new_vol", new_vol, 	"create new bloc used in current volum"},
    {"del_vol", del_vol, 	"delete bloc used in current volum"},

    {"new",  new,	"create a new partition"},
    {"del",  del,	"delete a partition"},
    {"save", save,	"save the MBR"},
    {"quit", quit,	"save the MBR and quit"},
    {"exit", xit,	"exit (without saving)"},
    {"help", help,	"display this help"},
    {0, none, 		"unknown command, try help"}
} ;

/* ------------------------------
   dialog and execute 
   ------------------------------------------------------------*/

static void
execute(const char *name)
{
    struct _cmd *c = commands; 
  
    while (c->name && strcmp (name, c->name))
	    c++;
    (*c->fun)(c);
}

static void
loop(void)
{
    char name[64];
    
    while (printf("> "), scanf("%62s", name) == 1)
	    execute(name) ;
}

/* ------------------------------
   command execution 
   ------------------------------------------------------------*/
static void
list(struct _cmd *c)
{
    volume_t *volume;
    int nb_volume = mbr->nb_volume;
    printf("%d partition(s) on HDD\n", nb_volume);

    printf("# cylinder\t# sector\tnb sector\ttype\n");

    for (int i = 0; i < nb_volume; i++){
        volume = (&mbr->volumes[i]);
        printf("%d\t\t%d\t\t%d\t\t%d\n", volume->debutCyl, volume->debutSect,volume->nbBlocs, volume->type);
    }

    printf("%s done\n", c->name); 
}

static void
read(struct _cmd *c)
{
    int vol, bloc;
    
    printf("Which Volum? : ");
    scanf("%d", &vol);
    printf("Which Bloc? : ");
    scanf("%d", &bloc);

    printf("0x");
    display_bloc(vol, bloc);

    printf("\n");

    
    printf("%s done\n", c->name); 
}

static void
write(struct _cmd *c)
{
    int vol, bloc;
    
    printf("Which Volum? : ");
    scanf("%d", &vol);
    printf("Which Bloc? : ");
    scanf("%d", &bloc);

    printf("Write what do you want:\n");
    scanf("%s", (char*) buffer);

    write_bloc(vol, bloc, buffer);
    
    printf("%s done\n", c->name); 
}

static void
format(struct _cmd *c)
{
    int vol, value;
    
    printf("Which Volum? : ");
    scanf("%d", &vol);
    printf("Which Value? : \n");
    scanf("%d", &value);
    
    format_vol(vol, value);
    
    printf("%s done\n", c->name); 
}


static void
load_vol(struct _cmd *c) {
    int choice;

    printf("Which Volum to load? : ");
    scanf("%d", &choice);

    current_vol = load_super(choice);
    printf("%s done\n", c->name); 
}

static void
save_vol(struct _cmd *c) {
    save_super();
    printf("%s done\n", c->name); 
}

static void
list_vol(struct _cmd *c) {
    display_current_vol();
    printf("%s done\n", c->name); 
}

static void
new_vol(struct _cmd *c) {
    int number = new_bloc();

    if (number < 0)
        printf("Please load a volum first.\n");
    else if (number == 0)
        printf("There are no bloc available.\n");
    else
        printf("You create a new bloc used, this is the number %d\n",number);

    printf("%s done\n", c->name); 
}

static void
del_vol(struct _cmd *c) {
    int result, choice;

    printf("Which Bloc to free? : ");
    scanf("%d", &choice);

    result = free_bloc(choice);

    switch (result) {

        case -2:
            printf("Please load a volum first.\n");
            break;
        case -1:
            printf("Bloc out of volum's size.\n");
            break;
        case -0:
            printf("Sorry, the bloc n°%d is already free.\n", choice);
            break;
        default:
            printf("Success free.\n");
            break;
    }

    printf("%s done\n", c->name); 
}

static void
new(struct _cmd *c)
{
    volume_t *last_part, *new_part;
    int nb_volume = mbr->nb_volume;
    int *cylSect;
    int nbloc;

    if (nb_volume >= 8){
        printf("%s aborted\n", c->name); 
        printf("There are already 8 volumes on disk.\n"); 
    }
    else {
        if (nb_volume != 0) {
            last_part = (&mbr->volumes[nb_volume-1]);
            cylSect = cytToSect(nb_volume-1, last_part->nbBlocs);
        } else {
            cylSect = malloc(2*sizeof(int));
            cylSect[0] = 0;
            cylSect[1] = 1;
        }
            
        printf("How many blocs in new vol? ");
        scanf("%d", &nbloc);

        new_part = malloc(sizeof(volume_t));
        new_part->debutCyl = cylSect[0];
        new_part->debutSect = cylSect[1];
        new_part->nbBlocs = nbloc;
        new_part->type = BASE;
        mbr->volumes[nb_volume] = *new_part;
        mbr->nb_volume += 1;

        free(cylSect);

        printf("%s done\n", c->name); 
    }
}

static void
del(struct _cmd *c)
{
    volume_t *tmp_part;
    int nb_volume = mbr->nb_volume;
    int vol;

    if (nb_volume <= 0){
        printf("%s aborted\n", c->name); 
        printf("There are no volumes on disk.\n"); 
    }
    else {
            
        printf("which vol delete ? : ");
        scanf("%d", &vol);

        vol++;

        while (vol < nb_volume) {
            tmp_part = (&mbr->volumes[vol]);
            mbr->volumes[vol-1] = *tmp_part; 
            vol++;
        } 

        mbr->nb_volume -= 1;

        printf("%s done\n", c->name); 
    }
}

static void
save(struct _cmd *c)
{
    printf("%s done\n", c->name); 
    save_mbr();
}

static void
quit(struct _cmd *c)
{
    save_mbr();
    save_super();
    printf("%s done\n", c->name); 
    exit(EXIT_SUCCESS);
}

static void
do_xit()
{
    exit(EXIT_SUCCESS);
}

static void
xit(struct _cmd *dummy)
{
    do_xit(); 
}

static void
help(struct _cmd *dummy)
{
    struct _cmd *c = commands;
  
    for (; c->name; c++) 
	printf ("%s\t-- %s\n", c->name, c->comment);
}

static void
none(struct _cmd *c)
{
    printf ("%s\n", c->comment) ;
}

int
main()
{   
    init_h();

    mbr = malloc(sizeof(mbr_t));
    buffer = malloc(HDA_SECTORSIZE);

    /* Read the mbr*/
    load_mbr();

    /* dialog with user */ 
    loop();

    /* abnormal end of dialog (cause EOF for xample) */
    do_xit();

    free(mbr);
    free(buffer);

    /* make gcc -W happy */
    exit(EXIT_SUCCESS);
}
