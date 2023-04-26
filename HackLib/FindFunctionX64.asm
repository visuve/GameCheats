IFDEF RAX

.CODE

; KERNEL32 is always loaded, so these are always the same, no matter which process
; It ain't pretty, but it works
EXTERN GetModuleHandleA: PROC
EXTERN GetProcAddress: PROC

FindFunctionAsm PROC
	nop ; NOTE: the function prototype is wrong in Process.cpp, hence the fill
	push rbx
	sub rsp, 20h
	mov rbx, rcx
	test rbx, rbx
	jne GetDllHandle
	mov eax, 1
	jmp TheEnd
GetDllHandle:
	mov rax, GetModuleHandleA
	call rax
	test rax, rax
	jne GetFunctionPointer
	mov eax, 2
	jmp TheEnd

GetFunctionPointer:
	mov rcx, rax
	lea rdx, [rbx+400h]
	mov rax, GetProcAddress
	call rax
	test rax, rax
	jne StoreResult
	mov eax, 3
	jmp TheEnd

StoreResult:
	mov [rbx+800h], rax
	xor eax, eax

TheEnd:
	add rsp, 20h
	pop rbx
	ret

	int 3
	int 3
	int 3

FindFunctionAsm ENDP

ENDIF ; RAX
END