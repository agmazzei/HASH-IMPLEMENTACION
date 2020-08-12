#define COLOR_ERROR "\x1b[0;31m"
#define COLOR_EXITO "\x1b[4;32m"
#define COLOR_EVENTO_DESTACADO "\x1b[33m"
#define TEXTO_RESET "\x1b[0m"
#define TEXTO_DESTACADO "\x1b[1m"

#include <stdlib.h>
#include <stdbool.h>

typedef struct test
{
	size_t pruebas_fallidas;
	size_t pruebas_hechas;
	
}tester_t; 

/*
	pre: -
	pos: Se obtiene memoria suficiente para un tester y se retorna un puntero
		 a esta.
*/
tester_t* crear_test();

/*
	pre: Existe un tester y es valido.
	pos: Se libera toda utilizada por el tester
*/
void destruir_tester(tester_t* tester);

/*
	pre: -
	pos: Muestra en pantalla un texto con cierto formato
*/
void evento(const char* evento_destacado);

/*
	pre: Existe un tester valido
	pos: Modifica el tester(pruebas hechas y fallidas) en funcion de si la
		 afirmacion hecha se cumple o no.
*/
void afirmar(tester_t* tester,const char* enunciado, bool resultado);

/*
	pre: Existe un tester y es valido.
	pos: Muestra en pantalla los datos actuales del tester: pruebas hechas y fallidas.
*/
void mostrar_resultados(tester_t* tester);


