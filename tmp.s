.intel_syntax noprefix
.global main
fibbonacci:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov rax, rbp
    mov rax, rbp
    sub rax, 8
    mov [rax], rdi
    mov rax, rbp
    sub rax, 8
    push rax
    pop rax
    mov rax, [rax]
    push rax
    push 0
    pop rdi
    pop rax
    cmp rax, rdi
    sete al
    movzb rax, al
    push rax
    pop rax
    cmp rax, 0
    je .Lelse1
    push 0
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    jmp .Lend1
.Lelse1:
    mov rax, rbp
    sub rax, 8
    push rax
    pop rax
    mov rax, [rax]
    push rax
    push 1
    pop rdi
    pop rax
    cmp rax, rdi
    sete al
    movzb rax, al
    push rax
    pop rax
    cmp rax, 0
    je .Lelse2
    push 1
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    jmp .Lend2
.Lelse2:
    mov rax, rbp
    sub rax, 8
    push rax
    pop rax
    mov rax, [rax]
    push rax
    push 1
    pop rdi
    pop rax
    sub rax, rdi
    push rax
    pop rax
    mov rdi, rax
    mov rax, rsp
    and rax, 15
    jnz .L.call.3
    mov rax, 0
    call fibbonacci
    jmp .L.end.3
.L.call.3:
    sub rsp, 8
    mov rax, 0
    call fibbonacci
    add rsp, 8
.L.end.3:
    push rax
    mov rax, rbp
    sub rax, 8
    push rax
    pop rax
    mov rax, [rax]
    push rax
    push 2
    pop rdi
    pop rax
    sub rax, rdi
    push rax
    pop rax
    mov rdi, rax
    mov rax, rsp
    and rax, 15
    jnz .L.call.4
    mov rax, 0
    call fibbonacci
    jmp .L.end.4
.L.call.4:
    sub rsp, 8
    mov rax, 0
    call fibbonacci
    add rsp, 8
.L.end.4:
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
.Lend2:
.Lend1:
    pop rax
    mov rsp, rbp
    pop rbp
    ret
main:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov rax, rbp
    push 5
    pop rax
    mov rdi, rax
    mov rax, rsp
    and rax, 15
    jnz .L.call.5
    mov rax, 0
    call fibbonacci
    jmp .L.end.5
.L.call.5:
    sub rsp, 8
    mov rax, 0
    call fibbonacci
    add rsp, 8
.L.end.5:
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    mov rsp, rbp
    pop rbp
    ret
