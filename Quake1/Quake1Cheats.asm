.CODE

InfAmmo PROC
	movsxd rdx, dword ptr [rbx+rsi*4] ; Stolen
	mov rax, [1418EA0C0h] ; Stolen
	mov ecx, [rbx+rdi*4] ; Stolen

	cmp rdx, 000005ACh
	je Stab
	cmp rdx, 000005B0h
	je Stab
	cmp rdx, 000005B4h
	je Stab
	cmp rdx, 000005B8h
	je Stab
	cmp rdx, 000005BCh
	je Stab
	jmp Store
Stab:
	mov ecx, 437F0000h ; 255 as float
Store:
	mov [rdx+rax],ecx ; Stolen. The annoying bytes.
	int 3
	int 3
	int 3
InfAmmo ENDP

END