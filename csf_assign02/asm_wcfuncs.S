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
	/* TODO: implement */
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
.LcmpLoop:
	// store value of rdi address into r10
	movb (%rdi), %r10b
	// store value of rsi address into r11
	movb (%rsi), %r11b
	// check if r10 has null terminator
	cmpb $0, %r10b
	// if it is null terminator, stop loop
	je .LcmpLoopDone
	// if not, check if r11 has null terminator
	cmpb $0, %r11b
	// if null terminator, stop loop
	je .LcmpLoopDone
	// compare the characters in r10 and r11
	cmpb %r10b, %r11b
	// if rhs > lhs, jump to set final negative answer
	jg .LrightBigger
	// compare the characters in r10 and r11 again
	cmpb %r10b, %r11b
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
	cmpb $0, %r10b
	// if null terminator, check if lhs is prefix of rhs
	je .LcheckLeftPrefix
	// check if rhs is null terminator
	cmpb $0, %r11b
	// if null terminator, check if rhs is prefix of lhs
	je .LcheckRightPrefix
	// if neither check hits, we have same string
	movl $0, %eax
	// jump to finish function
	jmp .LcmpOut
.LcheckLeftPrefix:
	// check if rhs is null terminator
	cmpb $0, %r11b
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
	cmpb $0, %r10b
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
.LcmpOut:
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
	/* TODO: implement */
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
	// if not alpha, move 1 into return register
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
	/* TODO: implement */
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
	/* TODO: implement */
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
	/* TODO: implement */
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
	/* TODO: implement */
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
	/* TODO: implement */
	ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
