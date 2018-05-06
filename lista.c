#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>

#define CANTIDAD_INICIAL 0

/* Definición del struct Nodo y Lista proporcionado por la cátedra.
 */

typedef struct nodo {
    void* valor;
    char* clave;
    struct nodo* prox;
}nodo_t;
/******************************************************************/
struct lista {
	nodo_t* prim;
	nodo_t* ult;
	size_t largo;
};
/* *****************************************************************/
struct lista_iter {
	nodo_t* act;
	nodo_t* ant;
	lista_t* lista;  
};
/* *****************************************************************
 *                    PRIMITIVAS DEL NODO
 * *****************************************************************/
 nodo_t* nodo_crear(void){

	nodo_t* nodo = malloc(sizeof(nodo_t));

	if (nodo == NULL) return NULL;

    return nodo;
}
/* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/
lista_t* lista_crear(void){

	lista_t* lista = malloc(sizeof(lista_t));

	if (!lista) return NULL;

    lista->prim = lista->ult = NULL;
    lista->largo = CANTIDAD_INICIAL;

    return lista;
}
void lista_destruir (lista_t* lista, void destruir_dato(void *)){

	void* auxiliar;	
	while(!lista_esta_vacia(lista)){
		auxiliar = lista_borrar_primero(lista);
		if(destruir_dato != NULL) destruir_dato(auxiliar);
	}
	free(lista);

}

bool lista_esta_vacia(const lista_t* lista){
	return lista->largo == 0;
}

void _asignar_primer_nodo(lista_t* lista,nodo_t* nodo){	

	nodo->prox = NULL;
	lista->prim = nodo;
	lista->ult = nodo;
}

bool lista_insertar_primero(lista_t* lista, void* dato){

	nodo_t* nodo_nuevo = nodo_crear();
	
	if (nodo_nuevo == NULL) return false;

	nodo_nuevo->valor = dato;

	if (lista_esta_vacia(lista)){
		_asignar_primer_nodo(lista,nodo_nuevo);
	}
	else{
		nodo_nuevo->prox = lista->prim;
		lista->prim = nodo_nuevo;
	}
	++ lista->largo;

	return true;
}

bool lista_insertar_ultimo(lista_t* lista, void* dato){

	nodo_t* nodo_nuevo =nodo_crear();
	
	if (!nodo_nuevo) return false;

	nodo_nuevo->valor = dato;

	if (lista_esta_vacia(lista)){
		_asignar_primer_nodo(lista,nodo_nuevo);
	}
	else{
		lista->ult->prox = nodo_nuevo;
		nodo_nuevo->prox = NULL;
		lista->ult=nodo_nuevo;
	}
	++ lista->largo;

	return true;
}

void* lista_ver_primero(const lista_t* lista){
	
	if(lista_esta_vacia(lista)) return NULL;
	return lista->prim->valor;
}

void* lista_ver_ultimo(const lista_t* lista){
	
	if(lista_esta_vacia(lista)) return NULL;
	return lista->ult->valor;
}

void* lista_borrar_primero(lista_t* lista){
	
	if(lista_esta_vacia(lista)) return NULL;
	
	void* puntero_de_valor = lista->prim->valor;
	nodo_t* puntero_de_nodo = lista->prim;
	
	lista->prim = lista->prim->prox;
	-- lista->largo;
	
	if(lista_esta_vacia(lista)) lista->ult = NULL;
	
	free(puntero_de_nodo);
	return puntero_de_valor;
}

size_t lista_largo(const lista_t* lista){
	return lista->largo;
}

/* *****************************************************************
 *                    PRIMITIVAS ITERADORES DE EXTERNO
 * *****************************************************************/
lista_iter_t* lista_iter_crear(lista_t *lista){
	
	lista_iter_t* lista_iter = malloc(sizeof(lista_iter_t));

	if (lista_iter == NULL) return NULL;
	
	lista_iter->act = lista->prim;
	lista_iter->ant = NULL;
	lista_iter->lista = lista;
	
	return lista_iter;
}

bool lista_iter_avanzar(lista_iter_t* iter){
	
	if(!iter->act)return false;// act puede ser Null
	iter->ant = iter->act;
	iter->act = iter->act->prox;

	return true;
}

void* lista_iter_ver_actual(const lista_iter_t *iter){
	if(!iter->act) return NULL;
	return iter->act->valor;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return(iter->act == NULL);
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){

	if (lista_esta_vacia(iter->lista)){//Estoy en una lista vacia
		lista_insertar_primero(iter->lista,dato);
		iter->act = iter->lista->prim;
		return true;
	}

	nodo_t* nodo_nuevo = nodo_crear();/*Creo nodo*/
	if (!nodo_nuevo) return false;
	nodo_nuevo->valor = dato;

	if(!iter->act ){//Estoy en NULL
		iter->ant->prox = nodo_nuevo;
		iter->act = nodo_nuevo;
		nodo_nuevo->prox = NULL;
		iter->lista->ult = nodo_nuevo;
	}
	else if(!iter->ant){// Un solo elemento
		nodo_nuevo->prox = iter->act;
		iter->act = nodo_nuevo;
		iter->lista->prim = nodo_nuevo;
	}
	else{//Todo los demas caso
		iter->ant->prox = nodo_nuevo;
		nodo_nuevo->prox = iter->act;
		iter->act = nodo_nuevo;
	}			
	++ iter->lista->largo;
	return true; 
}
void *lista_iter_borrar(lista_iter_t *iter){

	if(lista_iter_al_final(iter)) return NULL;// ACT ES NULL
	
	if(iter->act == iter->lista->prim){// ACT esta en el primero de la lista
		iter->act = iter->act->prox;
		return lista_borrar_primero(iter->lista);
	}
	void* puntero_de_valor = iter->act->valor;
	

	iter->ant->prox = iter->act->prox;// el ante apunte al proximo de act
	if(!iter->act->prox){//Mantener el Invariante
		iter->lista->ult = iter->ant;
	}
	free(iter->act);
	iter->act = iter->ant->prox; // act apunte al proximo
	-- iter->lista->largo;
	return puntero_de_valor;	 
}

/* *****************************************************************
 *                    PRIMITIVAS ITERADORES DE INTERNO
 * *****************************************************************/
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	nodo_t* aux_nodo = lista->prim;
	bool condicion;
	do{
		if(!aux_nodo)return;
		condicion=visitar(aux_nodo->valor,extra);
		aux_nodo = aux_nodo->prox;
		
	}while(condicion);
}