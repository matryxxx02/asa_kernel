#ifndef CONTEXT_H
#define CONTEXT_H 1

typedef void (func_t)(void*);

enum ctx_state_e {READY, ACTIVABLE, TERMINATED};

#define MAGIC 0xDEADBEEF

typedef struct ctx_s ctx_t;
struct ctx_s {
    unsigned       magic;
    void*          esp;
    void*          ebp;
    func_t*        entry;
    char          stack[8192];
    enum ctx_state_e  state;
    void            *args;

    struct ctx_s *next;
};

int init_ctx(ctx_t * ctx, func_t f, ctx_t* next);
void irq_enable();
void irq_disable();
void switch_to_ctx(ctx_t *target);
void yield();

#endif
