.CODE

InfAmmoFirearms PROC
	mov edx, eax
	mov [rsp + 50h], eax
	mov eax, esi
	cmovne eax, edx ; "energy2"
	mov [rsp + 40h], eax


	mov rax, [rbp] ; "loan" *

	cmp rax, 0
	je IsEnemy
	xor r12d, r12d ; r12 will be subtracted from the clip ammo
	jmp TheEnd
IsEnemy:
	; mov r12d, [rsp + 40h] ; uncomment to make enemies have a little less ammo...
TheEnd:
	xor rax, rax
	mov eax, [rsp + 40h] ; * put back

	int 3
	int 3
	int 3
InfAmmoFirearms ENDP

END