#include <stdlib.h>
#include "hash.h"

#define TAM_INICIAL 20 //SE PUEDE MODIFICAR

/* POSIBLES FUNCIONES HASH

static unsigned long sdbm(unsigned char *str)
    {
        unsigned long hash = 0;
        int c;

        while (c = *str++)
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
    }
 unsigned long hash(unsigned char *str)
    {
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; // hash * 33 + c 

        return hash;
    }
*/
/**
typedef struct nodo{
	char* clave;
	void* dato;
}nodo_t;
**/
typedef struct hash{
	lista_t** tabla;
	size_t tamanio;
	size_t ocupados;
	hash_destruir_dato_t destruir;
}hash_t;

typedef struct hash_iter{
	size_t pos;
	const hash_t* hash;
	lista_iter_t* iter_actual;
}hash_iter_t;

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

	hash_t* hash_nuevo = malloc(TAM_INICIAL*sizeof(hash_t));

	if(!hash_nuevo) return NULL;

/*PREGUNTAR SI SE CREAN LAS LISTAS AL MOMENTO DE INICIALIZAR EL HASH
 O AL MOMENTO DE AGREGAR UN NUEVO ELEMENTO */

	for(size_t i = 0; i < TAM_INICIAL; i++){
		hash_nuevo[i]->tabla = lista_crear();
		if(!hash_nuevo[i]->tabla) return NULL;
	}

	hash_nuevo->tamanio = TAM_INICIAL;
	hash_nuevo->ocupados = 0;
	hash_nuevo->destruir = destruir_dato;

	return hash_nuevo;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){

}

void *hash_borrar(hash_t *hash, const char *clave){

}

void *hash_obtener(const hash_t *hash, const char *clave){

}

bool hash_pertenece(const hash_t *hash, const char *clave){

}

size_t hash_cantidad(const hash_t *hash){

	return hash->ocupados;
}

void hash_destruir(hash_t *hash){

}

/*ITERADOR*/

hash_iter_t *hash_iter_crear(const hash_t *hash){

}

bool hash_iter_avanzar(hash_iter_t *iter){

}

const char *hash_iter_ver_actual(const hash_iter_t *iter){

}

bool hash_iter_al_final(const hash_iter_t *iter){

}

void hash_iter_destruir(hash_iter_t* iter){

}