#ifndef __NODO_H__
#define __NODO_H__

#include <stdbool.h>


typedef struct nodo
{
	void* dato;
	struct nodo* siguiente;

}nodo_t;

/*
	pre: -
	pos: Se reservo, si fue posible, memoria para un nodo nuevo y esta se
		 inicializo en 0. En caso de no poder reservar memoria devuelve NULL

*/
nodo_t* crear_nodo();

/*
	pre: El puntero a nodo actual existe y es valido para acceder a su contenido.
	pos: Devuelve un puntero a su nodo siguiente, sea nulo o no.
*/
nodo_t* tomar_siguiente(nodo_t* actual);

/*
	pre: El puntero a nodo actual existe y es valido para acceder a su contenido.
	pos: Asigna al nodo actual un siguiente, sea este un puntero nulo o una
		 direccion de memoria concreta.
*/
void asignar_sig(nodo_t* actual, nodo_t* nodo_siguiente);

/*
	pre: El puntero a nodo existe y es valido para acceder a su contenido
	pos: Devuelve un puntero a su nodo siguiente, sea nulo o no.
*/
void* tomar_dato(nodo_t* nodo);


#endif /* __NODO_H__ */