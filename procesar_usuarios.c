#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"
#include "strutil.h"

typedef struct usuario{
	char* nombre;
	size_t cant_tweets;
}usuario_t;

usuario_t* usuario_crear(const char* clave, size_t valor){
	usuario_t* usuario = malloc(sizeof(usuario_t));
	if(usuario == NULL){
		return NULL;
	}
	usuario->nombre = strdup(clave);
	usuario->cant_tweets = valor;
	return usuario;
}

usuario_t** pasar_a_vector(hash_t* hash){
	hash_iter_t* hash_iter = hash_iter_crear(hash);
	if(hash_iter == NULL){
		return NULL;
	}
	usuario_t** lista = malloc(sizeof(usuario_t*) * hash_cantidad(hash));
	if(lista == NULL){
		free(hash_iter);
		return NULL;
	}
	int i = 0;
	while(!hash_iter_al_final(hash_iter)){
		usuario_t* usuario = usuario_crear(hash_iter_ver_actual(hash_iter), hash_cantidad(hash_obtener(hash, hash_iter_ver_actual(hash_iter))) );
		if(usuario == NULL){
			hash_iter_destruir(hash_iter);
			return NULL;
		}
		lista[i] = usuario;
		i ++;
		hash_iter_avanzar(hash_iter);
	}
	hash_iter_destruir(hash_iter);
	return lista;
}

char** counting(usuario_t** arr, size_t n, size_t min, size_t max){
	size_t k = max - min;
	int contador[k + 1];
	for(int i = 0; i < k; i ++){
		contador[i] = 0;
	}
	for(int i = 0; i < n; i ++){
		size_t numero = arr[i]->cant_tweets;
		contador[numero - min] ++;
	}
	int suma_acumulada[k + 2];
	suma_acumulada[0] = 0;
	for(int i = 1; i < (k + 2); i ++){
		int cantidad = contador[i - 1];
		suma_acumulada[i] = suma_acumulada[i - 1] + cantidad;
	}
	char** resultado = malloc(sizeof(char*) * n);
	if(resultado == NULL){
		return NULL;
	}
	for(int i = 0; i < n; i ++){
		size_t valor = arr[i]->cant_tweets;
		int pos = suma_acumulada[valor - min];
		suma_acumulada[valor - min] ++;
		resultado[pos] = strdup(arr[i]->nombre);
	}
	return resultado;
}

int devolver_cant(int* arr, size_t min, size_t max){
	size_t k = max - min;
	int cant = 0;
	for(int i = 0; i < (k + 2); i ++){
		cant ++;
	}
	return cant;
}

int* suma_acumulada(usuario_t** arr, size_t n, size_t min, size_t max){
	size_t k = max - min;
	int* contador = calloc(k + 2,sizeof(int));
	if(contador == NULL){
		return NULL;
	}
	for(int i = 0; i < k; i ++){
		contador[i] = 0;
	}
	for(int i = 0; i < n; i ++){
		size_t numero = arr[i]->cant_tweets;
		contador[numero - min] ++;
	}
	int suma_acumulada[k + 2];
	suma_acumulada[0] = 0;
	for(int i = 1; i < (k + 2); i ++){
		int cantidad = contador[i - 1];
		suma_acumulada[i] = suma_acumulada[i - 1] + cantidad;
	}
	return contador;
}
	
void arrange(int n, char *x[]){
    char *temp;
    int i,str;
    for(str = 0; str < n-1; ++str){
        for(i = str+1; i < n; ++i){
            if(strcmp(x[str],x[i]) > 0){
                temp = x[str];
                x[str] = x[i];
                x[i] = temp;
            }
        }
    }
    return;
}

void imprimir(hash_t* hash, int* contador, char** string, int cantidad){
	int pos = 0;
	for(int i = 0; i < cantidad; i ++){
		if(contador[i] != 0){
			char** cadena = malloc(sizeof(char*) * contador[i]);
			int tam = 0;
			for(int x = 0; x < contador[i]; x ++){
				cadena[x] = strdup(string[pos]);
				tam ++;
				pos ++;
			}
			arrange(tam, cadena);
			fprintf(stdout, "%zu: ", hash_cantidad(hash_obtener(hash, cadena[0]) ));
			if(tam != 1){
				fprintf(stdout, "%s, ", cadena[0]);
				free(cadena[0]);
			}
			else{
				fprintf(stdout, "%s\n", cadena[0]);
				free(cadena[0]);
			}
			for(int z = 1; z < tam; z ++){
				if(z != tam - 1 && tam != 1){
					fprintf(stdout, "%s, ", cadena[z]);
					free(cadena[z]);
				}
				else{
					fprintf(stdout, "%s\n", cadena[z]);
					free(cadena[z]);
				}
			}
			free(cadena);
		}
	}
}



size_t* encontrar_min_max(hash_t* hash){
	hash_iter_t* hash_iter = hash_iter_crear(hash);
	if(hash_iter == NULL){
		return NULL;
	}
	size_t min = 1;
	size_t max = 1;
	size_t* valores = malloc(sizeof(size_t) * 2);
	while(!hash_iter_al_final(hash_iter)){
		size_t valor_actual = hash_cantidad(hash_obtener(hash, hash_iter_ver_actual(hash_iter)));
		if(valor_actual < min){
			min = valor_actual;
		}
		else if(valor_actual > max){
			max = valor_actual;
		}
		hash_iter_avanzar(hash_iter);
	}
	valores[0] = min;
	valores[1] = max;
	hash_iter_destruir(hash_iter);
	return valores;
}

void destruir_todo(hash_t* hash_usuarios, int* suma, usuario_t** usuarios, size_t* valores, char** nombres, char* linea){
	for(int i = 0; i < hash_cantidad(hash_usuarios); i ++){
		free(usuarios[i]->nombre);
		free(usuarios[i]);
	}
	free(usuarios);
	for(int i = 0; i < hash_cantidad(hash_usuarios); i ++){
		free(nombres[i]);
	}
	free(nombres);
	hash_iter_t* iter = hash_iter_crear(hash_usuarios);
	while(!hash_iter_al_final(iter)){
		hash_destruir( hash_obtener(hash_usuarios, hash_iter_ver_actual(iter)) );
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
	hash_destruir(hash_usuarios);
	free(valores);
	free(linea);
	free(suma);
}

int procesar_usuarios(const char* archivo){
	FILE* arch = fopen(archivo, "r");
	if(arch == NULL){
		fprintf(stderr,"Error: archivo fuente inaccesible\n");
		return -1;
	}
	hash_t* hash_usuarios = hash_crear(NULL);
	if(hash_usuarios == NULL){
		fclose(arch);
		return -1;
	}
	char *linea = NULL;
	size_t tam = 0;
	ssize_t letras = 1;
	letras = getline(&linea,&tam,arch);
	linea[strlen(linea) - 1] = '\0';
	while(letras != -1){
		char** cadena = split(linea, ',');
		if(!hash_pertenece(hash_usuarios, cadena[0])){
			hash_t* hash_tweets = hash_crear(NULL);
			if(hash_tweets == NULL){
				free_strv(cadena);
				free(linea);
				hash_destruir(hash_usuarios);
				fclose(arch);
				return -1;
			}
			hash_guardar(hash_usuarios, cadena[0], hash_tweets);
		}
		int i = 1;
		while(cadena[i] != NULL){
			if(!hash_pertenece(hash_obtener(hash_usuarios, cadena[0]), cadena[i])){
				hash_guardar(hash_obtener(hash_usuarios, cadena[0]), cadena[i], NULL);
			}
			i ++;
		}
		free_strv(cadena);
		letras = getline(&linea,&tam,arch);
		linea[strlen(linea) - 1] = '\0';
	}
	size_t* valores = encontrar_min_max(hash_usuarios);
	usuario_t** usuarios = pasar_a_vector(hash_usuarios);
	char** nombres = counting(usuarios, hash_cantidad(hash_usuarios), valores[0], valores[1]);
	int* suma = suma_acumulada(usuarios, hash_cantidad(hash_usuarios), valores[0], valores[1]);
	int cant = devolver_cant(suma, valores[0], valores[1]);
	imprimir(hash_usuarios, suma, nombres, cant);
	destruir_todo(hash_usuarios, suma, usuarios, valores, nombres, linea);
	fclose(arch);
	return 0;
}
int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr,"Error: Cantidad erronea de parametros\n");
		return -1;
	}
	const char *archivo = argv[1];
	procesar_usuarios(archivo);
	return 0;
}