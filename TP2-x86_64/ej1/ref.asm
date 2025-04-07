string_proc_list_create():
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
string_proc_node_create(unsigned char, char*):
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
string_proc_list_add_node(string_proc_list_t*, unsigned char, char*):
        push    rbp
        mov     rbp, rsp
        sub     rsp, 48
        mov     QWORD PTR [rbp-24], rdi
        mov     eax, esi
        mov     QWORD PTR [rbp-40], rdx
        mov     BYTE PTR [rbp-28], al
        movzx   eax, BYTE PTR [rbp-28]
        mov     rdx, QWORD PTR [rbp-40]
        mov     rsi, rdx
        mov     edi, eax
        call    string_proc_node_create(unsigned char, char*)
        mov     QWORD PTR [rbp-8], rax
        mov     rax, QWORD PTR [rbp-24]
        mov     rax, QWORD PTR [rax]
        test    rax, rax
        jne     .L6
        mov     rax, QWORD PTR [rbp-24]
        mov     rdx, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax], rdx
        mov     rax, QWORD PTR [rbp-24]
        mov     rdx, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax+8], rdx
        jmp     .L8
.L6:
        mov     rax, QWORD PTR [rbp-24]
        mov     rax, QWORD PTR [rax+8]
        mov     rdx, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax], rdx
        mov     rax, QWORD PTR [rbp-24]
        mov     rdx, QWORD PTR [rax+8]
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax+8], rdx
        mov     rax, QWORD PTR [rbp-24]
        mov     rdx, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax+8], rdx
.L8:
        nop
        leave
        ret
string_proc_list_concat(string_proc_list_t*, unsigned char, char*):
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        mov     QWORD PTR [rbp-40], rdi
        mov     eax, esi
        mov     QWORD PTR [rbp-56], rdx
        mov     BYTE PTR [rbp-44], al
        mov     rax, QWORD PTR [rbp-56]
        mov     rdi, rax
        call    strlen
        add     rax, 1
        mov     rdi, rax
        call    malloc
        mov     QWORD PTR [rbp-8], rax
        mov     rdx, QWORD PTR [rbp-56]
        mov     rax, QWORD PTR [rbp-8]
        mov     rsi, rdx
        mov     rdi, rax
        call    strcpy
        mov     rax, QWORD PTR [rbp-40]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-16], rax
        jmp     .L10
.L12:
        mov     rax, QWORD PTR [rbp-16]
        movzx   eax, BYTE PTR [rax+16]
        cmp     BYTE PTR [rbp-44], al
        jne     .L11
        mov     rax, QWORD PTR [rbp-16]
        mov     rdx, QWORD PTR [rax+24]
        mov     rax, QWORD PTR [rbp-8]
        mov     rsi, rdx
        mov     rdi, rax
        call    str_concat(char*, char*)
        mov     QWORD PTR [rbp-24], rax
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    free
        mov     rax, QWORD PTR [rbp-24]
        mov     QWORD PTR [rbp-8], rax
.L11:
        mov     rax, QWORD PTR [rbp-16]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-16], rax
.L10:
        cmp     QWORD PTR [rbp-16], 0
        jne     .L12
        movzx   ecx, BYTE PTR [rbp-44]
        mov     rdx, QWORD PTR [rbp-8]
        mov     rax, QWORD PTR [rbp-40]
        mov     esi, ecx
        mov     rdi, rax
        call    string_proc_list_add_node(string_proc_list_t*, unsigned char, char*)
        mov     rax, QWORD PTR [rbp-8]
        leave
        ret
string_proc_list_destroy(string_proc_list_t*):
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     QWORD PTR [rbp-24], rdi
        mov     rax, QWORD PTR [rbp-24]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-8], rax
        mov     QWORD PTR [rbp-16], 0
        jmp     .L15
.L16:
        mov     rax, QWORD PTR [rbp-8]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-16], rax
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    string_proc_node_destroy(string_proc_node_t*)
        mov     rax, QWORD PTR [rbp-16]
        mov     QWORD PTR [rbp-8], rax
.L15:
        cmp     QWORD PTR [rbp-8], 0
        jne     .L16
        mov     rax, QWORD PTR [rbp-24]
        mov     QWORD PTR [rax], 0
        mov     rax, QWORD PTR [rbp-24]
        mov     QWORD PTR [rax+8], 0
        mov     rax, QWORD PTR [rbp-24]
        mov     rdi, rax
        call    free
        nop
        leave
        ret
string_proc_node_destroy(string_proc_node_t*):
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], rdi
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax], 0
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax+8], 0
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax+24], 0
        mov     rax, QWORD PTR [rbp-8]
        mov     BYTE PTR [rax+16], 0
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    free
        nop
        leave
        ret
str_concat(char*, char*):
        push    rbp
        mov     rbp, rsp
        sub     rsp, 48
        mov     QWORD PTR [rbp-40], rdi
        mov     QWORD PTR [rbp-48], rsi
        mov     rax, QWORD PTR [rbp-40]
        mov     rdi, rax
        call    strlen
        mov     DWORD PTR [rbp-4], eax
        mov     rax, QWORD PTR [rbp-48]
        mov     rdi, rax
        call    strlen
        mov     DWORD PTR [rbp-8], eax
        mov     edx, DWORD PTR [rbp-4]
        mov     eax, DWORD PTR [rbp-8]
        add     eax, edx
        mov     DWORD PTR [rbp-12], eax
        mov     eax, DWORD PTR [rbp-12]
        add     eax, 1
        cdqe
        mov     rdi, rax
        call    malloc
        mov     QWORD PTR [rbp-24], rax
        mov     rdx, QWORD PTR [rbp-40]
        mov     rax, QWORD PTR [rbp-24]
        mov     rsi, rdx
        mov     rdi, rax
        call    strcpy
        mov     rdx, QWORD PTR [rbp-48]
        mov     rax, QWORD PTR [rbp-24]
        mov     rsi, rdx
        mov     rdi, rax
        call    strcat
        mov     rax, QWORD PTR [rbp-24]
        leave
        ret
.LC0:
        .string "List length: %d\n"
.LC1:
        .string "\tnode hash: %s | type: %d\n"
string_proc_list_print(string_proc_list_t*, _IO_FILE*):
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     QWORD PTR [rbp-24], rdi
        mov     QWORD PTR [rbp-32], rsi
        mov     DWORD PTR [rbp-4], 0
        mov     rax, QWORD PTR [rbp-24]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-16], rax
        jmp     .L21
.L22:
        add     DWORD PTR [rbp-4], 1
        mov     rax, QWORD PTR [rbp-16]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-16], rax
.L21:
        cmp     QWORD PTR [rbp-16], 0
        jne     .L22
        mov     edx, DWORD PTR [rbp-4]
        mov     rax, QWORD PTR [rbp-32]
        mov     esi, OFFSET FLAT:.LC0
        mov     rdi, rax
        mov     eax, 0
        call    fprintf
        mov     rax, QWORD PTR [rbp-24]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-16], rax
        jmp     .L23
.L24:
        mov     rax, QWORD PTR [rbp-16]
        movzx   eax, BYTE PTR [rax+16]
        movzx   ecx, al
        mov     rax, QWORD PTR [rbp-16]
        mov     rdx, QWORD PTR [rax+24]
        mov     rax, QWORD PTR [rbp-32]
        mov     esi, OFFSET FLAT:.LC1
        mov     rdi, rax
        mov     eax, 0
        call    fprintf
        mov     rax, QWORD PTR [rbp-16]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rbp-16], rax
.L23:
        cmp     QWORD PTR [rbp-16], 0
        jne     .L24
        nop
        nop
        leave
        ret