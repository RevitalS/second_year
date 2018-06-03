.section .rodata
int_in_format: .string "%d"
string_in_format: .string "%s"
    
.text

.globl main
.type main, @function
main:    
    pushq   %r12            #storing callee save register for the both pstrings' pointers
    pushq   %r13
    
    pushq   %rbp            #creates backup of %rbp and saving original rsp
    movq    %rsp,%rbp
    
    subq    $4,%rsp         #getting first pstring length
    movq    %rsp,%rsi
    movq    $int_in_format,%rdi
    movq    $0,%rax
    call    scanf 
    
    movzbq  (%rsp),%r13     #saving the length of the first pstring txt
    addq    $4,%rsp         #and deallocating its memory
    
    decq    %rsp            #adding \0 for the end of the pstring
    movb    $0,(%rsp)
    
    subq    %r13,%rsp       #allocating memory for the pstring
    decq    %rsp    
    
    leaq    1(%rsp),%rsi    #getting first pstring txt
    movq    $string_in_format,%rdi
    movq    $0,%rax
    call    scanf
    
    movb    %r13b,(%rsp)    #inserts the length to the stack

    movq    %rsp,%r12       #saving pointer to pstring1  

    subq    $4,%rsp         #getting second pstring length
    movq    %rsp,%rsi
    movq    $int_in_format,%rdi
    movq    $0,%rax
    call    scanf
    
    movzbq  (%rsp),%r13    #deallocating the length memory of the second pstring txt
    addq    $4,%rsp
    
    decq    %rsp           #adding \0 for the end of the pstring
    movb    $0,(%rsp)
    
    subq    %r13,%rsp      #allocating memory for the pstring
    decq    %rsp    
    
    leaq    1(%rsp),%rsi   #getting second pstring txt
    movq    $string_in_format,%rdi
    movq    $0,%rax
    call    scanf
    
    movb    %r13b,(%rsp)   #inserts the length to the stack
    movq    %rsp,%r13      #saving pointer to pstring2
    
    subq    $4,%rsp        #getting user choise
    movq    %rsp,%rsi
    movq    $int_in_format,%rdi
    movq    $0,%rax
    call    scanf
    
    movzbq  (%rsp),%rdi    #calling run_func
    addq    $4,%rsp  
    movq    %r12,%rsi
    movq    %r13,%rdx
    call    run_func
     
    movq    %rbp,%rsp      #restoring all callee registers
    popq    %rbp
    popq    %r13
    popq    %r12 
    xorq    %rax,%rax
    ret
