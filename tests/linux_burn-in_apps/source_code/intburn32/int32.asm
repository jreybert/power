;  nasm -f elf int32.asm   for int32.o

;  nasm -f elf busasm.asm   for busasm.o

section .text

    global _cpuida   

 _cpuida:

    push eax
    push ebx
    push ecx
    push edx
    push edi

    mov  eax, 1
    CPUID
    mov [eaxCode1], eax  ; Features Code
    mov [edxCode1], edx  ; family/model/stepping
    mov [ebxCode1], ebx
    mov [ecxCode1], ecx

    mov  edi, idString1
    mov  eax, 0
    CPUID
    mov[edi],   ebx
    mov[edi+4], edx
    mov[edi+8], ecx
    mov ecx, 0
    mov[edi+12], ecx
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
    mov  edi, idString2
    mov eax,80000002h
    CPUID
    mov[edi],    eax
    mov[edi+4],  ebx
    mov[edi+8],  ecx
    mov[edi+12], edx
    mov eax,80000003h
    CPUID
    mov[edi+16], eax
    mov[edi+20], ebx
    mov[edi+24], ecx
    mov[edi+28], edx
    mov eax,80000004h
    CPUID
    mov[edi+32], eax
    mov[edi+36], ebx
    mov[edi+40], ecx
    mov[edi+44], edx
    mov ecx, 0
    mov[edi+48], ecx
    jmp nomore
   intel:
    mov eax,80000000h
    CPUID
    cmp eax,80000004h
    jc nomore                
    mov  edi, idString2
    mov eax,80000002h
    CPUID
    mov[edi],    eax
    mov[edi+4],  ebx
    mov[edi+8],  ecx
    mov[edi+12], edx
    mov eax,80000003h
    CPUID
    mov[edi+16], eax
    mov[edi+20], ebx
    mov[edi+24], ecx
    mov[edi+28], edx
    mov eax,80000004h
    CPUID
    mov[edi+32], eax
    mov[edi+36], ebx
    mov[edi+40], ecx
    mov[edi+44], edx
    mov ecx, 0
    mov[edi+48], ecx
   nomore:

    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
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

        push eax
        push edx
        push ebx 
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
        pop ebx
        pop edx
        pop eax
        ret
section .data

 extern  startCount
 extern  endCount
 extern  cycleCount



section .text

    global _readData   

 _readData:
    push eax
    push ebx
    push ecx
    push edx
    push edi
    push esi
    mov    eax, [xx]
    mov    ecx, [eax] 
    mov    edx, ecx
    mov    edi, ecx
    mov    esi, ecx
lp1:mov    eax, [xx]
    mov    ebx, [mempasses]
    align 16
lp2:sub    ecx, [eax]
    sub    edx, [eax+4]
    sub    edi, [eax+8]
    sub    esi, [eax+12]
    sub    ecx, [eax+16]
    sub    edx, [eax+20]
    sub    edi, [eax+24]
    sub    esi, [eax+28]
    add    ecx, [eax+32]
    add    edx, [eax+36]
    add    edi, [eax+40]
    add    esi, [eax+44]
    add    ecx, [eax+48]
    add    edx, [eax+52]
    add    edi, [eax+56]
    add    esi, [eax+60]
    add    eax, [passbytes]
    dec    ebx
    jnz    lp2
    mov    ebx, [outerLoop]
    dec    ebx
    mov    [outerLoop], ebx     
    jnz    lp1
    sub    ecx, edx         
    add    ecx, edi
    sub    ecx, esi           
    add    ecx, edi
    mov    [calcResult], ecx
    mov    eax, [xx]
    mov    [eax], ecx 
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

    global _errorcheck 
      
_errorcheck:
    push eax
    push ebx
    push ecx
    push edx
    push edi
    push esi
    lea    eax, [sumCheck]
    mov    ecx,[eax]
    mov    edx,[eax]   
    mov    edi,[eax+4] 
    mov    esi,[eax+4] 
    mov    eax, [xx]
    mov    ebx, [mempasses]
    align 16
lp3:or     ecx,[eax]   
    or     ecx,[eax+4] 
    or     ecx,[eax+8] 
    or     ecx,[eax+12] 
    or     edi,[eax+16] 
    or     edi,[eax+20] 
    or     edi,[eax+24] 
    or     edi,[eax+28] 
    or     ecx,[eax+32] 
    or     ecx,[eax+36] 
    or     ecx,[eax+40] 
    or     ecx,[eax+44] 
    or     edi,[eax+48] 
    or     edi,[eax+52]
    or     edi,[eax+56]
    or     edi,[eax+60]
    or     ecx,[eax+64]   
    or     ecx,[eax+68] 
    or     ecx,[eax+72] 
    or     ecx,[eax+76] 
    or     edi,[eax+80] 
    or     edi,[eax+84] 
    or     edi,[eax+88] 
    or     edi,[eax+92] 
    or     ecx,[eax+96] 
    or     ecx,[eax+100] 
    or     ecx,[eax+104] 
    or     ecx,[eax+108] 
    or     edi,[eax+112] 
    or     edi,[eax+116]
    or     edi,[eax+120]
    or     edi,[eax+124]
    and    edx,[eax]    
    and    edx,[eax+4]  
    and    edx,[eax+8] 
    and    edx,[eax+12] 
    and    esi,[eax+16] 
    and    esi,[eax+20] 
    and    esi,[eax+24] 
    and    esi,[eax+28] 
    and    edx,[eax+32] 
    and    edx,[eax+36] 
    and    edx,[eax+40] 
    and    edx,[eax+44] 
    and    esi,[eax+48] 
    and    esi,[eax+52]
    and    esi,[eax+56]
    and    esi,[eax+60]    
    and    edx,[eax+64]   
    and    edx,[eax+68] 
    and    edx,[eax+72] 
    and    edx,[eax+76] 
    and    esi,[eax+80] 
    and    esi,[eax+84] 
    and    esi,[eax+88] 
    and    esi,[eax+92] 
    and    edx,[eax+96] 
    and    edx,[eax+100] 
    and    edx,[eax+104] 
    and    edx,[eax+108] 
    and    esi,[eax+112] 
    and    esi,[eax+116]
    and    esi,[eax+120]
    and    esi,[eax+124]
    add    eax,128 
    dec    ebx        
    jnz    lp3         
    lea    eax, [sumCheck]
    mov    [eax],ecx  
    mov    [eax+8],edx
    mov    [eax+16],edi
    mov    [eax+24],esi
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

    global _writeData 

 _writeData:
    push eax
    push ebx
    push ecx
    push edx
    push edi
    mov    eax, [xx]
    mov    ebx, [mempasses]
    mov    ecx,[eax]    
    mov    edx,[eax+16] 
    add    eax,32 
    align 16      
lp4:mov    [eax],ecx    
    mov    [eax+4],ecx  
    mov    [eax+8],ecx  
    mov    [eax+12],ecx  
    mov    [eax+16],edx  
    mov    [eax+20],edx  
    mov    [eax+24],edx  
    mov    [eax+28],edx  
    mov    [eax+32],ecx  
    mov    [eax+36],ecx  
    mov    [eax+40],ecx  
    mov    [eax+44],ecx  
    mov    [eax+48],edx  
    mov    [eax+52],edx  
    mov    [eax+56],edx  
    mov    [eax+60],edx  
    mov    [eax+64],ecx  
    mov    [eax+68],ecx  
    mov    [eax+72],ecx  
    mov    [eax+76],ecx  
    mov    [eax+80],edx  
    mov    [eax+84],edx  
    mov    [eax+88],edx  
    mov    [eax+92],edx  
    mov    [eax+96],ecx  
    mov    [eax+100],ecx  
    mov    [eax+104],ecx  
    mov    [eax+108],ecx  
    mov    [eax+112],edx  
    mov    [eax+116],edx  
    mov    [eax+120],edx  
    mov    [eax+124],edx  
    add    eax,128    
    dec    ebx        
    jnz    lp4
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret                     

 extern  outerLoop
 extern  xx
 extern  mempasses
 extern  passbytes
 extern  calcResult
 extern  sumCheck














