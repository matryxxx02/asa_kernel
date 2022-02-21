/* ------------------------------
   $Id: c.ins,v 1.1 2003-02-21 14:33:51 marquet Exp $
   ------------------------------------------------------------

   Exemple minimal dump de la pile 
   
   Philippe Marquet, Jan 2021
   
*/

#include <stdlib.h>
#include <stdio.h>

void
dump0(int a)
{
    void *esp;
    asm("mov %%esp,%0"
        : "=r" (esp));
    printf("&a\t %p\nesp\t %p\n", (void *) &a, esp); 
}

int
main(int argc, char **argv)
{
    int i= 42;
    
    /* do it */
    dump0(i);

    /* and exit! */
    exit(EXIT_SUCCESS);
}
