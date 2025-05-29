#include <exec.h>

KMEMORY_DECLARE

// Esta funcion prepara el stack del proceso para que contenga los argumentos
// pasados por argv
static char **prepare_argv_on_stack(struct proc *proc, int argc,
                                    char *const argv[],
                                    char **proc_stack_current_addr_ptr) {
  char *current_stack_ptr = *proc_stack_current_addr_ptr;
  char **user_argv_ptr_array_kernel_buf =
      kmalloc(kernel_mem, (argc + 1) * sizeof(char *));

  if (!user_argv_ptr_array_kernel_buf && argc >= 0) {
    printk("prepare_argv_on_stack: kmalloc for user_argv_ptr_array_kernel_buf "
           "failed (size %d)\n",
           (argc + 1) * sizeof(char *));
    return NULL;
  }

  for (int i = 0; i < argc; ++i) {
    const char *current_arg_str = argv[i];
    if (current_arg_str == NULL) {
      current_arg_str = "";
    }

    size_t len = str_len(current_arg_str) + 1;
    current_stack_ptr -= len;
    str_cpy(current_stack_ptr, current_arg_str);
    user_argv_ptr_array_kernel_buf[i] = current_stack_ptr;
  }
  user_argv_ptr_array_kernel_buf[argc] = NULL;

  *proc_stack_current_addr_ptr = current_stack_ptr;
  return user_argv_ptr_array_kernel_buf;
}

// Esta funcion copia los punteros a los strings de argv al stack del proceso
static char *
copy_argv_ptr_array_to_stack(char *proc_stack_current_addr, int argc,
                             char **user_argv_ptr_array_kernel_buf) {
  // Acomodamos el stack del proceso para que quede alineado a sizeof(uintptr_t)
  proc_stack_current_addr =
      (char *)((uintptr_t)proc_stack_current_addr & -(sizeof(uintptr_t)));

  size_t argv_array_total_size =
      (argc + 1) *
      sizeof(char *); // Hago espacio para los punteros a los strings
  proc_stack_current_addr -= argv_array_total_size;
  memcpy((void *)proc_stack_current_addr, user_argv_ptr_array_kernel_buf,
         argv_array_total_size);
  return proc_stack_current_addr;
}

// Esta funcion pushea argc y argv al stack del proceso
static char *push_argc_argv_to_stack(char *proc_stack_current_addr, int argc,
                                     uint64_t *user_argv_on_stack) {
  // Push argv (pointer to the array of argument strings on the stack)
  proc_stack_current_addr -= sizeof(uint64_t *);
  *(uint64_t **)proc_stack_current_addr = user_argv_on_stack;

  // Push argc
  proc_stack_current_addr -= sizeof(int64_t);
  *(int64_t *)proc_stack_current_addr = argc;

  return proc_stack_current_addr;
}

// Esta funcion prepara el stack del proceso para que contenga el frame inicial
// de iretq
static uint64_t *setup_initial_stack_frame(struct proc *proc,
                                           uint64_t *current_stack_top) {
  uint64_t *stack = current_stack_top;

  // Orden: SS, RSP, RFLAGS, CS, RIP (para iretq)
  *(--stack) = X86_SS;
  *(--stack) =
      (uint64_t)current_stack_top; // RSP (Stack pointer for the new process)
  *(--stack) = X86_RFLAGS;
  *(--stack) = X86_CS;
  *(--stack) = (uint64_t)process_wrapper; // RIP (Entry point)

  return stack;
}

/**
 * Esta funcion es la que se encarga de ejecutar un proceso, recibe el argc y
 * argv y se encarga de inicializar el proceso y encolarlo en el scheduler
 */
int execv(struct proc *proc, int argc, char *const argv[]) {
  printk("execv called\n");

  char *proc_stack_current_addr = (char *)proc->stack_pointer;

  // 1. Copy argument strings to process stack and get kernel buffer of pointers
  char **user_argv_ptr_array_kernel_buf =
      prepare_argv_on_stack(proc, argc, argv, &proc_stack_current_addr);
  if (!user_argv_ptr_array_kernel_buf) {
    return -1; // TODO: Usar una constante de error definida
  }

  /*
  Stack after prepare_argv_on_stack:
  (Higher Addresses - closer to initial proc->stack_pointer) 0x610030
    ... (any unused stack space above the arguments) ...
    [ argv_strings[0] (e.g., "program_name\0") ]
    [ argv_strings[1] (e.g., "arg1\0") ]
    ...
    [ argv_strings[argc-1] (e.g., "last_arg\0") ] <-- proc_stack_current_addr
  points here (Lower Addresses) user_argv_ptr_array_kernel_buf contains pointers
  to these strings.
  */

  proc_stack_current_addr = copy_argv_ptr_array_to_stack(
      proc_stack_current_addr, argc, user_argv_ptr_array_kernel_buf);

  /*
  Stack after copy_argv_ptr_array_to_stack:
  (Higher Addresses - closer to initial proc->stack_pointer)
    ... (any unused stack space above the arguments) ...
    [ argv_strings[0] (e.g., "program_name\0") ]
    ...
    [ argv_strings[argc-1] (e.g., "last_arg\0") ]
    [ ... alignment padding bytes (if any) ... ]
    [ NULL pointer (argv[argc]) ]
    [ pointer to argv_strings[argc-1] on stack ]
    ...
    [ pointer to argv_strings[0] on stack ]   <-- proc_stack_current_addr points
  here (Lower Addresses)
  */
  uint64_t *user_argv_on_stack = (uint64_t *)proc_stack_current_addr;

  kmm_free(user_argv_ptr_array_kernel_buf, kernel_mem);
  user_argv_ptr_array_kernel_buf = NULL;

  proc_stack_current_addr = push_argc_argv_to_stack(proc_stack_current_addr,
                                                    argc, user_argv_on_stack);

  proc->stack_pointer = (uint64_t *)proc_stack_current_addr;

  proc->stack_pointer = setup_initial_stack_frame(proc, proc->stack_pointer);

  return 0;
}
