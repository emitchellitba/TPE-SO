GLOBAL register_array
GLOBAL load_registers_array


SECTION .text 

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

load_registers_array:
    pushState

    mov rax, [rsp + 16*8]       ; Captura RIP de la pila antes de modificar rsp
    mov [register_array + 128], rax  ; Guarda RIP en la posición correcta en el array

    mov rax, register_array

    push rbx
    mov rbx, [rbp - 8]
    mov QWORD[rax], rbx
    pop rbx
    
    add rax, 8
    mov QWORD[rax], rbx
    add rax, 8
    mov QWORD[rax], rcx
    add rax, 8
    mov QWORD[rax], rdx
    add rax, 8
    mov QWORD[rax], rsi
    add rax, 8
    mov QWORD[rax], rdi
    add rax, 8

    push rbx
    mov rbx, [rbp]              ;levanto el rbp del stack
    mov QWORD[rax], rbx             
    pop rbx       
    
    add rax, 8
    mov QWORD[rax], rbp             ;en rbp tengo el rsp original
    add rax, 8
    mov QWORD[rax], r8
    add rax, 8
    mov QWORD[rax], r9
    add rax, 8
    mov QWORD[rax], r10
    add rax, 8
    mov QWORD[rax], r11
    add rax, 8
    mov QWORD[rax], r12
    add rax, 8
    mov QWORD[rax], r13
    add rax, 8
    mov QWORD[rax], r14
    add rax, 8
    mov QWORD[rax], r15
    add rax, 8
    
    popState
    ret


section .bss
    register_array resq 17