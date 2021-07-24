#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "strutil.h"
#include "count_min_sketch.h"
#include "heap.h"
#include "hash.h"
#include "pila.h"

typedef struct tweet{
	char* nombre;
	size_t menor_aparicion;
}tweet_t;

tweet_t* tweet_crear(char* nombre, size_t menor_aparicion){
	tweet_t* tweet = malloc(sizeof(tweet_t));
	if(tweet == NULL){
		return NULL;
	}
	char* nom = strdup(nombre);
	tweet->nombre = nom;
	tweet->menor_aparicion = menor_aparicion;
	return tweet;
}

int comparacion(const void* t1,const void* t2){
	const tweet_t* tweet1 = t1;
	const tweet_t* tweet2 = t2;
	if(tweet1->menor_aparicion > tweet2->menor_aparicion){
		return -1;
	}
	else if(tweet1->menor_aparicion < tweet2->menor_aparicion){
		return 1;
	}
	else{
		if(strcmp(tweet1->nombre, tweet2->nombre) > 0){
			return 1;
		}
		if(strcmp(tweet1->nombre, tweet2->nombre) < 0){
			return -1;
		}
	}
	return 0;
}

void imprimir_tts(count_min_t* count_min, heap_t* heap, pila_t* pila, int numero){
	int z = 0;
	while(!heap_esta_vacio(heap)){
		pila_apilar(pila, heap_desencolar(heap));
		z ++;
	}
	fprintf(stdout, "--- %i\n", numero);
	for(int i = 0; i < z; i ++){
		tweet_t* tweet = pila_ver_tope(pila);
		fprintf(stdout, "%zu %s\n", tweet->menor_aparicion , tweet->nombre );
		tweet_t* aux = pila_desapilar(pila);
		free(aux->nombre);
		free(aux);
	}
}

void destruir_heap(heap_t* heap){
	while(!heap_esta_vacio(heap)){
		tweet_t* tweet = heap_desencolar(heap);
		free(tweet->nombre);
		free(tweet);
	}
	heap_destruir(heap, NULL);
}

void buscar_tweet_y_reemplazar(heap_t* heap, heap_t* heap_aux, char** tweets, size_t i, count_min_t* count_min){
	while(!heap_esta_vacio(heap)){
		tweet_t* top = heap_ver_max(heap);
		if(strcmp(top->nombre, tweets[i]) != 0){
			heap_encolar(heap_aux, top);
			heap_desencolar(heap);
		}
		else{
			tweet_t* nuevo_tweet = tweet_crear(top->nombre, devolver_menor(count_min, tweets[i]));
			heap_encolar(heap_aux, nuevo_tweet);
			tweet_t* aux = heap_desencolar(heap);
			free(aux->nombre);
			free(aux);
		}
	}
	while(!heap_esta_vacio(heap_aux)){
		heap_encolar(heap, heap_desencolar(heap_aux));
	}
}

void reemplazar_tope_heap(hash_t* hash, heap_t* heap, char** tweets, size_t i, tweet_t* tope, count_min_t* count_min){
	hash_borrar(hash, tope->nombre);
	tweet_t* tweet_aux = heap_desencolar(heap);
	free(tweet_aux->nombre);
	free(tweet_aux);
	tweet_t* info_tweet = tweet_crear(tweets[i], devolver_menor(count_min, tweets[i]));
	heap_encolar(heap, info_tweet);
	hash_guardar(hash, tweets[i], NULL);
}

void guardar_tweets(count_min_t* count_min, heap_t* heap, hash_t* hash, int k, int i, char** tweets){
	count_min_guardar(count_min, tweets[i]);
	heap_t* heap_aux = heap_crear(comparacion);
	if(heap_cantidad(heap) < k && !hash_pertenece(hash, tweets[i])){
		tweet_t* info_tweet = tweet_crear(tweets[i], devolver_menor(count_min, tweets[i]));
		heap_encolar(heap, info_tweet);
		hash_guardar(hash, tweets[i], NULL);
	}
	else if(heap_cantidad(heap) < k && hash_pertenece(hash, tweets[i])){
		buscar_tweet_y_reemplazar(heap, heap_aux, tweets, i, count_min);
	}
	else{
		tweet_t* tope = heap_ver_max(heap);
		if(!hash_pertenece(hash, tweets[i])){
			if(devolver_menor(count_min, tope->nombre) < devolver_menor(count_min, tweets[i])){
				reemplazar_tope_heap(hash, heap, tweets, i, tope, count_min);
			}
			else if(devolver_menor(count_min, tope->nombre) == devolver_menor(count_min, tweets[i]) && strcmp(tope->nombre, tweets[i]) > 0){
				reemplazar_tope_heap(hash, heap, tweets, i, tope, count_min);
			}
		}
		else{
			buscar_tweet_y_reemplazar(heap, heap_aux, tweets, i, count_min);
		}
	}
	heap_destruir(heap_aux, NULL);
}

int procesar_tweets(int n, int k){
	char *linea = NULL;
	size_t tam = 0;
	ssize_t letras = 1;
	letras = getline(&linea,&tam,stdin);
	linea[strlen(linea) - 1] = '\0';
	count_min_t* count_min = count_min_crear();
	if(count_min == NULL){
		return -1;
	}
	int numero_iteracion = 1;
	while(letras != -1){
		heap_t* heap = heap_crear(comparacion);
		if(heap == NULL){
			count_min_destruir(count_min);
			free(linea);
			return -1;
		}
		hash_t* hash = hash_crear(NULL);
		if(hash == NULL){
			count_min_destruir(count_min);
			heap_destruir(heap, NULL);
			free(linea);
			return -1;
		}
		int cant_lineas = 0;
		while(letras != -1 && cant_lineas < n){
			char** tweets = split(linea, ',');
			int i = 1;
			while(tweets[i] != NULL){
				guardar_tweets(count_min, heap, hash, k, i, tweets);
				i ++;
			}
			letras = getline(&linea,&tam,stdin);
			linea[strlen(linea) - 1] = '\0';
			free_strv(tweets);
			cant_lineas ++;
		}
		pila_t* pila = pila_crear();
		if(pila == NULL){
			heap_destruir(heap, NULL);
			hash_destruir(hash);
			count_min_destruir(count_min);
			free(linea);
			return -1;
		}
		imprimir_tts(count_min, heap, pila, numero_iteracion);
		pila_destruir(pila);
		destruir_heap(heap);
		hash_destruir(hash);
		numero_iteracion ++;	
	}
	count_min_destruir(count_min);
	free(linea);
	return 0;
}

int main(int argc, char *argv[]){
	if(argc != 3){
		fprintf(stderr,"Error: Cantidad erronea de parametros\n");
		return -1;
	}
	int n = atoi(argv[1]);
	int k = atoi(argv[2]);
	procesar_tweets(n, k);
	return 0;
}