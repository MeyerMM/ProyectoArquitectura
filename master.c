#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int numeroDatos = 0;

int contarDatos(){
    FILE *stream;
    char nombreArchivo[] = "datos.csv";
    stream = fopen(nombreArchivo, "r");
    int ch;
    while(!feof(stream)){
        ch = fgetc(stream);
        if(ch == '\n'){
            numeroDatos++;
        }
    }
    fclose(stream);
    return numeroDatos;
    
}

int escribirResultados(int resultados[], int numeroDatos){
    FILE *stream;
    char nombreArchivo[] = "Porcentajes.csv";
    stream = fopen(nombreArchivo, "w");
    
    for(int i = 1; i < 10; i++){
        fprintf(stream, "\nPorcentaje de datos que inician con digito %i: %g%\n", i, 100.0*((float)resultados[i])/numeroDatos);
    }
    
    fclose(stream);
    printf("\nResultados guardados en Porcentajes.csv\n");
    return numeroDatos;
    
}
 
int main(int argc, char *argv[]){
    
    double tiempoInicio = MPI_Wtime();
    
    int numeroProcesos;
    int numeroMaster = 1;
    int numeroReduce = 9;
    int numeroMaps;
    int datosPorMap;
    int rangoDeDatosAEvaluar[2] = {0, 0};
    int idProceso;
    int resultados[10];
    MPI_Status status; 
    int zero = 0;
	MPI_Comm reduce_comm;
    
    
    MPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numeroProcesos); 
    MPI_Comm_rank(MPI_COMM_WORLD,&idProceso); 
    
    
    
    double tiempoInicioSplit = MPI_Wtime();
    // Excluir al Master de los comunicadores de procesos Reduce
     for(int i = 1; i < 10; i++){
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, idProceso, &reduce_comm);
    }
    double tiempoFinSplit = MPI_Wtime();
    printf("\n El CommSplit tomo %g segundos en ejecutar\n", tiempoFinSplit - tiempoInicioSplit);
    
    numeroMaps = numeroProcesos - numeroMaster - numeroReduce;
    contarDatos();
    printf("\n numero de datos: %i", numeroDatos);
    
    datosPorMap = numeroDatos/numeroMaps;
    if(numeroDatos%numeroMaps != 0){
        datosPorMap++;
    }
    printf("\n datos por map: %i", datosPorMap);
    printf("\n numero de maps a usar: %i", numeroDatos/datosPorMap);
    
    
    int acumDatos =0;
    // Para cada proceso Map
    for(int i = 0 + numeroMaster + numeroReduce; i<numeroProcesos; i++){
        // Calcular qué rango de datos evalúa
        rangoDeDatosAEvaluar[0] = acumDatos;
        rangoDeDatosAEvaluar[1] = acumDatos + datosPorMap;
        // Si el map no tiene datos a ser evalúados
        if(rangoDeDatosAEvaluar[0] >= numeroDatos){
            // Mensaje de tag 0 es que el map no debe iniciarse
              MPI_Send(rangoDeDatosAEvaluar, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
        }   
        else{
            if(rangoDeDatosAEvaluar[1] > numeroDatos){
                rangoDeDatosAEvaluar[1] = numeroDatos;
            }
            // Mensaje de tag 1 permite iniciar el map. El buffer le indica al Map qué rango de datos evalúa.
            MPI_Send(rangoDeDatosAEvaluar, 2, MPI_INT, i, 1, MPI_COMM_WORLD);
            acumDatos += datosPorMap;
        }
    }   

    // Recibir resultados de procesos reduce
    for(int i = 1; i < 10; i++){
        MPI_Recv(&(resultados[i]), 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
    }
    
    
    escribirResultados(resultados, numeroDatos);


    MPI_Finalize();
    
    double tiempoFin = MPI_Wtime();
    printf("\n El programa tomo %g segundos en ejecutar\n", tiempoFin - tiempoInicio);
    return 0;
}
 
