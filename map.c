 
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
 int *datos;
 
void leerDatos(int indiceInicio, int indiceFin){
    FILE *stream;
    char fileName[] = "datos.csv";
    stream = fopen(fileName, "r");
    int ch;
    int indice = 0;

    while(indice < indiceInicio){
        ch = fgetc(stream);
        if(ch == '\n'){
            indice++;
        }
    }

    int auxiliarDato = 0;
    while(indice < indiceFin){
        ch = fgetc(stream);
        if(ch >= 0x30 && ch <= 0x39){
            auxiliarDato = auxiliarDato * 10 + (ch - 0x30);
        }
        else if(ch == '\n'){
            datos[indice - indiceInicio] = auxiliarDato;
            indice++;
            auxiliarDato = 0;
        }
    }
    
    fclose(stream);
}
 
 
int main(int argc, char *argv[])
{
    // Proceso 0: Master
    // Proceso 1-9: Reduce
    //int datos[]={5,15,12,23,42,81,17,60,1,1200,5000,8, 0};
    
    int map[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int rangoDeDatosAEvaluar[2];
    int numeroProcesos;
    int idProceso;
    int zero = 0;

    MPI_Status status; 
    
    MPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numeroProcesos); 
    MPI_Comm_rank(MPI_COMM_WORLD,&idProceso); 
    
    
    // Tag 0: No iniciar map
    // Tag 1: Iniciar Map
    // Recibir en el buffer posición de inicio de datos (incluido) y posición de fin de datos (no incluido)
    MPI_Recv(rangoDeDatosAEvaluar, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
     //printf("\n Map de id %i recibio tag %i\n", idProceso, status.MPI_TAG);
    if(status.MPI_TAG == 1){
        //printf("\n Map de id %i evalua datos desde %i (inclusive) a %i \n", idProceso, rangoDeDatosAEvaluar[0], rangoDeDatosAEvaluar[1]);
        datos = calloc(rangoDeDatosAEvaluar[1] - rangoDeDatosAEvaluar[0], sizeof(int));
        leerDatos(rangoDeDatosAEvaluar[0], rangoDeDatosAEvaluar[1]);     
        
        for(int i = 0; i < (rangoDeDatosAEvaluar[1] - rangoDeDatosAEvaluar[0]); i++){
            int datoEnEvaluacion = datos[i];
            int digitoEnEvaluacion;
            while(datoEnEvaluacion != 0){
                digitoEnEvaluacion = datoEnEvaluacion%10;
                datoEnEvaluacion = datoEnEvaluacion/10;
            }
            map[digitoEnEvaluacion]++;
        }
		free(datos);
        for(int i = 1; i < 10; i++){
           // printf("\n Número de datos que empiezan por %i: %i \n", i,  map[i]);
            MPI_Reduce(&(map[i]), &(map[i]), 1, MPI_INT, MPI_SUM, i, MPI_COMM_WORLD);

        }
    }
    
    else{
         for(int i = 1; i < 10; i++){
            MPI_Reduce(&zero, &zero, 1, MPI_INT, MPI_SUM, i, MPI_COMM_WORLD);

        }
    }
    
    MPI_Finalize();
    return 0;
}
