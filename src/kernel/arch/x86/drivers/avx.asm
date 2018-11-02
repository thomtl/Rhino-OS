[bits 32]
section .text

global init_avx
init_avx:
    call detect_avx
    cmp eax, 0
    je no_avx

    mov eax, cr4
    bts eax, 18 ; Set bit OSXSAVE, Required by AVX
    mov cr4, eax

    push eax
    push edx
    push ecx

    xor ecx, ecx
    xgetbv
    bts eax, 0 ; Set XCR0 X87 / MMX State
    bts eax, 1 ; Set XCR0 SSE State
    bts eax, 2 ; Set XCR0 AVX State
    xsetbv

    pop ecx
    pop edx
    pop eax

    call detect_avx_512
    cmp eax, 1
    jge init_avx_512


    mov eax, 1
    ret

global detect_avx
detect_avx:
    mov eax, 1
    cpuid
    bt ecx, 28
    jnc no_avx
    mov eax, 1
    ret

no_avx:
    mov eax, 0
    ret

global detect_avx_512
detect_avx_512:
    mov eax, 0
    cpuid
    cmp eax, 7
    jl no_avx

    mov eax, 7
    cpuid
    bt ebx, 16
    jnc no_avx

    mov eax, 1
    ret

global init_avx_512
init_avx_512:
    push eax
    push edx
    push ecx

    xor ecx, ecx
    xgetbv
    bts eax, 5 ; Set XCR0 OPMASK
    bts eax, 6 ; Set XCR0 ZMM_Hi256
    bts eax, 7 ; Set XCR0 Hi16_ZMM
    xsetbv

    pop ecx
    pop edx
    pop eax
    ret