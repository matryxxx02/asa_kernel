#include <stdlib.h>
#include "context.h"

ctx_t *listeContextes = NULL;
ctx_t *courant = NULL;

void irq_enable() {
     __asm volatile("sti");//set interruption
}

void irq_disable() {
     __asm volatile("cli");//clear interruption
}

int init_ctx(ctx_t* ctx, func_t f, ctx_t* next) {
    ctx->magic = MAGIC;
    ctx->entry = f;
    //ctx->args = args;
    ctx->state = READY;    
    ctx->esp = ctx->stack + sizeof(ctx->stack) - 4;
    ctx->ebp = ctx->esp;
    ctx->next = next;
    return 0;
}

void switch_to_ctx(ctx_t *target) {
    /* sauvegarder l'état du contexte courant si besoin*/
    if(courant != NULL) {
        asm("movl %%esp, %0" "\n\t"        
            "movl %%ebp, %1"
            : "=r"(courant->esp), "=r"(courant->ebp)
            :   
            :
        );
    }

    courant = target;
    asm("movl %%esp, %0" "\n\t"        
            "movl %%ebp, %1"
            : "=r"(courant->esp), "=r"(courant->ebp)
            :   
            :
        );
    
    if (courant->state == READY) {
        courant->state = ACTIVABLE;
        courant->entry(courant->args);
    }
}

void yield() {
    ctx_t *oldCourant = courant;
    if(oldCourant == NULL) {
        oldCourant = listeContextes;
    } else {
        oldCourant = oldCourant->next;
    }
    switch_to_ctx(oldCourant);
}

// struct ctx *new_ctx(func_t f, void *args) {
//     struct ctx *n = find_free_context();
//     if(n == NULL) {
//         puts('panic no free space on context array!\n');
//         for (;;)
//             ;
//     }
//     init_ctx(n, f, args);
//     if(listeContextes == NULL) {
//         n->next = n;
//         listeContextes = n;
//     } else {
//         n->next = listeContextes->next;
//         listeContextes->next = n;
//     }
//     return n;
// }
