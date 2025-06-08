%include "macros.inc"

GLOBAL do_exit
GLOBAL aquire
GLOBAL release
GLOBAL cpu_vendor
GLOBAL get_sec
GLOBAL get_min
GLOBAL get_hour
GLOBAL get_day
GLOBAL get_month
GLOBAL get_year
GLOBAL get_scan_code
GLOBAL inb
GLOBAL outb
GLOBAL register_array
GLOBAL load_registers_array

section .text
	
cpu_vendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret


get_sec:
	push rbp
	mov rbp, rsp

	mov al, 0x00
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

get_min: 
	push rbp
	mov rbp, rsp

	mov al, 0x02
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

get_hour:
	push rbp
	mov rbp, rsp

	mov al, 0x04
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

get_day:
	push rbp
	mov rbp, rsp

	mov al, 0x07
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

get_month: 
	push rbp
	mov rbp, rsp

	mov al, 0x08
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

get_year:
	push rbp
	mov rbp, rsp

	mov al, 0x09
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

get_scan_code: 
	push rbp
	mov rbp, rsp
	mov rax, 0

.loop:
	in al, 64h
	and al, 0x01
	je .loop

	in al, 60h

	mov rsp, rbp
	pop rbp
	ret	


inb:
	mov rdx, rdi
	in al, dx
	ret
               

outb:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret

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

aquire:
    mov al, 0
.retry
    xchg [rdi], al
    test al, al
    jz .retry
    ret

do_exit:
	mov rax, 14h
	int 80h
	ret

release:
    mov byte [rdi], 1
    ret

section .bss
    register_array resq 17
