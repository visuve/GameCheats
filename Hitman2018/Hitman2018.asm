.CODE

InfAmmo PROC
	xor ecx, ecx ; stolen
	lea edx, [rax-01] ; stolen
	mov rax, [rdi] ; stolen
	test edx, edx ; stolen
	cmove edx, ecx ; stolen
	mov rcx, rdi ; stolen

	cmp r13, 0
	je IsEnemy
	mov rdx, 9999
	; mov [rcx+358], rdx ; for debugging
IsEnemy:
	nop

	int 3
	int 3
	int 3
InfAmmo ENDP

END