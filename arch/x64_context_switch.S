
.text
.global x64_thread_context_switch
.type x64_thread_context_switch, @function
x64_thread_context_switch:
    pushf
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rsp, (%rdi)
    movq (%rsi), %rsp

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx
    popf

    retq
