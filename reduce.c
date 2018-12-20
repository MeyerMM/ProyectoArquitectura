  
#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
 
int main(int argc, char *argv[]){
    int reduce = 0;
    int zero = 0;
    int numeroProcesos;
    int idProceso;
	MPI_Comm reduce_comm;

    MPI_Status status; 
    MPI_Init(&argc,&argv); 
    MPI_Comm_size(MPI_COMM_WORLD,&numeroProcesos); 
    MPI_Comm_rank(MPI_COMM_WORLD,&idProceso); 
    
    for(int i = 1; i < 10; i++){
           // printf("\n Número de datos que empiezan por %i: %i \n", i,  map[i]);
		
		if(i == idProceso){
			MPI_Comm_split(MPI_COMM_WORLD, i, idProceso, &reduce_comm);
			MPI_Reduce(&zero, &reduce, 1, MPI_INT, MPI_SUM, i, reduce_comm);
		}
		else{
			MPI_Comm_split(MPI_COMM_WORLD, i, MPI_UNDEFINED, &reduce_comm);
		}
    }
    
    MPI_Send(&reduce, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
   
    MPI_Finalize();
    return 0;
}

