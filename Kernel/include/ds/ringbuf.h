#ifndef _DS_RINGBUF_H
#define _DS_RINGBUF_H

#include <kernel.h>
#include <memory_manager.h>
#include <stddef.h>
#include <sys/types.h>

KMEMORY_DECLARE

#define RING_INDEX(ring, i) ((i) % ((ring)->size))

/**
 * \brief ring buffer
 */
struct ringbuf {
  char *buf;
  size_t size;
  size_t head;
  size_t tail;
};

/**
 * \brief create a new statically allocated ring buffer
 */
#define RINGBUF_NEW(sz)                                                        \
  (&(struct ringbuf){.buf = (char[sz]){0}, .size = sz, .head = 0, .tail = 0})

/**
 * \brief create a new dynamically allocated ring buffer
 *
 */
static inline struct ringbuf *ringbuf_new(size_t size) {
  struct ringbuf *ring = kmalloc(kernel_mem, sizeof(struct ringbuf));

  if (!ring)
    return NULL;

  ring->buf = kmalloc(kernel_mem, size * sizeof(char));

  if (!ring->buf) {
    kmm_free(ring, kernel_mem);
    return NULL;
  }

  ring->size = size;
  ring->head = 0;
  ring->tail = 0;

  return ring;
}

/**
 * \brief free a dynamically allocated ring buffer
 *
 */
static inline void ringbuf_free(struct ringbuf *r) {
  if (!r)
    return;

  kmm_free(r->buf, kernel_mem);
  kmm_free(r, kernel_mem);
}

/**
 * \brief read from a ring buffer
 */
static inline size_t ringbuf_read(struct ringbuf *ring, size_t n, char *buf) {
  size_t size = n;

  while (n) {
    if (ring->head == ring->tail) /* Ring is empty */
      break;
    if (ring->head == ring->size)
      ring->head = 0;
    *buf++ = ring->buf[ring->head++];
    n--;
  }

  return size - n;
}

/**
 * \brief peek (non-destructive read) from a ring buffer
 */
static inline size_t ringbuf_read_noconsume(struct ringbuf *ring, off_t off,
                                            size_t n, char *buf) {
  size_t size = n;
  size_t head = ring->head + off;

  if (ring->head < ring->tail && head > ring->tail)
    return 0;

  while (n) {
    if (head == ring->size)
      head = 0;
    if (head == ring->tail) /* Ring is empty */
      break;
    *buf++ = ring->buf[head++];
    n--;
  }

  return size - n;
}

static inline size_t ringbuf_write(struct ringbuf *ring, size_t n, char *buf) {
  size_t size = n;

  while (n) {
    if (RING_INDEX(ring, ring->tail + 1) ==
        RING_INDEX(ring, ring->head)) // Buffer lleno
      break;

    if (ring->tail == ring->size)
      ring->tail = 0;

    ring->buf[ring->tail++] = *buf++;
    n--;
  }

  return size - n;
}

static inline size_t ringbuf_write_overwrite(struct ringbuf *ring, size_t n,
                                             char *buf) {
  size_t size = n;

  while (n) {
    if (RING_INDEX(ring, ring->head) == RING_INDEX(ring, ring->tail) + 1) {
      /* move head to match */
      ring->head = RING_INDEX(ring, ring->head) + 1;
    }

    if (ring->tail == ring->size)
      ring->tail = 0;

    ring->buf[ring->tail++] = *buf++;
    n--;
  }

  return size - n;
}

static inline size_t ringbuf_available(struct ringbuf *ring) {
  if (ring->tail >= ring->head)
    return ring->tail - ring->head;

  return ring->tail + ring->size - ring->head;
}

/**
 * \brief read from a ring buffer until character c is found or count is reached
 */
static inline size_t ringbuf_read_until(struct ringbuf *ring, char *buf,
                                        size_t count, char c) {
  size_t read = 0;
  while (read < count && ring->head != ring->tail) {
    if (ring->head == ring->size)
      ring->head = 0;
    char ch = ring->buf[ring->head++];
    buf[read++] = ch;
    if (ch == c)
      break;
  }
  return read;
}

/**
 * \brief returns the number of elements from the head up to and including the
 * character c. If c is not found, returns 0.
 */
static inline size_t ringbuf_find(struct ringbuf *ring, char c) {
  size_t head = ring->head;
  size_t count = 0;
  while (head != ring->tail) {
    if (head == ring->size)
      head = 0;
    count++;
    if (ring->buf[head] == c)
      return count;
    head++;
  }
  return 0;
}

static inline void ringbuf_unwrite(struct ringbuf *ring) {
  if (ring->tail == ring->head)
    return;
  if (ring->tail == 0)
    ring->tail = ring->size - 1;
  else
    ring->tail--;
}

#endif /* ! _DS_RINGBUF_H */
