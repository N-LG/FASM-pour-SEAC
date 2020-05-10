fasmfr:
; flat assembler interface for SELG
; Copyright (c) 1999-2019, Tomasz Grysztar.
; All rights reserved.


; adapté par Nicolas Leprince-Granger



pile equ 4096 ;definition de la taille de la pile
include "../../PROG/fe.inc"
db "Compilateur FASM"
scode:
org 0

;données du segment CS

	mov ax,sel_dat1
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax

	mov esi,_logo
	call display_string

	call	get_params
	jc	information

	mov	dword[stack_limit],pile

	mov	dword[additional_memory],sdata2
	mov	ecx,[memory_setting]         ;mémoir a uttiliser en Ko
	shl	ecx,10
	jnz	allocate_memory
	mov	ecx,1000000h                   
      allocate_memory:                      
	mov	eax,8
	add	ecx,[additional_memory]     
	mov	dx,sel_dat1
	int	61h
	mov	[memory_end],ecx
	sub	ecx,[additional_memory]
	shr	ecx,2
	add	ecx,[additional_memory]
	mov	[additional_memory_end],ecx
	mov	[memory_start],ecx
	

	mov	esi,_memory_prefix
	call	display_string
	mov	eax,[memory_end]
	sub	eax,[memory_start]
	add	eax,[additional_memory_end]
	sub	eax,[additional_memory]
	shr	eax,10
	call	display_number
	mov	esi,_memory_suffix
	call	display_string


	call	make_timestamp
	mov	[start_time],eax

	and	[preprocessing_done],0
	call	preprocessor
	or	[preprocessing_done],-1
	call	parser
	call	assembler
	call	formatter

	call	display_user_messages
	movzx	eax,[current_pass]
	inc	eax
	call	display_number

	mov	esi,_passes_suffix
	call	display_string

	call	make_timestamp
	sub	eax,[start_time]
;	jnc	time_ok
;	add	eax,3600000
;      time_ok:
	xor	edx,edx
	mov	ebx,400
	div	ebx
	or	eax,eax
	jz	display_bytes_count
	xor	edx,edx
	mov	ebx,10
	div	ebx
	push	edx
	call	display_number
	mov	dl,'.'
	call	display_character
	pop	eax
	call	display_number
	mov	esi,_seconds_suffix
	call	display_string
      display_bytes_count:
	mov	eax,[written_size]
	call	display_number
	mov	esi,_bytes_suffix
	call	display_string
	xor	al,al
	jmp	exit_program


;*********************************************
information:
	mov	esi,_usage
	call	display_string
	mov	al,1
	jmp	exit_program


;*********************************************
get_params:
	mov	[input_file],0
	mov	[output_file],0
	mov	[symbols_file],0
	mov	[memory_setting],0
	mov	[passes_limit],100


	mov	eax,3
	mov	edx,commande
	int	61h


	mov	[definitions_pointer],predefinitions
	mov	esi,commande
	mov	edi,params
    find_command_start:
	lodsb
	cmp	al,20h
	je	find_command_start
	cmp	al,22h
	je	skip_quoted_name
    skip_name:
	lodsb
	cmp	al,20h
	je	find_param
	or	al,al
	jz	all_params
	jmp	skip_name
    skip_quoted_name:
	lodsb
	cmp	al,22h
	je	find_param
	or	al,al
	jz	all_params
	jmp	skip_quoted_name
    find_param:
	lodsb
	cmp	al,20h
	je	find_param
	cmp	al,'-'
	je	option_param
	cmp	al,0Dh
	je	all_params
	or	al,al
	jz	all_params
	cmp	[input_file],0
	jne	get_output_file
	mov	[input_file],edi
	jmp	process_param
      get_output_file:
	cmp	[output_file],0
	jne	bad_params
	mov	[output_file],edi
    process_param:
	cmp	al,22h
	je	string_param
    copy_param:
	cmp	edi,params+1000h
	jae	bad_params
	stosb
	lodsb
	cmp	al,20h
	je	param_end
	cmp	al,0Dh
	je	param_end
	or	al,al
	jz	param_end
	jmp	copy_param
    string_param:
	lodsb
	cmp	al,22h
	je	string_param_end
	cmp	al,0Dh
	je	param_end
	or	al,al
	jz	param_end
	cmp	edi,params+1000h
	jae	bad_params
	stosb
	jmp	string_param
    option_param:
	lodsb
	cmp	al,'m'
	je	memory_option
	cmp	al,'M'
	je	memory_option
	cmp	al,'p'
	je	passes_option
	cmp	al,'P'
	je	passes_option
	cmp	al,'d'
	je	definition_option
	cmp	al,'D'
	je	definition_option
	cmp	al,'s'
	je	symbols_option
	cmp	al,'S'
	je	symbols_option
    bad_params:
	stc
	ret
    get_option_value:
	xor	eax,eax
	mov	edx,eax
    get_option_digit:
	lodsb
	cmp	al,20h
	je	option_value_ok
	cmp	al,0Dh
	je	option_value_ok
	or	al,al
	jz	option_value_ok
	sub	al,30h
	jc	invalid_option_value
	cmp	al,9
	ja	invalid_option_value
	imul	edx,10
	jo	invalid_option_value
	add	edx,eax
	jc	invalid_option_value
	jmp	get_option_digit
    option_value_ok:
	dec	esi
	clc
	ret
    invalid_option_value:
	stc
	ret
    memory_option:
	lodsb
	cmp	al,20h
	je	memory_option
	cmp	al,0Dh
	je	bad_params
	or	al,al
	jz	bad_params
	dec	esi
	call	get_option_value
	or	edx,edx
	jz	bad_params
	cmp	edx,1 shl (32-10)
	jae	bad_params
	mov	[memory_setting],edx
	jmp	find_param
    passes_option:
	lodsb
	cmp	al,20h
	je	passes_option
	cmp	al,0Dh
	je	bad_params
	or	al,al
	jz	bad_params
	dec	esi
	call	get_option_value
	or	edx,edx
	jz	bad_params
	cmp	edx,10000h
	ja	bad_params
	mov	[passes_limit],dx
	jmp	find_param
    definition_option:
	lodsb
	cmp	al,20h
	je	definition_option
	cmp	al,0Dh
	je	bad_params
	or	al,al
	jz	bad_params
	dec	esi
	push	edi
	mov	edi,[definitions_pointer]
	call	convert_definition_option
	mov	[definitions_pointer],edi
	pop	edi
	jc	bad_params
	jmp	find_param
    symbols_option:
	mov	[symbols_file],edi
      find_symbols_file_name:
	lodsb
	cmp	al,20h
	jne	process_param
	jmp	find_symbols_file_name
    param_end:
	dec	esi
    string_param_end:
	cmp	edi,params+1000h
	jae	bad_params
	xor	al,al
	stosb
	jmp	find_param
    all_params:
	cmp	[input_file],0
	je	bad_params
	mov	eax,[definitions_pointer]
	mov	byte [eax],0
	mov	[initial_definitions],predefinitions
	clc
	ret
    convert_definition_option:
	mov	ecx,edi
	cmp	edi,predefinitions+1000h
	jae	bad_definition_option
	xor	al,al
	stosb
      copy_definition_name:
	lodsb
	cmp	al,'='
	je	copy_definition_value
	cmp	al,20h
	je	bad_definition_option
	cmp	al,0Dh
	je	bad_definition_option
	or	al,al
	jz	bad_definition_option
	cmp	edi,predefinitions+1000h
	jae	bad_definition_option
	stosb
	inc	byte [ecx]
	jnz	copy_definition_name
      bad_definition_option:
	stc
	ret
      copy_definition_value:
	lodsb
	cmp	al,20h
	je	definition_value_end
	cmp	al,0Dh
	je	definition_value_end
	or	al,al
	jz	definition_value_end
	cmp	al,'\'
	jne	definition_value_character
	cmp	byte [esi],20h
	jne	definition_value_character
	lodsb
      definition_value_character:
	cmp	edi,predefinitions+1000h
	jae	bad_definition_option
	stosb
	jmp	copy_definition_value
      definition_value_end:
	dec	esi
	cmp	edi,predefinitions+1000h
	jae	bad_definition_option
	xor	al,al
	stosb
	clc
	ret











include '..\errors.inc'
include '..\symbdump.inc'
include '..\preproce.inc'
include '..\parser.inc'
include '..\exprpars.inc'
include '..\assemble.inc'
include '..\exprcalc.inc'
include '..\formats.inc'
include '..\x86_64.inc'
include '..\avx.inc'



include 'system.inc'

include 'messages_fr.inc'    ;message d'erreur en français
;include '..\messages.inc'    ;message d'erreur en anglais

include '..\tables.inc'
include '..\variable.inc'
include '..\version.inc'

_copyright db 'Copyright (c) 1999-2019, Tomasz Grysztar',13,0

_logo db 'flat assembler  version ',VERSION_STRING,0
_usage db 13
       db 'syntaxe de commande: fasm <source> [output]',13
       db 'options:',13
       db ' -m <limite>        définis la limite en Kilo-octets de la mémoire uttilisable',13
       db ' -p <limite>        définis le nombre maximum de passes',13
       db " -d <name>=<value>  definis la valeur d'une variable",13
       db ' -s <file>          créer un fichier de coresspondance de symbole pour le débuggage',13,0
_memory_prefix db '  (',0
_memory_suffix db ' kiloctets de mémoire)',13,0
_passes_suffix db ' passes, ',0
_seconds_suffix db ' secondes, ',0
_bytes_suffix db ' octets.',13,0



command_line dd ?
memory_setting dd ?
definitions_pointer dd ?
environment dd ?
start_time dd ?
displayed_count dd ?
last_displayed db ?
character dw ?
preprocessing_done db ?

predefinitions rb 1000h

params rb 1000h

sdata2:
org 0
;données du segment ES
sdata3:
org 0
;données du segment FS
sdata4:
org 0
;données du segment GS
findata:
