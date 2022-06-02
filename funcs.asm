section .data
    const_2.0 dt 2.0
    const_3.0 dt 3.0

section .text
global f1, f2, f3, f1der, f2der, f3der, f1der_, f2der_, f3der_
f1:
    fld qword[esp+4]
    sub esp, 8
    fstp qword[esp]
    call exp
    add esp, 8
    fld tword[const_2.0]
    faddp st1, st0
    ret

f2:
    fld1
    fdiv qword[esp+4]
    fchs
    ret

f3:
    fld qword[esp+4]
    fld1
    faddp st1, st0
    fld tword[const_2.0]
    fmulp st1, st0
    fchs
    fld tword[const_3.0]
    fdivp st1, st0
    ret

f1der:
    fld qword[esp+4]
    sub esp, 8
    fstp qword[esp]
    call exp
    add esp, 8
    ret

f2der:
    fld1
    fld qword[esp+4]
    fld st0
    fmulp st1, st0
    fdivp st1, st0
    ret

f3der:
    fld tword[const_2.0]
    fld tword[const_3.0]
    fdivp st1, st0
    fchs
    ret

f1der_:
    fld qword[esp+4]
    sub esp, 8
    fstp qword[esp]
    call exp
    add esp, 8
    ret

f2der_:
    fld tword[const_2.0]
    fld qword[esp+4]
    fld st0
    fld st0
    fmulp st1, st0
    fmulp st1, st0
    fdivp st1, st0
    fchs
    ret

f3der_:
    fldz
    ret

exp:
    fld qword[esp+4]
    fldl2e
    fmul
    fld st0
    frndint
    fsub st1, st0
    fxch st1
    f2xm1
    fld1
    fadd
    fscale
    fstp st1
    ret
