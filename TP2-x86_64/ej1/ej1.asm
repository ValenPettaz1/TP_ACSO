; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text
    global string_proc_list_create
    global string_proc_node_create
    global string_proc_list_add_node
    global string_proc_list_concat

    extern malloc
    extern free
    extern strlen
    extern strcpy
    extern str_concat

; -------------------------------
; string_proc_list* string_proc_list_create()
; -------------------------------
string_proc_list_create:
    mov rdi, 16                ; sizeof(string_proc_list)
    call malloc
    mov qword [rax], 0         ; first = NULL
    mov qword [rax+8], 0       ; last  = NULL
    ret

; -------------------------------
; string_proc_node* string_proc_node_create(uint8_t type, char* hash)
; rdi = type, rsi = hash
; -------------------------------
string_proc_node_create:
    push rbx
    mov rbx, rdi               ; guardar type
    mov rdi, 32                ; sizeof(string_proc_node)
    call malloc
    mov byte  [rax], bl        ; node->type
    mov qword [rax+8], rsi     ; node->hash
    mov qword [rax+16], 0      ; node->next = NULL
    mov qword [rax+24], 0      ; node->previous = NULL
    pop rbx
    ret

; -------------------------------
; void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash)
; rdi = list, sil = type, rdx = hash
; -------------------------------
string_proc_list_add_node:
    movzx rsi, sil
    mov rdi, rsi               ; type
    mov rsi, rdx               ; hash
    call string_proc_node_create
    mov rcx, rdi               ; list
    mov rdx, rax               ; node

    mov rax, [rcx]             ; list->first
    test rax, rax
    je .empty_list

    ; lista no vacía
    mov rax, [rcx + 8]         ; list->last
    mov [rax + 16], rdx        ; last->next = node
    mov [rdx + 24], rax        ; node->previous = last
    mov [rcx + 8], rdx         ; list->last = node
    ret

.empty_list:
    mov [rcx], rdx             ; list->first = node
    mov [rcx + 8], rdx         ; list->last = node
    ret

; -------------------------------
; char* string_proc_list_concat(string_proc_list* list, uint8_t type, char* prefix)
; rdi = list, sil = type, rdx = prefix
; -------------------------------
string_proc_list_concat:
    ; strlen(prefix) + 1
    mov rdi, rdx
    call strlen
    add rax, 1
    mov rdi, rax
    call malloc

    mov rbx, rax               ; new_hash
    mov rdi, rbx
    mov rsi, rdx
    call strcpy

    mov r8, rdi                ; list
    mov r9b, sil               ; type
    mov rsi, [r8]              ; current_node = list->first

.loop_concat:
    test rsi, rsi
    je .end_concat

    mov al, [rsi]              ; current_node->type
    cmp al, r9b
    jne .next_node

    mov rdi, rbx               ; new_hash
    mov rsi, [rsi + 8]         ; current_node->hash
    call str_concat
    mov rdi, rbx
    call free
    mov rbx, rax

.next_node:
    mov rsi, [rsi + 16]        ; current_node = current_node->next
    jmp .loop_concat

.end_concat:
    mov rdi, r8                ; list
    mov sil, r9b               ; type
    mov rdx, rbx               ; new_hash
    call string_proc_list_add_node
    mov rax, rbx               ; return new_hash
    ret


