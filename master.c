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
    char nombreOutput[] = "Porcentajes.csv";
    stream = fopen(nombreOutput, "w");
    
    for(int i = 1; i < 10; i++){
        fprintf(stream, "\nPorcentaje de datos que inician con digito %i: %g%\n", i, 100.0*((float)resultados[i])/numeroDatos);
    }
    
    fclose(stream);
    return numeroDatos;
    
}
 
int main(int argc, char *argv[]){
	FILE *logStream;
    char nombreLog[] = "Log.csv";
    logStream = fopen(nombreLog, "w");
    
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
    
    
    MPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numeroProcesos); 
    MPI_Comm_rank(MPI_COMM_WORLD,&idProceso); 
    
    numeroMaps = numeroProcesos - numeroMaster - numeroReduce;
    
    
    contarDatos();
    fprintf(logStream, "\n numero de datos: %i", numeroDatos);
    
    datosPorMap = numeroDatos/numeroMaps;
    if(numeroDatos%numeroMaps != 0){
        datosPorMap++;
    }
    fprintf(logStream, "\n datos por map: %i", datosPorMap);
    fprintf(logStream, "\n numero de maps a usar: %i", numeroDatos/datosPorMap);
    
    
    int acumDatos =0;
    for(int i = 0 + numeroMaster + numeroReduce; i<numeroProcesos; i++){
        rangoDeDatosAEvaluar[0] = acumDatos;
        rangoDeDatosAEvaluar[1] = acumDatos + datosPorMap;
        
        if(rangoDeDatosAEvaluar[0] >= numeroDatos){
            // Mensaje de tag 0 es que el map no debe iniciarse
              MPI_Send(rangoDeDatosAEvaluar, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
        }   
        else{
            if(rangoDeDatosAEvaluar[1] > numeroDatos){
                rangoDeDatosAEvaluar[1] = numeroDatos;
            }
            // Mensaje de tag 1 permite iniciar el map
            MPI_Send(rangoDeDatosAEvaluar, 2, MPI_INT, i, 1, MPI_COMM_WORLD);
            acumDatos += datosPorMap;
        }
    }   
    
    for(int i = 1; i < 10; i++){
        MPI_Reduce(&zero, &zero, 1, MPI_INT, MPI_SUM, i, MPI_COMM_WORLD);
    }
    
    for(int i = 1; i < 10; i++){
        MPI_Recv(&(resultados[i]), 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
    }
    
    escribirResultados(resultados, numeroDatos);
	fprintf(logStream, "\nResultados guardados en Porcentajes.csv\n");

    MPI_Finalize();
    
    double tiempoFin = MPI_Wtime();
    fprintf(logStream, "\n El programa tomo %g segundos en ejecutar\n", tiempoFin - tiempoInicio);
	fclose(logStream);
    return 0;
}
 
