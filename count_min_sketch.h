#ifndef COUNT_MIN_H
#define COUNT_MIN_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct count_min count_min_t;

//Crea el count_min
count_min_t* count_min_crear();

//Suma 1 en cada posicion de cada vector dependiendo de su funcion de hash correspondiente
void count_min_guardar(count_min_t* count_min, char* clave);

//Devuelve la menor cantidad de apariciones de la clave dada entre los vectores de count min
size_t devolver_menor(count_min_t* count_min, char* clave);

//Libera la memoria de el  count min, liberando antes la memoria de cada uno de sus vectores
void count_min_destruir(count_min_t* count_min);

#endif