#include <stdlib.h>
#include <string.h>
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

//SE PUEDE MODIFICAR A UN NOMBRE MAS DESCRIPTIVO
typedef struct nodo{
	char* clave;
	void* dato;
}nodo_t;

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

nodo_t* crear_nodo(char* clave, void* dato){

	nodo_t* nodo_nuevo = malloc(sizeof(nodo_t));

	if(!nodo_nuevo) return NULL;

	nodo_nuevo->clave = clave;
	nodo_nuevo->dato = dato;

	return nodo_nuevo;
}

void* buscar_clave(lista_t* lista, char* clave){

	nodo_t* nodo_actual;
	lista_iter_t* iter_lista = lista_iter_crear(lista);

	if(!lista_iter) return NULL;

	while(!lista_iter_al_final(iter_lista)){
		nodo_actual = lista_iter_ver_actual(iter_lista);
		if(strcmp(nodo_actual->clave, clave) == 0){
			return nodo_actual->dato;
		}
	}

	return NULL;
}

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

	nodo_t* nodo = crear_nodo(clave, dato);

	if(!nodo) return false;

	unsigned int pos = hash_f(clave);	//hash_f es la función de hash

	return lista_insertar_ultimo(hash[pos]->tabla, nodo);
}

void *hash_borrar(hash_t *hash, const char *clave){

}

void *hash_obtener(const hash_t *hash, const char *clave){

	unsigned int pos = hash_f(clave);

	void* valor = buscar_clave(hash[pos]->tabla, clave);

	return valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){

	unsigned int pos = hash_f(clave);

	return buscar_clave(hash[pos]->tabla, clave) != NULL;
}

size_t hash_cantidad(const hash_t *hash){

	return hash->ocupados;
}

void hash_destruir(hash_t *hash){

	nodo_t* nodo;

	for(size_t i = 0; i < hash->tamanio; i++){
		while(!lista_esta_vacia(hash[i]->tabla)){
			nodo = lista_borrar_primero(hash[i]->tabla);
			if(hash[i]->destruir != NULL){
				free(nodo->dato);
			}
			free(nodo);
		}
	}

	free(hash);
}

/*ITERADOR*/

hash_iter_t *hash_iter_crear(const hash_t* hash){
	
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if (!hash_iter) return NULL;

	hash_iter->hash = hash;
	hash_iter->pos = 0; //Esto no se si lo estoy haciendo bien , creo que deberia ir buscando la primera lista del hash
	hash_iter->iter_actual = lista_iter_crear(hash->tabla[pos]);
	
	if (!hash_iter->iter_actual){
		free(hash_iter);
		return NULL;
	}

	return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t* iter){
	if(iter->hash->tamanio < iter->pos)return false;//Aca creo que es tamaño
	
	free(iter->iter_actual);
	/*++ iter->pos;
	iter->hash->iter_actual = lista_iter_crear(iter->hash->tabla[pos]);*///Creo que aca deberia ir buscando todas las casillas del hash
																	// hasta encontrar una lista ,porque si no apuntaria a Null	
	do{//Asi
		++iter->pos;
		if(iter->hash->tabla[iter->pos]){
			iter->iter_actual = lista_iter_crear(iter->hash->tabla[pos]);
			return true;
		}
	}while(iter->pos < iter->hash->tamanio);//Pos se cuenta desde cero por ende no es  menor igual

	return false; 
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	nodo_t* campo = lista_iter_ver_actual(iter->iter_actual);//lisya_iter_ver_actual cumple condicion de const
	return campo->clave;
}

bool hash_iter_al_final(const hash_iter_t* iter){
	return(iter->pos == iter->hash->tamanio);
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter->iter_actual);
	free(iter);
}