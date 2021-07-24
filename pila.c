#include "pila.h"
#include <stdlib.h>

#define CANT_INI 0
#define CAP_INI 10
#define COEF_AGRANDAR 2
#define COEF_ACHICAR 2
#define VALOR_PROPORCION 4

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

//Funcion auxiliar
bool redimensionar(pila_t* pila, size_t nuevo_tam){
	void** nuevos_dat = realloc(pila->datos, sizeof(void*) * nuevo_tam);
	if(nuevos_dat == NULL){
		return false;
	}
	pila->datos = nuevos_dat;
	pila->capacidad = nuevo_tam;
	return true;
}
//

pila_t* pila_crear(void){
    pila_t* pila = malloc(sizeof(pila_t));
    if(pila == NULL){
        return NULL;
    }
    pila->cantidad = CANT_INI;
    pila->capacidad = CAP_INI;
    pila->datos = malloc(pila->capacidad * sizeof(void*));
    if(pila->datos == NULL){
        free(pila);
        return NULL;
    }
    return pila;
}

void pila_destruir(pila_t *pila){
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila){
    return pila->cantidad == 0;
}

bool pila_apilar(pila_t *pila, void* valor){
    if(pila->cantidad == pila->capacidad){
		size_t nuevo_tamanio = pila->capacidad * COEF_AGRANDAR;
        if(!redimensionar(pila,nuevo_tamanio)){
			return false;
		}
    }
    pila->cantidad ++;
    pila->datos[pila->cantidad - 1] = valor;
    return true;
}

void* pila_ver_tope(const pila_t *pila){
    if(pila_esta_vacia(pila)){
        return NULL;
    }
    return pila->datos[pila->cantidad - 1];
}

void* pila_desapilar(pila_t *pila){    
    if(pila_esta_vacia(pila)){
        return NULL;
    }
    if(pila->cantidad * VALOR_PROPORCION <= pila->capacidad){
		size_t nuevo_tamanio = pila->capacidad / COEF_ACHICAR;
        redimensionar(pila,nuevo_tamanio);
    }
    pila->cantidad = pila->cantidad - 1;
    return pila->datos[pila->cantidad];
}
