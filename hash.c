#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "hash.h"

#define TAM_INICIAL 20 //SE PUEDE MODIFICAR
#define TAM_AGRANDAR 2
#define TAM_ACHICAR 4
#define PORCENTAJE_AGRANDAR 70
#define PORCENTAJE_ACHICAR 20

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

	return hash_v;
}

//SE PUEDE MODIFICAR A UN NOMBRE MAS DESCRIPTIVO
typedef struct nodo{
	const char* clave;
	void* dato;
}nodo_t;

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

bool inicializar_tabla(hash_t* hash_nuevo, size_t tam){

	for(size_t i = 0; i < tam; i++){
		hash_nuevo->tabla[i] = lista_crear();
		if(!hash_nuevo->tabla[i]){
			for(size_t j = i; j > -1; j--){
				free(hash_nuevo->tabla[j]);
			}
			free(hash_nuevo);
			return false;
		}
	}

	return true;
}

bool pasar_datos(hash_t* hash_viejo, hash_t* hash_nuevo){

	for(size_t i = 0; i < hash_viejo->tamanio; i++){
		lista_iter_t* iter_lista = lista_iter_crear(hash_viejo->tabla[i]);

		if(!iter_lista) return false;

		while(!lista_iter_al_final(iter_lista)){
			nodo_t* campo = lista_iter_ver_actual(iter_lista);
			if(!hash_guardar(hash_nuevo, campo->clave, campo->dato)){
				return false;
			}
			hash_nuevo->ocupados++;
			lista_iter_avanzar(iter_lista);
		}
		lista_iter_destruir(iter_lista);
	}

	hash_destruir(hash_viejo);

	return true;
}

hash_t* redimensionar_hash(hash_t* hash_viejo, size_t tam_nuevo){

	hash_t* hash_nuevo = malloc(sizeof(hash_t));

	if(!hash_nuevo)	return hash_viejo;

	hash_nuevo->tabla = malloc(tam_nuevo*sizeof(lista_t*));

	if(!hash_nuevo->tabla){
		free(hash_nuevo);
		return hash_viejo;
	}

	if(!inicializar_tabla(hash_nuevo, tam_nuevo)){
		return hash_viejo;
	}

	hash_nuevo->tamanio = tam_nuevo;
	//PODRIA PASARSE EL HASH_VIEJO->OCUPADOS PERO EN PRINCIPIO NO HAY SEGURIDAD QUE PASEN TODOS LOS DATOS
	hash_nuevo->ocupados = 0;
	hash_nuevo->destruir = hash_viejo->destruir;

	if(!pasar_datos(hash_viejo, hash_nuevo)){
		hash_destruir(hash_nuevo);
		return hash_viejo;
	}

	return hash_nuevo;
}

nodo_t* crear_nodo(const char* clave, void* dato){

	nodo_t* nodo_nuevo = malloc(sizeof(nodo_t));

	if(!nodo_nuevo) return NULL;

	nodo_nuevo->clave = clave;
	nodo_nuevo->dato = dato;

	return nodo_nuevo;
}

void* buscar_clave(lista_t* tabla, const char* clave){

	nodo_t* nodo_actual;
	lista_iter_t* iter_lista = lista_iter_crear(tabla);

	if(!iter_lista) return NULL;

	while(!lista_iter_al_final(iter_lista)){
		nodo_actual = lista_iter_ver_actual(iter_lista);
		if(strcmp(nodo_actual->clave, clave) == 0){
			lista_iter_destruir(iter_lista);
			return nodo_actual->dato;
		}
	}

	return NULL;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

	hash_t* hash_nuevo = malloc(sizeof(hash_t));

	if(!hash_nuevo) return NULL;

	hash_nuevo->tabla = malloc(TAM_INICIAL*sizeof(lista_t*));

	if(!hash_nuevo->tabla){
		free(hash_nuevo);
		return NULL;
	}

	if(!inicializar_tabla(hash_nuevo, TAM_INICIAL)){
		return NULL;
	}

	hash_nuevo->tamanio = TAM_INICIAL;
	hash_nuevo->ocupados = 0;
	hash_nuevo->destruir = destruir_dato;

	return hash_nuevo;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){

	if((100*hash->ocupados)/hash->tamanio >= PORCENTAJE_AGRANDAR){
		size_t tam_nuevo = hash->tamanio*TAM_AGRANDAR;
		hash = redimensionar_hash(hash, tam_nuevo);
	}

	nodo_t* campo = crear_nodo(clave, dato);

	if(!campo) return false;

	size_t pos = hash_f(clave, hash->tamanio);	//hash_f es la función de hash

	if(!lista_insertar_ultimo(hash->tabla[pos], campo)){
		return false;
	}

	hash->ocupados++;

	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){

	if(!hash || hash->ocupados == 0) return NULL; //POR LAS DUDAS, VERIFICAR CON LAS PRUEBAS

	if((100*hash->ocupados)/hash->tamanio <= PORCENTAJE_ACHICAR){
		if(hash->tamanio > TAM_INICIAL){
			size_t tam_nuevo = hash->tamanio/TAM_ACHICAR;
			hash = redimensionar_hash(hash, tam_nuevo);
		}
	}

	size_t pos = hash_f(clave, hash->tamanio);

	lista_iter_t* iter_lista = lista_iter_crear(hash->tabla[pos]);

	if(!iter_lista) return NULL;

	while(!lista_iter_al_final(iter_lista)){
		nodo_t* campo = lista_iter_ver_actual(iter_lista);
		if(strcmp(campo->clave, clave) == 0){
			lista_iter_borrar(iter_lista);
			hash->ocupados--;
			return campo->dato; //VERIFICAR QUE NO DEVUELVA BASURA
		}
	}

	return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){

	if(hash->ocupados == 0)	return NULL;

	size_t pos = hash_f(clave, hash->tamanio);

	void* valor = buscar_clave(hash->tabla[pos], clave);

	return valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){

	if(hash->ocupados == 0)	return false;

	size_t pos = hash_f(clave, hash->tamanio);

	return (buscar_clave(hash->tabla[pos], clave) != NULL);
}

size_t hash_cantidad(const hash_t *hash){

	return hash->ocupados;
}

void hash_destruir(hash_t *hash){

	nodo_t* campo;

	for(size_t i = 0; i < hash->tamanio; i++){
		while(!lista_esta_vacia(hash->tabla[i])){
			campo = lista_borrar_primero(hash->tabla[i]);
			if(hash->destruir != NULL){
				free(campo->dato);
			}
			free(campo);
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
	hash_iter->iter_actual = lista_iter_crear(hash->tabla[hash_iter->pos]);
	
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
			iter->iter_actual = lista_iter_crear(iter->hash->tabla[iter->pos]);
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