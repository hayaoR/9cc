.intel_syntax noprefix
.global main
main:
    push 1
    push 1
    pop rdi
    pop rax
    cmp rax, rdi
    setne al
    movzb rax, al
    push rax
    pop rax
    ret
