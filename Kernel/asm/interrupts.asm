%include "macros.inc"

GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL esc

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq60Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscall_dispatcher
EXTERN getStackBase
EXTERN schedule
EXTERN register_array
EXTERN load_registers_array


SECTION .text

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
	pushState
	
	mov rsi, rsp
	mov rdi, 0h
	call irqDispatcher

	mov rdi, rsp
	call schedule
	mov rsp, rax

	;Send EOI
	mov al, 20h
	out 20h, al

	popState

	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;SyscallHandler
_irq60Handler:
	pushStateNoRAX

	mov r9, r8
	mov r8, rcx
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, rax

	call syscall_dispatcher

	popStateNoRAX
	
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
