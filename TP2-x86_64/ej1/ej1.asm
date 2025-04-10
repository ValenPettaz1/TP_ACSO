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
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi          ; r12 = list
    mov r13b, sil         ; r13b = type
    mov r14, rdx          ; r14 = prefix

    ; new_hash = malloc(strlen(prefix) + 1)
    mov rdi, r14
    call strlen
    add rax, 1
    mov rdi, rax
    call malloc
    mov rbx, rax          ; rbx = new_hash

    ; strcpy(new_hash, prefix)
    mov rdi, rbx
    mov rsi, r14
    call strcpy

    ; current_node = list->first
    mov r15, [r12]        ; r15 = current_node

.loop_concat:
    test r15, r15
    je .end_concat

    mov al, [r15]         ; current_node->type
    cmp al, r13b
    jne .next_node

    ; concatenar new_hash con current_node->hash
    mov rdi, rbx
    mov rsi, [r15 + 8]    ; current_node->hash
    call str_concat

    ; liberar old new_hash
    mov rdi, rbx
    call free

    ; actualizar new_hash
    mov rbx, rax

.next_node:
    mov r15, [r15 + 16]   ; current_node = current_node->next
    jmp .loop_concat

.end_concat:
    ; agregar a la lista
    mov rdi, r12
    mov sil, r13b
    mov rdx, rbx
    call string_proc_list_add_node_asm

    mov rax, rbx

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    ret


