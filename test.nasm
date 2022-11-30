[global do_the_float]
do_the_float:
        ;mov rax, float_number

        movsd xmm0, qword [float_number]
        movsd xmm1, qword [float_number]
        addsd xmm0, xmm1

        movsd xmm8, qword [float_number]

        movsd xmm0, qword [0xABAB]

        cvtsi2sd xmm0, rax

        mov rcx, 0xAABBAABBAABB

        ;movq rax, xmm0
        ;movq xmm0, rax

        ret

float_number:
        dq 5.1
        align 8
