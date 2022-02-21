#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hardware.h"

#define TIMER_PARAM     0xF4
#define TIMER_ALARM     0xF8
#define TIMER_IRQ       2
#define HARDWARE_INI "hardware.ini"


void irq_enable() {
    _mask(1);
}

void irq_disable() {
    _mask(15);
}

typedef void (func_t) (void*);

enum etat_t { READY, ACTIVABLE, TERMINATED, WAITING };

struct sem_s {
    int val;
    struct ctx_s *first_blocked;
};
struct sem_s mon_semaphore;

struct ctx_s {
  void* copie_rsp;
  void* copie_rbp;
  int magic;
  void *args;

/* stocke une valeur speciale pour qu'on evite d'utiliser un contexte non initialise */

  char *stack; /* pointe sur la pile associee a ce contexte */

  /* pointeur de fonction qui determine le point d'entree du contexte */
  func_t *entryPoint;
  
  enum etat_t etat;
  struct ctx_s *next;
  struct ctx_s *next_blocked;
};

void switch_to_ctx(struct ctx_s *ctx);
void sem_up(struct sem_s *sem);
void sem_down(struct sem_s *sem);

#define MAGIC 0xDEADBEEF
struct ctx_s *ctx_ring = NULL; /* la liste chainée circulaire de tous les contextes */ 
struct ctx_s *current_ctx = NULL; /* le contexte courant */
/* renvoie 0 en cas de succes, autre valeur en cas d'echec (par ex. erreur de malloc) */


int create_ctx(int stack_size, func_t f, void *args) {
   
    struct ctx_s *ctx;

    irq_disable();

    ctx = malloc(sizeof(struct ctx_s));
    if (!ctx_ring) {
        /* Je suis entrain de créer le premier contexte */
        ctx->next = ctx;
        ctx_ring = ctx;
    } else {
        /* J'insere un nouveau contexte dans une liste existante */
        ctx->next = ctx_ring->next;
        ctx_ring->next = ctx;
    }

    /* insertion du nouveau ctx dans ctx_ring */
    ctx->magic = MAGIC;
    ctx->stack = malloc(stack_size);
    ctx->entryPoint = f;
    ctx->etat = READY;   
    ctx->copie_rsp = ctx->stack + stack_size - 8;
    ctx->copie_rbp = ctx->copie_rsp;
    ctx->next_blocked = NULL;

    irq_enable();
    return 0;
}

void yield() {
    if (!current_ctx) {
        /* C'est la premiere fois qu'on appelle yield */
        switch_to_ctx(ctx_ring); /* on peut supposer qu aucun contexte n est terminated */ 
    } else {
        /* C'est pas la premiere fois (c-a-d: un contexte est déja en cours d'execution) */
        irq_disable();
        struct ctx_s *target = current_ctx->next;

        struct ctx_s *precedent = current_ctx; 
        while (target->etat == TERMINATED || target->etat == WAITING) {
            if (target == current_ctx) {
                exit(1); /* il n'y a plus de contexte du tout */
            }

        if (target->etat == TERMINATED) {
            /* gerer la suppression de contexte (supprimer le contexte pointe par target */
            if (ctx_ring == target)
            ctx_ring = target->next;
            /* on est maintenant assuré que ctx_ring ne pointe pas sur target */ 
            precedent->next = target->next;
            free(target->stack);
            free(target);
            }
            precedent = target;
            target = target->next;
        }
        /* on est sur que target n'est pas TERMINATED */
        irq_enable();
        switch_to_ctx(target);
    }
}
/* 
 * pourquoi on a besoin des deux? ctx_ring et current_ctx
 * reponse: parce que c'est possible que current_ctx soit NULL mais que ctx_ring ne soit pas NULL
 * dans le cas où on a bien créé des contextes, mais que pour l'instant on en a lancé aucun (au début du programme)
*/
void switch_to_ctx(struct ctx_s *ctx) {
    irq_disable();
    if(current_ctx != NULL){
        asm("mov %%rsp, %0" "\n\t" /* instruction pour copier RSP vers copie_rsp */
        "mov %%rbp, %1"      /* instruction pour copier RBP vers copie_rbp */
        : "=r" (current_ctx->copie_rsp), "=r" (current_ctx->copie_rbp) /* dans quelles variables on va ecrire */
        :               /* depuis quelles variables on va lire */
        :               /* lister l'ensemble des registres qu'on a modifié */
        );
    }
current_ctx = ctx;

        asm ("mov %0, %%rsp" "\n\t"           /* instruction pour copier copie_rsp vers RSP */                 
       "mov %1, %%rbp"                  /* une instruction pour copier copie_rbp vers RBP */             
       :                                /* dans quelles variables on va ecrire */                        
       : "r" (current_ctx->copie_rsp), "r" (current_ctx->copie_rbp)   /* depuis quelles variables on va lire */        
       :                                /* lister l'ensemble des registres qu'on a modifié*/             
       );                               /*Ici on ne les liste pas à cause de gcc*/
       
    if (current_ctx->etat == READY){ 
        current_ctx->etat = ACTIVABLE;

    irq_enable();
        current_ctx->entryPoint(current_ctx->args);
    /* gerer la terminaison de contexte */ 
    current_ctx->etat = TERMINATED;
    yield();
    } 
    irq_enable();
    return;
}


void ping(void* arg) {
        int i = 0;
        while (i < 2000000) {
                printf("ping! %d\n", i);
                i++;
        }
}
void pong(void* arg) {
        int j = 0;
        while (j < 1000000) {
                printf("pong! %d\n", j);
                j++;
        }
}

static void
count()
{
    while (1)
        ;
}static void
empty_it(void)
{
    return;
}

static void
timer_it() {
    static int tick = 0; 
    fprintf(stderr, "hello from IT %d\n", ++tick);
    _out(TIMER_ALARM,0xFFFFFFFE);
    yield();
}

void sem_init(struct sem_s *sem, unsigned int val) {
    sem->val = val;
    sem->first_blocked = NULL;
}

/* "prends" un jeton. Si il n'y en a pas, on bloque le contexte en cours */ 
void sem_down(struct sem_s *sem) {
    irq_disable();
    if (sem->val >=1) {
        sem->val -= 1;
        irq_enable();
    }
    else {
        current_ctx -> etat = WAITING;
        current_ctx->next_blocked = sem->first_blocked;
        sem->first_blocked = current_ctx;
        sem->val -= 1;
        irq_enable(); /* le faire avant le yield */
        yield();
    }
}

/* "rends" un jeton */ 
void sem_up(struct sem_s *sem) {
  irq_disable();
  if (sem->val >=0) {
     sem->val += 1;
  } else {
  /* reveiller un contexte */ 
     sem->first_blocked -> etat = ACTIVABLE ;
     sem->first_blocked = sem->first_blocked->next_blocked;
  }
  irq_enable();
}

int
main(int argc, char **argv)
{
    unsigned int i;

    sem_init(&mon_semaphore, 3);
    
    /* init hardware */
    if (init_hardware(HARDWARE_INI) == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
    }
    /* je cree mes contextes */ 
        create_ctx(0x4000, ping, NULL);
        create_ctx(0x4000, pong, NULL);
    
    /* dummy interrupt handlers */
    for (i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

    /* program timer */
    IRQVECTOR[TIMER_IRQ] = timer_it;   /* j'associe le callback timer_it() a l'evenement 2 (== TIMER_IRQ) */ 
    _out(TIMER_PARAM,128+64+32+8); /* reset + alarm on + 8 tick / alarm */
    _out(TIMER_ALARM,0xFFFFFFFE);  /* alarm at next tick (at 0xFFFFFFFF) */

   /* allows all IT */
    _mask(1);

    /* count for a while... */
    while(1) {
    }
    /* and exit! */
    exit(EXIT_SUCCESS);
}