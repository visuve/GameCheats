.CODE

InfAmmoFirearms PROC
	mov [rsp + 000001E0h], r13 ; stolen
	mov [rsp + 000001A0h], r15 ; stolen

	mov r15, [rbp] ; "loan" *

	cmp r15, 0
	je TheEnd
	xor r12, r12 ; r12 will be subtracted from the clip ammo
	jmp TheEnd
IsEnemy:
	; For some odd reason, this do not seem to have any effect
	; on the enemies, even though it should. 
	; mov r12, 255
TheEnd:
	mov r15, [rsp+000001A0h] ; * put back

	int 3
	int 3
	int 3
InfAmmoFirearms ENDP

END