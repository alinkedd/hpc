# High Performance Computing (HPC)

Лабораторні роботи з предмету "Високопродуктивні обчислення".

Для компіляції програми:

```
make
```

Скомпільований бінарний файл *lab1* знаходитиметься у папці *bin*.

***

Для одного запуску за допомогою OpenMPI:

```
mpirun [-np X] bin/lab1  
```

Час виконання (в секундах) записується у відповідний файл *lab1_p[P]_x[X]* (P -
кількість процесів, X - аргумент x з файлу *input/x.txt*) у папку *output*.

***

Щоб запустити прогін на 1000 ітерацій для кількості процесів 1-4, виконати:

```
chmod +x run.sh
./run.sh
```

***

Щоб порахувати середнє, виконати:

```
python get_average.py
```

Середній час виконання (в секундах) записується у відповідний файл
*lab1_p[P]_x[X]* у папку *averages*.
