// Important: do not add any additional #include directives!
// The only C library functions that may be used are
//
// - fgetc
// - malloc
// - free

#include <stdint.h>
#include <stdlib.h>
#include "wcfuncs.h"

// Compute a hash code for the given NUL-terminated
// character string.
//
// The hash algorithm should be implemented as follows:
//
// uint32_t hash_code = 5381
// for each character c of w in order {
//   hash_code = hash_code * 33 + c
// }
//
// Note that the character values should be treated as
// being unsigned (in the range 0..255)
uint32_t wc_hash(const unsigned char *w) {
  uint32_t hash_code = 5381;
  const unsigned char* curr = w;
  while (*curr != '\0') {
    unsigned char c = *curr;
    hash_code = hash_code * 33 + c;
    curr++;
  }
  return hash_code;
}

// Compare two strings lexicographically. Return
//
// - a negative value if lhs string is less than rhs string
// - 0 if lhs string is identical to rhs string
// - a positive value if lhs string is greater than rhs string
//
// Lexicographical comparison is a generalization of alphabetical
// order, but using character codes. If one string is a prefix
// of the other, it is considered as "less than". E.g.,
// "hi" would compare as less than "high".
int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs) {
  while (*lhs != '\0' && *rhs != '\0') {
    // if lhs less than rhs, we need to return negative value
    if (*lhs < *rhs) {
      return -1; 
    } 
    // if rhs less than lhs, we need to return positive value
    else if (*lhs > *rhs) {
      return 1; // lhs is greater than rhs
    }
    lhs++;
    rhs++;
  }
  
  // if lhs is a prefix of rhs, then lhs is smaller lexicographically, thus return negative
  if (*lhs == '\0' && *rhs != '\0') {
    return -1; 
  } 
  // if rhs, is a prefix of lhs, then rhs is smaller lexicographically, thus return positive
  else if (*lhs != '\0' && *rhs == '\0') {
    return 1; 
  }

  // if both strings the same, return zero
  return 0; 
}

// Copy NUL-terminated source string to the destination buffer.
void wc_str_copy(unsigned char *dest, const unsigned char *source) {
  const unsigned char* curr = source;
  int len = 0;
  while (*curr != '\0') {
    unsigned char c = *curr;
    dest[len] = c;
    curr++;
    len++;
  }
  dest[len] = '\0';
}

// Return 1 if the character code in c is a whitespace character,
// false otherwise.
//
// For the purposes of this function, a whitespace character is one of
//
//   ' '
//   '\t'
//   '\r'
//   '\n'
//   '\f'
//   '\v'
int wc_isspace(unsigned char c) {
  switch(c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case '\f':
    case '\v':
      return 1;
    default:
      return 0;
  }
}

// Return 1 if the character code in c is an alphabetic character
// ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
int wc_isalpha(unsigned char c) {
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        return 1;
    } 
  else {
        return 0;
    }
}

// Read the next word from given input stream, storing
// the word in the array pointed-to by w. (This array should be
// assumed to be MAX_WORDLEN+1 elements in size.) Return 1
// if a word is read successfully, 0 otherwise.
//
// For the purposes of this function, a word is a sequence of
// 1 or more non-whitespace characters.
//
// If a sequence of non-whitespace characters has more than
// MAX_WORDLEN characters, then only the first MAX_WORDLEN
// characters in the sequence should be stored in the array.
int wc_readnext(FILE *in, unsigned char *w) {
  if (in == NULL) {
    return 0;
  }

  int i = 0;
  int c = fgetc(in);
  while (c != EOF && wc_isspace(c) == 1){
    c = fgetc(in);
  }

  while (c != EOF && wc_isspace(c) == 0) {
    if (i < MAX_WORDLEN) {
      w[i] = (unsigned char)c;
      i++;
    }
    c = fgetc(in);
  }
  w[i] = '\0';
  //printf("Word read: %s\n", w);
  if (i > 0) {
    return 1;
  } else {
    return 0;
  }
}

// Convert the NUL-terminated character string in the array
// pointed-to by w so that every letter is lower-case.
void wc_tolower(unsigned char *w) {
  unsigned char* curr = w;
  int index = 0;
  while(*curr != '\0') {
    unsigned char c = *curr;
    // only fix letters
    if (wc_isalpha(c)) {
      // only fix uppcase letters to lower
      if (c >= 'A' && c < 'a') {
        c = c + 32;
        w[index] = c;
      }
    }
    index++;
    curr++;
  }
}

// Remove any non-alphaabetic characters from the end of the
// NUL-terminated character string pointed-to by w.
void wc_trim_non_alpha(unsigned char *w) {
  unsigned char* curr = w;
  int len = 0;
  // loop through w to get to end of string
  while (*curr != '\0') {
    len++;
    curr++;
  }
  // get index of last letter
  len--;
  // remove non alpha characters until we see normal letter
  while (len >= 0 && !wc_isalpha(w[len])) {
    w[len] = '\0';
    len--;
  }
}

// Search the specified linked list of WordEntry objects for an object
// containing the specified string.
//
// If a matching object is found, set the int variable pointed-to by
// inserted to 0 and return a pointer to the matching object.
//
// If a matching object is not found, allocate a new WordEntry object,
// set its next pointer to point to what head points to (i.e., so the
// new object is at the head of the list), set the variable pointed-to
// by inserted to 1, and return a pointer to the new node. Note that
// the new node should have its count value set to 0. (It is the caller's
// job to update the count.)
struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
  struct WordEntry* curr = head;
  while (curr != NULL) {
    int index = 0;
    int matches = 0;
    while (s[index] != '\0' && curr->word[index] != '\0') {
      if (s[index] == curr->word[index]) {
        matches++;
      }
      index++;
    }
    // if both strings same length and have the exact same letters, we have same word
    if (s[index] == '\0' && curr->word[index] == '\0' && index == matches) {
      *inserted = 0;
      return curr;
    }
    curr = curr->next;
  }

  struct WordEntry* new = malloc(sizeof(struct WordEntry));
  new->count = 0;
  int index = 0;
  while (s[index] != '\0') {
    new->word[index] = s[index];
    index++;
  }
  new->word[index] = '\0';
  new->next = head;
  *inserted = 1;
  return new;
}

// Find or insert the WordEntry object for the given string (s), returning
// a pointer to it. The head of the linked list which contains (or should
// contain) the entry for s is the element of buckets whose index is the
// hash code of s mod num_buckets.
//
// Returns a pointer to the WordEntry object in the appropriate linked list
// which represents s.
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s) {
  // TODO: implement
}

// Free all of the nodes in given linked list of WordEntry objects.
void wc_free_chain(struct WordEntry *p) {
  while (p != NULL) {
    struct WordEntry* temp = p;
    p = p->next;
    free(temp);
  }
}
