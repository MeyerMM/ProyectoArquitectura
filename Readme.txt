Comando compilación:
Make

Comando de ejecución:
mpirun -oversubscribe -mca btl ^openib -hostfile myhosts -np 1 master : -np 9 reduce : -np 1 map

Sustituir 1 por el número de maps

Crear hostfile:
touch myhosts
echo '172.31.34.55' >> myhosts
echo '172.31.44.90' >> myhosts

mpirun -mca btl ^openib -hostfile myhosts -np 1 master : -np 9 reduce : -np 1 map





