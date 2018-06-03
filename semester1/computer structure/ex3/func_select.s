.section .rodata
    .align 8    # Align address to multiple of 8
    dummy:  .string "%c"
    gettingChar_format: .string "%c %c"
    gettingInt_format: .string "%d"
    case50_out: .string "first pstring length: %d, second pstring length: %d\n"
    case51_out: .string "old char: %c, new char: %c, first string: %s, second string: %s\n"
    lenAndtxt_out: .string "length: %d, string: %s\n"
    case54_out: .string "compare result: %d\n"
    invalid_out:  .string  "invalid option!\n"   
    
.switchCases: 
    .quad .case50   # Case 50: call pstrlen
    .quad .case51   # Case 51: call replaceChar
    .quad .case52   # Case 52: call pstrijcpy
    .quad .case53   # Case 53: call swapCase
    .quad .case54   # Case 54: call pstrijcmp
    .quad .default  # default
    
.text        
.globl run_func         #run one pstring's function according to the option.
                        #rdi should contain the option, rsi pstring1's pointer
                        #and rdx pstring2's pointer
run_func:
    pushq   %r12           #creates backups for the pstrings' pointers
    pushq   %r13
    movq    %rsi,%r12
    movq    %rdx,%r13
    
    subq    $50,%rdi       #calculate case
    js      .default
    cmpq    $4,%rdi
    jg      .default
    jmp     *.switchCases(,%rdi,8)
    
    
.case50:
    movq    %r12,%rdi      #gets pstring1's length using pstrlen 
    call    pstrlen
    movq    %rax,%rsi
    
    movq    %r13,%rdi      #gets pstring2's length using pstrlen 
    call    pstrlen
    movq    %rax,%rdx
    
    movq    $case50_out,%rdi     #printing the lengthes 
    movq    $0,%rax
    call    printf
    jmp     .end

.case51:
    subq    $2,%rsp        #allocating memory for the oldChar and newChar
    
    movq    %rsp,%rsi      #dummy gets the last \n
    movq    $dummy,%rdi
    movq    $0,%rax
    call    scanf
    
    movq    %rsp,%rsi      #getting the oldChar and the newChar
    leaq    1(%rsp),%rdx
    movq    $gettingChar_format,%rdi
    movq    $0,%rax
    call    scanf
    
    movzbq  (%rsp),%rsi    #calling replaceChar,replacing every oldChar in pstring1 with newChar
    movzbq  1(%rsp),%rdx
    movq    %r12,%rdi
    call    replaceChar
    
    movzbq  (%rsp),%rsi    #calling replaceChar,replacing every oldChar in pstring2 with newChar
    movzbq  1(%rsp),%rdx
    movq    %r13,%rdi
    call    replaceChar
    
    movq    $case51_out,%rdi    #prints the pstrings after the replacement
    movzbq  (%rsp),%rsi
    incq    %rsp
    movzbq  (%rsp),%rdx
    leaq    1(%r12),%rcx
    leaq    1(%r13),%r8
    movq    $0,%rax
    call    printf
    
    incq    %rsp
    jmp     .end

.case52:
    subq    $8,%rsp         #allocates memory for indexes i and j 
    
    movq    %rsp,%rsi       #getting startIndex    
    movq    $gettingInt_format,%rdi
    movq    $0,%rax
    call    scanf
    
    leaq    4(%rsp),%rsi    #getting endIndex
    movq    $gettingInt_format,%rdi
    movq    $0,%rax
    call    scanf
    
    movq    %r12,%rdi       #copy from the src pstring to the dst pstring 
    movq    %r13,%rsi
    movzbq  (%rsp),%rdx
    addq    $4,%rsp
    movzbq  (%rsp),%rcx
    addq    $4,%rsp
    call    pstrijcpy
    
    movq    %r12,%rdi       #prints the dst after the copy
    call    pstrlen
    movq    %rax,%rsi
    movq    $lenAndtxt_out,%rdi
    leaq    1(%r12),%rdx
    movq    $0,%rax
    call    printf
    
    movq    %r13,%rdi       #prints the src after the copy
    call    pstrlen
    movq    %rax,%rsi
    movq    $lenAndtxt_out,%rdi
    leaq    1(%r13),%rdx
    movq    $0,%rax
    call    printf
    
    jmp     .end
    
.case53:
    movq    %r12,%rdi    #swaps pstring1's letters
    call    swapCase
    movq    %r13,%rdi    #swaps pstring2's letters
    call    swapCase
    
    movq    %r12,%rdi    #prints pstring1 after the swap
    call    pstrlen
    movq    %rax,%rsi
    movq    $lenAndtxt_out,%rdi
    leaq    1(%r12),%rdx
    movq    $0,%rax
    call    printf
    
    movq    %r13,%rdi    #prints pstring2 after the swap
    call    pstrlen
    movq    %rax,%rsi
    movq    $lenAndtxt_out,%rdi
    leaq    1(%r13),%rdx
    movq    $0,%rax
    call    printf
    
    jmp     .end
    
.case54:
    subq    $8,%rsp         #allocates memory for indexes i and j 
    
    movq    %rsp,%rsi       #getting startIndex    
    movq    $gettingInt_format,%rdi
    movq    $0,%rax
    call    scanf
    
    leaq    4(%rsp),%rsi    #getting endIndex
    movq    $gettingInt_format,%rdi
    movq    $0,%rax
    call    scanf
    
    movq    %r12,%rdi   #calling pstrijcmp
    movq    %r13,%rsi
    movzbq  (%rsp),%rdx
    addq    $4,%rsp
    movzbq  (%rsp),%rcx
    addq    $4,%rsp
    call    pstrijcmp
    movq    %rax,%rsi
    
    movq    $case54_out,%rdi    #prints result
    movq    $0,%rax
    call    printf
    
    jmp     .end

.default:   #default case
    movq    $invalid_out,%rdi
    movq    $0,%rax
    call    printf
    
.end:
    popq   %r13     #resoring callee register
    popq   %r12
    ret
    