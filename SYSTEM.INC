
; flat assembler interface for SELG
; Copyright (c) 1999-2019, Tomasz Grysztar.
; All rights reserved.
; modifié par Nicolas Leprince-Granger






exit_program:             ;fait ce que son nom indique
	int	60h

get_environment_variable:
	mov	ebx,esi
	mov	esi,[environment]
      compare_variable_names:
	mov	edx,ebx
      compare_character:
	lodsb
	mov	ah,[edx]
	inc	edx
	cmp	al,'='
	je	end_of_variable_name
	or	ah,ah
	jz	next_variable
	sub	ah,al
	jz	compare_character
	cmp	ah,20h
	jne	next_variable
	cmp	al,41h
	jb	next_variable
	cmp	al,5Ah
	jna	compare_character
      next_variable:
	lodsb
	or	al,al
	jnz	next_variable
	cmp	byte [esi],0
	jne	compare_variable_names
	ret
      end_of_variable_name:
	or	ah,ah
	jnz	next_variable
      copy_variable_value:
	lodsb
	cmp	edi,[memory_end]
	jae	out_of_memory
	stosb
	or	al,al
	jnz	copy_variable_value
	dec	edi
	ret



bidon equ 60


open:
pushad
	mov	eax,0
open_create:
	xor	ebx,ebx
	int	64h 
	cmp 	eax,0
	jne file_error

	mov	edi,table_fichier
    boucle_open_create:
	cmp	dword[edi],0
	je trouve_open_create
	add	edi,8
	cmp	edi,table_fichier+512
	jne boucle_open_create

    file_errorpp:
	pop eax
    file_errorp:
	pop eax
    file_error:
	popad
	stc
	ret


    trouve_open_create:
	mov	[edi],ebx
	mov	dword[edi+4],0
	sub	edi,table_fichier
	shr	edi,3
	add	edi,bidon
	ss
	mov	[esp+16],edi ;dans le ebx de la pile
	popad
	clc
	ret

create:
	pushad
	mov	eax,2
	jmp open_create


close:
	pushad
	push	edx
	mov	esi,ebx
	sub	esi,bidon
	cmp	esi,512/8
	jae file_errorp
	shl	edi,3
	add	edi,table_fichier
	mov	ebx,[esi]
	pop	edi
	mov	eax, 1
	int	64h
	cmp 	eax,0
	jne file_error
	popad
	clc
	ret	

write:
	pushad
	push	edx
	mov	edi,ebx
	sub	edi,bidon
	cmp	edi,512/8
	jae file_errorp
	shl	edi,3
	add	edi,table_fichier
	mov	ebx,[edi]
	mov	edx,[edi+4]
	pop	esi
	mov	eax,5
	int	64h
	cmp	eax,0
	jne file_error
	add	[esi+4],ecx
	popad
	clc
	ret

read:
	pushad
	push	edx
	mov	esi,ebx
	sub	esi,bidon
	cmp	esi,512/8
	jae file_errorp
	shl	esi,3
	add	esi,table_fichier
	mov	ebx,[esi]
	mov	edx,[esi+4]
	pop	edi
	mov	eax,4
	int	64h
	cmp	eax,0
	jne file_error
	add	[esi+4],ecx
	popad
	clc
	ret


lseek:
	pushad
	push	eax
	push	edx
	mov	esi,ebx
	sub	esi,bidon
	cmp	esi,512/8
	jae	file_errorpp
	shl	esi,3
	add	esi,table_fichier
	mov	ebx,[esi]
	mov	al,6     ;récupère la taille du fichier
	mov	ah,1
	mov	edx,buffer
	int	64h
	pop	ecx
	pop	edx

	cmp 	eax,0
	jne file_error
	cmp	dl,0
	je lseek_debut
	cmp	dl,1
	je lseek_mil
	cmp	dl,2
	jne file_error

	add ecx,[buffer]
	mov [esi+4],ecx
	jmp lseek_fin

    lseek_mil:
	add [esi+4],ecx
	jmp lseek_fin

    lseek_debut:
	mov [esi+4],ecx

    lseek_fin:
	cmp	ecx,[buffer]
	jbe lseek_ok
	mov	ecx,[buffer]
	mov [esi+4],ecx

    lseek_ok:
	mov	ecx,[esi+4]
	ss
	mov	[esp+28],ecx   ;dans le eax de la pile
	popad
	clc
	ret


display_string:         ;affiche le message pointé par esi
	push    eax
	push    edx
	mov 	edx,esi
	mov 	eax,6
	int 	61h
	pop 	edx
	pop 	eax
	ret

display_character:       ;affiche le caractère dans dl
	push    eax
	push    edx
	mov 	dh,0
	mov	[character],dx
	mov     edx,character
	mov 	eax,6
	int 	61h
	pop 	edx
	pop 	eax
	ret



display_number:
	pushad
	mov	ecx,1000000000
	xor	edx,edx
	xor	bl,bl
      display_loop:
	div	ecx
	push	edx
	cmp	ecx,1
	je	display_digit
	or	bl,bl
	jnz	display_digit
	or	al,al
	jz	digit_ok
	not	bl
      display_digit:
	mov	dl,al
	add	dl,30h
	push	ebx ecx
	call	display_character
	pop	ecx ebx
      digit_ok:
	mov	eax,ecx
	xor	edx,edx
	mov	ecx,10
	div	ecx
	mov	ecx,eax
	pop	eax
	or	ecx,ecx
	jnz	display_loop
	popad	
	ret

display_user_messages:
	mov	[displayed_count],0
	call	show_display_buffer
	cmp	[displayed_count],0
	je	line_break_ok
	cmp	[last_displayed],0Ah
	je	line_break_ok
	mov	dl,13
	call	display_character
      line_break_ok:
	ret

display_block:
	jecxz	block_displayed
	add	[displayed_count],ecx
	mov	al,[esi+ecx-1]
	mov	[last_displayed],al
	push    eax
	push    edx
	mov 	edx,esi
	mov 	eax,6
	int 	61h
	pop 	edx
	pop 	eax
	
      block_displayed:
	ret

fatal_error:
	mov	esi,error_prefix
	call	display_string
	pop	esi
	call	display_string
	mov	esi,error_suffix
	call	display_string
	mov	al,0FFh
	jmp	exit_program


assembler_error:                          ;adresse du texte de l'erreur dans la pile
	call	display_user_messages
	mov	ebx,[current_line]
	test	ebx,ebx
	jz	display_error_message
	push	dword 0
      get_error_lines:
	mov	eax,[ebx]
	cmp	byte [eax],0
	je	get_next_error_line
	push	ebx
	test	byte [ebx+7],80h
	jz	display_error_line
	mov	edx,ebx
      find_definition_origin:
	mov	edx,[edx+12]
	test	byte [edx+7],80h
	jnz	find_definition_origin
	push	edx
      get_next_error_line:
	mov	ebx,[ebx+8]
	jmp	get_error_lines
      display_error_line:
	mov	esi,[ebx]
	call	display_string
	mov	esi,line_number_start
	call	display_string
	mov	eax,[ebx+4]
	and	eax,7FFFFFFFh
	call	display_number
	mov	dl,']'
	call	display_character
	pop	esi
	cmp	ebx,esi
	je	line_number_ok

	mov	dl,20h
	call	display_character
	push	esi
	mov	esi,[esi]
	movzx	ecx,byte [esi]
	inc	esi
	call	display_block
	mov	esi,line_number_start
	call	display_string
	pop	esi
	mov	eax,[esi+4]
	and	eax,7FFFFFFFh
	call	display_number
	mov	dl,']'
	call	display_character
      line_number_ok:

	mov	esi,ebx
	mov	edx,[esi]
	call	open
	jc line_data_displayed
	mov	al,2
	xor	edx,edx
	call	lseek

	mov	edx,[esi+8]
	sub	eax,edx
	jz	line_data_displayed
	push	eax
	xor	al,al
	call	lseek

	pop ecx
	mov	edx,[additional_memory]
	lea	eax,[edx+ecx]
	cmp	eax,[additional_memory_end]
	ja	out_of_memory
	call	read

	call	close


	mov	esi,[additional_memory]
      get_line_data:
	mov	al,[esi]
	cmp	al,0Ah
	je	display_line_data
	cmp	al,0Dh
	je	display_line_data
	cmp	al,1Ah
	je	display_line_data
	or	al,al
	jz	display_line_data
	inc	esi
	loop	get_line_data
      display_line_data:
	mov byte[esi],0
	mov	esi,line_data_start
	call	display_string
	mov	esi,[additional_memory]
	call	display_string
      line_data_displayed:

	mov	esi,lf
	call	display_string
	pop	ebx
	or	ebx,ebx
	jnz	display_error_line
	cmp	[preprocessing_done],0
	je	display_error_message
	mov	esi,preprocessed_instruction_prefix
	call	display_string
	mov	esi,[current_line]
	add	esi,16
	mov	edi,[additional_memory]
	xor	dl,dl
      convert_instruction:
	lodsb
	cmp	al,1Ah
	je	copy_symbol
	cmp	al,22h
	je	copy_symbol
	cmp	al,3Bh
	je	instruction_converted
	stosb
	or	al,al
	jz	instruction_converted
	xor	dl,dl
	jmp	convert_instruction
      copy_symbol:
	or	dl,dl
	jz	space_ok
	mov	byte [edi],20h
	inc	edi
      space_ok:
	cmp	al,22h
	je	quoted
	lodsb
	movzx	ecx,al
	rep	movsb
	or	dl,-1
	jmp	convert_instruction
      quoted:
	mov	al,27h
	stosb
	lodsd
	mov	ecx,eax
	jecxz	quoted_copied
      copy_quoted:
	lodsb
	stosb
	cmp	al,27h
	jne	quote_ok
	stosb
      quote_ok:
	loop	copy_quoted
      quoted_copied:
	mov	al,27h
	stosb
	or	dl,-1
	jmp	convert_instruction
      instruction_converted:
	xor	al,al
	stosb
	mov	esi,[additional_memory]
	call	display_string
	mov	esi,lf
	call	display_string
  
    display_error_message:
	mov	esi,error_prefix
	call	display_string
	pop	esi
	call	display_string
	mov	esi,error_suffix
	call	display_string
	mov	al,2
	jmp	exit_program

make_timestamp:                ;renvoie dans eax(LSB),edx(MSB) un compteur temps
	mov	eax,12
	int	61h
	ret


sdata1:
org 0



error_prefix db 'erreur: ',0
error_suffix db '.'
lf db 13,0
line_number_start db ' [',0
line_data_start db ':',13,0
preprocessed_instruction_prefix db 'processed: ',0


commande:
rb 512
table_fichier:
rb 512
buffer:
dq 0