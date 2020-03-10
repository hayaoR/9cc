.intel_syntax noprefix
.global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    push 3
    pop rax
    mov RDI, rax
    push 4
    pop rax
    mov RSI, rax
    mov rax, rsp
    and rax, 15
    jnz .L.call.1
    mov rax, 0
    call fooo
    jmp .L.end.1
.L.call.1:
    sub rsp, 8
    mov rax, 0
    call fooo
    add rsp, 8
.L.end.1:
    pop rax
    push 1
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    pop rax
    mov rsp, rbp
    pop rbp
    ret
