#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <ds/queue.h>
#include <shared_info.h>
#include <stdint.h>

extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);

#define MAX_SEMAPHORES 1024

/**
 * @brief Inicializa la tabla de semáforos.
 */
void my_sem_init();

/**
 * @brief Inicializa un semáforo con el id y valor inicial dados. Enfocado a
 * semaforos de kernel.
 * @param sem Punteor al semáforo a inicializar.
 * @param value Valor inicial del semáforo.
 */
semaphore_t *my_ksem_init(uint64_t, uint64_t value);

/**
 * @brief Crea un semáforo con el id y valor inicial dados.
 * @param id Identificador del semáforo.
 * @param value Valor inicial del semáforo.
 * @return Puntero al semáforo creado, o NULL si falla.
 */
uint64_t my_sem_create(uint64_t id, uint64_t value);

/**
 * @brief Libera el semáforo pasado como parámetro.
 * @param sem Puntero al semáforo a destruir.
 */
uint64_t my_sem_destroy(uint64_t id);

/**
 * @brief Abre un semáforo existente (lo busca por id).
 * @param id Identificador del semáforo.
 * @return Puntero al semáforo si existe y está en uso, o NULL si no.
 */
uint64_t my_sem_open(uint64_t id);

/**
 * @brief Realiza la operación post (signal) sobre el semáforo.
 *        Si hay procesos esperando, despierta uno; si no, incrementa el valor.
 * @param sem Puntero al semáforo.
 * @return 0 en éxito, -1 en error.
 */
uint64_t my_sem_post(uint64_t id);

/**
 * @brief Realiza la operación wait (down) sobre el semáforo.
 *        Si el valor es 0, bloquea el proceso actual.
 * @param sem Puntero al semáforo.
 * @return 0 en éxito, -1 en error.
 */
uint64_t my_sem_wait(uint64_t id);

/**
 * @brief Indica si el semáforo está abierto y en uso.
 * @param sem Puntero al semáforo.
 * @return 1 si está abierto, 0 si no.
 */
uint8_t sem_is_open(uint64_t id);

#endif
