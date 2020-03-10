.intel_syntax noprefix
.global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    call foo
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
