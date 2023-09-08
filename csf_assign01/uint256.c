#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  result.data[0] = val;
  for (int x = 1; x < 8; x++) {
    result.data[x] = 0;
  }
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for (int x = 0; x < 8; x++) {
    result.data[x] = data[x];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  int len = strlen(hex);
  const char *cur = len == 1 ? hex : hex + len - 1;
  int remaining = len; // Initialize the remaining characters to read
  for (int i = 0; i < 64; i += 8) {
      if (remaining == 0) {
        result.data[i/8] = 0;
      }
      else {
        // Calculate the number of characters to read for the current int we want
        int read_size = remaining < 8 ? remaining : 8;
        // provide store for up to 8 hex digits plus null terminator
        char cur_int[read_size+1]; 
        // set storage to all zeros
        memset(cur_int, 0, sizeof(cur_int));
        for (int x = 0; x < 8; x++) {
          // if we stil have digits to read, read them
          if (x < read_size) {
            cur_int[x] = *cur; 
          }
          cur--;
        }
        cur_int[read_size] = '\0';
        uint32_t value = (uint32_t)strtoul(cur_int, NULL, 16);
        result.data[i / 8] = value;
        remaining -= read_size;
      }
    }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = (char*) malloc(65);
  char *hex_temp = hex;
  for (int i = 7; i >= 0; i--) {
    uint32_t cur_val = val.data[i];
    char buf[9];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%x", cur_val);
    strcpy(hex_temp, buf);
    hex_temp += strlen(buf);
  }
  trimLeadingZeros(hex);
  return hex;
}

void trimLeadingZeros(char *str) {
    // Find the first non-zero character
    int nonZeroIndex = 0;
    while (str[nonZeroIndex] == '0') {
        nonZeroIndex++;
    }
    // If string is just leading zeros, make it just '0'
    if (nonZeroIndex == 8) {
      str[0] = '0';
      str[1] = '\0';
    }
    // If there are leading zeros and the string isn't '00000000', shift the string to remove them
    if (nonZeroIndex > 0 && nonZeroIndex < 8) {
        int length = strlen(str);
        // +1 for the null terminator
        memmove(str, str + nonZeroIndex, length - nonZeroIndex + 1); 
    }
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits; 
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
