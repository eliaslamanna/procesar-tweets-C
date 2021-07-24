#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "heap.h"

#define TAM_INICIAL 20

struct heap{
	void** datos;
	size_t cant;
	size_t tam;
	cmp_func_t cmp;
};

//Funciones auxiliares para encontrar posiciones, devuelven -1 si se pasan de rango (es decir que no existen)
size_t pos_hijo_izq(size_t pos){
	size_t pos_hijo = 2*pos + 1;
	return pos_hijo;
}

size_t pos_hijo_der(size_t pos){
	size_t pos_hijo = 2*pos + 2;
	return pos_hijo;
}

size_t pos_padre(size_t pos){
	size_t pos_padre = (pos - 1)/ 2;
	return pos_padre;
}
//

bool heap_redimensionar(heap_t* heap, size_t tam_nuevo){
	void** datos_nuevos = realloc(heap->datos, sizeof(void*) * tam_nuevo);
	if(datos_nuevos == NULL){
		return false;
	}
	heap->datos = datos_nuevos;
	heap->tam = tam_nuevo;
	return true;
}

void swap(void* datos[], size_t pos_1, size_t pos_2){
	void* aux = datos[pos_1];
	datos[pos_1] = datos[pos_2];
	datos[pos_2] = aux;
}

void upheap(void* datos[], size_t pos, cmp_func_t cmp){
	if(pos == 0){
		return;
	}
	size_t padre = pos_padre(pos);
	if(cmp(datos[padre], datos[pos]) < 0){
		swap(datos, padre, pos);
		upheap(datos, padre, cmp);
	}
}

void downheap(void* datos[], size_t cant, size_t pos, cmp_func_t cmp){
	if(pos >= cant){
		return;
	}
	size_t max = pos;
	size_t hijo_der = pos_hijo_der(pos);
	size_t hijo_izq = pos_hijo_izq(pos);
	if( hijo_izq < cant && cmp(datos[hijo_izq], datos[max]) > 0){
		max = hijo_izq;
	}
	if( hijo_der < cant && cmp(datos[hijo_der], datos[max]) > 0){
		max = hijo_der;
	}
	if(max != pos){
		swap(datos, max, pos);
		downheap(datos, cant, max, cmp);
	}
}

heap_t *heap_crear(cmp_func_t cmp){
	heap_t* heap = malloc(sizeof(heap_t));
	if(heap == NULL){
		return NULL;
	}
	void** datos = malloc(sizeof(void*) * TAM_INICIAL);
	if(datos == NULL){
		free(heap);
		return NULL;
	}
	heap->datos = datos;
	heap->cant = 0;
	heap->tam = TAM_INICIAL;
	heap->cmp = cmp;
	return heap;
}

size_t heap_cantidad(const heap_t *heap){
	if(heap == NULL){
		return 0;
	}
	return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap){
	return heap->cant == 0;
}

void *heap_ver_max(const heap_t *heap){
	if(heap_esta_vacio(heap)){
		return NULL;
	}
	return heap->datos[0];
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	for(size_t i = 0; i < heap->cant; i ++){
		if(destruir_elemento){
			destruir_elemento(heap->datos[i]);
		}
	}
	free(heap->datos);
	free(heap);
}

bool heap_encolar(heap_t *heap, void *elem){
	if(elem == NULL){
		return false;
	}
	size_t pos_actual = heap->cant;
	if(pos_actual >= heap->tam){
		if(!heap_redimensionar(heap, heap->tam * 2))
			return false;
	}
	heap->datos[pos_actual] = elem;
	upheap(heap->datos, pos_actual, heap->cmp);
	heap->cant ++;
	return true;
}

void *heap_desencolar(heap_t *heap){
	if(heap_esta_vacio(heap)){
		return NULL;
	}
	void* dato = heap->datos[0];
	swap(heap->datos, 0, heap->cant - 1);
	heap->cant --;
	downheap(heap->datos, heap->cant, 0, heap->cmp);
	return dato;
}

void heapify(void* datos[], size_t n, cmp_func_t cmp){
	for(int ultimo_padre = (int)pos_padre(n - 1); ultimo_padre >= 0; ultimo_padre --){
		downheap(datos, n, ultimo_padre, cmp);
	}
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
	heap_t* heap = heap_crear(cmp);
	if(heap == NULL){
		return NULL;
	}
	void** nuevo = malloc(sizeof(void*) * TAM_INICIAL);
	for(size_t i = 0; i < n; i ++){
		nuevo[i] = arreglo[i];
	}
	heapify(nuevo, n, cmp);
	free(heap->datos);
	heap->datos = nuevo;
	heap->cant = n;
	return heap;
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
	heapify(elementos, cant, cmp);
	for(size_t i = cant - 1; i > 0; i --){
		swap(elementos, 0, i);
		downheap(elementos, i, 0, cmp);
	}
}