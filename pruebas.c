#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hash.h"
#include "hash_iterador.h"
#include "tester.h"

/*
  Duplica un string devolviendo un puntero a la copia
*/

char* duplicar_string(const char* s);


/*Estructura de prueba para facilitar la creacion de elementos a insertar y eliminar del 
  hash. */

typedef struct clave_valor
{
  char* clave;
  char* valor;
}clave_valor_t;

/*Destructor de prueba  */
void hash_destruir_dato(void* algo);

/*Funcion de prueba para el iterador interno  */
bool funcion_con_cada_clave(hash_t* hash, const char* clave, void* aux);

/*Comprueba que los elementos en el arreglo clave_valor esten en el hash  */
bool elementos_estan(hash_t* hash,clave_valor_t* clave_valores,size_t tam);



void hash_obtener_con_elemento_que_esta_lo_devuelve(tester_t* tester);

/*Pruebas hash_insertar*/
void hash_insertar_varios_elementos_con_algunas_claves_repetidas_y_estas_son_actualizadas(tester_t* tester);
void hash_insertar_en_hash_con_tamanio_suficiente(tester_t* tester);
void hash_insertar_provoca_un_rehash_y_este_genera_una_nueva_tabla_correcta(tester_t* tester);

/*Pruebas hash_quitar*/
void hash_quitar_de_hash_con_1_elemento(tester_t* tester);
void hash_quitar_con_todos_los_elementos_del_hash(tester_t* tester);


/*Prueba iterador interno*/
void hash_con_cada_clave_recorre_todas_las_claves_hasta_encontrar_valor_asociado_buscado_y_guarda_su_clave(tester_t* tester);

/*Prueba iterador externo*/
void hash_iterador_externo_recorre_todas_las_claves(tester_t* tester);

/*Prueba de volumen e integral */
void hash_pruebas_de_volumen_integrales(tester_t* tester);


int main()
{                  
	tester_t* tester = crear_test();
  hash_obtener_con_elemento_que_esta_lo_devuelve(tester);
  hash_insertar_en_hash_con_tamanio_suficiente(tester);
	hash_insertar_varios_elementos_con_algunas_claves_repetidas_y_estas_son_actualizadas(tester);
	hash_quitar_de_hash_con_1_elemento(tester);
  hash_quitar_con_todos_los_elementos_del_hash(tester);
 hash_insertar_provoca_un_rehash_y_este_genera_una_nueva_tabla_correcta(tester);
  hash_con_cada_clave_recorre_todas_las_claves_hasta_encontrar_valor_asociado_buscado_y_guarda_su_clave(tester);
  hash_iterador_externo_recorre_todas_las_claves(tester);
  hash_pruebas_de_volumen_integrales(tester);

  mostrar_resultados(tester);
  destruir_tester(tester);
	return 0;
}


void hash_iterador_externo_recorre_todas_las_claves(tester_t* tester)
{

  evento("Prueba sobre iterador externo ");
  hash_iterador_t* it_hash = NULL;
  const char* clave = NULL;
  size_t cant_claves = 0;
  hash_t* hash = hash_crear(hash_destruir_dato,5);

  clave_valor_t clave_valor[10] = {{"PAL3SDZ","Auto0"},
                                  {"ADSPQWJ","Auto1"},
                                  {"5K1SGXC","Auto2"},
                                  {"X01K4XS","Auto3"},
                                  {"ZPQOE53","Auto4"},
                                  {"FPPGT4S","Auto5"},
                                  {"XDQ210C","Auto6"},
                                  {"AWPMFV7Z","Auto7"},
                                  {"CR78QECE","Auto8"},
                                  {"DZ4RLL3","Auto9"}};
  
  for(size_t i=0; i<10; i++)
  {
    hash_insertar(hash, clave_valor[i].clave, duplicar_string(clave_valor[i].valor));
  }            
  
  it_hash = hash_iterador_crear(hash);               

  while(hash_iterador_tiene_siguiente(it_hash))
  {
    clave = hash_iterador_siguiente(it_hash);
    
    for(size_t i=0; i<10; i++)
    {
      if(strcmp(clave,clave_valor[i].clave) == 0)
        cant_claves++;
    }
  }

  afirmar(tester,"El iterador recorre todas las claves del hash",cant_claves == 10);

  hash_iterador_destruir(it_hash);
  hash_destruir(hash);
}

bool funcion_con_cada_clave(hash_t* hash, const char* clave, void* aux)
{
  void* ptr_valor = NULL;
  ptr_valor = hash_obtener(hash,clave);

  if(strcmp((char*)ptr_valor,((clave_valor_t*)aux)->valor) != 0)
    return false;

  ((clave_valor_t*)aux)->clave = duplicar_string(clave);
  return true;
}

void hash_con_cada_clave_recorre_todas_las_claves_hasta_encontrar_valor_asociado_buscado_y_guarda_su_clave(tester_t* tester)
{
  evento("Prueba con iterador interno del hash");
  hash_t* hash = hash_crear(hash_destruir_dato,10);
  clave_valor_t aux_esta = {NULL,"Auto7"};
  clave_valor_t aux_no_esta = {NULL,"VACIO"};
  clave_valor_t clave_valor[10] = {{"PAL3SDZ","Auto0"},
                                  {"ADSPQWJ","Auto1"},
                                  {"5K1SGXC","Auto2"},
                                  {"X01K4XS","Auto3"},
                                  {"ZPQOE53","Auto4"},
                                  {"FPPGT4S","Auto5"},
                                  {"XDQ210C","Auto6"},
                                  {"AWPMFV7Z","Auto7"},
                                  {"CR78QECE","Auto8"},
                                  {"DZ4RLL3","Auto9"}};
  
  for(size_t i=0; i<10; i++)
  {
    hash_insertar(hash, clave_valor[i].clave, duplicar_string(clave_valor[i].valor));
  }            
                
  evento(" ->Recorro las claves del hash hasta encontrar la que se asocia con un valor buscado");
  
  hash_con_cada_clave(hash,&funcion_con_cada_clave,(void*)&aux_esta);                 
  afirmar(tester,"  La clave asociado al valor buscado es la correcta",strcmp((char*)(hash_obtener(hash,aux_esta.clave)),"Auto7") == 0);
  free(aux_esta.clave);
  
  afirmar(tester,"  Recorro todas las claves al pedir un valor que no se asocia con ninguna clave",hash_con_cada_clave(hash,&funcion_con_cada_clave,(void*)&aux_no_esta) == 10 && !aux_no_esta.clave);

  
  hash_destruir(hash);
}

void hash_insertar_provoca_un_rehash_y_este_genera_una_nueva_tabla_correcta(tester_t* tester)
{
  evento("Creo un hash con capacidad de 4 y provoco un re-hash insertando 10");
  hash_t* hash = hash_crear(hash_destruir_dato,4);

  clave_valor_t clave_valor[10] = {{"PAL3SDZ","Auto0"},
                                  {"ADSPQWJ","Auto1"},
                                  {"5K1SGXC","Auto2"},
                                  {"X01K4XS","Auto3"},
                                  {"ZPQOE53","Auto4"},
                                  {"FPPGT4S","Auto5"},
                                  {"XDQ210C","Auto6"},
                                  {"AWPMFV7Z","Auto7"},
                                  {"CR78QECE","Auto8"},
                                  {"DZ4RLL3","Auto9"}};


  for(size_t i=0; i<10; i++)
  {
    hash_insertar(hash, clave_valor[i].clave, duplicar_string(clave_valor[i].valor));
  }
  afirmar(tester,"  -> El hash no cambio en contenido y cantidad del mismo",hash_cantidad(hash) == 10 && elementos_estan(hash,clave_valor,10));
  hash_destruir(hash);
}


void hash_quitar_de_hash_con_1_elemento(tester_t* tester)
{
    hash_t* hash = hash_crear(hash_destruir_dato,1);

    evento(" -> Creo un hash con 1 elemento");
    hash_insertar(hash,"PAL3SDZ", duplicar_string("Auto0"));

    afirmar(tester,"  -> Borro el elemento asociado a la clave del hash",hash_quitar(hash,"PAL3SDZ") == 0);
    afirmar(tester,"  -> El hash ahora tiene 0 elementos",hash_cantidad(hash) == 0);
    hash_destruir(hash);
}
void hash_quitar_con_todos_los_elementos_del_hash(tester_t* tester)
{
    hash_t* hash = hash_crear(hash_destruir_dato,10);
    bool borrado_fallido = false;

    clave_valor_t clave_valor[10] = {{"PAL3SDZ","Auto0"},
                                {"ADSPQWJ","Auto1"},
                                {"5K1SGXC","Auto2"},
                                {"X01K4XS","Auto3"},
                                {"ZPQOE53","Auto4"},
                                {"FPPGT4S","Auto5"},
                                {"XDQ210C","Auto6"},
                                {"AWPMFV7Z","Auto7"},
                                {"CR78QECE","Auto8"},
                                {"EHESTBVW","Auto9"}};


    evento(" -> Creo un hash con 10 elementos");
    for(size_t i=0; i<10; i++)
    {
      hash_insertar(hash, clave_valor[i].clave, duplicar_string(clave_valor[i].valor));
    }


    for(size_t i=0; i<10 && !borrado_fallido; i++)
    {
      if(hash_quitar(hash,clave_valor[i].clave) == -1)
        borrado_fallido = true;
    }


    afirmar(tester,"  -> Elimino todos los elementos del hash",!borrado_fallido);
    afirmar(tester,"  -> El hash queda con 0 elementos en el",hash_cantidad(hash) == 0);
    hash_destruir(hash); 
}


char* duplicar_string(const char* s)
{
  if(!s)
    return NULL;
  
  char* p = malloc(strlen(s)+1);
  strcpy(p,s);
  return p;
}

void hash_destruir_dato(void* algo)
{
  if(!algo) return;
  free(algo);
}

bool elementos_estan(hash_t* hash,clave_valor_t* clave_valores,size_t tam)
{
  void* valor_actual;
  bool ok = true;

  for(size_t i=0; i<tam && ok; i++)
  {
    valor_actual = hash_obtener(hash,clave_valores[i].clave);
    if(strcmp((char*)valor_actual,clave_valores[i].valor) != 0)
      ok = false;
  }
  return ok;
}

void hash_insertar_en_hash_con_tamanio_suficiente(tester_t* tester)
{
    hash_t* hash = hash_crear(hash_destruir_dato,7);
    clave_valor_t clave_valor[7] = {{"PAL3SDZ","Auto0"},
                                {"ADSPQWJ","Auto1"},
                                {"5K1SGXC","Auto2"},
                                {"X01K4XS","Auto3"},
                                {"ZPQOE53","Auto4"},
                                {"FPPGT4S","Auto5"},
                                {"XDQ210C","Auto6"}};


    evento(" -> inserto 7 elementos en el hash sin claves repetidas");
    hash_insertar(hash, clave_valor[0].clave, duplicar_string(clave_valor[0].valor));
    hash_insertar(hash, clave_valor[1].clave ,duplicar_string(clave_valor[1].valor));
    hash_insertar(hash, clave_valor[2].clave, duplicar_string(clave_valor[2].valor));
    hash_insertar(hash, clave_valor[3].clave, duplicar_string(clave_valor[3].valor));
    hash_insertar(hash, clave_valor[4].clave, duplicar_string(clave_valor[4].valor));
    hash_insertar(hash, clave_valor[5].clave, duplicar_string(clave_valor[5].valor));
    hash_insertar(hash, clave_valor[6].clave, duplicar_string(clave_valor[6].valor));
 

    afirmar(tester,"  -> Al terminar de insertar el hash tiene 7 elementos",hash_cantidad(hash) == 7);
    afirmar(tester,"  -> Las claves se asocian con los valores correctos",elementos_estan(hash,&clave_valor[0],7));
    hash_destruir(hash);
}

void hash_insertar_varios_elementos_con_algunas_claves_repetidas_y_estas_son_actualizadas(tester_t* tester)
{
    hash_t* hash = hash_crear(hash_destruir_dato,7);
    clave_valor_t clave_valor_a_insertar[7] = {{"PAL3SDZ","Auto0"},
                                              {"ADSPQWJ","Auto1"},
                                              {"5K1SGXC","Auto2"},
                                              {"X01K4XS","Auto3"},
                                              {"5K1SGXC","Auto2 denuevo"},
                                              {"FPPGT4S","Auto5"},
                                              {"PAL3SDZ","Auto0 denuevo"}};
    
    clave_valor_t clave_valor_al_finalizar[7] = {{"PAL3SDZ","Auto0 denuevo"},
                                                {"5K1SGXC","Auto2 denuevo"},
                                                {"ADSPQWJ","Auto1"},
                                                {"FPPGT4S","Auto5"},
                                                {"X01K4XS","Auto3"}};


    evento(" -> inserto 7 elementos en el hash con 2 claves repetidas");
    hash_insertar(hash, clave_valor_a_insertar[0].clave, duplicar_string(clave_valor_a_insertar[0].valor));
    hash_insertar(hash, clave_valor_a_insertar[1].clave ,duplicar_string(clave_valor_a_insertar[1].valor));
    hash_insertar(hash, clave_valor_a_insertar[2].clave, duplicar_string(clave_valor_a_insertar[2].valor));
    hash_insertar(hash, clave_valor_a_insertar[3].clave, duplicar_string(clave_valor_a_insertar[3].valor));
    hash_insertar(hash, clave_valor_a_insertar[4].clave, duplicar_string(clave_valor_a_insertar[4].valor));
    hash_insertar(hash, clave_valor_a_insertar[5].clave, duplicar_string(clave_valor_a_insertar[5].valor));
    hash_insertar(hash, clave_valor_a_insertar[6].clave, duplicar_string(clave_valor_a_insertar[6].valor));
 

    afirmar(tester,"  -> Al terminar de insertar el hash tiene 5 elementos",hash_cantidad(hash) == 5);
    afirmar(tester,"  -> Las claves repetidas actualizaron el valor correctamente",elementos_estan(hash,&clave_valor_al_finalizar[0],5));
    hash_destruir(hash);
}





void hash_obtener_con_elemento_que_esta_lo_devuelve(tester_t* tester)
{
    hash_t* hash = hash_crear(hash_destruir_dato,7);
    clave_valor_t clave_valor[7] = {{"PAL3SDZ","Auto0"},
                                {"ADSPQWJ","Auto1"},
                                {"5K1SGXC","Auto2"},
                                {"X01K4XS","Auto3"},
                                {"ZPQOE53","Auto4"},
                                {"FPPGT4S","Auto5"},
                                {"XDQ210C","Auto6"}};


    evento(" -> Creo un hash y le inserto 7 elementos sin claves repetidas");
    hash_insertar(hash, clave_valor[0].clave, duplicar_string(clave_valor[0].valor));
    hash_insertar(hash, clave_valor[1].clave ,duplicar_string(clave_valor[1].valor));
    hash_insertar(hash, clave_valor[2].clave, duplicar_string(clave_valor[2].valor));
    hash_insertar(hash, clave_valor[3].clave, duplicar_string(clave_valor[3].valor));
    hash_insertar(hash, clave_valor[4].clave, duplicar_string(clave_valor[4].valor));
    hash_insertar(hash, clave_valor[5].clave, duplicar_string(clave_valor[5].valor));
    hash_insertar(hash, clave_valor[6].clave, duplicar_string(clave_valor[6].valor));
 

    afirmar(tester,"  -> Al terminar de insertar el hash tiene 7 elementos",hash_cantidad(hash) == 7);
    afirmar(tester,"  -> Puedo obtener todos los elementos con la clave correcta",elementos_estan(hash,&clave_valor[0],7));
    afirmar(tester,"  -> La clave NNQO197 no esta en el hash",hash_obtener(hash,"NNQO197") == NULL);
    hash_destruir(hash);
}



void hash_pruebas_de_volumen_integrales(tester_t* tester)
{
  evento(" -> inserto 103 elementos en el hash sin con algunas claves repetidas y sin capacidad suficiente");
  hash_t* hash = hash_crear(hash_destruir_dato,8);

  hash_insertar(hash, "AC123BD",duplicar_string("Auto de Mariano")); 
  hash_insertar(hash, "OPQ976", duplicar_string("Auto de Lucas"));
    hash_insertar(hash, "A421ACB",duplicar_string("Moto de Manu"));
    hash_insertar(hash, "AA442CD",duplicar_string("Auto de Guido"));
    hash_insertar(hash, "AC152AD", duplicar_string("Auto de Agustina"));
    hash_insertar(hash, "DZE443", duplicar_string("Auto de Jonathan"));
    hash_insertar(hash, "AA436BA", duplicar_string("Auto de Gonzalo"));
    hash_insertar(hash, "A421ACB", duplicar_string("Auto de Manu denuevo"));
    hash_insertar(hash, "QDM443", duplicar_string("Auto de Daniela"));
    hash_insertar(hash, "BD123AC", duplicar_string("Auto de Pablo"));
    hash_insertar(hash, "PQO697", duplicar_string("Auto de Juan"));
    hash_insertar(hash, "DXE743", duplicar_string("Auto de Menem"));
    hash_insertar(hash, "AS416IA", duplicar_string("Auto de Carrio"));
    hash_insertar(hash, "XDQ233", duplicar_string("Auto de Lanata"));
    hash_insertar(hash, "AXQ24AC", duplicar_string("Auto de Macri"));
    hash_insertar(hash, "NK834AZ", duplicar_string("Auto de Viejaloca"));
    hash_insertar(hash, "NNQO197", duplicar_string("Auto de Providas"));
    hash_insertar(hash, "AA1A3AD",duplicar_string("Auto de ALGUIEN1"));
  hash_insertar(hash, "ODD1X0X", duplicar_string("Auto de ALGUIEN2"));
    hash_insertar(hash, "1427XXX",duplicar_string("Moto de ALGUIEN3"));
    hash_insertar(hash, "165DX42",duplicar_string("Auto de ALGUIEN4"));
    hash_insertar(hash, "HC122RR", duplicar_string("Auto de ALGUIEN5"));
    hash_insertar(hash, "DZ4RLL3", duplicar_string("Auto de ALGUIEN6"));
    hash_insertar(hash, "GG4M6BA", duplicar_string("Auto de ALGUIEN7"));
    hash_insertar(hash, "KKJMY43", duplicar_string("Auto de ALGUIEN8"));
    hash_insertar(hash, "zZ1B4AC", duplicar_string("Auto de ALGUIEN9"));
    hash_insertar(hash, "D4787AB", duplicar_string("Auto de ALGUIEN10"));
    hash_insertar(hash, "BBOVV6X", duplicar_string("Auto de ALGUIEN11"));
    hash_insertar(hash, "VXE74VV", duplicar_string("Auto de ALGUIEN12"));
    hash_insertar(hash, "AS546IZ", duplicar_string("Auto de ALGUIEN13"));
    hash_insertar(hash, "XDQ210C", duplicar_string("Auto de ALGUIEN14"));
    hash_insertar(hash, "SX2549C", duplicar_string("Auto de ALGUIEN15"));
    hash_insertar(hash, "XKA51Z", duplicar_string("Auto de ALGUIEN16"));
    hash_insertar(hash, "AKX51Z", duplicar_string("Auto de ALGUIEN17"));
    hash_insertar(hash,"SSA5AD",duplicar_string("Auto1 de ALGUIEN1")); 
  hash_insertar(hash, "OID1H1X", duplicar_string("Auto2 de ALGUIEN2"));
    hash_insertar(hash, "61HVZFC",duplicar_string("Moto3 de ALGUIEN3"));
    hash_insertar(hash, "1AGAW4A",duplicar_string("Auto4 de ALGUIEN4"));
    hash_insertar(hash, "HHGAHAS", duplicar_string("Auto5 de ALGUIEN5"));
    hash_insertar(hash, "DHAWQ63", duplicar_string("Auto6 de ALGUIEN6"));
    hash_insertar(hash, "PAL3SDZ", duplicar_string("Auto7 de ALGUIEN7"));
    hash_insertar(hash, "ADSPQWJ", duplicar_string("Auto8 de ALGUIEN8"));
    hash_insertar(hash, "5K1SGXC", duplicar_string("Auto9 de ALGUIEN9"));
    hash_insertar(hash, "AÑDAG5S", duplicar_string("Auto10 de ALGUIEN10"));
    hash_insertar(hash, "X01K4XS", duplicar_string("Auto11 de ALGUIEN11"));
    hash_insertar(hash, "ZPQOE53", duplicar_string("Auto12 de ALGUIEN12"));
    hash_insertar(hash, "FPPGT4S", duplicar_string("Auto13 de ALGUIEN13"));
    hash_insertar(hash, "XDQ210C", duplicar_string("Auto14 de ALGUIEN14"));
    hash_insertar(hash, "SX2549C", duplicar_string("Auto15 de ALGUIEN15"));
    hash_insertar(hash, "FA101XZ", duplicar_string("Auto16 de ALGUIEN16"));
    hash_insertar(hash, "01FFX1X", duplicar_string("Auto17 de ALGUIEN17"));
    hash_insertar(hash,"JK6B6H39",duplicar_string("Auto0"));
    hash_insertar(hash,"YZHE8VUT",duplicar_string("Auto1"));
    hash_insertar(hash,"8M9JVXNM",duplicar_string("Auto3"));
    hash_insertar(hash,"78PWQ6Z5",duplicar_string("Auto4"));
    hash_insertar(hash,"9RLH8L6B",duplicar_string("Auto5"));
    hash_insertar(hash,"ZEDYYX4X",duplicar_string("Auto6"));
    hash_insertar(hash,"3T6H4DWR",duplicar_string("Auto7"));
    hash_insertar(hash,"BAZCVUL5",duplicar_string("Auto8"));
    hash_insertar(hash,"KMXXRNCR",duplicar_string("Auto9"));
    hash_insertar(hash,"ESFATF9X",duplicar_string("Auto10"));
    hash_insertar(hash,"P8QRJDEF",duplicar_string("Auto12"));
    hash_insertar(hash,"P8QRJDEF",duplicar_string("Auto13"));
    hash_insertar(hash,"4VJLEZWH",duplicar_string("Auto14"));
    hash_insertar(hash,"B3NDXVMR",duplicar_string("Auto15"));
    hash_insertar(hash,"HTBMT7BC",duplicar_string("Auto16"));
    hash_insertar(hash,"MJZRFJRU",duplicar_string("Auto17"));
    hash_insertar(hash,"N7F8BGRC",duplicar_string("Auto18"));
    hash_insertar(hash,"KJTG2L9X",duplicar_string("Auto19"));
    hash_insertar(hash,"QGKCH7Z2",duplicar_string("Auto21"));
    hash_insertar(hash,"68DXM6YM",duplicar_string("Auto31"));
    hash_insertar(hash,"AWPMFV7Z",duplicar_string("Auto41"));
    hash_insertar(hash,"B8JU2T8E",duplicar_string("Auto51"));
    hash_insertar(hash,"CR78QECE",duplicar_string("Auto61"));
    hash_insertar(hash,"S33CBTDK",duplicar_string("Auto71"));
    hash_insertar(hash,"EHESTBVW",duplicar_string("Auto81"));
    hash_insertar(hash,"GYPZ5F7A",duplicar_string("Auto91"));
    hash_insertar(hash,"YFF3SY6Y",duplicar_string("Auto01"));
    hash_insertar(hash,"X7W983VR",duplicar_string("Auto111"));
    hash_insertar(hash,"G96G9DMQ",duplicar_string("Auto121"));
    hash_insertar(hash,"MQZPSFYC",duplicar_string("Auto131"));
    hash_insertar(hash,"N6F6CG59",duplicar_string("Auto141"));
    hash_insertar(hash,"TDPWUNPR",duplicar_string("Auto151"));
    hash_insertar(hash,"NQGYW97Q",duplicar_string("Auto1512"));
    hash_insertar(hash,"REL6FN8P",duplicar_string("Auto15121"));
    hash_insertar(hash,"WPVEBUYG",duplicar_string("Auto99"));
    hash_insertar(hash,"3EQR5RXJ",duplicar_string("Auto1251"));
    hash_insertar(hash,"BPQE3HKK",duplicar_string("Auto999 "));
    hash_insertar(hash,"YRMB3P3V",duplicar_string("Auto1562"));
    hash_insertar(hash,"GEVQZ423",duplicar_string("Auto9999"));
    hash_insertar(hash,"P2EK57UT",duplicar_string("Auto151"));
    hash_insertar(hash,"MBFMGV3Y",duplicar_string("Auto99999"));
    hash_insertar(hash,"YH8M7BLA",duplicar_string("Auto1511"));
    hash_insertar(hash,"V33L3GRC",duplicar_string("Auto15221"));
    hash_insertar(hash,"VKAMLL3T",duplicar_string("Auto15651"));
    hash_insertar(hash,"7FA7XGF8",duplicar_string("Auto1571"));
    hash_insertar(hash,"U7F64N2T",duplicar_string("Auto1581"));
    hash_insertar(hash,"HRERCFA6",duplicar_string("Auto1511"));
    hash_insertar(hash,"A9QN8PJQ",duplicar_string("Auto1515"));
    hash_insertar(hash,"7VXHN63U",duplicar_string("Auto15641"));
    hash_insertar(hash,"DSNTRENU",duplicar_string("Auto151241"));
    hash_insertar(hash,"NP76M98G",duplicar_string("Auto157351"));
    hash_insertar(hash,"X85X2MJX",duplicar_string("Auto15741"));

    
  hash_destruir(hash);
}

