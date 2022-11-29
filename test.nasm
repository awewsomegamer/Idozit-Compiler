[global do_the_float]
do_the_float:
        ;mov rax, float_number

        movsd xmm0, qword [float_number]
        movsd xmm1, qword [float_number]
        addsd xmm0, xmm1

        ;movq rax, xmm0
        ;movq xmm0, rax

        ret

float_number:
        dq 5.1
        align 8
