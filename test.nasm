[global do_the_float]
do_the_float:
        ;mov rax, float_number

        ; movsd xmm0, qword [float_number]
        ; movsd xmm1, qword [float_number]
        ; addsd xmm0, xmm1

        ; movsd xmm8, qword [float_number]

        ; movsd xmm0, qword [0xABAB]

        ; cvtsi2sd xmm0, rax

        ; mov rcx, 0xAABBAABBAABB

        ;movq rax, xmm0
        ;movq xmm0, rax

        push rbp
        mov rbp, rsp
        mov rcx, float_number
        movsd xmm0, qword [rcx + 0x0]
        mov rax, 0x5
        cvtsi2sd xmm1, rax
        addsd xmm0, xmm1

        movsd xmm1, qword [rbp-0x1000000]

        pop rbp

        mov rax, $

        ret

float_number:
        db 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x14, 0x40
