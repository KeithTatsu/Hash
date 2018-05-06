#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE ESTRUCTURA
 * *****************************************************************/

/* La Lista está planteada como una lista de punteros genéricos. */

struct lista;
typedef struct lista lista_t;

struct lista_iter; 
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una Lista.
// Post: devuelve una nueva Lista vacía.
lista_t* lista_crear(void);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la Lista llama a destruir_dato.
// Pre: la cola fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir (lista_t* lista, void destruir_dato(void *));


// Devuelve verdadero o falso, según si la lista tiene o no elementos enlistados.
// Pre: la cola fue creada.
bool lista_esta_vacia(const lista_t* lista);

// Agrega un nuevo elemento a la lista en la posicion primero. 
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al inicio
// de la lista.
bool lista_insertar_primero(lista_t* lista, void* dato);

// Agrega un nuevo elemento a la lista en la posicion ultima. 
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al ultimo
// de la lista.
bool lista_insertar_ultimo(lista_t* lista, void* dato);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void* lista_ver_primero(const lista_t* lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del elemento, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void* lista_ver_ultimo(const lista_t* lista);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void* lista_borrar_primero(lista_t* lista);

//Obtiene el largo de la lista
// Pre: la lista fue creada.
// Post: se devolvió el laargo la lista
size_t lista_largo(const lista_t* lista);

/* ******************************************************************
 *                    PRIMITIVAS DE LA ITERADORES EXTERNO
 * *****************************************************************/
// Crea un Iterador externo.
//Pre:Existe una lista
// Post: devuelve una nueva Lista vacía.
lista_iter_t* lista_iter_crear(lista_t *lista);
//Pre: Iterador creado
//Post:Avanza el iterador al siguiente elemento del TDA
bool lista_iter_avanzar(lista_iter_t* iter);
//Pre:Iterador Creado y lista existente
//Post: Muestra el valor del elemento en el Iterador se encuentra
void* lista_iter_ver_actual(const lista_iter_t *iter);

//Pre:Iterador Creado
//Post:Indica si te encuentras en el final del TDA
bool lista_iter_al_final(const lista_iter_t *iter);
//Pre:Iterador Creado y lista existente
//Post:Destruye elIterador
void lista_iter_destruir(lista_iter_t *iter);
//Pre:Iterador Creado y lista existente
//Post:Insertar un elelemento a TDA, actualizando la ubicacion 
bool lista_iter_insertar(lista_iter_t *iter, void *dato);
//Pre: Iterador creado y lista existente
//Post:Borrar elelemento de TDA
void *lista_iter_borrar(lista_iter_t *iter);
/* ******************************************************************
 *                    PRIMITIVAS DE LA ITERADORES INTERNO
 * *****************************************************************/
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);
/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
void pruebas_lista_alumno(void); // En el original esto esta en main fijate si el corrector lo lee

#endif