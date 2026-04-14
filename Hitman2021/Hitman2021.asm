.CODE

InfAmmo PROC
	test rbx, rbx
	jz IsEnemy
	mov eax, 1000
	ret

IsEnemy:
	mov eax, [rax+rcx*4] ; Original instruction for enemies
	ret
InfAmmo ENDP

END