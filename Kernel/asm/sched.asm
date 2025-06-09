GLOBAL call_timer_tick
call_timer_tick:
	int 20h
    _cli
	ret

GLOBAL kernel_syscall_create_process
kernel_syscall_create_process:
    mov rax, 15         
    int 0x80            

    ret 
