.CODE

InfAmmo PROC
	cmp r9d, r8d
	jg IsEnemy
	mov eax, 1000
	ret
IsEnemy:
	mov eax,[rax+rcx*4]
	ret
	int 3
	int 3
	int 3
InfAmmo ENDP

END