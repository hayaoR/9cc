.intel_syntax noprefix
.global main
sum:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov rax, rbp
    mov rax, rbp
    sub rax, 8
    push rax
    push 3
    pop rdi
    pop rax
    mov [rax], rdi
    push rdi
    pop rax
    mov rax, rbp
    sub rax, 16
    push rax
    push 6
    pop rdi
    pop rax
    mov [rax], rdi
    push rdi
    pop rax
    mov rax, rbp
    sub rax, 8
    push rax
    pop rax
    mov rax, [rax]
    push rax
    mov rax, rbp
    sub rax, 16
    push rax
    pop rax
    mov rax, [rax]
    push rax
    pop rdi
    pop rax
    add rax, rdi
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    mov rsp, rbp
    pop rbp
    ret
main:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov rax, rbp
    mov rax, rsp
    and rax, 15
    jnz .L.call.1
    mov rax, 0
    call sum
    jmp .L.end.1
.L.call.1:
    sub rsp, 8
    mov rax, 0
    call sum
    add rsp, 8
.L.end.1:
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    mov rsp, rbp
    pop rbp
    ret
