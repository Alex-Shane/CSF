#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *)left_;
  int64_t right = *(int64_t *)right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

void seq_sort(int64_t *arr, size_t begin, size_t end) {
  size_t num_elements = end - begin;
  qsort(arr + begin, num_elements, sizeof(int64_t), compare_i64);
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;

  if (size <= threshold) {
    seq_sort(arr, begin, end);
    return;
  }

  // recursively sort halves in parallel

  size_t mid = begin + size/2;

  // Use fork to create two child processes
  pid_t left_child, right_child;
  left_child = fork();

  // check that child process made successfully
  if (left_child < 0) {
    fatal("Fork failed");
  } else if (left_child == 0) {
    // In the left child process, sort the left half recursively
    merge_sort(arr, begin, mid, threshold);
    // Terminate the left child once its process finishes
    exit(0);  
  } else {
    right_child = fork();
    // check if fork failed
    if (right_child < 0) {
      fatal("Fork failed");
    } else if (right_child == 0) {
      // In the right child process, sort the right half recursively
      merge_sort(arr, mid, end, threshold);
      // Terminate the right child process once its process finishes
      exit(0);  
    }
  }

  // Parent process waits for both child processes to complete
  int status;
  waitpid(left_child, &status, 0);
  waitpid(right_child, &status, 0);
  if (!WIFEXITED(status)) {
    // subprocess crashed, was interrupted, or did not exit normally
    fatal("Subprocess crashed, was interrupted, or did not exit normally.");
}
if (WEXITSTATUS(status) != 0) {
    // subprocess returned a non-zero exit code
    // if following standard UNIX conventions, this is also an error
    fatal("Subprocess didn't return zero exit code");
}

  // allocate temp array now, so we can avoid unnecessary work
  // if the malloc fails
  int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
  if (temp_arr == NULL)
    fatal("malloc() failed");

  // child processes completed successfully, so in theory
  // we should be able to merge their results
  merge(arr, begin, mid, end, temp_arr);

  // copy data back to main array
  for (size_t i = 0; i < size; i++)
    arr[begin + i] = temp_arr[i];

  // now we can free the temp array
  free(temp_arr);

  // success!
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    // is this right?
    fatal("Threshold value is invalid");
  }

// try to open file
int fd = open(filename, O_RDWR);
if (fd < 0) {
  fatal("Your file could not be opened. Please use another file");
}
struct stat statbuf;
int rc = fstat(fd, &statbuf);
// check that fstat worked
if (rc != 0) {
  fatal("There was an error with fstat. Please try again.");
}
// get file size
size_t file_size_in_bytes = statbuf.st_size;
int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)
// close file descriptor, do I do this here or below??
close(fd);
// check that mmap was successful
if (data == MAP_FAILED) {
    fatal("There was an mmap error. Please try again.");
}

merge_sort(data, 0, file_size_in_elements, threshold);

  // Unmap the memory-mapped file
if (munmap(data, file_size_in_bytes) == -1) {
   fatal("Munmap failed! Please try process again.");
}

// Close the file
if (close(fd) == -1) {
    fatal("Error in closing file. Please try running again.");
}

return 0;
}
