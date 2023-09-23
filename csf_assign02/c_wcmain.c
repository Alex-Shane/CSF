#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  // initialize hashtable of Word Entry objects
  struct WordEntry *words[HASHTABLE_SIZE];
  for (int x = 0; x < HASHTABLE_SIZE; x++) {
    words[x] = NULL;
  }

  FILE *file;
  int from_stdin = 0;
  // if text file passed in as argument, attempt to open file
  if (argc == 2) { 
    file = fopen(argv[1], "r");
    // Check if the file was successfully opened
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for reading.\n", argv[1]);
        return 1; 
    }
  }
  // if no text file passed in as extra arugment, read text from stdin
  else {
    file = stdin;
    from_stdin = 1;
  }
  
  // main loop: process words until we reach end of file
  unsigned char next_word[MAX_WORDLEN+1];
  while (wc_readnext(file, next_word) != 0) {
    total_words++;
    wc_tolower(next_word);
    wc_trim_non_alpha(next_word);
    struct WordEntry* entry = wc_dict_find_or_insert(words, HASHTABLE_SIZE, next_word);
    // we have unique word if the count of the word is zero
    if (entry->count == 0) {
      unique_words++;
    }
    entry->count++;
  }

  // loop through entire hashtable of words to find the most common one
  for (int k = 0; k < HASHTABLE_SIZE; k++) {
    struct WordEntry* cur_entry = words[k];
    while (cur_entry != NULL) {
      // update best word if current word has more occurrences 
      if (cur_entry->count > best_word_count) {
        best_word_count = cur_entry->count;
        best_word = cur_entry->word;
      }
      // if best word and curr word have same occurrences, compare them lexicographically to determine best word
      else if (cur_entry->count == best_word_count) {
        int compare = wc_str_compare(cur_entry->word, best_word);
        if (compare < 0) {
          best_word = cur_entry->word;
        }
      }
      cur_entry = cur_entry->next;
    }
  }

  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  // close file, if appropriate
  if (!from_stdin) {
    fclose(file);
  }
  
  // free all dynamically allocated data from hashtable
  for (int x = 0; x < HASHTABLE_SIZE; x++) {
    wc_free_chain(words[x]);
  }

  return 0;
}
