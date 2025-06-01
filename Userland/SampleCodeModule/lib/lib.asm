GLOBAL read
GLOBAL write_stdin
GLOBAL get_date_time
GLOBAL screen_clear
GLOBAL change_color
GLOBAL draw_sqr
GLOBAL sleep_time
GLOBAL zoom
GLOBAL free_drawing
GLOBAL get_regist
GLOBAL make_sound
; TODO: Hacer que todas las syscalls usen EXPORT_FUNC

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

%macro do_syscall_direct 1
    pushState
		mov rax, %1
    int 80h
    popState
    ret
%endmacro

%macro do_syscall 4
    pushState
    mov rax, %1
    mov rdx, %2
    mov rsi, %3
    mov rdi, %4
    int 80h
    popState
    ret
%endmacro

%macro EXPORT_FUNC 1
    GLOBAL _%1
    _%1:
%endmacro

;-------------------------------------------------------------------------------
; Funcion : read
;-------------------------------------------------------------------------------
; Argumentos:
;   1 - buffer
;   2 - count
;-------------------------------------------------------------------------------
read:
    do_syscall 0x00, rsi, rdi, 0x00   ; rdi=buffer, rsi=count

;-------------------------------------------------------------------------------
; Funcion : write_stdin
;-------------------------------------------------------------------------------
; Argumentos:
;   1 - string
;   2 - count
;-------------------------------------------------------------------------------
write_stdin:
	do_syscall 0x01, rsi, rdi, 0x01   ; rdi=string, rsi=count


;-------------------------------------------------------------------------------
; 0x02 - screen_clear
;-------------------------------------------------------------------------------
screen_clear:
    do_syscall 0x02, 0, 0, 0

;-------------------------------------------------------------------------------
; 0x03 - change_color
;-------------------------------------------------------------------------------
change_color:
    do_syscall 0x03, 0, 0, rdi

;-------------------------------------------------------------------------------
; 0x04 - draw_sqr
;-------------------------------------------------------------------------------
draw_sqr:
    do_syscall 0x04, rdx, rsi, rdi

;-------------------------------------------------------------------------------
; 0x05 - free_drawing
;-------------------------------------------------------------------------------
free_drawing:
    do_syscall 0x05, rdx, rsi, rdi

;-------------------------------------------------------------------------------
; 0x06 - get_date_time
;-------------------------------------------------------------------------------
get_date_time:
    do_syscall 0x06, 0, 0, 0

;-------------------------------------------------------------------------------
; 0x07 - sleep_time
;-------------------------------------------------------------------------------
sleep_time:
    do_syscall 0x07, 0, 0, rdi

;-------------------------------------------------------------------------------
; 0x08 - zoom
;-------------------------------------------------------------------------------
zoom:
    do_syscall 0x08, 0, 0, rdi

;-------------------------------------------------------------------------------
; 0x09 - get_regist
;-------------------------------------------------------------------------------
get_regist:
    do_syscall 0x09, 0, 0, rdi

;-------------------------------------------------------------------------------
; 0x0A - make_sound
;-------------------------------------------------------------------------------
;make_sound:
;    do_syscall 0x0A, rsi, 0, rdi

;-------------------------------------------------------------------------------
; 0x0B - read_kmsg
;-------------------------------------------------------------------------------
EXPORT_FUNC read_kmsg
    do_syscall 0x0B, 0, rsi, rdi

;-------------------------------------------------------------------------------
; 0x0F - new_proc
;-------------------------------------------------------------------------------
; Argumentos:
;   1 - name
;   2 - entry point
;   3 - argc
;   4 - argv
;-------------------------------------------------------------------------------
EXPORT_FUNC new_proc
	do_syscall_direct 0x0F
