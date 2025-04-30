GLOBAL cpuVendor
GLOBAL getSec
GLOBAL getMin
GLOBAL getHour
GLOBAL getDay
GLOBAL getMonth
GLOBAL getYear
GLOBAL getScanCode
GLOBAL outb
GLOBAL inb

section .text
	
cpuVendor:
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


getSec:
	push rbp
	mov rbp, rsp

	mov al, 0x00
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getMin: 
	push rbp
	mov rbp, rsp

	mov al, 0x02
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getHour:
	push rbp
	mov rbp, rsp

	mov al, 0x04
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getDay:
	push rbp
	mov rbp, rsp

	mov al, 0x07
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getMonth: 
	push rbp
	mov rbp, rsp

	mov al, 0x08
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getYear:
	push rbp
	mov rbp, rsp

	mov al, 0x09
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getScanCode: 
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


