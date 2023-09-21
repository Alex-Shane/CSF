#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  struct WordEntry *words[HASHTABLE_SIZE];
  for (int x = 0; x < HASHTABLE_SIZE; x++) {
    words[x] = NULL;
  }

  // open file and get first word

  while (wc_readnext(file, next_word) != 0) {
    total_words++;
    wc_tolower(next_word);
    wc_trim_non_alpha(next_word);
    int inserted;
    struct WordEntry* bucket = wc_dict_find_or_insert(words, HASHTABLE_SIZE, next_word);
    struct WordEntry* entry = wc_find_or_insert(bucket, next_word, &inserted);
    entry->count++;
    if (inserted == 1) {
      unique_words++;
    }
    if (entry->count > best_word_count) {
      best_word = next_word;
    }
    else if (entry->count == best_word_count) {
      int compare = wc_str_compare(next_word, best_word);
      if (compare < 0) {
        best_word = next_word;
      }
    }
  }


  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  // TODO: make sure file is closed (if one was opened)
  // TODO: make sure memory is freed

  return 0;
}

// TODO: definitions of helper functions
