;  nasm -f elf64 -g -F stabs int64.asm   ;for sse64.o

section .text

    global _cpuida   

 _cpuida:

    push rax
    push rbx
    push rcx
    push rdx
    push rdi

    mov  eax, 1
    CPUID
    mov [eaxCode1], eax  ; Features Code
    mov [edxCode1], edx  ; family/model/stepping
    mov [ebxCode1], ebx
    mov [ecxCode1], ecx

    mov  rdi, idString1
    mov  eax, 0
    CPUID
    mov[rdi],   ebx
    mov[rdi+4], edx
    mov[rdi+8], ecx
    mov ecx, 0
    mov[rdi+12], ecx
    mov [intel1amd2], ebx


    ; get specials
    mov eax,80000000h
    CPUID
    cmp eax,80000000h
    jc nomore

    mov eax, [intel1amd2]
    cmp eax, 756E6547h    ; uneG
    jz intel
    cmp eax, 68747541h    ; htuA
    jnz nomore
    mov eax,80000001h
    CPUID

    mov [ext81edx], edx

    mov eax,80000000h
    CPUID
    cmp eax,80000004h
    jc nomore                
    mov  rdi, idString2
    mov eax,80000002h
    CPUID
    mov[rdi],    eax
    mov[rdi+4],  ebx
    mov[rdi+8],  ecx
    mov[rdi+12], edx
    mov eax,80000003h
    CPUID
    mov[rdi+16], eax
    mov[rdi+20], ebx
    mov[rdi+24], ecx
    mov[rdi+28], edx
    mov eax,80000004h
    CPUID
    mov[rdi+32], eax
    mov[rdi+36], ebx
    mov[rdi+40], ecx
    mov[rdi+44], edx
    mov ecx, 0
    mov[rdi+48], ecx
    jmp nomore
   intel:
    mov eax,80000000h
    CPUID
    cmp eax,80000004h
    jc nomore                
    mov  rdi, idString2
    mov eax,80000002h
    CPUID
    mov[rdi],    eax
    mov[rdi+4],  ebx
    mov[rdi+8],  ecx
    mov[rdi+12], edx
    mov eax,80000003h
    CPUID
    mov[rdi+16], eax
    mov[rdi+20], ebx
    mov[rdi+24], ecx
    mov[rdi+28], edx
    mov eax,80000004h
    CPUID
    mov[rdi+32], eax
    mov[rdi+36], ebx
    mov[rdi+40], ecx
    mov[rdi+44], edx
    mov ecx, 0
    mov[rdi+48], ecx
   nomore:

    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

section .data

 extern  idString1
 extern  idString2
 extern  eaxCode1
 extern  ebxCode1
 extern  ecxCode1
 extern  edxCode1
 extern  intel1amd2
 extern  ext81edx


section .text

    global _calculateMHz   

 _calculateMHz:

        push rax
        push rdx
        push rbx 
        RDTSC 
        mov [startCount], eax 
        mov ebx, 25000
  outerloop:
        mov edx, 25000
  innerloop:    
        dec     edx
        jne     innerloop
        dec     ebx
        jne     outerloop
        RDTSC
        mov     [endCount], eax
        sub     eax, [startCount]
        mov     [cycleCount], eax        
        pop rbx
        pop rdx
        pop rax
        ret

section .data

 extern  startCount
 extern  endCount
 extern  cycleCount


section .text

    global _readData   

 _readData:
    push  rax
    push  rbx
    push  rcx
    push  r8
    push  r9
    push  r10
    push  r11
    push  r12
    push  r13
    push  r14
    push  r15
        mov    rcx, [outerLoop]
        mov    rax, [xx]
        mov    r8, [rax] 
        mov    r9, r8
        mov    r10, r8
        mov    r11, r8
        mov    r12, r8
        mov    r13, r8
        mov    r14, r8
        mov    r15, r8
   lp1: mov    rax, [xx]
        mov    rbx, [mempasses]
        align 16
   lp2: sub    r8, [rax]
        sub    r9, [rax+8]
        sub    r10, [rax+16]
        sub    r11, [rax+24]
        sub    r12, [rax+32]
        sub    r13, [rax+40]
        sub    r14, [rax+48]
        sub    r15, [rax+56]
        add    r8, [rax+64]
        add    r9, [rax+72]
        add    r10, [rax+80]
        add    r11, [rax+88]
        add    r12, [rax+96]
        add    r13, [rax+104]
        add    r14, [rax+112]
        add    r15, [rax+120]
        add    rax, [passbytes]
        dec    rbx
        jnz    lp2
        dec    rcx
        jnz    lp1
        sub    r8, r9         
        add    r8, r10
        sub    r8, r11           
        add    r8, r12     
        sub    r8, r13           
        add    r8, r14 
        sub    r8, r15
        add    r8, r10
        mov    [calcResult], r8
        mov    rax, [xx]
        mov    [rax], r8
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rcx
    pop  rbx
    pop  rax
        ret

    global _errorcheck 
      
_errorcheck:
    push  rax
    push  rbx
    push  rcx
    push  r8
    push  r9
    push  r10
    push  r11
    push  r12
    push  r13
    push  r14
    push  r15
    mov    rax, [xx]
    mov    rbx, [mempasses]
    lea    rcx, [sumCheck]
    mov    r8,[rcx]   
    mov    r9,[rcx]   
    mov    r10,[rcx+8] 
    mov    r11,[rcx+8] 
    align 16
lp3:or     r8,[rax]   
    or     r8,[rax+8] 
    or     r8,[rax+16] 
    or     r8,[rax+24] 
    or     r10,[rax+32] 
    or     r10,[rax+40] 
    or     r10,[rax+48] 
    or     r10,[rax+56] 
    or     r8,[rax+64] 
    or     r8,[rax+72] 
    or     r8,[rax+80] 
    or     r8,[rax+88] 
    or     r10,[rax+96] 
    or     r10,[rax+104]
    or     r10,[rax+112]
    or     r10,[rax+120]
    and    r9,[rax]    
    and    r9,[rax+8]  
    and    r9,[rax+16] 
    and    r9,[rax+24] 
    and    r11,[rax+32] 
    and    r11,[rax+40] 
    and    r11,[rax+48] 
    and    r11,[rax+56] 
    and    r9,[rax+64] 
    and    r9,[rax+72] 
    and    r9,[rax+80] 
    and    r9,[rax+88] 
    and    r11,[rax+96] 
    and    r11,[rax+104]
    and    r11,[rax+112]
    and    r11,[rax+120]    
    add    rax,128    
    dec    rbx        
    jnz    lp3         
    mov    [rcx],r8  
    mov    [rcx+8],r9
    mov    [rcx+16],r10
    mov    [rcx+24],r11
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rcx
    pop  rbx
    pop  rax
    ret

    global _writeData 

 _writeData:
    push rax
    push rbx
    push  r8
    push  r9
    push  r10
    push  r11
    push  r12
    push  r13
    push  r14
    push  r15
    mov    rax, [xx]
    mov    rbx, [mempasses]
    mov    r8,[rax]    
    mov    r9,[rax+32] 
    add    rax,64 
    align 16      
lp4:mov    [rax],r8    
    mov    [rax+8],r8  
    mov    [rax+16],r8  
    mov    [rax+24],r8  
    mov    [rax+32],r9  
    mov    [rax+40],r9  
    mov    [rax+48],r9  
    mov    [rax+56],r9  
    mov    [rax+64],r8  
    mov    [rax+72],r8  
    mov    [rax+80],r8  
    mov    [rax+88],r8  
    mov    [rax+96],r9  
    mov    [rax+104],r9  
    mov    [rax+112],r9  
    mov    [rax+120],r9  
    add    rax,128    
    dec    rbx        
    jnz    lp4
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rbx
    pop  rax
    ret                     

 extern  outerLoop
 extern  xx
 extern  mempasses
 extern  passbytes
 extern  calcResult
 extern  sumCheck














