#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "hash.h"
#include "hash_iterador.h"

#define uint unsigned int
#define FACTOR_CARGA 3.0f
#define or ||

/*
gcc *.c -o hash -g -std=c99 -Wall -Wconversion -Wtype -limits -pedantic -Werror -O0

valgrind --leak-check=full --track -origins=yes --show-reachable=yes ./hash
*/


struct hash  
{
	lista_t** tabla_hash;
	size_t cantidad;
	size_t tamanio_tabla;
	void (*hash_destruir_dato_t)(void*);

};

/* El iterador externo lleva cuenta de la posicion en la tabla hash y un iterador a la lista
   en esa posicion */

struct hash_iter
{
	size_t pos_tabla;
	size_t max_tabla;
	lista_t** tabla_hash;
	lista_iterador_t* it_lista;
};

/* Estructura para el par clave-valor que almacena cada lista de la tabla hash */

typedef struct celda
{
	char* clave;
	void* valor;
}celda_t;

/* Funcion de hash del TDA hash */

size_t hashear(const char* clave, const size_t tamanio_tabla)
{
	size_t pos_tabla = 1;
	size_t largo = strlen(clave);
	int multiplicador = 1 ;

	for(size_t i=0; i<largo; i++)
	{
		pos_tabla += (unsigned int)((int)clave[i] * multiplicador);
		multiplicador= multiplicador * 10;		
	}

	return pos_tabla % tamanio_tabla;
}

/*
	Busca y guarda en el array todos los numeros primos menores o iguales a numero. En caso
	de que el tamanio del array sea insuficiente guarda hasta donde puede y sale.
	En cualquier caso devuelve el total de numeros primos que guardo en el arreglo.
	El 1 no se considera numero primo.
*/

size_t primos_hasta(uint* const array, const size_t tope, uint numero)
{
	if(!array) return 0;

	bool factorizable = false;
	size_t cant_primos = 0;

	for(uint i=2; i<=numero && cant_primos<tope; i++)
	{
		for(uint j=0; j<cant_primos && !factorizable; j++)
		{
			if((i % array[j] == 0) && array[j] > 1 && array[j] < numero)
			 	factorizable = true;
		}
		
		if(!factorizable)
		{
			array[cant_primos] = i;
			cant_primos++;
		}
		factorizable = false;
	}
	return cant_primos;
}

/*
	Busca el proximo primo del numero dado.
	El 1 no se considera numero primo, de pedir el proximo de 0, devolvera 2.
*/

uint proximo_primo(uint numero)
{	
	bool es_primo = false;
	bool factorizable = false;
	size_t cant_primos = 0;
	
	if(numero == 0) return 2;

	uint* ptr_array = malloc(numero*sizeof(uint));
	if(!ptr_array) return 0; 

	cant_primos = primos_hasta(ptr_array,numero,numero);

	while(!es_primo)
	{
		numero++;

		for(uint j=0; j<cant_primos && !factorizable; j++)
		{
			if(ptr_array[j] != 1 && (numero % ptr_array[j]) == 0)
				factorizable = true;
		}
		
		if(!factorizable) 
			es_primo = true;
		else 
			factorizable = false;
	}

	free(ptr_array);
	return numero;
}

/* Duplicar el string s y devuelve un puntero a la memoria reservada para el mismo*/

char* duplicar_clave(const char* s){
  if(!s)
    return NULL;
  
  char* p = malloc(strlen(s)+1);
  strcpy(p,s);
  return p;
}

/* Destructor de la tabla hash */

void destruir_tabla(lista_t** tabla_hash, size_t tamanio)
{
	for(size_t i=0; i<tamanio; i++)
		lista_destruir(tabla_hash[i]);
	
	free(tabla_hash);	
}

/* Crea una nueva tabla hash con el tamanio solicitado */

lista_t** crear_tabla(size_t tamanio)
{
	if(!tamanio) return NULL;
	bool  exito = true;

	lista_t** tabla = calloc(tamanio,sizeof(lista_t*));
	if(!tabla) return NULL;

	for(size_t i=0; i<tamanio && exito == ((tabla[i] = lista_crear()) != NULL); i++);
	
	if(exito) 
		return tabla;
	else 
		destruir_tabla(tabla,tamanio);

	return NULL;
}



hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad)
{
	size_t tamanio_hash = 0;

	if(capacidad < 3) capacidad = 3;

	tamanio_hash = proximo_primo((uint)capacidad/3);

	hash_t* hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;

	hash->tabla_hash = malloc(tamanio_hash*sizeof(lista_t*));
	if(!hash->tabla_hash)
	{
		free(hash);
		return NULL;
	}

	for(size_t i=0; i<tamanio_hash && (hash->tabla_hash[i] = lista_crear()) != NULL; i++);

	hash->tamanio_tabla = tamanio_hash;
	hash->cantidad = 0;
	hash->hash_destruir_dato_t = destruir_elemento;

	return hash;
}

bool hash_contiene(hash_t* hash, const char* clave)
{
	void* ptr_celda = NULL;
	bool encontrado = false;

	size_t pos_tabla = hashear(clave,hash->tamanio_tabla);

	lista_iterador_t* it = lista_iterador_crear(hash->tabla_hash[pos_tabla]);
	
	while(lista_iterador_tiene_siguiente(it) && !encontrado)
	{
		ptr_celda = lista_iterador_siguiente(it);
		if(strcmp(((celda_t*)ptr_celda)->clave,clave) == 0)
			encontrado = true;
	}
	lista_iterador_destruir(it);
	
	return encontrado ? true : false;	
}

int reemplazar_valor(const hash_t* hash, const size_t pos_tabla ,const char* clave ,void* valor)
{
	bool encontrado = false;
	celda_t* ptr_celda = NULL;
	lista_iterador_t* it = lista_iterador_crear(hash->tabla_hash[pos_tabla]);


	while(lista_iterador_tiene_siguiente(it) && !encontrado)
	{
		ptr_celda = lista_iterador_siguiente(it);
		if(strcmp(((celda_t*)ptr_celda)->clave,clave) == 0)
		{
			if(hash->hash_destruir_dato_t)
				hash->hash_destruir_dato_t(((celda_t*)(ptr_celda))->valor);
			ptr_celda->valor = valor;
			encontrado = true;
		}
	}
	lista_iterador_destruir(it);

	return encontrado ? 0 : -1;	
}

int rehashear(hash_t* hash)
{
	void* ptr_celda = NULL;
	bool fallo_insercion = false;
	size_t pos_nueva_tabla = 0;
	lista_iterador_t* it = NULL;
	lista_t** tabla_vieja = hash->tabla_hash;
	
	size_t tam_nueva_tabla = proximo_primo((uint)(2*hash->tamanio_tabla));
	lista_t** nueva_tabla = crear_tabla(tam_nueva_tabla);

	if(!tam_nueva_tabla or !nueva_tabla) return -1;
	
	for(size_t i=0; i < hash->tamanio_tabla && !fallo_insercion; i++)
	{
		it = lista_iterador_crear(tabla_vieja[i]);
		while(lista_iterador_tiene_siguiente(it) && !fallo_insercion)
		{
			ptr_celda = lista_iterador_siguiente(it);
			pos_nueva_tabla = hashear(((celda_t*)ptr_celda)->clave,tam_nueva_tabla);
			if((lista_insertar(nueva_tabla[pos_nueva_tabla],ptr_celda)) == -1) 
				fallo_insercion = true;
		}
		lista_iterador_destruir(it);
	}
	if(fallo_insercion)
	{
		destruir_tabla(nueva_tabla,tam_nueva_tabla);
		return -1;
	}
	destruir_tabla(tabla_vieja,hash->tamanio_tabla);
		
	hash->tamanio_tabla = tam_nueva_tabla;
	hash->tabla_hash = nueva_tabla;
	return 0;
}

int hash_insertar(hash_t* hash, const char* clave, void* elemento)
{
	if(!hash or !clave or !clave[0]) return -1;
	celda_t* ptr_celda = NULL;
	
	size_t pos_tabla = hashear(clave,hash->tamanio_tabla);

	if(hash_contiene(hash,clave))
		return reemplazar_valor(hash,pos_tabla,clave,elemento);	
	
	
	ptr_celda = calloc(1,sizeof(celda_t));
	if(!ptr_celda) return -1;

	ptr_celda->valor = elemento;
	ptr_celda->clave = duplicar_clave(clave);

	lista_insertar(hash->tabla_hash[pos_tabla],(void*)ptr_celda);
	hash->cantidad++;
	
	if(((float)hash->cantidad / (float)hash->tamanio_tabla) >= FACTOR_CARGA)
		return rehashear(hash);

	//STEAM: pankito#4503 O DISCORD?
	return 0;
}

void hash_destruir(hash_t* hash)
{
	if(!hash) return;

	celda_t* ptr_celda = NULL;
	for(size_t i=0; i<hash->tamanio_tabla; i++)
	{

		while(hash->tabla_hash[i] != NULL && !lista_vacia(hash->tabla_hash[i]))
		{
			ptr_celda = lista_ultimo(hash->tabla_hash[i]);
			if(hash->hash_destruir_dato_t)
				hash->hash_destruir_dato_t(((celda_t*)ptr_celda)->valor);
			free(((celda_t*)ptr_celda)->clave);
			free(ptr_celda);
			lista_borrar(hash->tabla_hash[i]);
		}

		lista_destruir(hash->tabla_hash[i]);
	}

	free(hash->tabla_hash);
	free(hash);
}

int hash_quitar(hash_t* hash, const char* clave)
{
	if(!hash or !clave or !clave[0]) return -1;

	celda_t* ptr_celda = NULL;
	size_t pos_lista = 0;
	bool encontrado = false;
	size_t pos_tabla = hashear(clave,hash->tamanio_tabla);

	lista_iterador_t* it = lista_iterador_crear(hash->tabla_hash[pos_tabla]);
	if(!it) return -1;

	while(lista_iterador_tiene_siguiente(it) && !encontrado)
	{
		ptr_celda = lista_iterador_siguiente(it);
		if(strcmp(((celda_t*)ptr_celda)->clave,clave) == 0)
			encontrado = true;
		else
			pos_lista++;
	}
	lista_iterador_destruir(it);

	if(!encontrado) return -1;
	
	if(hash->hash_destruir_dato_t)
		hash->hash_destruir_dato_t(((celda_t*)ptr_celda)->valor);
	
	free(((celda_t*)(ptr_celda))->clave);
	free(ptr_celda);

	hash->cantidad--;
	return lista_borrar_de_posicion(hash->tabla_hash[pos_tabla], pos_lista);
}


size_t hash_cantidad(hash_t* hash)
{
	return hash ? hash->cantidad : 0;
}

/*
	VERSION DE HASH OBTENER SIN MODIFICAR LISTA.H
*/

void* hash_obtener(hash_t* hash, const char* clave)
{
	if(!hash or !clave or !clave[0]) return NULL;

	celda_t* ptr_celda = NULL;
	bool encontrado = false;
	
	size_t pos_tabla_hash = hashear(clave,hash->tamanio_tabla);
	lista_iterador_t* it = lista_iterador_crear(hash->tabla_hash[pos_tabla_hash]);
	if(!it) return NULL;

	while(lista_iterador_tiene_siguiente(it) && !encontrado)
	{
		ptr_celda = lista_iterador_siguiente(it);
		if(strcmp(((celda_t*)ptr_celda)->clave, clave) == 0 )
			encontrado = true;
	}
	lista_iterador_destruir(it);
	return encontrado ? ptr_celda->valor : NULL;
}


size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux)
{
	lista_iterador_t* it = NULL;
	void* ptr_celda = NULL;
	bool parar_recorrido = false;
	size_t claves_recorridas = 0;

	for(size_t i=0; i<hash->tamanio_tabla && !parar_recorrido; i++)
	{
		it = lista_iterador_crear(hash->tabla_hash[i]);

		while(lista_iterador_tiene_siguiente(it) && !parar_recorrido)
		{
			ptr_celda = lista_iterador_siguiente(it);
			if(funcion(hash,((celda_t*)(ptr_celda))->clave,aux))
				parar_recorrido = true;
			
			claves_recorridas++;
		}
		lista_iterador_destruir(it);
	}

	return claves_recorridas;
}


/*
	*************************IMPLEMENTACION DEL ITERADOR EXTERNO
*/
hash_iterador_t* hash_iterador_crear(hash_t* hash)
{

	if(!hash or hash->cantidad == 0) return NULL;

	hash_iterador_t* iter_hash = malloc(sizeof(hash_iterador_t));
	if(!iter_hash) return NULL;
	
	size_t i;
	for(i = 0; i<hash->tamanio_tabla && lista_vacia(hash->tabla_hash[i]); i++);

	lista_iterador_t* iter_lista = lista_iterador_crear(hash->tabla_hash[i]);
	if(!iter_lista)
	{
		free(iter_hash);
		return NULL;
	} 
	iter_hash->max_tabla = hash->tamanio_tabla;
	iter_hash->pos_tabla = i;
	iter_hash->it_lista = iter_lista;
	iter_hash->tabla_hash = hash->tabla_hash;

	return iter_hash;
}

const char* hash_iterador_siguiente(hash_iterador_t* iterador)
{
	if(!iterador) return NULL;
	void* ptr_celda = NULL;

	if(lista_iterador_tiene_siguiente(iterador->it_lista))
	{
		ptr_celda = lista_iterador_siguiente(iterador->it_lista);
		return ((celda_t*)ptr_celda)->clave;
	}

	if(iterador->pos_tabla == iterador->max_tabla - 1)
		return NULL;

	lista_iterador_destruir(iterador->it_lista);
	iterador->it_lista = NULL;

	size_t i;
	for(i = iterador->pos_tabla+1; i<iterador->max_tabla && lista_vacia(iterador->tabla_hash[i]); i++);
	
	iterador->pos_tabla = i;
	
	if(i == iterador->max_tabla) return NULL;
	

	iterador->it_lista = lista_iterador_crear(iterador->tabla_hash[i]);
	if(!iterador->it_lista) return NULL;

	return hash_iterador_siguiente(iterador);
}

bool hay_lista(hash_iterador_t* it)
{
	size_t i;
	for(i=it->pos_tabla+1; i<it->max_tabla && lista_vacia(it->tabla_hash[i]); i++);

	return (i < it->max_tabla);
}

bool hash_iterador_tiene_siguiente(hash_iterador_t* iterador)
{
	if(!lista_iterador_tiene_siguiente(iterador->it_lista) && !hay_lista(iterador))
		return false;

	return true;
}

void hash_iterador_destruir(hash_iterador_t* iterador)
{	
	if(!iterador) return;

	lista_iterador_destruir(iterador->it_lista);
	free(iterador);
}


/*
 -*****************VERSION DE HASH OBTENER CON LISTA MODIFICADA

void* hash_obtener(hash_t* hash, const char* clave)
{
	if(!hash or !clave or !clave[0]) return NULL;

	celda_t* ptr_celda = NULL;
	size_t pos_tabla_hash = hashear(clave,hash->tamanio_tabla);

	ptr_celda = lista_buscar(hash->tabla_hash[pos_tabla_hash],&comparador_hash,(void*)clave);

	return ptr_celda ? ptr_celda->valor : NULL;
}


bool comparador_hash(void* celda, void* clave)
{
	if(strcmp(((celda_t*)celda)->clave,(char*)clave) == 0) return true;

	return false;
}


void desplegar_tablahash(hash_t* hash)
{
	celda_t* ptr_celda = NULL;
	lista_iterador_t* it = NULL;
	for(size_t i=0; i<hash->tamanio_tabla; i++)
	{
		printf("Tabla en %li contiene %li elemenetos \n",i,lista_elementos(hash->tabla_hash[i]));
	}

	for(int i=0; i<hash->tamanio_tabla; i++)
	{
		it = lista_iterador_crear(hash->tabla_hash[i]);
		while(lista_iterador_tiene_siguiente(it))
		{
			ptr_celda = lista_iterador_siguiente(it);
			printf("Clave: %s, Valor: %s \n",((celda_t*)ptr_celda)->clave,(char*)(((celda_t*)ptr_celda)->valor));
		}
		lista_iterador_destruir(it);
	}
}
*/
