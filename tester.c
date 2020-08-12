#include "tester.h"
#include <stdio.h>
#include <stdlib.h>


tester_t* crear_test()
{
	return calloc(1,sizeof(tester_t));
}

void destruir_tester(tester_t* tester)
{
	free(tester);
}

void mostrar_resultados(tester_t* tester)
{
	printf("\n");
	printf("\n");
	printf(TEXTO_DESTACADO"Pruebas hechas: %li \n"TEXTO_RESET,tester->pruebas_hechas);
	printf(TEXTO_DESTACADO"Pruebas fallidas: %li \n"TEXTO_RESET,tester->pruebas_fallidas);

}

void afirmar(tester_t* tester,const char* enunciado, bool resultado)
{
    printf(resultado?TEXTO_DESTACADO"%s:"COLOR_EXITO"PASO \n"TEXTO_RESET:COLOR_ERROR"%s FALLO \n"TEXTO_RESET,enunciado);
    tester->pruebas_hechas++;
    
    if(!resultado)
    	tester->pruebas_fallidas++;
}

void evento(const char* evento_destacado)
{
	printf(TEXTO_DESTACADO COLOR_EVENTO_DESTACADO"%s \n"TEXTO_RESET,evento_destacado);
}
