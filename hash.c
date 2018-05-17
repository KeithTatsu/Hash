#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "hash.h"

#define TAM_INICIAL 103 //MIENTRAS MAS GRANDE EL NUMERO PRIMO MAS RAPIDO
#define TAM_AGRANDAR 2
#define TAM_ACHICAR 4
#define PORCENTAJE_AGRANDAR 70
#define PORCENTAJE_ACHICAR 20
#define CANT_PROMEDIO 3

/* POSIBLES FUNCIONES HASH

static unsigned long sdbm(unsigned char *str)
    {
        unsigned long hash = 0;
        int c;

        while (c = *str++)
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
    }
    */
size_t hash_f(const char *clave, size_t tam){

	size_t hash_v = tam, i = 0;
	int c;

	while(clave[i] != '\0'){
		c = clave[i];
		hash_v = ((hash_v << 5) + hash_v) + c; // hash * 33 + c 
		i++;
	}

	return hash_v%tam;
}

typedef struct campo{
	const char* clave;
	void* dato;
}campo_t;

struct hash{
	lista_t** tabla;
	size_t tamanio;
	size_t ocupados;
	hash_destruir_dato_t destruir;
};

struct hash_iter{
	size_t pos;
	const hash_t* hash;
	lista_iter_t* iter_actual;
};

campo_t* crear_campo(const char* clave, void* dato){

	campo_t* campo_nuevo = malloc(sizeof(campo_t));

	if(!campo_nuevo) return NULL;

	campo_nuevo->clave = clave;
	campo_nuevo->dato = dato;

	return campo_nuevo;
}

campo_t* buscar_clave(lista_t* tabla, const char* clave){

	campo_t* campo_actual;
	lista_iter_t* iter_lista = lista_iter_crear(tabla);

	if(!iter_lista) return NULL;

	while(!lista_iter_al_final(iter_lista)){
		campo_actual = lista_iter_ver_actual(iter_lista);
		if(strcmp(campo_actual->clave, clave) == 0){
			lista_iter_destruir(iter_lista);
			return campo_actual;
		}
		lista_iter_avanzar(iter_lista);
	}

	return NULL;
}

bool insertar_existente(lista_t* tabla, const char* clave, void* dato, hash_destruir_dato_t destruir){

	campo_t* campo = buscar_clave(tabla, clave);

	if(destruir){
		void* dato_aux = campo->dato;
		free(dato_aux);
	}

	campo->dato = dato;

	return true;
}

bool insertar_en_tabla(lista_t* tabla, const char* clave, void* dato){

	campo_t* campo = crear_campo(clave, dato);

	if(!campo) return false;

	return lista_insertar_ultimo(tabla, campo);
}

bool pasar_datos(hash_t* hash, lista_t** tabla_nueva){

	for(size_t i = 0; i < hash->tamanio; i++){
		lista_iter_t* iter_lista = lista_iter_crear(hash->tabla[i]);

		if(!iter_lista){
			free(tabla_nueva);
			return false;
		}

		while(!lista_iter_al_final(iter_lista)){
			campo_t* campo = lista_iter_ver_actual(iter_lista);
			if(!insertar_en_tabla(tabla_nueva[i], campo->clave, campo->dato)){
				for(size_t j = i-1; j > 0; j--){
					lista_destruir(tabla_nueva[j], hash->destruir);
				}
				return false;
			}
			lista_iter_avanzar(iter_lista);
		}
		lista_iter_destruir(iter_lista);
	}

	return true;
}

bool inicializar_tabla(lista_t** tabla, size_t tam){

	for(size_t i = 0; i < tam; i++){
		tabla[i] = lista_crear();
		if(!tabla[i]){// esto anda ??
			for(size_t j = i; j > 0; j--){
				lista_destruir(tabla[j], NULL);
			}
			return false;
		}
	}

	return true;
}

lista_t** redimensionar_hash(hash_t* hash, size_t tam_nuevo){

	lista_t** tabla_nueva = malloc(tam_nuevo*sizeof(lista_t*));

	if(!tabla_nueva) return hash->tabla;

	if(!inicializar_tabla(tabla_nueva, tam_nuevo)){
		free(tabla_nueva);
		return hash->tabla;
	}

	if(!pasar_datos(hash, tabla_nueva)) return hash->tabla;

	return tabla_nueva;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

	hash_t* hash_nuevo = malloc(sizeof(hash_t));

	if(!hash_nuevo) return NULL;

	hash_nuevo->tabla = malloc(TAM_INICIAL*sizeof(lista_t*));

	if(!hash_nuevo->tabla){
		free(hash_nuevo);
		return NULL;
	}

	if(!inicializar_tabla(hash_nuevo->tabla, TAM_INICIAL)){
		free(hash_nuevo);
		return NULL;
	}

	hash_nuevo->tamanio = TAM_INICIAL;
	hash_nuevo->ocupados = 0;
	hash_nuevo->destruir = destruir_dato;

	return hash_nuevo;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){

	if((100*hash->ocupados)/(hash->tamanio*CANT_PROMEDIO) >= PORCENTAJE_AGRANDAR){
		size_t tam_nuevo = hash->tamanio*TAM_AGRANDAR;
		hash->tabla = redimensionar_hash(hash, tam_nuevo);
	}

	size_t pos = hash_f(clave, hash->tamanio);

	if(hash_pertenece(hash, clave)){
		insertar_existente(hash->tabla[pos], clave, dato, hash->destruir);
		return true;
	}

	if(!insertar_en_tabla(hash->tabla[pos], clave, dato)) return false;

	hash->ocupados++;

	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){

	if(!hash || hash->ocupados == 0) return NULL;

	if((100*hash->ocupados)/(hash->tamanio*CANT_PROMEDIO) <= PORCENTAJE_ACHICAR){
		if(hash->tamanio > TAM_INICIAL){
			size_t tam_nuevo = hash->tamanio/TAM_ACHICAR;
			hash->tabla = redimensionar_hash(hash, tam_nuevo);
		}
	}

	size_t pos = hash_f(clave, hash->tamanio);

	lista_iter_t* iter_lista = lista_iter_crear(hash->tabla[pos]);

	if(!iter_lista) return NULL;

	while(!lista_iter_al_final(iter_lista)){
		campo_t* campo = lista_iter_ver_actual(iter_lista);
		if(strcmp(campo->clave, clave) == 0){
			lista_iter_borrar(iter_lista);
			hash->ocupados--;
			return campo->dato;
		}
		lista_iter_avanzar(iter_lista);
	}

	return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){

	if(hash->ocupados == 0)	return NULL;

	size_t pos = hash_f(clave, hash->tamanio);

	campo_t* campo = buscar_clave(hash->tabla[pos], clave);

	if(!campo) return NULL;

	return campo->dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){

	if(hash->ocupados == 0)	return false;

	size_t pos = hash_f(clave, hash->tamanio);

	campo_t* campo = buscar_clave(hash->tabla[pos], clave);

	return (campo != NULL);
}

size_t hash_cantidad(const hash_t *hash){

	return hash->ocupados;
}

void hash_destruir(hash_t *hash){

	campo_t* campo;

	for(size_t i = 0; i < hash->tamanio; i++){
		if(hash->tabla[i] != NULL){
			while(!lista_esta_vacia(hash->tabla[i])){
				campo = lista_borrar_primero(hash->tabla[i]);
				if(hash->destruir != NULL){
					free(campo->dato);
				}
				free(campo);
			}
		}
	}

	free(hash);
}

/*ITERADOR*/

size_t _encontrar_lista_no_vacia(lista_t** tabla,size_t pos,size_t tamanio){
	
	size_t new_pos = pos;
	while(lista_esta_vacia(tabla[new_pos])){
		++new_pos;
		if((tamanio-1) <= new_pos) return pos;// pos seria la ubicacion de la ultima lista, la misma, estoy al final
	}
	return new_pos;
}

hash_iter_t *hash_iter_crear(const hash_t* hash){
	
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if (!hash_iter) return NULL;
	hash_iter->hash = hash;

	size_t i = _encontrar_lista_no_vacia(hash->tabla,0,hash->tamanio);// el cero , numero magico  usar define
	hash_iter->pos = i;

	hash_iter->iter_actual = lista_iter_crear(hash->tabla[i]);	
	if (!hash_iter->iter_actual){
		free(hash_iter);
		return NULL;
	}
	return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t* iter){
	if(hash_iter_al_final(iter)) return false;
	
	if(!lista_iter_al_final(iter->iter_actual)){
		lista_iter_avanzar(iter->iter_actual);
		return true;
	}		
	size_t new_pos = _encontrar_lista_no_vacia(iter->hash->tabla,iter->pos,iter->hash->tamanio);
	lista_iter_t* lista_iter = lista_iter_crear(iter->hash->tabla[new_pos]);
	if(!lista_iter) return false;
	free(iter->iter_actual);
	
	iter->iter_actual = lista_iter;
	iter->pos = new_pos;
	return true;																
}
const char *hash_iter_ver_actual(const hash_iter_t *iter){

	campo_t* campo =lista_iter_ver_actual(iter->iter_actual);
	if(!campo) return NULL;
	return campo->clave;
}

bool hash_iter_al_final(const hash_iter_t* iter){
	if(!lista_iter_al_final(iter->iter_actual)){
		return false;
	}
	size_t new_pos = _encontrar_lista_no_vacia(iter->hash->tabla,iter->pos,iter->hash->tamanio);
	if (new_pos != iter->pos) return false; //para en el null de la ultima lista no vacia
	return true;
}

void hash_iter_destruir(hash_iter_t* iter){
	lista_iter_destruir(iter->iter_actual);
	free(iter);
}