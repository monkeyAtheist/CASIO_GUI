#include <stddef.h>

/*
 * libstdc++ utilise getenv() dans certaines parties de son runtime.
 * fxlibc ne fournit pas encore cette fonction.
 *
 * La Graph 90+E ne possède pas d'environnement de processus classique,
 * donc aucune variable d'environnement n'est disponible.
 */
char *getenv(const char *name)
{
    (void)name;
    return NULL;
}