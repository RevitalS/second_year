.section .rodata
    invalid_out2:  .string  "invalid input!\n" 
 
.text
.globl pstrlen  #return the length of the pstring
                #rdi should contain the  pstring's pointer
pstrlen:
    movzbq    (%rdi),%rax
    ret
    
.globl replaceChar  #replace every old char in the pstring with new char
                    #rdi should contain the pstring's pointer, rsi the old char and rdx the new char 
replaceChar:
    call    pstrlen #setting counter and loop's limit
    movq    %rax,%r8
    movq    $0,%rcx
    incq    %rdi
    
.cmpChar:     #checking whether current char is the oldChar  
    cmpb    (%rdi),%sil
    jne     .continue
    movb    %dl,(%rdi)  #replace oldChar with newChar 
    
.continue:
    incq    %rdi        #increasing counter and pointer
    incq    %rcx
    cmpq    %r8,%rcx    #check whether to continue loop
    jl      .cmpChar
    
    incq    %r8         #set rdi to its original position
    subq    %r8,%rdi
    
    movq    %rdi,%rax
    ret

.globl pstrijcpy    #copy subsring from src pstring to dst pstring according to start and end indexes
                    #rdi should contain pstring1's pointer, rsi pstring2's pointer,
                    # rdx the start index and rcx the end index
pstrijcpy:
    pushq   %rbx    #creates backup for every caller register
    pushq   %r12
    pushq   %r13
    pushq   %r14
    movq    %rdi,%rbx
    movq    %rsi,%r12
    movq    %rdx,%r13
    movq    %rcx,%r14
    
    call    .validateIJ      #check i and j validity
    cmpq    $1,%rax
    je      .valid
    call    .printInvalid    # if i or j not valid print invalid messege and return
    jmp     .end2
    
.valid:
    incq    %rbx             #setting the pstrings' pointer on the ith index
    incq    %r12
    addq    %r13,%rbx
    addq    %r13,%r12
    
.copy:                      #copy pstring1[i:j] into pstring2[i:j]
    movzbq  (%r12),%r10
    movb    %r10b,(%rbx)
    incq    %rbx
    incq    %r12
    incq    %r13
    cmpq    %r13,%r14
    jge     .copy

    subq    %r13,%rbx      #set rdi to its original position
    jmp     .end2
    
.end2:
    movq    %rbx,%rax      
    
    popq    %r14            #restoring callee registers
    popq    %r13
    popq    %r12
    popq    %rbx
    ret

 
.globl swapCase           #swap every lowerCase to upperCase and every upperCase to lowerCase
                          # rdi should contain the pstring's pointer
swapCase:
    pushq   %r13          #creates backup of original dst's pointer
    movq    %rdi,%r13
    
    call    pstrlen       #sets loop's limit as the pstring's length
    movq    %rax,%rdx
    
    movq    %rdi,%rsi     #creates copy of the pointer for moving on the txt
    
    incq    %r13          #moving the pointer to its txt's start 
    movq    $0,%rcx
    
.changeCase:
    movzbq  (%r13),%rdi   #check if the char is upperCase letter and change it to lowerCase
    call    .isUpper
    cmpq    $1,%rax
    je      .toLower
    
    movzbq  (%r13),%rdi   #check if the char is lowerCase letter and change it to upperCase
    call    .isLower
    cmpq    $1,%rax
    je      .toUpper
    
    jmp     .next   #the char is not letter 
    
.toLower:      #change the letter to lowerCase
    addq    $32, (%r13)          
    jmp     .next
    
.toUpper:      #change the letter to upperCase    
    subq    $32, (%r13) 
             
.next:         #continue the loop
    incq    %r13
    incq    %rcx
    cmpq    %rdx,%rcx
    jl      .changeCase
    
    incq    %rcx
    subq    %rcx,%r13
    movq    %r13,%rax
    popq    %r13 
    ret
    
.isUpper:        #check whether the char is UpperCase letter
                 #rdi should contain the char
    cmpq    $65,%rdi
    jl      .notUpper
    cmpq    $90,%rdi
    jg      .notUpper
    movq    $1,%rax
    jmp     .endUpper
    
.notUpper:      
    movq    $0,%rax
    
.endUpper:
    ret

.isLower:   #check whether the char is lowerCase letter
            #rdi should contain the char
    cmpq    $97,%rdi
    jl      .notLower
    cmpq    $122,%rdi
    jg      .notLower
    movq    $1,%rax
    jmp     .endLower
    
.notLower:      
    movq    $0,%rax
    
.endLower:
    ret
        
.globl pstrijcmp        #compare between subsring of two pstrings according to start and end indexes
                        # by lexicographically order.return 1 if pstring1 is bigger,-1  if pstring2 is bigger
                        # and 0 if they are equal.
                        #rdi should contain pstring1's pointer, rsi pstring2's pointer,
                        # rdx the start index and rcx the end index
pstrijcmp:
    pushq   %rbx        #creates backup for every caller register
    pushq   %r12
    pushq   %r13
    pushq   %r14
    movq    %rdi,%rbx
    movq    %rsi,%r12
    movq    %rdx,%r13
    movq    %rcx,%r14
    
    call    .validateIJ     #chacke indexes' validity
    cmpq    $1,%rax
    je      .valid2
    call    .printInvalid
    movq    $-2,%rax
    jmp     .endCmp

.valid2:
    incq    %rbx            #setting the pstrings' pointer on the ith index
    incq    %r12
    addq    %r13,%rbx
    addq    %r13,%r12
    
.cmp:                       #compare pstring1[i:j] with pstring2[i:j]
    movq    (%rbx),%rdi
    movq    (%r12),%rsi
    call    .cmpLex
    cmpq    $1,%rax
    je      .endCmp
    cmpq    $-1,%rax
    je      .endCmp
    
    incq    %rbx            #check whether to end the loop
    incq    %r12
    incq    %r13
    cmpq    %r13,%r14
    jae     .cmp
    
.endCmp:    
    popq   %r14             #restores calle registers
    popq   %r13
    popq   %r12
    popq   %rbx
    ret
    
.cmpLex:                    #compare between two chars by lexicographically order.
                            #return 1 if char1 is bigger,-1 if char2 is bigger and 0 if they are equal.
                            #rdi should contain the first char and rsi the second one.
    cmpb    %dil,%sil
    jl      .firstBigger
    jg      .secondBigger
    
    movq    $0,%rax         #the both chars are equal
    jmp     .retLex

.firstBigger:               #char1 is bigger        
    movq    $1,%rax
    jmp     .retLex
    
.secondBigger:              #char2 is bigger    
    movq    $-1,%rax
    
.retLex:
    ret   

.validateIJ:    #check the validity of indexes i and j
                #rdi  should contain the first pstring's pointer, rsi the second one,
                # rdx the start index and rcx the end index
                
    cmpb    (%rdi),%dl     #check whether i is not bigger then the length of the dst
    jg     .invalid
    cmpb    (%rsi),%dl     #check whether i is not bigger then the length of the src
    jg      .invalid
    cmpb    (%rdi),%cl     #check whether j is not bigger then the length of the dst
    jg     .invalid
    cmpb    (%rsi),%cl     #check whether j is not bigger then the length of the src
    jg      .invalid
    
    movq    $1,%rax        #i,j are valid
    jmp     .return
    
.invalid:
    movq    $0,%rax
    
.return:
    ret

.printInvalid:      #prints invalid messege
    movq    $invalid_out2,%rdi
    movq    $0,%rax
    call    printf
    ret
    