Comando compilación:
Make
Comando de ejecución:
mpirun -mca btl ^openib -np 1 master : -np 9 reduce : -np 1 map
Sustituir 1 por el número de maps