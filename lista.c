#include "lista.h"
#include "nodo.h"
#include <stdlib.h>
#include <stdio.h>

#define ERROR -1
#define EXITO 0


struct lista_iterador
{
	nodo_t* actual;

};

struct lista
{
	nodo_t* primero;
	nodo_t* ultimo;
	size_t cantidad_elementos;

};

/*  Funcion interna de la lista para obtener un nodo en una posicion dada.
	Supone que la lista existe y es valida, asi como tambien la posicion.
*/
nodo_t* lista_avanzar_hasta(lista_t* lista, size_t pos_final)
{
	nodo_t* actual = lista->primero;
	for(size_t i=0; i< pos_final; i++)
	{
		actual = tomar_siguiente(actual);
	}
	return actual;
}

size_t lista_elementos(lista_t* lista)
{
	if(lista == NULL)
		return 0;

	return lista->cantidad_elementos;
}

bool lista_vacia(lista_t* lista)
{
	if(lista == NULL) return true;	
	
	return (lista->cantidad_elementos == 0);
}

void lista_destruir(lista_t* lista)
{
	if(lista == NULL) return;
	nodo_t* actual = lista->primero;
	nodo_t* siguiente;

	while(actual != NULL)
	{
		siguiente = tomar_siguiente(actual);
		free(actual);
		actual = siguiente;
	}
	free(lista);
}

lista_t* lista_crear()
{

	return calloc(1,sizeof(lista_t));
}

int lista_borrar(lista_t* lista)
{
	if(lista_vacia(lista)) return ERROR;

	nodo_t* actual = lista->primero;

	if(lista_elementos(lista) == 1)
	{
		free(lista->ultimo);
		lista->ultimo = NULL;
		lista->primero = NULL;
		lista->cantidad_elementos = 0;
	}
	else
	{
		actual = lista_avanzar_hasta(lista,lista_elementos(lista)-2);

		free(lista->ultimo);

		lista->ultimo = actual;
		asignar_sig(actual,NULL);
		lista->cantidad_elementos--;
	}
	return EXITO;
}

int lista_insertar(lista_t* lista, void* elemento)
{
	if(lista == NULL) return ERROR;

	nodo_t* nuevo = crear_nodo(elemento);
	if(nuevo  == NULL) return ERROR;

	if(lista_vacia(lista))
		lista->primero = nuevo;
	else
		asignar_sig(lista->ultimo,nuevo);
		
	lista->ultimo = nuevo;
	lista->cantidad_elementos++;

	return EXITO;
}

void* lista_ultimo(lista_t* lista)
{
	if(lista_vacia(lista)) return NULL;
	return tomar_dato(lista->ultimo);
}

void* lista_primero(lista_t* lista)
{
	if(lista_vacia(lista)) return NULL;
	return tomar_dato(lista->primero);
}

int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion)
{
	if(lista == NULL) return ERROR;
	
	if(posicion >= lista_elementos(lista))
		return lista_insertar(lista,elemento);

	nodo_t* nuevo = crear_nodo(elemento);
	if(nuevo == NULL) return ERROR;

	if(posicion == 0)
	{
		asignar_sig(nuevo,lista->primero);
		lista->primero = nuevo;
	}
	else
	{
		nodo_t* actual = lista_avanzar_hasta(lista,posicion-1);
		asignar_sig(nuevo,tomar_siguiente(actual));
		asignar_sig(actual,nuevo);

	}
	lista->cantidad_elementos++;
	return EXITO;

}


lista_iterador_t* lista_iterador_crear(lista_t* lista)
{
	if(lista_vacia(lista)) return NULL;
	
	lista_iterador_t* nuevo = calloc(1,sizeof(lista_iterador_t*));
	if(nuevo == NULL) return NULL;
	
	nuevo->actual = lista->primero;
	return nuevo;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador)
{
	if(iterador == NULL) 
		return false;
	else
		return iterador->actual != NULL;
}

void* lista_iterador_siguiente(lista_iterador_t* iterador)
{
	if(iterador == NULL) return NULL;
	void* dato = NULL;

	if(lista_iterador_tiene_siguiente(iterador))
	{
		dato = tomar_dato(iterador->actual);
		iterador->actual = tomar_siguiente(iterador->actual);
	}
	
	return dato;
}

void lista_iterador_destruir(lista_iterador_t* iterador)
{
	free(iterador);
}

void* lista_elemento_en_posicion(lista_t* lista, size_t posicion)
{
	if(lista_vacia(lista) || posicion >= lista_elementos(lista))
		return NULL;

	nodo_t* actual = lista_avanzar_hasta(lista,posicion);

	return tomar_dato(actual);
}

int lista_borrar_de_posicion(lista_t* lista, size_t posicion)
{
	if(lista_vacia(lista)) return ERROR;

	if(lista_elementos(lista) == 1 || posicion >= lista_elementos(lista) - 1)
		return lista_borrar(lista);

	if(posicion == 0)
	{
		nodo_t* aux = tomar_siguiente(lista->primero);
		free(lista->primero);
		lista->primero = aux;
	}
	else
	{
		nodo_t* anterior_pos = lista_avanzar_hasta(lista,posicion-1);
		nodo_t* siguiente_pos = tomar_siguiente(tomar_siguiente(anterior_pos));	
		free(tomar_siguiente(anterior_pos));
		asignar_sig(anterior_pos,siguiente_pos);
	}
	lista->cantidad_elementos--;

	return EXITO;
}

int lista_apilar(lista_t* lista, void* elemento)
{
	return lista_insertar(lista,elemento);
}

int lista_desapilar(lista_t* lista)
{
	return lista_borrar(lista);
}

void* lista_tope(lista_t* lista)
{
	if(lista_vacia(lista )) return NULL;

	return tomar_dato(lista->ultimo);
}

int lista_encolar(lista_t* lista, void* elemento)
{
	return lista_insertar(lista,elemento);
}

int lista_desencolar(lista_t* lista)
{
	return lista_borrar_de_posicion(lista,0);
}

void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*, void*), void *contexto)
{
	if(lista_vacia(lista)) return;

	nodo_t* actual = lista->primero;

	for(size_t i=0; i<lista_elementos(lista); i++)
	{
		funcion((void*)tomar_dato(actual),contexto);
		actual = tomar_siguiente(actual);
	}
}

void* lista_buscar(lista_t* lista, bool (*funcion)(void*, void*), void* contexto)
{
	if(!lista || !funcion) return NULL;

	nodo_t* actual = lista->primero;
	bool encontrado = false;

	for(size_t i=0; i<lista_elementos(lista) && !encontrado; i++)
	{
		encontrado = funcion((void*)tomar_dato(actual),contexto);
		if(!encontrado)
			actual = tomar_siguiente(actual);
		
	}
	return encontrado ? tomar_dato(actual) : NULL;
}

/* 
*********************  IMPLEMENTACION DE LAS PRIMITIVAS DEL NODO



*/
void asignar_dato(nodo_t* nodo,void* dato)
{
	nodo->dato = dato;
}

nodo_t* crear_nodo(void* elemento)
{	
	nodo_t* nuevo;
	nuevo = calloc(1, sizeof(nodo_t));
	if(nuevo)
		asignar_dato(nuevo,elemento);
	
	return nuevo;
}

nodo_t* tomar_siguiente(nodo_t* actual)
{
	return actual->siguiente;
}

void asignar_sig(nodo_t* actual, nodo_t* nodo_siguiente)
{
	actual->siguiente = nodo_siguiente;
}

void* tomar_dato(nodo_t* nodo)
{

	return nodo->dato;
}
