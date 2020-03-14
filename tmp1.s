.intel_syntax noprefix
.global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov rax, rbp
    push 5
#rdi
    pop rdi
    push 20
#rax
    pop rax
    add rax, rdi
    push rax
#rdi
    pop rdi
    push 4
#rax
    pop rax
    sub rax, rdi
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    mov rsp, rbp
    pop rbp
    ret
