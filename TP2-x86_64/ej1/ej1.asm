; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text
    global string_proc_list_create_asm
    global string_proc_node_create_asm
    global string_proc_list_add_node_asm
    global string_proc_list_concat_asm

    extern malloc
    extern free
    extern strlen
    extern strcpy
    extern str_concat

; ----------------------------------------
; string_proc_list* string_proc_list_create()
; ----------------------------------------
string_proc_list_create_asm:
    mov rdi, 16                ; sizeof(string_proc_list)
    call malloc
    mov qword [rax], 0         ; list->first = NULL
    mov qword [rax + 8], 0     ; list->last = NULL
    ret

; ----------------------------------------
; string_proc_node* string_proc_node_create(uint8_t type, char* hash)
; rdi = type, rsi = hash
; ----------------------------------------
string_proc_node_create_asm:
    push rbx
    mov rbx, rdi               ; guardar type
    mov rdi, 32                ; sizeof(string_proc_node)
    call malloc
    mov byte  [rax], bl        ; node->type = type
    mov qword [rax + 8], rsi   ; node->hash = hash
    mov qword [rax + 16], 0    ; node->next = NULL
    mov qword [rax + 24], 0    ; node->previous = NULL
    pop rbx
    ret

; ----------------------------------------
; void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash)
; rdi = list, sil = type, rdx = hash
; ----------------------------------------
string_proc_list_add_node_asm:
    movzx rsi, sil             ; pasar type a rsi
    mov rdi, rsi               ; rdi = type
    mov rsi, rdx               ; rsi = hash
    call string_proc_node_create_asm
    mov rcx, rdi               ; rcx = list
    mov rdx, rax               ; rdx = node

    mov rax, [rcx]             ; rax = list->first
    test rax, rax
    je .empty_list

    ; lista no vacía
    mov rax, [rcx + 8]         ; rax = list->last
    mov [rax + 16], rdx        ; last->next = node
    mov [rdx + 24], rax        ; node->previous = last
    mov [rcx + 8], rdx         ; list->last = node
    ret

.empty_list:
    mov [rcx], rdx             ; list->first = node
    mov [rcx + 8], rdx         ; list->last = node
    ret

; ----------------------------------------
; char* string_proc_list_concat(string_proc_list* list, uint8_t type, char* prefix)
; rdi = list, sil = type, rdx = prefix
; ----------------------------------------
string_proc_list_concat_asm:
    ; calcular tamaño del nuevo hash
    mov rdi, rdx
    call strlen
    add rax, 1
    mov rdi, rax
    call malloc

    mov rbx, rax               ; new_hash
    mov rdi, rbx
    mov rsi, rdx
    call strcpy

    mov r8, rdi                ; r8 = list
    mov r9b, sil               ; r9b = type
    mov r10, [r8]              ; r10 = current_node = list->first

.loop_concat:
    test r10, r10
    je .end_concat

    mov al, [r10]              ; current_node->type
    cmp al, r9b
    jne .next_node

    mov rdi, rbx               ; rdi = new_hash
    mov rsi, [r10 + 8]         ; rsi = current_node->hash
    call str_concat
    mov rdi, rbx
    call free
    mov rbx, rax               ; new_hash = result

.next_node:
    mov r10, [r10 + 16]        ; current_node = current_node->next
    jmp .loop_concat

.end_concat:
    mov rdi, r8                ; list
    mov sil, r9b               ; type
    mov rdx, rbx               ; new_hash
    call string_proc_list_add_node_asm
    mov rax, rbx               ; return new_hash
    ret

section .data
; No se usan mensajes en esta versión
