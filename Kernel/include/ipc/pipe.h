#ifndef _PIPE_H
#define _PIPE_H

#include <filedesc.h>
#include <memory_manager.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

KMEMORY_DECLARE

#define PIPE_BUFFER_SIZE 1024
#define MAX_PIPES 64

#define PIPE_READ 0
#define PIPE_WRITE 1

typedef struct pipe_t pipe_t;
typedef struct pipe_table_t pipe_table_t;

/**
 * @brief Encuentra un pipe por su identificador.
 */
pipe_t *find_pipe_by_id(const char *id);

/**
 * @brief Abre un pipe en modo de lectura o escritura para un proceso.
 *
 * Si el pipe identificado por 'id' no existe, lo crea y luego lo abre en el
 * modo especificado.
 *
 * @param mode Modo de apertura (lectura o escritura).
 * @return Un descriptor de pipe en caso de éxito, o un valor negativo en caso
 * de error.
 */
int open_pipe(fd_entry_t *fd, const char *id, int mode);

/**
 * @brief Crea un pipe y devuelve un puntero al mismo.
 *
 * @return Puntero al pipe creado, o NULL si no se pudo crear.
 */
pipe_t *create_pipe(const char *id);

/**
 * @brief Libera los recursos asociados al pipe.
 *
 * @param pipe Puntero a la estructura pipe_t que se desea liberar.
 */
void pipe_free(struct pipe_t *pipe);

#endif // _PIPE_H
