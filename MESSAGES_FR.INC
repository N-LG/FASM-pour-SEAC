messagefr:
; flat assembler core
; Copyright (c) 1999-2019, Tomasz Grysztar.
; All rights reserved.

_out_of_memory db 'pas assez de mémoire',0
_stack_overflow db 'débordement de pile',0
_main_file_not_found db 'fichier source non trouvé',0
_unexpected_end_of_file db 'fin de fichier non attendu',0
_code_cannot_be_generated db 'le code ne peut pas être généré',0
_format_limitations_exceeded db 'limite du format de fichier dépassé',0
_invalid_definition db 'définition fournie non valide',0
_write_failed db "erreur d'écriture",0
_file_not_found db 'fichier non trouvé',0
_error_reading_file db 'erreur de lecture du fichier',0
_invalid_file_format db 'format de fichier inccorecte',0
_invalid_macro_arguments db 'argument de macro non conforme',0
_incomplete_macro db 'macro incomplete',0
_unexpected_characters db 'caractères non attendu',0
_invalid_argument db 'argument invalide',0
_illegal_instruction db 'instruction illégale',0
_invalid_operand db 'opérande invalide',0
_invalid_operand_size db "taille d'opérande non valide",0
_operand_size_not_specified db "taille d'opérande non spécifiée",0
_operand_sizes_do_not_match db "les tailles d'opérande ne correspondent pas",0
_invalid_address_size db "taille d'adresse non valide",0
_address_sizes_do_not_agree db "les tailles d'adresse ne correspondent pas",0
_disallowed_combination_of_registers db 'combinaison de registres non autorisée',0
_long_immediate_not_encodable db 'non encodable avec immédiat long',0
_relative_jump_out_of_range db 'saut relatif hors de portée',0
_invalid_expression db 'expression invalide',0
_invalid_address db 'adresse invalide',0
_invalid_value db 'valeur invalide',0
_value_out_of_range db 'valeur en dehors des limites',0
_undefined_symbol db 'symbole non définie',0
_symbol_out_of_scope_1 db 'symbole',0
_symbol_out_of_scope_2 db 'hors de portée',0
_invalid_use_of_symbol db "uttilisation incorrect d'un symbole",0
_name_too_long db 'nom trop long',0
_invalid_name db 'nom incorrect',0
_reserved_word_used_as_symbol db 'mot réservé uttilisé comme symbole',0
_symbol_already_defined db 'symbole déja définis',0
_missing_end_quote db 'marque de fin manquante',0
_missing_end_directive db 'directive de fin manquante',0
_unexpected_instruction db 'instruction inattendue',0
_extra_characters_on_line db 'caractères en trop sur la ligne',0
_section_not_aligned_enough db "la section n'est pas suffisamment alignée",0
_setting_already_specified db 'paramètre déjà spécifié',0
_data_already_defined db 'données déjà définies',0
_too_many_repeats db 'trop de répétitions',0
_invoked_error db "directive d'erreur rencontrée dans le fichier source",0
_assertion_failed db 'assertion échouée',0
