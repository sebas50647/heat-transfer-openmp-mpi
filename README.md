# Simulación de transferencia de calor

## Descripción del Proyecto
se enfoca en la implementación de una simulación concurrente para modelar la transferencia de calor en una lámina rectangular utilizando técnicas de paralelismo de datos. El objetivo es encontrar el momento de equilibrio térmico de la lámina, donde la distribución de calor se estabiliza. Este proyecto requiere la creación de una base de código desde cero, implementando los principios de concurrencia y paralelismo mediante el uso de OpenMP, además de la distribucion de tareas con MPI.

## Problema a Resolver


Se necesita una simulación que modele la transferencia de calor en una lámina rectangular dividida en una matriz de celdas cuadradas. La lámina está sometida a una inyección constante de calor por sus bordes. El objetivo es encontrar el punto de equilibrio térmico donde la distribución de calor en la lámina se estabiliza. La lámina es un rectángulo bidimensional de un mismo material, dividido en filas y columnas de igual alto y ancho. Cada celda de la matriz almacena una temperatura que puede cambiar con el tiempo. La temperatura de una celda en un momento dado depende de su temperatura en el instante anterior y de las temperaturas de sus celdas vecinas.

## Funcionalidades Principales
- **Heatsim**: El simulador debe de ser capaz de dividir los jobs entre cores y que estos a su vez utilicen hilos para realizar los procesos que se requieren hacer en cada core.}
- **HeatDistributionSimulator**: Los usuarios ingresan el trabajo que quieren calcular, y el simulador se encarga de realizar los calculos necesarios con los parametros que fueron enviados desde el archivo que ingresa el usuario.
- **JobWriter**:  Escribe los resultados de los trabajos en archivos. Esto incluye tanto la información de los trabajos como las matrices de temperaturas finales después de que la simulación ha sido ejecutada.
- **JobReader**: Lee y carga trabajos desde un archivo de texto proporcionado por el usuario. Cada línea en el archivo de texto describe un trabajo con sus parámetros y la ruta a un archivo binario que contiene la matriz de temperaturas iniciales.


### Manual de Uso del Programa

Este manual te guiará en la compilación y ejecución de tu programa para la simulación de transferencia de calor.

##### Requisitos
El programa debe ser ejecutado en un sistema POSIX, además necesita contar con el compilador g++ para poder ejecutar el archivo makefile.
Se debe de contar con la capacidad de correr programas utilizando mpi, para el uso de estas librerías.
En caso de no contar con el compilador se puede instalar ingresando las siguientes lineas en la terminal.
```sh
 sudo apt install build-essential
 sudo apt install openmpi-bin openmpi-common libopenmpi-dev
```
Esto instalará las herramientas y librerías necesarias para la compilación junto con el compilador g++.
#### 1. Compilación

Para compilar el proyecto, abre la terminal en el directorio raíz del proyecto y ejecuta el siguiente comando:

```sh
make
```

Este comando ejecutará el proceso de compilación, generando los archivos binarios necesarios para ejecutar la aplicación.

#### 2. Ejecución

Una vez que la compilación haya finalizado correctamente, puedes iniciar el servidor web utilizando el comando siguiente:

```sh
./bin/mas_cquest_que_nunca_concurrente_2024a <job file> <num_cores> <num_threads>
```

Donde:

- `<job file>` es el archivo sobre el que quires hacer la simulacion. Ej: (documento.txt)
- `<num_cores>` es el número de hilos que deseas utilizar. Este parámetro es opcional. Si no se proporciona, se utilizarán los hilos disponibles en el sistema.
- `<num_threads>` es el número de hilos que deseas utilizar. Este parámetro es opcional. Si no se proporciona, se utilizarán los hilos disponibles en el sistema.

Ejemplo de ejecución con ambos parámetros:

```sh
bin/mas_cquest_que_nunca_concurrente_2024a job1.txt 15 3
```

Ejemplo de ejecución sin el parámetro opcional (usando todos los hilos disponibles en el sistema):

```sh
bin/mas_cquest_que_nunca_concurrente_2024a job1.txt
```

#### 3. Uso de la Aplicación Web

Una vez que se haya ejecutado el programa, este generara unos archvos en una carpeta `tsv`, en caso de que ya haya una este sobreescribirá el contenido de esta con el resultado de la simulacion. En el archivo .tsv se encontrará el resultado final


# Creditos 
Este programa fue desarrollado por: 
- Sebastian Arce Flores C10577
- Yerlan Irola Rodriguez C13851
- Jeshua Kantrell Reyes Carmona C06447