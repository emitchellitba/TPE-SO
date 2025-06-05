%macro pushStateNoRAX 0
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

%macro popStateNoRAX 0
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
%endmacro

%macro pushState 0
	push rax
	pushStateNoRAX
%endmacro

%macro popState 0
	popStateNoRAX
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	mov al, 20h  ; signal pic EOI (End of Interrupt)
	out 20h, al

	popState

	cmp BYTE [esc], 0  ;check esc
	je .end
	call load_registers_array
	mov BYTE [esc], 0

.end:
	iretq

%endmacro

%macro exceptionHandler 1
	call load_registers_array
	pushState
	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

	popState

	call getStackBase
    mov [rsp + 8*3], rax
    mov rax, userland
    mov [rsp], rax

	
	iretq
%endmacro