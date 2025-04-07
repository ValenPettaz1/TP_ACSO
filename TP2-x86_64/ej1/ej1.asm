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

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat


string_proc_list_create_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 16
    mov     edi, 16
    call    malloc
    mov     QWORD PTR [rbp-8], rax
    mov     rax, QWORD PTR [rbp-8]
    mov     QWORD PTR [rax], 0
    mov     rax, QWORD PTR [rbp-8]
    mov     QWORD PTR [rax+8], 0
    mov     rax, QWORD PTR [rbp-8]
    leave
    ret


string_proc_node_create_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32
    mov     eax, edi
    mov     QWORD PTR [rbp-32], rsi
    mov     BYTE PTR [rbp-20], al
    mov     edi, 32
    call    malloc
    mov     QWORD PTR [rbp-8], rax
    mov     rax, QWORD PTR [rbp-8]
    movzx   edx, BYTE PTR [rbp-20]
    mov     BYTE PTR [rax+16], dl
    mov     rax, QWORD PTR [rbp-8]
    mov     rdx, QWORD PTR [rbp-32]
    mov     QWORD PTR [rax+24], rdx
    mov     rax, QWORD PTR [rbp-8]
    mov     QWORD PTR [rax], 0
    mov     rax, QWORD PTR [rbp-8]
    mov     QWORD PTR [rax+8], 0
    mov     rax, QWORD PTR [rbp-8]
    leave
    ret

string_proc_list_add_node_asm:

string_proc_list_concat_asm:

