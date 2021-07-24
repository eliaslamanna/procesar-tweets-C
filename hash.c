#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "hash.h"
#define CAPACIDAD_INICIAL 51


/*--------------------------------------------------------------------
  |								Structs								 |
  --------------------------------------------------------------------*/

enum estados {
	vacio,
	ocupado,
	borrado
};

typedef struct nodo{
	char* clave;
	void* dato;
	enum estados estado;
}nodo_t;

struct hash{
	nodo_t* datos;
	size_t tam;
	size_t ocupado;
	hash_destruir_dato_t dest;
};

struct hash_iter{
	size_t pos;
	const hash_t* hash;
};


/*--------------------------------------------------------------------
  |						Funciones auxiliares						 |
  --------------------------------------------------------------------*/


size_t funcion_hash(const char* clave, size_t tam){
	size_t hashval;
	for(hashval = 0; *clave != '\0'; clave ++){
		hashval = *clave + 31*hashval;
	}
	return hashval % tam;
}

nodo_t* inicializar_datos(size_t tam){
	nodo_t* datos = malloc(sizeof(nodo_t) * tam);
	if(datos == NULL) return NULL;
	
	for(size_t i = 0; i < tam; i ++){
		datos[i].estado = vacio;
		datos[i].dato   = NULL;
		datos[i].clave  = NULL;
	}
	return datos;
}

bool hash_redimensionar(hash_t* hash, size_t nuevo_tam){
	nodo_t* nuevos_datos = inicializar_datos(nuevo_tam);
	if(nuevos_datos == NULL) return false;
	
	nodo_t* datos_aux = hash->datos;
	size_t viejo_tam = hash->tam;
	hash->datos = nuevos_datos;
	hash->tam = nuevo_tam;
	hash->ocupado = 0;

	for(size_t i = 0; i < viejo_tam; i ++){
		if(datos_aux[i].estado == ocupado){
			hash_guardar(hash, datos_aux[i].clave, datos_aux[i].dato);
		}
		free(datos_aux[i].clave);
		
	}
	free(datos_aux);
	return true;
}

bool guardar_dato(hash_t *hash, char* aux, void *dato, size_t pos){
	hash->datos[pos].clave = aux;
	hash->datos[pos].dato = dato;
	hash->datos[pos].estado = ocupado;
	hash->ocupado ++;
	return true;
}

size_t buscar_clave(hash_t* hash,const char* clave){
	size_t lugar = funcion_hash(clave, hash->tam);

	while(true){
		if(hash->datos[lugar].estado == ocupado){
			if(strcmp(hash->datos[lugar].clave, clave) == 0) return lugar;
		}
		lugar++;
		if(lugar == hash->tam) lugar = 0;
	}
}


/*--------------------------------------------------------------------
  |						Primitivas del Hash							 |
  --------------------------------------------------------------------*/

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(hash == NULL) return NULL;
	
	nodo_t* datos = inicializar_datos(CAPACIDAD_INICIAL);
	if(datos == NULL){
		free(hash);
		return NULL;
	}
	hash->ocupado = 0;
	hash->datos = datos;
	hash->tam = CAPACIDAD_INICIAL;
	hash->dest = destruir_dato;
	return hash;
}

bool sobrecarga(hash_t* hash, size_t cantidad, size_t capacidad){
	return((hash->ocupado * 100/ hash->tam) > 60);
}

size_t hash_cantidad(const hash_t *hash){
	return hash->ocupado;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if(sobrecarga(hash, hash->ocupado, hash->tam)){
		size_t nuevo_tam = hash->tam * 2;
		if(!hash_redimensionar(hash, nuevo_tam)) return false;
	}
	if(hash_pertenece(hash, clave)){
		size_t pos = buscar_clave(hash,clave);
		if(hash->dest != NULL){
			hash->dest(hash->datos[pos].dato);
		}
		hash->datos[pos].dato = dato;
		return true;
	}

	char* aux = malloc(sizeof(char) * (strlen(clave)+1));
	if(aux == NULL) return false;
	strcpy(aux, clave);
	
	size_t lugar = funcion_hash(clave, hash->tam);
	while(true){
		if(hash->datos[lugar].estado != ocupado){
			if(hash->dest != NULL){
				hash->dest(hash->datos[lugar].dato);
			}
			return guardar_dato(hash, aux, dato, lugar);
		}
		lugar ++;
		if(lugar >= hash->tam) lugar = 0;
	}
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if(!hash_pertenece(hash, clave)) return NULL;
	size_t lugar = funcion_hash(clave, hash->tam);
	
	while(hash->datos[lugar].estado != vacio){
		if(strcmp(hash->datos[lugar].clave, clave) == 0) return hash->datos[lugar].dato;
		lugar++;
		if (lugar == hash->tam) lugar = 0;
	}
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t pos = funcion_hash(clave, hash->tam);
	if(hash->datos[pos].estado == ocupado){
		if(strcmp(hash->datos[pos].clave, clave) == 0) return true;
	}
	size_t lugar = pos;
	while(hash->datos[lugar].estado != vacio){
		lugar ++;
		if(lugar >= hash->tam) lugar = 0;
		
		else if(lugar == pos) return false;

		else if(hash->datos[lugar].estado == ocupado){
			if(strcmp(hash->datos[lugar].clave, clave) == 0) return true;
		}
	}
	return false;
}

void *hash_borrar(hash_t *hash, const char *clave){
	size_t pos = funcion_hash(clave, hash->tam);
	if(!hash_pertenece(hash, clave)) return NULL;

	while(hash->datos[pos].estado != vacio){
		if( hash->datos[pos].estado == ocupado && strcmp(hash->datos[pos].clave, clave) == 0 ){
			void* dato = hash->datos[pos].dato;
			hash->datos[pos].dato = NULL;
			free(hash->datos[pos].clave);
			hash->datos[pos].estado = borrado;
			hash->ocupado --;
			return dato;
		}
		pos++;
		if (pos == hash->tam) pos = 0;
	}
	return NULL;
}

void hash_destruir(hash_t* hash){
	for(size_t i = 0; i < hash->tam; i ++){
		if(hash->datos[i].estado == ocupado){
			if(hash->dest != NULL){
				hash->dest(hash->datos[i].dato);
			}
			free(hash->datos[i].clave);
		}
	}
	free(hash->datos);
	free(hash);
}


/*--------------------------------------------------------------------
  |				Primitivas del Iterador del Hash					 |
  --------------------------------------------------------------------*/

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if(hash_iter == NULL) return NULL;

	hash_iter->hash = hash;
	if(hash_cantidad(hash_iter->hash) == 0){
		hash_iter->pos = hash_iter->hash->tam;
	}
	else{
		size_t i = 0;
		while(hash_iter->hash->datos[i].estado != ocupado && i < hash_iter->hash->tam){
			i ++;
		}
		hash_iter->pos = i;
	}
	return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter)) return false;

	iter->pos ++;
	
	if(hash_iter_al_final(iter)) return false;
	
	while(iter->hash->datos[iter->pos].estado != ocupado){
		iter->pos ++;

		if(hash_iter_al_final(iter)) return false;
	}
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	return iter->hash->datos[iter->pos].clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return (iter->pos == iter->hash->tam);
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}