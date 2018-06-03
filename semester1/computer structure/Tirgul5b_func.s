	#This program shows a program with several functions:
	.data
	.align	4	# we want all data to be save in an address that divise with their size.
x:	.long	3	# value for our calculations.
y:	.long	5
z:	.long	10
	.section	.rodata
result:	.string	"(x+y)*z = %d\n"	#string for printf.
	#########################
	.text	#the beginning of the code.
.globl	main
	.type	main, @function
main:
	movq	$x, %rax	#%rax is a pointer to x.
	movl	4(%rax),%esi	#passing 2nd parameter (y). important: note that size of pointed data (y) is 4 bytes, while pointer size is 8 bytes.
	movl	(%eax),%edi	#passing x - the first parameter for sum. again, mov destination is %edi because pointed data is of size 4 bytes.
	call	sum		#calling the function sum.

	#returning from sum: - %eax is holding (x+y).
	movq	$z, %rsi	#%rsi is a pointer to z.
	movl	(%rsi),%esi	#passing z the second parameter for mult (note the destination of mov).
	movq	%rax,%rdi	#passing (x+y) the first parameter for mult.
	call	mult

	#returning from mult: - %eax is holding (x+y)*z.
	movq	%rax,%rsi	#passing the results - the second parameter for printf.
	movq	$result,%rdi	#passing the string the first parameter for printf.
	movq	$0,%rax
	call	printf		#calling printf.

	#end of the main function:
	movq	$0, %rax	#return value is zero.	
	ret			#return to caller function (OS)

	#this function gets two parameters and return their sum.
	.type	sum, @function
sum:	
	movq	%rdi, %rax	#geting x - the first parameter.
	add	%rsi, %rax	#adding y, the second parameter, to x.

	# result is already at %eax - the return value register.

	ret			#return to caller function (main)
	
	#this function gets two parameters and return their product.
	.type	mult, @function
mult:	
	movq	%rdi, %rax	#geting (x+y) - the first parameter.
	imulq	%rsi, %rax	#mult with z - the second parameter.

	# result is already at %eax - the return value register.

	ret			#return to caller function (main)

