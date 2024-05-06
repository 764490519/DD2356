# Assignment3 Bonus

## Compile

```shell
gcc -o a.exe matrix_multiply_fox.c -l msmpi -L "C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -I "C:\Program Files (x86)\Microsoft SDKs\MPI\Include"

mpiexec -n 8 a.exe
```