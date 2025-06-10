# Proyecto de Sistema Operativo

Este proyecto es un sistema operativo simple desarrollado como parte de un trabajo práctico.

## Requisitos Previos

- Docker instalado.

## Configuración y Ejecución

A continuación se detallan los pasos para configurar y ejecutar el sistema operativo en un contenedor Docker:

1.  **Descargar la Imagen de Docker:**
    Se debe tener la imagen de Docker necesaria. Se descarga con el siguiente comando:
    ```sh
    docker pull agodio/itba-so-multi-platform:3.0
    ```

2.  **Crear el Contenedor de Docker:**
    Utiliza el script `container.sh` para crear el entorno de Docker.
    **Importante:** No debe existir previamente un contenedor con el nombre `TPE_SO`. Si existe, 
    hay que eliminarlo antes de ejecutar el script.
    ```sh
    ./container.sh
    ```

3.  **Compilar el Sistema Operativo:**
    El script `compile.sh` se utiliza para compilar el proyecto.
    -   Para compilar con el gestor de memoria "Dummy" (comportamiento por defecto):
        ```sh
        ./compile.sh
        ```
        o también:
        ```sh
        ./compile.sh USE_SIMPLE_MM
        ```
    -   Para compilar con el gestor de memoria "Buddy":
        ```sh
        ./compile.sh USE_BUDDY_MM
        ```

4.  **Ejecutar el Sistema Operativo en QEMU:**
    Una vez compilado, se puede ejecutar el sistema operativo usando el script `run.sh`.
    -   Para ejecutar en modo normal:
        ```sh
        ./run.sh
        ```
    -   Para ejecutar en modo debug (se iniciará QEMU en modo de espera para conexión GDB en el puerto 1234):
        ```sh
        ./run.sh -d
        ```
        Se puede conectar GDB utilizando la configuración en el archivo [.gdbinit](.gdbinit).

## Scripts Principales

-   [`container.sh`](container.sh): Configura el contenedor Docker.
-   [`compile.sh`](compile.sh): Compila el proyecto.
-   [`run.sh`](run.sh): Ejecuta el sistema operativo en QEMU.
