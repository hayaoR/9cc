.intel_syntax noprefix
.global main
sum:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov rax, rbp
    mov rax, rbp
    sub rax, 8
    mov [rax], rdi
    mov rax, rbp
    sub rax, 16
    mov [rax], rsi
    mov rax, rbp
    sub rax, 24
    push rax
    push 0
    pop rdi
    pop rax
    mov [rax], rdi
    push rdi
    pop rax
    mov rax, rbp
    sub rax, 32
    push rax
    mov rax, rbp
    sub rax, 8
    push rax
    pop rax
    mov rax, [rax]
    push rax
    pop rdi
    pop rax
    mov [rax], rdi
    push rdi
.Lbegin1:
    mov rax, rbp
    sub rax, 32
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
    cmp rax, rdi
    setle al
    movzb rax, al
    push rax
    pop rax
    cmp rax, 0
    je .Lend1
    mov rax, rbp
    sub rax, 24
    push rax
    mov rax, rbp
    sub rax, 24
    push rax
    pop rax
    mov rax, [rax]
    push rax
    mov rax, rbp
    sub rax, 32
    push rax
    pop rax
    mov rax, [rax]
    push rax
    pop rdi
    pop rax
    add rax, rdi
    push rax
    pop rdi
    pop rax
    mov [rax], rdi
    push rdi
    mov rax, rbp
    sub rax, 32
    push rax
    mov rax, rbp
    sub rax, 32
    push rax
    pop rax
    mov rax, [rax]
    push rax
    push 1
    pop rdi
    pop rax
    add rax, rdi
    push rax
    pop rdi
    pop rax
    mov [rax], rdi
    push rdi
    jmp .Lbegin1
.Lend1:
    pop rax
    mov rax, rbp
    sub rax, 24
    push rax
    pop rax
    mov rax, [rax]
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
    push 1
    pop rax
    mov rdi, rax
    push 10
    pop rax
    mov rsi, rax
    mov rax, rsp
    and rax, 15
    jnz .L.call.2
    mov rax, 0
    call sum
    jmp .L.end.2
.L.call.2:
    sub rsp, 8
    mov rax, 0
    call sum
    add rsp, 8
.L.end.2:
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    mov rsp, rbp
    pop rbp
    ret
