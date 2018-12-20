 
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
	MPI_Comm reduce_comm[9];

    MPI_Status status; 
    
    MPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numeroProcesos); 
    MPI_Comm_rank(MPI_COMM_WORLD,&idProceso); 
    
    
    // Tag 0: No iniciar map
    // Tag 1: Iniciar Map
    // Recibir en el buffer posición de inicio de datos (incluido) y posición de fin de datos (no incluido)
    MPI_Recv(rangoDeDatosAEvaluar, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if(status.MPI_TAG == 1){
         // Hacer división de comunicadores para que cada reduce tenga un comunicador solo con los procesos Map
        for(int i = 1; i < 10; i++){
            MPI_Comm_split(MPI_COMM_WORLD, i, idProceso, &(reduce_comm[i-1]));
        }
        
        // Solicitar espacio para guardar datos a evaluar y luego leer los datos y guardarlos
        datos = calloc(rangoDeDatosAEvaluar[1] - rangoDeDatosAEvaluar[0], sizeof(int));
        leerDatos(rangoDeDatosAEvaluar[0], rangoDeDatosAEvaluar[1]);     
        // Para cada dato
        for(int i = 0; i < (rangoDeDatosAEvaluar[1] - rangoDeDatosAEvaluar[0]); i++){
            int datoEnEvaluacion = datos[i];
            int digitoEnEvaluacion;
			// Extraer el primer dígito
            while(datoEnEvaluacion != 0){
                digitoEnEvaluacion = datoEnEvaluacion%10;
                datoEnEvaluacion = datoEnEvaluacion/10;
            }
            // Contabilizarlo según cuál sea su primer dígito
            map[digitoEnEvaluacion]++;
        }
		free(datos);
        // Enviar resultados a los distintos reduce
        for(int i = 1; i < 10; i++){
            //printf("\nMap halló %i datos que empiezan por %i\n", map[i], i);
            MPI_Reduce(&(map[i]), &(map[i]), 1, MPI_INT, MPI_SUM, 0, reduce_comm[i-1]);
        }
    }
    
    else{
        // Si el map no va a ser iniciado, exclurilo de comunicadores con procesos Reduce
         for(int i = 1; i < 10; i++){
            MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, idProceso, &(reduce_comm[i-1]));

        }
    }
    
    MPI_Finalize();
    return 0;
}
