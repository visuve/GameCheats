.CODE

InfAmmo PROC
	; When the map is initializing, r10 holds this weird constant
	; 0x0E38E38E38E38E39 which can be found in several locations
	; within the binary.
	mov rax,[1400DDCF4h]
	cmp r10, rax
	je Original
	; If the enemy is firing, 1st value in stack is a pointer.
	; Otherwise it is the amount of projectiles.
	pop rax
	cmp rax, 255
	jg PushRax
	mov ebp, 999
PushRax:
	push rax
Original:
	mov [rcx+r14*4],ebp
	lea rcx,[rbx-000003F8h]
	mov rax,[rbx-000003F8h]

	int 3
	int 3
	int 3
InfAmmo ENDP

END