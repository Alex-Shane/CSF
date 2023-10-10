/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)
#define WORDENTRY_SIZE          (WORDENTRY_NEXT_OFFSET+8)

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
	.globl wc_hash
wc_hash:
	// save callee saved registers 
	pushq %r12 
	pushq %r13 
	// move starting hash value into r12
	movl $5381, %r12d
	// go into main loop
	jmp .Loophash
.Loophash:
	// check for null terminator
	cmpb $0, (%rdi)
	// if null terminator, go to next function 
	je .LoophashDone
	// move current letter of rdi (zero extended) into r13
	movzbl (%rdi), %r13d
	// multiply current hash by 33
	imull $33, %r12d
	// add ascii value of current character by 33
	add %r13d, %r12d
	// go to next character in rdi string
	inc %rdi
	// go back up to loop
	jmp .Loophash
.LoophashDone:
	// move final hash into return register
	movl %r12d, %eax 
	// restore callee saved registers 
	popq %r13 
	popq %r12 
	ret

/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare:
	// align stack pointer
	subq $8, %rsp 
	// save callee saved registers 
	pushq %r12
	pushq %r13 
	// check if lhs is null 
	cmpq $0, %rdi
	// if lhs is null, move to next function to handle it 
	je .LhandleLHSNull
	// check if rhs is null 
	cmpq $0, %rsi
	// if its null, and we know lhs isn't, left is bigger 
	je .LleftBigger
.LcmpLoop:
	// store value of rdi address into r12
	movb (%rdi), %r12b
	// store value of rsi address into r13
	movb (%rsi), %r13b
	// check if r12 has null terminator
	cmpb $0, %r12b
	// if it is null terminator, stop loop
	je .LcmpLoopDone
	// if not, check if r13 has null terminator
	cmpb $0, %r13b
	// if null terminator, stop loop
	je .LcmpLoopDone
	// compare the characters in r12 and r13
	cmpb %r12b, %r13b
	// if rhs > lhs, jump to set final negative answer
	jg .LrightBigger
	// compare the characters in r12 and r13 again
	cmpb %r12b, %r13b
	// if rhs < lhs, jump to set final positive answer
	jl .LleftBigger
	// move to next character in lhs
	inc %rdi
	// move to next character in rhs
	inc %rsi 
	// go back into loop
	jmp .LcmpLoop
.LcmpLoopDone:
	// check if lhs is null terminator
	cmpb $0, %r12b
	// if null terminator, check if lhs is prefix of rhs
	je .LcheckLeftPrefix
	// check if rhs is null terminator
	cmpb $0, %r13b
	// if null terminator, check if rhs is prefix of lhs
	je .LcheckRightPrefix
	// if neither check hits, we have same string
	movl $0, %eax
	// jump to finish function
	jmp .LcmpOut
.LcheckLeftPrefix:
	// check if rhs is null terminator
	cmpb $0, %r13b
	// if it isn't null terminator, we know lhs is prefix of rhs
	jne .LrightBigger
	// if null terminator, equal strings
	movl $0, %eax
	// jump to finish function
	jmp .LcmpOut
.LrightBigger:
	// if we determine right bigger, make answer negative
	movl $-1, %eax
	// jump to finish function
	jmp .LcmpOut
.LcheckRightPrefix:
	// check if lhs is null terminator
	cmpb $0, %r12b
	// if not null terminator, we know rhs is prefix of lhs
	jne .LleftBigger
	// if null terminator, strings equal and return zero
	movl $0, %eax
	// jump to finish function
	jmp .LcmpOut
.LleftBigger:
	// if we determine lhs bigger, make answer positive
	movl $1, %eax
	// jump to finish function
	jmp .LcmpOut
.LhandleLHSNull:
	// check if rhs is also null 
	cmpq $0, %rsi
	// if rhs isn't null, rhs is bigger 
	jne .LrightBigger
	// if both strings null, then equal 
	movl $0, %eax
	// jump to finish function
	jmp .LcmpOut
.LcmpOut:
	// restore values in callee saved registers 
	popq %r13
	popq %r12 
	// align stack pointer
	addq $8, %rsp
	// return to finish function
	ret

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	// align stack pointer
	subq $8, %rsp
	// save callee saved registers  
	pushq %r12 
	pushq %r13 
	// move source word into r12 
	movq %rsi, %r12
	// jump to main loop
	jmp .Loopcopy
.Loopcopy:
	// check for null terminator
	cmpb $0, (%r12)
	// if null terminator, go to next function
	je .LendCopy
	// move character from source string into temp register
	movb (%r12), %r13b 
	// move character from temp register into dest string
	movb %r13b, (%rdi)
	// increment source pointer
	inc %r12 
	// increment dest pointer
	inc %rdi 
	// jump to top of loop
	jmp .Loopcopy
.LendCopy:
	// explicitly set null terminator in dest string
	movb $0, (%rdi)
	// restore callee saved registers 
	popq %r13 
	popq %r12 
	// align stack pointer
	addq $8, %rsp
	ret

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
	// align stack pointer
	subq $8, %rsp 
	// check if character we are interested in (stored in %dil) is equal to space character (32 in ascii)
	cmpb $32, %dil
	// if equal to space, jump to update function, if not continue with checks
	je .Lspace_out
	// check if char in %dil is equal to tab char (9 ascii)
	cmpb $9, %dil
	// if equal to tab, jump to update function, if not continue with checks
	je .Lspace_out
	// check if char in %dil is equal to carriage return char (13 ascii)
	cmpb $13, %dil
	// if equal, jump to update function, if not, continue
	je .Lspace_out
	// check if char in %dil is equal to newline char (10 ascii)
	cmpb $10, %dil
	// if equal, jump to update function, if not, continue
	je .Lspace_out
	// check if char in %dil is equal to new page char (12 ascii)
	cmpb $12, %dil
	// if equal, jump to update function, if not, continue
	je .Lspace_out
	// check if char in %dil is equal to vertical tab char (11 ascii)
	cmpb $11, %dil
	// if equal, jump to update function, if not, continue
	je .Lspace_out
	// if no checks were successful, we don't have space character, so move zero in return register
	movl $0, %eax
	// jump to return function
	jmp .Lspace_return
.Lspace_out:
	// we have space character, so move 1 into return register
	movl $1, %eax
.Lspace_return:
	// realign stack pointer
	addq $8, %rsp 
	// done with function, so return, which will return our value in %eax
	ret

/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
	.globl wc_isalpha
wc_isalpha:
	// align stack pointer
	subq $8, %rsp
	// check if char in %dil is greater than or equal to 65 (ascii of 'A')
	cmpb $65, %dil
	// if less than 'A', jump to next checks, if g/e, continue checks in current function
	jl .Lcheck_lower
	// check if char in %dil is also less than or equal to 'Z'
	cmpb $90, %dil
	// if greater than 'Z', jump to next checks, if l/e, continue in current function
	jg .Lcheck_lower
	// since value is between 'A' and 'Z', we know it's an alpha, so move 1 into return register
	movl $1, %eax
	// jump to return funciton
	jmp .Lalpha_out
.Lcheck_lower:
	// check if char in %dil is greater than or equal to 'a'
	cmpb $97, %dil
	// if less than 'a', we know it's not a letter, so call false function
	jl .Lalpha_false
	// check if char in %dil is less than or equal to 'z'
	cmpb $122, %dil
	// if greater than 'z', not a character, so call false function
	jg .Lalpha_false
	// if between 'a' and 'z', we have alpha so move 1 into return register
	movl $1, %eax
	// jump to return function
	jmp .Lalpha_out
.Lalpha_false:
	// if not alpha, move 0 into return register
	movl $0, %eax
.Lalpha_out:
	// align stack pointer
	addq $8, %rsp
	// return/finish function
	ret

/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
	.globl wc_readnext
wc_readnext:
	// save callee saved registers
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15 
	// r12 stores file pointer 
	movq %rdi, %r12
	// r13 holds the dest string 
	movq %rsi, %r13
	// check if file pointer is null 
	cmpq $0, %rdi 
	// if it is, return failure 
	je .Lfail
	// set length of word to zero 
	movl $0, %r14d
	// rdi is already file pointer, so call fgetc 
	call fgetc
	// move curr char into r15 
	movb %al, %r15b
	// begin looping 
	jmp .LloopUntilNonSpace
.LloopUntilNonSpace:
	// check for EOF
	cmpb $-1, %r15b
	// if EOF, jump to finish function
	je .LfinishRead 
	// move current char into dil  
	movb %r15b, %dil
	// check if curr char is a space 
	call wc_isspace
	cmpl $1, %eax 
	// if it's not a space, then we can proceed to read word 
	jne .LreadWord
	// if it is space, we need to get next letter to check if that is a space or not 
	movq %r12, %rdi 
	call fgetc
	// move return of fgetc into r15b 
	movb %al, %r15b 
	// repeat loop with new char 
	jmp .LloopUntilNonSpace
.LreadWord:
	// check for EOF
	cmpb $-1, %r15b
	// if EOF, jump to finish function
	je .LfinishRead
	// move curr letter to be checked for space 
	movb %r15b, %dil
	// check for space 
	call wc_isspace
	cmpl $1, %eax
	// if a space, then word done and we can finish function 
	je .LfinishRead
	// if our dest word is less than max len, update it
	cmpl $MAX_WORDLEN, %r14d 
	jl .LupdateWord
	// get next letter of word 
	jmp .LnextLetter
.LnextLetter:
	// move file pointer into rdi 
	movq %r12, %rdi 
	call fgetc
	// move result of getc into r15b 
	movb %al, %r15b
	// continue reading word 
	jmp .LreadWord 
.LupdateWord:
	// update value of current letter in dest word 
	movb %r15b, (%r13)
	// move to next letter in dest word 
	incq %r13 
	// increase length of dest word 
	incl %r14d 
	// jump to get next letter
	jmp .LnextLetter
.LfinishRead:
	// set null terminator 
	movb $0, (%r13)
	// check that our word is not empty
	cmpl $0, %r14d
	jle .Lfail
	// if not empty, successful read 
	movl $1, %eax 
	jmp .LendRead
.Lfail:
	// fail code of zero 
	movl $0, %eax 
.LendRead:
	// restore callee saved registers 
	popq %r15 
	popq %r14
	popq %r13
	popq %r13 
	ret 


/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
	// align stack pointer 
	subq $8, %rsp 
	// save callee saved register 
	pushq %r12
	// move source word into r12 
	movq %rdi, %r12 
	// jump to main loop
	jmp .LloopLower
.LloopLower:
	// check for null terminator
	cmpb $0, (%r12)
	// if null terminator, end loop
	je .LlowerDone
	// move cur letter into dil 
	movb (%r12), %dil 
	// check if character is a letter
	call wc_isalpha
	// check if our character was actually a letter
	cmpl $1, %eax 
	// if yes, check to see if it is uppercase and needs to be adjusted
	je .LadjustLetter
	// if not letter, continue the loop
	jmp .LcontinueLowerLoop 
.LadjustLetter:
	// check if letter is uppercase, between 'A' and 'Z', if not, continue loop
	cmpb $65, (%r12)
	jl .LcontinueLowerLoop
	cmpb $97, (%r12)
	jge .LcontinueLowerLoop
	// if uppercase, add 32 to ascii value to make it lowercase
	addl $32, (%r12) 
	// continue loop 
	jmp .LcontinueLowerLoop
.LcontinueLowerLoop:
	// move to next letter
	inc %r12 
	// jump back to main loop
	jmp .LloopLower
.LlowerDone:
	// restore callee saved register 
	popq %r12
	// align stack pointer 
	addq $8, %rsp 
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
	// save callee saved registers 
	pushq %r12 
	pushq %r13 
	// hold initial string address in r12, will be used in loop comparison 
	movq %rdi, %r12 
	// move source word into r13 as well
	movq %rdi, %r13 
	// go to main loop
	jmp .LloopTrim
.LloopTrim:
	// check for null terminator
	cmpb $0, (%r13)
	// if null terminator, move to next function
	je .LdecreasePointer
	// if not null terminator, move to next character
	inc %r13 
	// continue loop
	jmp .LloopTrim
.LdecreasePointer:
	// once we reach end of string, decrease address by 1 so r13 points to last character of string
	decq %r13
	// jump to second loop
	jmp .LnextLoopTrim
.LnextLoopTrim:
	// check if address of r13 has passed its initial address
	cmpq %r12, %r13 
	// if it has, end the loop
	jl .LloopTrimDone
	// move the current character into dil
	movb (%r13), %dil 
	// check if current character is a letter
	call wc_isalpha
	// if character is a letter, loop is done
	cmp $1, %eax
	je .LloopTrimDone
	// if not a letter, make current character null terminator
	movb $0, (%r13)
	// move to the left by one character
	decq %r13 
	// continue loop
	jmp .LnextLoopTrim
.LloopTrimDone:
	// restore callee saved register 
	popq %r13
	popq %r12 
	ret

/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
	// align stack pointer 
	subq $8, %rsp 
	// save callee saved registers 
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15  
	// head 
	movq %rdi, %r12
	// source word
	movq %rsi, %r13
	// inserted 
	movq %rdx, %r14 
	// curr 
	movq %rdi, %r15
	jmp .LloopFind
.LloopFind:
	// check if word entry is NULL 
	cmpq $0, %r15  
	je .LnotFound
	// move source word into rdi
	movq %r13, %rdi 
	// move word of current word entry into rsi 
	movq %r15, %rsi 
	// call compare 
	call wc_str_compare
	// check if words were equal
	cmpl $0, %eax
	// if equal, we found match and move to next function
	je .Lfound 
	// if not equal, go to next word entry object 
	movq WORDENTRY_NEXT_OFFSET(%r15), %r15
	// continue loop 
	jmp .LloopFind
.Lfound:
	// if we found match, move zero into inserted 
	movq $0, (%r14) 
	// move word entry pointed to by rdi into return register 
	movq %r15, %rax 
	// finish function 
	jmp .LendFind
.LnotFound:
	// move number of bytes needed for word entry into rdi 
	movq $WORDENTRY_SIZE, %rdi 
	// allocate the memory for word entry 
	call malloc 
	// move return of malloc into rdi
	movq %rax, %r15  
	// move malloced address into rdi 
	leaq WORDENTRY_WORD_OFFSET(%r15), %rdi  
	// move source word into rsi 
	movq %r13, %rsi 
	// copy word from source word 
	call wc_str_copy
	// make count of new word entry zero
	movl $0, WORDENTRY_COUNT_OFFSET(%r15) 
	// move head into next word of word entry 
	movq %r12, WORDENTRY_NEXT_OFFSET(%r15) 
	// set inserted to 1
	movl $1, (%r14) 
	// move new pointer into eax 
	movq %r15, %rax
	// finish the function 
	jmp .LendFind
.LendFind:
	// restore callee saved registers 
	popq %r15
	popq %r14
	popq %r13
	popq %r12  
	// align stack pointer 
	addq $8, %rsp
	ret

/*
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
	// save callee saved registers  
	pushq %r12
	pushq %r13
	pushq %r14 
	pushq %r15 
	// buckets pointer
	movq %rdi, %r12
	// num buckets 
	movl %esi, %r13d
	// source word 
	movq %rdx, %r14 
	// save buckets table to stack 
	//pushq %rdi 
	// move source word into rdi 
	movq %r14, %rdi 
	// get the hash code into %eax 
	call wc_hash  
	// prep division (overwrites source word)
	movl $0, %edx 
	// perform division 
	divl %r13d  
	// move the remainder of the division into r15d 
	movl %edx, %r15d 
	// add hash to buckets address to get desired element. move address into rdi
	movq (%r12, %r15, 8), %rdi 
	// move source string into rsi 
	movq %r14, %rsi 
	// save frame pointer 
	pushq %rbp
	// move stack pointer into frame pointer 
	movq %rsp, %rbp 
	// allocate 8 bytes for inserted
	subq $8, %rsp 
	// set inserted address, overwrite rsi as we don't need #buckets anymore
	leaq -8(%rbp), %r13 
	// set value of inserted to zero 
	movq $0, (%r13)
	// mov r13 into rdx 
	movq %r13, %rdx  
	// call find or insert
	call wc_find_or_insert 
	// check if inserted is one
	cmpq $1, (%r13)
	// if so, update the buckets table 
	je .Lchange
	// if not, finish the function
	jmp .LendDictFind

.Lchange:
	// store inserted node in buckets[hash]
	movq %rax, (%r12, %r15, 8) 
	// jump to finish function
	jmp .LendDictFind

.LendDictFind:
	// align stack after local variable for inserted 
	addq $8, %rsp
	// restore all changed callee saved registers 
	popq %rbp 
	popq %r15
	popq %r14
	popq %r13
	popq %r12 
	ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	// save callee saved registers 
	pushq %r12 
	pushq %r13
	// current struct pointer 
	movq %rdi, %r12 
.LfreeLoop:
	// check that current struct pointer is not null 
	cmpq $0, %r12 
	// if it is, we're done 
	je .LfreeDone
	// r13 is temp value 
	movq %r12, %r13
	// move to next pointer in r12 
	movq WORDENTRY_NEXT_OFFSET(%r12), %r12
	// move temp address to rdi to get freed 
	movq %r13, %rdi 
	// free address in rdi 
	call free 
	// repeat loop 
	jmp .LfreeLoop
.LfreeDone:
	// restore callee saved registers 
	popq %r13
	popq %r12
	ret

/*
vim:ft=gas:
*/
