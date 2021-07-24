#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "count_min_sketch.h"

#define TAM_INICIAL 150

struct count_min{
	size_t* datos_1;
	size_t* datos_2;
	size_t* datos_3;
	size_t* datos_4;
	size_t tam;
};

count_min_t* count_min_crear(){
	count_min_t* count_min = malloc(sizeof(count_min_t));
	if(count_min == NULL){
		return NULL;
	}
	size_t* datos_1 = malloc(sizeof(size_t) * TAM_INICIAL);
	if(datos_1 == NULL){
		free(count_min);
		return NULL;
	}
	size_t* datos_2 = malloc(sizeof(size_t) * TAM_INICIAL);
	if(datos_2 == NULL){
		free(count_min);
		free(datos_1);
		return NULL;
	}
	size_t* datos_3 = malloc(sizeof(size_t) * TAM_INICIAL);
	if(datos_3 == NULL){
		free(count_min);
		free(datos_1);
		free(datos_2);
		return NULL;
	}
	size_t* datos_4 = malloc(sizeof(size_t) * TAM_INICIAL);
	if(datos_4 == NULL){
		free(count_min);
		free(datos_1);
		free(datos_2);
		free(datos_3);
		return NULL;
	}
	for(int i = 0; i < TAM_INICIAL; i ++){
		datos_1[i] = 0;
		datos_2[i] = 0;
		datos_3[i] = 0;
		datos_4[i] = 0;
	}
	count_min->tam = TAM_INICIAL;
	count_min->datos_1 = datos_1;
	count_min->datos_2 = datos_2;
	count_min->datos_3 = datos_3;
	count_min->datos_4 = datos_4;
	return count_min;
}

size_t funcion_hash_1(const char* clave, size_t tam){
	size_t hashval;
	for(hashval = 0; *clave != '\0'; clave ++){
		hashval = *clave + 31*hashval;
	}
	return hashval % tam;
}

size_t funcion_hash_2(const char *clave, size_t tam){
    unsigned long pos = 5381;
    int c = *clave++;

    while(c == *clave){
    	c = *clave++;
        pos = ((pos << 5) + pos) + c;
    }
    return pos % tam;
}

size_t funcion_hash_3(const char *word, size_t tam) {
    size_t hash = 0;
    int n;
    for (int i = 0; word[i] != '\0'; i++) {
        if (isalpha(word[i]))
            n = word[i] - 'a' + 1;
        else
            n = 27;

        hash = ((hash << 3) + n) % tam;
    }
    return hash % tam;
}

size_t funcion_hash_4(const char* cp, size_t tam)
{
    size_t hash = 5381;
    while(*cp){
        hash = 33 * hash ^ (unsigned char) *cp++;
    }
    return hash % tam;
}

void count_min_guardar(count_min_t* count_min, char* clave){
	size_t pos_1 = funcion_hash_1(clave, count_min->tam);
	size_t pos_2 = funcion_hash_2(clave, count_min->tam);
	size_t pos_3 = funcion_hash_3(clave, count_min->tam);
	size_t pos_4 = funcion_hash_4(clave, count_min->tam);
	count_min->datos_1[pos_1] ++;
	count_min->datos_2[pos_2] ++;
	count_min->datos_3[pos_3] ++;
	count_min->datos_4[pos_4] ++;
}

size_t devolver_menor(count_min_t* count_min, char* clave){
	size_t pos_1 = funcion_hash_1(clave, count_min->tam);
	size_t pos_2 = funcion_hash_2(clave, count_min->tam);
	size_t pos_3 = funcion_hash_3(clave, count_min->tam);
	size_t pos_4 = funcion_hash_4(clave, count_min->tam);
	size_t cant_1 = count_min->datos_1[pos_1];
	size_t cant_2 = count_min->datos_2[pos_2];
	size_t cant_3 = count_min->datos_3[pos_3];
	size_t cant_4 = count_min->datos_4[pos_4];
	size_t menor = cant_1;
	if(cant_1 > cant_2){
		menor = cant_2;
	}
	if(menor > cant_3){
		menor = cant_3;
	}
	if(menor > cant_4){
		menor = cant_4;
	}
	return menor;
}

void count_min_destruir(count_min_t* count_min){
	free(count_min->datos_1);
	free(count_min->datos_2);
	free(count_min->datos_3);
	free(count_min->datos_4);
	free(count_min);
}