#include <kernel.h>
#include <semaphores/semaphore.h>
#include <proc/process.h>
#include <proc/scheduler.h>

void queue_test();

memory_manager_adt kernel_mem;

static int kernel_log_level = LOG_DEBUG;
LOGGER_DEFINE(kernel, kernel_log, kernel_log_level)

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;
static void *const heapModuleAddress = (void *)0x600000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
  memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
  return (void *)((uint64_t)&endOfKernel +
                  PageSize * 8       // The size of the stack itself, 32KiB
                  - sizeof(uint64_t) // Begin at the top of the stack
  );
}

void *initializeKernelBinary() {
  void *moduleAddresses[] = {sampleCodeModuleAddress, sampleDataModuleAddress};

  loadModules(&endOfKernelBinary, moduleAddresses);

  clearBSS(&bss, &endOfKernel - &bss);

  return getStackBase();
}

int main() {
  load_idt();

  kernel_mem = kmm_init(heapModuleAddress);

#if defined(USE_BUDDY_MM)
  kernel_log(LOG_INFO, "Buddy memory manager initialized\n");
#else
  kernel_log(LOG_INFO, "Simple memory manager initialized\n");
#endif

  _cli();

  initialize_scheduler();

  semaphore_block_test();

  // char *name = "init";
  // fs_load(name, (fs_entry_point_t)sampleCodeModuleAddress);

  // char *argv[] = {name, NULL};
  // int argc = 1;

  // syscall_dispatcher(SPAWN_PROCESS_SYSCALL_ENTRY, (uint64_t)name, argc, argv);

  call_timer_tick();

  return 0;
}

void queue_test() {
  /* Con el dummy no hay kfree pero no pasa nada */

  struct queue *queue = QUEUE_NEW();
  queue->flags |= QUEUE_TRACE; // Enable tracing

  struct qnode *node1 = enqueue(queue, (void *)0x1);
  struct qnode *node2 = enqueue(queue, (void *)0x2);
  struct qnode *node3 = enqueue(queue, (void *)0x3);

  kernel_log(LOG_DEBUG, "Queue count after enqueue: %d\n", queue->count); // 3

  void *val = dequeue(queue);
  kernel_log(LOG_DEBUG, "Dequeued: %p\n", val); // 0x1

  queue_remove(queue, (void *)0x2);
  kernel_log(LOG_DEBUG, "Queue count after remove: %d\n", queue->count); // 1

  queue_node_remove(queue, node3);
  kernel_log(LOG_DEBUG, "Queue count after node_remove: %d\n",
             queue->count); // 0

  struct qnode *node4 = enqueue(queue, (void *)0x4);
  kernel_log(LOG_DEBUG, "Enqueued node4: %p\n", node4); // 0x4

  while (queue->count > 0) {
    void *v = dequeue(queue);
    kernel_log(LOG_DEBUG, "Dequeued: %p\n", v); // 0x4
  }

  dequeue(queue); // Esto no deberia hacer nada
  kernel_log(LOG_DEBUG, "Queue count after dequeue: %d\n", queue->count); // 0

  kernel_log(LOG_DEBUG, "Queue count at end: %d\n", queue->count); // 0

  // kfree(queue);
}

semaphore_t *test_sem = NULL;

void sem_blocked_proc(int argc, char **argv) {
    test_sem = my_sem_create(0, 0); // Semáforo bloqueante
    // Este proceso debería bloquearse aquí
    my_sem_wait(test_sem);
    kernel_log(LOG_DEBUG, "Proceso desbloqueado por semáforo!\n");
    // Aquí puedes setear una variable global para verificar que llegó
    // o terminar el proceso
}

void sem_unblocker_proc(int argc, char **argv) {
    test_sem = my_sem_open(0);
    // Espera un poco (simulado)
    for (volatile int i = 0; i < 100; ++i);
    my_sem_post(test_sem);
    yield();
}

void semaphore_block_test() {
    my_sem_init();

    // Proceso que se bloqueará
    char *name1 = "bloqueado";
    char *argv1[] = {name1, NULL};
    int argc1 = 1;
    fs_load(name1, (fs_entry_point_t)sem_blocked_proc);
    // El entrypoint debe estar linkeado y ser visible
    syscall_dispatcher(SPAWN_PROCESS_SYSCALL_ENTRY, (uint64_t)name1, argc1, argv1);

    // Proceso que desbloquea
    char *name2 = "desbloqueador";
    char *argv2[] = {name2, NULL};
    int argc2 = 1;
    fs_load(name2, (fs_entry_point_t)sem_unblocker_proc);
    syscall_dispatcher(SPAWN_PROCESS_SYSCALL_ENTRY, (uint64_t)name2, argc2, argv2);
}
