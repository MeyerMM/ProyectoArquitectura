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
	double tiempoPasoDeMensajes = 0;
	double tiempoInicioPasoDeMensajes = 0;
	double tiempoFinPasoDeMensajes = 0;
    
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
    printf("\n numero de datos: %i", numeroDatos);
    
    datosPorMap = numeroDatos/numeroMaps;
    if(numeroDatos%numeroMaps != 0){
        datosPorMap++;
    }
    printf("\n datos por map: %i", datosPorMap);
    printf("\n numero de maps a usar: %i", numeroMaps);
    
    
    int acumDatos =0;
	// Para cada proceso Map
    for(int i = 0 + numeroMaster + numeroReduce; i<numeroProcesos; i++){
		 // Calcular qué rango de datos evalúa
        rangoDeDatosAEvaluar[0] = acumDatos;
        rangoDeDatosAEvaluar[1] = acumDatos + datosPorMap;
        
        if(rangoDeDatosAEvaluar[0] >= numeroDatos){
            // Mensaje de tag 0 es que el map no debe iniciarse
			tiempoInicioPasoDeMensajes = MPI_Wtime();
            MPI_Send(rangoDeDatosAEvaluar, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
			tiempoFinPasoDeMensajes = MPI_Wtime();
			tiempoPasoDeMensajes = tiempoPasoDeMensajes + tiempoFinPasoDeMensajes - tiempoInicioPasoDeMensajes;
			printf("\n Se utlizará un map menos que los solicitados\n");
        }   
        else{
            if(rangoDeDatosAEvaluar[1] > numeroDatos){
                rangoDeDatosAEvaluar[1] = numeroDatos;
            }
             // Mensaje de tag 1 permite iniciar el map. El buffer le indica al Map qué rango de datos evalúa.
			tiempoInicioPasoDeMensajes = MPI_Wtime();
            MPI_Send(rangoDeDatosAEvaluar, 2, MPI_INT, i, 1, MPI_COMM_WORLD);
			tiempoFinPasoDeMensajes = MPI_Wtime();
			tiempoPasoDeMensajes = tiempoPasoDeMensajes + tiempoFinPasoDeMensajes - tiempoInicioPasoDeMensajes;
            acumDatos += datosPorMap;
        }
    }   
    tiempoInicioPasoDeMensajes = MPI_Wtime();
	// Recibir resultados de procesos reduce
    for(int i = 1; i < 10; i++){
        MPI_Reduce(&zero, &zero, 1, MPI_INT, MPI_SUM, i, MPI_COMM_WORLD);
    }
    
    for(int i = 1; i < 10; i++){
        MPI_Recv(&(resultados[i]), 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
    }
	tiempoFinPasoDeMensajes = MPI_Wtime();
	tiempoPasoDeMensajes = tiempoPasoDeMensajes + tiempoFinPasoDeMensajes - tiempoInicioPasoDeMensajes;
    
    escribirResultados(resultados, numeroDatos);


    MPI_Finalize();
    
    double tiempoFin = MPI_Wtime();
    printf("\n El programa tomo %g segundos en ejecutar\n", tiempoFin - tiempoInicio);
	printf("\n El programa tomo %g segundos pasando mensajes\n", tiempoPasoDeMensajes);
	printf("\n El %g/% del tiempo de ejecucion transcurrio en el pase de mensajes\n", 100.0* tiempoPasoDeMensajes/(tiempoFin - tiempoInicio));
    return 0;
}
 
