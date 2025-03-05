GLOBAL read
GLOBAL write_stdin
GLOBAL get_date_time
GLOBAL screen_clear
GLOBAL change_color
GLOBAL opCodeExc
GLOBAL draw_sqr
GLOBAL sleep_time
GLOBAL zoom
GLOBAL free_drawing
GLOBAL get_regist
GLOBAL make_sound

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

;-------------------------------------------------------------------------------
; Funcion : read
;-------------------------------------------------------------------------------
; Argumentos:
;   1 - buffer
;   2 - count
;-------------------------------------------------------------------------------
read:
    pushState

    mov rax, 0x00
    mov rdx, rsi        ;cargo argumento count
	mov rsi, rdi		;cargo argumento buffer
	mov rdi, 0x00
    int 80h

    popState
    
    ret

;-------------------------------------------------------------------------------
; Funcion : write_stdin
;-------------------------------------------------------------------------------
; Argumentos:
;   1 - string
;   2 - count
;-------------------------------------------------------------------------------
write_stdin:
    pushState

    mov rax, 0x01
	mov rdx, rsi    ;cargo argumento count
    mov rsi, rdi    ;cargo argumento string
	mov rdi, 0x01
    int 80h

    popState
    
    ret


;-------------------------------------------------------------------------------
; Function : get_date_time 
;-------------------------------------------------------------------------------

get_date_time: 
	pushState
	mov rax, 0x06
	int 80h
	popState

	ret

screen_clear: 
	pushState
	mov rax, 0x02
	int 80h
	popState

	ret

change_color:
	pushState
	
	mov rax, 0x03
	int 80h
	
	popState
	
	ret

sleep_time:
	pushState
	
	mov rax, 0x07
	int 80h
	
	popState
	
	ret

draw_sqr:
	pushState

	mov rax, 0x04
	int 80h

	popState
	ret

opCodeExc:
	ud2
	ret

zoom:
	pushState

	mov rax, 0x08
	int 80h

	popState

	ret

free_drawing:
	pushState
	
	mov rax, 0x05
	int 80h

	popState

	ret


get_regist:
	pushState

	mov rax, 0x09
	int 80h

	popState

	ret

make_sound:
	pushState

	mov rax, 0x0A;
	int 80h

	popState

	ret