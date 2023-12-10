.CODE

InfAmmo PROC
	cmp bx, 5200h
	je IsEnemy
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