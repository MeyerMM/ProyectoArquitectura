  
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
 
int main(int argc, char *argv[]){
    int reduce = 0;
    int zero = 0;
    int numeroProcesos;
    int idProceso;

    MPI_Status status; 
    MPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numeroProcesos); 
    MPI_Comm_rank(MPI_COMM_WORLD,&idProceso); 
    
    for(int i = 1; i < 10; i++){
           // printf("\n Número de datos que empiezan por %i: %i \n", i,  map[i]);
            MPI_Reduce(&zero, &reduce, 1, MPI_INT, MPI_SUM, i, MPI_COMM_WORLD);
    }
    
    MPI_Send(&reduce, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
   
    MPI_Finalize();
    return 0;
}

