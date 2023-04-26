IFNDEF RAX

.486
.MODEL FLAT, C

.CODE

; KERNEL32 is always loaded, so these are always the same, no matter which process
; It ain't pretty, but it works
EXTERN GetModuleHandleA@4: PROC
EXTERN GetProcAddress@8: PROC

FindFunctionAsm PROC
	nop ; NOTE: the function prototype is wrong in Process.cpp, hence the fill
	push ebp
	mov ebp, esp
	sub esp, 8
	push edi
	push esi

	mov esi, [ebp+8]

	test esi, esi
	jne GetDllHandle
	mov eax, 1
	jmp TheEnd
GetDllHandle:
	push esi
	mov eax, GetModuleHandleA@4
	call eax
	test eax, eax
	jne GetFunctionPointer
	mov eax, 2
	jmp TheEnd

GetFunctionPointer:
	lea ecx, [esi+400h]
	push ecx
	push eax
	mov eax, GetProcAddress@8
	call eax
	test eax, eax
	jne StoreResult
	mov eax, 3
	jmp TheEnd

StoreResult:
	mov [esi+800h], eax
	xor eax, eax

TheEnd:
	pop esi
	pop edi
	mov esp, ebp
	pop ebp
	ret

	int 3
	int 3
	int 3

FindFunctionAsm ENDP

ENDIF ; RAX
END