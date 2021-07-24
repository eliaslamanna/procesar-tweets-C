#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "strutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char* substr(const char* str, size_t cant){
	char* cadena = strndup(str, cant);
	return cadena;	
}

char** split(const char* str, char sep){
	if(sep == '\0'){
		char **lista = malloc(sizeof(char*) * 2);
		if(lista == NULL){
			return NULL;
		}
		lista[0] = strdup(str);
		lista[1] = NULL;
		return lista;
	}
	size_t ultima_coma = 0;
	size_t separadores = 0;
	int pos = 0;
	size_t largo = strlen(str);
	for(int i = 0; i < largo; i ++){
		if(str[i] == sep){
			separadores++;
		}
	}
	char **cadena = malloc(sizeof(char*) * (separadores + 2));
	if(cadena == NULL){
		return NULL;
	}
	for(size_t i = 0; i < largo + 1; i ++){
		while(str[i] == sep){
			cadena[pos] = strndup(&str[ultima_coma], (i - ultima_coma));
			pos++;
			i++;
			ultima_coma = i;
		}
		if(str[i] == '\0'){
			cadena[pos] = strndup(&str[ultima_coma], (i - ultima_coma));
			pos++;
		}
	}
	cadena[pos] = NULL;
	return cadena;
}

void insertar(char** string, char separador, char* cadena){
	size_t aux = 0;
	for(size_t j = 0; string[j] != NULL; j ++){
		size_t largo2 = strlen(string[j]);
		for(size_t x = 0; x < largo2 + 1; x ++){
			if(string[j][x] == '\0' && string[j + 1] != NULL && separador != '\0'){
				cadena[aux] = separador;
				aux ++;
			}
			else if(string[j][x] != '\0'){
				cadena[aux] = string[j][x];
				aux ++;
			}
			
		}
	}
}

char* join(char** strv, char sep){
	size_t cantidad = 0;
	size_t chars = 0;
	while(strv[cantidad] != NULL){
		chars += strlen(strv[cantidad]);
		cantidad ++;
	}
	char *cadena = calloc((chars + cantidad + 1), sizeof(char));
	if(cadena == NULL){
		return NULL;
	}
	if(strv[0] == NULL){
		cadena[0] = '\0';
	}
	else{
		insertar(strv,sep,cadena);
	}
	return cadena;
}

void free_strv(char* strv[]){
	int i = 0;
	while(strv[i] != NULL){
		free(strv[i]);
		i ++;
	}
	free(strv);
}