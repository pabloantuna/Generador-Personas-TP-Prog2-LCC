// Trabajo Practico N°2 - Programacion II - Primer Año - LCC
// Integrantes del grupo: Espindola, Ignacio Lionel - Antuña, Pablo
// Profesores: Guimpel - Fernandez - Zimmermann - Bini - Locascio
// Este programa tiene la funcionalidad de:
//          - Recibir tres archivos (uno con ciudades y codigos; uno con datos de personas y uno de salida)
//          - Con estos dos primeros archivos, genera otro con la informacion combinada de ambos
//          - Este archivo de salida esta pensado para ser recibido por el TP N°1 de la catedra, el cual se encarga de
//            dado un archivo con personas, generar el maximo posible de parejas
// Tuvimos dos enfoques para realizar esta tarea:
//  - La primera, es la que se encuentra en este codigo, y funciona asi: Primero genera una cantidad (ingresada por el
//    usuario) de numeros aleatorios, y luego trabaja solo con esas lineas del archivo de entrada de personas
//  - La segunda constaba de generar una estructura con todas las personas y sus respectivos datos, y aunque este
//    metodo dio resultado, notamos que esta implementacion utilizaba mucha mas memoria sin optimizar los tiempos. 
//    En archivos de entrada grandes (rondando el millon de personas), los tiempos de esta alternativa nos resultaron
//    inaceptables, y alli fue que nos decidimos por utilizar la primer version. 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//DEFINICION DE CONSTANTES
#define MAX_CIUDAD 70
#define MAX_NOMBRE 50
#define MAX_LINEA 170
#define MAX_DIGITOS_EDAD 3
#define ASCII_RETORNO_CARRO 13
//Son los tamanos maximos que pueden tomar los strings de ciudad, nombre (y apellido), edad y el total de la linea.
//Pueden modificarse manualmente si el archivo de entrada asi lo necesitase

//cantidadLineas: FILE* -> int
//Esta funcion recibe un puntero a un Archivo y se encarga de leer la cantidad de lineas de este
//Lo realiza contando la cantidad de veces que aparece el caracter '\n'
int cantidadLineas(FILE *file){
    int cantLineas = 0;
    char aux[MAX_LINEA];
    while(!feof(file)){
        fscanf(file, "%[^\n]", aux);
        if(fgetc(file) == '\n') cantLineas++;           //Esta linea esta para que sea consistente leyendo tanto
    }                                                   //archivos que terminan con y sin una linea vacia
    return cantLineas;
}

//borrarEspacios: char[]
//Esta funcion consume un arreglo de caracteres y se encarga de "limpiar" los espacios que se encuentran al final
void borrarEspacios(char nombreCiudad[]){
    int indice = 0, indice2 = 0, largoNombre = strlen(nombreCiudad);
    char *auxCiudad = malloc(sizeof(char)*largoNombre);
    while (indice < largoNombre-1) {                                   
        if (nombreCiudad[indice] != ' ') {                          // si el caracter es distinto de espacio entonces simplemente lo guarda
                                                                    // en una variable temporal
            auxCiudad[indice2] = nombreCiudad[indice];
            indice2++;
        }
        else if(nombreCiudad[indice+1] != ' '){                     // si el caracter es un espacio lo guarda en el temporal solo si
                                                                    // el caracter siguiente no es un espacio (evita guardar espacios consecutivos)
            auxCiudad[indice2] = ' ';
            indice2++;
        }
        indice++;
    }
    if(nombreCiudad[indice] != ' ' && nombreCiudad[indice] != ASCII_RETORNO_CARRO){     // en el loop no analiza el caso de que el ultimo caracter no sea un espacio
                                                                                        // asi que lo revisamos fuera del mismo
                                                                                        // comparamos con ASCII_RETORNO_CARRO ya que en linux presentaba problemas con el salto de linea
        auxCiudad[indice2] = nombreCiudad[indice];
        indice2++;
    }
    auxCiudad[indice2] = '\0';                                      // agrega el terminador a la variable temporal
    strcpy(nombreCiudad, auxCiudad);                                // copia la cadena a la variable original
    free(auxCiudad);                                                // libera la memoria pedida para la variable temporal
}

//armarCiudades: FILE* -> int -> char[][]
//armarCiudades toma un archivo, la cantidad de ciudades (que se obtuvo de leer la cantidad de lineas del archivo), 
//y una estructura bidimensional donde se guardarán por referencia todas las ciudades
void armarCiudades(FILE *ciudadFile, int cantCiudad, char nombreCiudad[][MAX_CIUDAD]){
    char aux[MAX_CIUDAD];
    int indice = 0;
    while (indice < cantCiudad) {
        fscanf(ciudadFile, "%[^,],", aux);                  //Avanza hasta la coma
        fscanf(ciudadFile, "%[^\n]\n", nombreCiudad[indice]); //Y guarda lo que se encuentre entre la coma y '\n'
        borrarEspacios(nombreCiudad[indice]);               //Sin los espacios que sobran
        indice++;
    }
}

//obtenerInteres: char -> char
//Esta funcion recibe un caracter ('1', '2', '3' o '4') y devuelve un caracter ('F', 'M', 'A' O 'N')
//que corresponde al genero de interes de la persona (Femenino, Masculino, Ambos, Ninguno)
char obtenerInteres(char generoInteres){
    char valor;
    switch (generoInteres)
    {
    case '1':
        valor = 'F';
        break;
    case '2':
        valor = 'M';
        break;
    case '3':
        valor = 'A';
        break;
    case '4':
        valor = 'N';
        break;
    }
    return valor;
}

//inicializaNumeros: int[] -> int
//Esta funcion simplemente inicializa un arreglo de numeros con el valor de su indice
//Es decir, en la posicion n guarda el numero n
void inicializarNumeros(int numeros[], int largo){
    int indice;
    for(indice = 0; indice < largo; indice++) numeros[indice] = indice;
}

//swap: int* -> int*
//La funcion swap toma dos direcciones de memoria que apuntan a numeros enteros y los cambia de posicion entre si
void swap(int *primerNumero, int *segundoNumero){
    int aux = *primerNumero;
    *primerNumero = *segundoNumero;
    *segundoNumero = aux;
}

//escribirLinea: FILE* -> char[][] -> FILE*
//Esta funcion toma el archivo de entrada de personas y de salida, ademas de la estructura donde estan guardadas las
//ciudades, y escribe en el archivo de salida las lineas ya formateadas
void escribirLinea(FILE *personasFile, char ciudades[][MAX_CIUDAD], FILE *archivoSalida){
    char auxCiudad[MAX_CIUDAD], auxGenero, auxGeneroInteres, auxNombre[MAX_NOMBRE], auxApellido[MAX_NOMBRE], auxEdad[MAX_DIGITOS_EDAD];
    //Lee cada linea separando por comas y las guarda en variables auxiliares
    fscanf(personasFile, "%[^,], %[^,], %[^,], %[^,]", auxNombre, auxApellido, auxCiudad, auxEdad);
    auxGenero = fgetc(personasFile);
    auxGeneroInteres = fgetc(personasFile);
    fgetc(personasFile);                                // Leo el \n final para no guardarlo en la proxima lectura
    if(atoi(auxCiudad) == 0) strcpy(auxCiudad, "1");    //Esta linea existe solo porque en uno de los archivos de ejemplo
    strcpy(auxCiudad,ciudades[atoi(auxCiudad) - 1]);    //habia personas con codigo de ciudad 0, y no existe tal ciudad
    if(auxGenero == '1') auxGenero = 'M';
    else auxGenero = 'F';                               //Aqui reemplaza el numero que determina el genero por una letra
    auxGeneroInteres = obtenerInteres(auxGeneroInteres);//Obtiene el genero de interes

    fprintf(archivoSalida, "%s,%s,%s,%s,%c,%c\n",
        auxNombre,
        auxApellido,
        auxCiudad,
        auxEdad,
        auxGenero,
        auxGeneroInteres);                              //Y luego de hacer todos los cambios pertinentes,
}                                                       //escribe todo en el archivo

// comparaNumeros: void const * -> void const * -> int
// esta funcion es utilizada por qsort para definir la comparacion entre
// los elementos de la lista a ordenar, nos guiamos con el manual de C subido al campus
// para la realizacion de la misma. De esta forma se ordena de forma ascendente
int comparaNumeros(void const *i, void const *j) {
    int *a, *b;
    a = (int *) i;
    b = (int *) j;
    return *a - *b;
}

//armarSalida: int -> FILE* -> FILE* -> int -> char [][]
//Esta funcion genera el array de nueros random que serán las posiciones de las personas que se tomaran para el archivo
//de salida, luego los ordena, y por ultimo escribe en el archivo las lineas con los indices obtenidos al azar
void armarSalida(int cantidadPersonasAElegir, FILE *archivoSalida, FILE *archivoPersonas, int cantidadPersonasTotal, char ciudades[][MAX_CIUDAD]){
    int indice, valorRandom, limite = cantidadPersonasTotal, *resultados = malloc(sizeof(int) *cantidadPersonasAElegir), *numeros = malloc(sizeof(int)*cantidadPersonasTotal), cantPersonasElegidas = 0;
    char aux[MAX_LINEA];
    inicializarNumeros(numeros, cantidadPersonasTotal);         //Genera array [1,2,3,...,n] de largo n
    srand(time(NULL));
    for(indice = 0; indice < cantidadPersonasAElegir; indice++){
        valorRandom = rand()%limite;                            //Obtiene un valor random, ingresa al array en esa posicion
        resultados[indice] = numeros[valorRandom];              //Lo guarda, lo cambia de posicion con el ultimo numero
        swap(&numeros[valorRandom], &numeros[limite-1]);        //aun no utilizado, y luego decrece el rango del rand
        limite--;                                               //De esta forma, nunca puede obtener dos veces un mismo numero
    }
    qsort(resultados, cantidadPersonasAElegir, sizeof(int), &comparaNumeros);   //Ordena los numeros random para solo
                                                                                //recorrer una vez el archivo
    for(indice = 0; indice < cantidadPersonasTotal && cantPersonasElegidas < cantidadPersonasAElegir; indice++){
        if(resultados[cantPersonasElegidas] == indice){         //Si el numero de linea se encuentra en los numeros random
            escribirLinea(archivoPersonas, ciudades, archivoSalida); //Formateo y escribo esa linea en el archivo de salida
            cantPersonasElegidas++;                             //Y avanzo en el array de numeros random
        }
        fscanf(archivoPersonas, "%[^\n]\n", aux);               //Luego, habiendo escrito o no, avanzo consumiendo el '\n'
    }
    free(resultados);                                           //Y libero la memoria utilizada para punteros
    free(numeros);
}


int main(int argc, char *argv[]) {
    if(argc != 4) {                             //Chequeo que se ingresen todos los argumentos (nombres de archivos)
        printf("ERROR! Ingrese nombre de: ARCHIVO CIUDADES; ARCHIVO DE PERSONAS y ARCHIVO DE SALIDA\n");
        return -1;
    }
    int cant;

    FILE *ciudades, *personasFile, *archivoSalida;
    ciudades = fopen(argv[1],"r");              //Abro el archivo de entrada de ciudades
    personasFile = fopen(argv[2],"r");          //Abro el archivo de entrada de personas
    archivoSalida = fopen(argv[3],"w+");        //Abro el archivo de salida
    if (ciudades == NULL || personasFile == NULL) {     //Chequeo haber podido leer los archivos
        printf("No se encontraron los archivos.\n");
        return -1;
    }
    int cantidadCiudades = cantidadLineas(ciudades);    //Leo el archivo contando las lineas de ciudades
    rewind(ciudades);                                   //Vuelvo al inicio del archivo
    char nombreCiudad[cantidadCiudades][MAX_CIUDAD];
    armarCiudades(ciudades, cantidadCiudades, nombreCiudad);    //Genero la estructura de Ciudades
    fclose(ciudades);

    int cantPersonas = cantidadLineas(personasFile), bandera = 0;   //Leo la cantidad de lineas del archivo de personas
    rewind(personasFile);                                           //Vuelvo al comienzo del archivo
    while (bandera == 0) {
        printf("Ingrese cuantas personas quiere que tenga su archivo de salida: \n");   //Ingreso la cantidad de personas
        fflush(stdin);                                                                  //Que quiero en la salida
        scanf("%d", &cant);
        if (cantPersonas < cant){                                   //Chequeo de que no sean mas que las lineas del archivo
            printf("No puede ingresar una cantidad mayor a la cantidad de personas.\n");
        } else {
            bandera = 1;
            armarSalida(cant, archivoSalida, personasFile, cantPersonas, nombreCiudad);
        }
    }
    fclose(personasFile);                               //Cierro los archivos
    fclose(archivoSalida);
    return 0;
}