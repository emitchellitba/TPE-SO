GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL esc

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq60Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscall_dispatcher
EXTERN getStackBase

EXTERN register_array
EXTERN load_registers_array


SECTION .text

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


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;SyscallHandler
_irq60Handler:
	pushState

	mov r9, r8
	mov r8, rcx
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, rax

	call syscall_dispatcher

	popState
	
	iretq

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0
	
;Invalid Operation Code
_exception6Handler: 
	exceptionHandler 6
	

haltcpu:
	cli
	hlt
	ret

section .data
	userland equ 0x400000
	esc db 0

SECTION .bss
	aux resq 1
