# Assignment3 Bonus



## Connecting to Dardel
```shell
ssh -i ~/.ssh/id-ed25519-pdc guoqingl@dardel.pdc.kth.se
salloc --nodes=4 -t 01:00:00 -A edu24.DD2356 -p shared --ntasks=512 --cpus-per-task=1
```


## Generate input matrix

```shell
gcc generate_matrices.c -o generate.exe
./generate.exe 4    
```

## Generate standard results

```shell
gcc matrix_multiply.c -o multiply.exe
./multiply.exe 4
```

## Generate results with Fox algorithm
```shell
gcc -o fox.exe matrix_multiply_fox.c -l msmpi -L "C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -I "C:\Program Files (x86)\Microsoft SDKs\MPI\Include"

mpiexec -n 4 fox.exe 4
```



## on windows

```shell
gcc -o fox.exe matrix_multiply_fox.c -l msmpi -L "C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -I "C:\Program Files (x86)\Microsoft SDKs\MPI\Include"

mpiexec -n 8 a.exe
```

## on linux

```shell
mpicxx -o a.exe activeMatter_MPI.cpp

mpirun -np 4 ./a.exe
```

## on Dardel

```shell
cc -o a.exe activeMatter_MPI.cpp

srun -n 4 ./a.exe
```
