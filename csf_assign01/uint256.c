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
  int index = 0;
  for (int k = len; k >= 0; k-=8) {
    if (index == 8) {
      break;
    }
    else {
      int digits_to_read = k >= 8 ? 8 : k;
      const char* chunk = hex + (k - digits_to_read);
      // account for null terminator
      char* read = malloc(9*sizeof(char)); 
      strncpy(read, chunk, digits_to_read);
      // explicity set null terminator
      read[digits_to_read] = '\0';
      result.data[index] = strtoul(read, NULL, 16);
      free(read);
      index++;
    }
  }
  // if not all indexes have been initialized, make the remaining ones zero
  while (index != 8) {
    result.data[index] = 0;
    index++;
  }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = malloc(65*sizeof(char));
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
  uint32_t carry = 0;
  uint32_t cur_left_val;
  uint32_t cur_right_val;
  uint32_t temp_sum;
  for (int i = 0; i < 8; i++) {
    int bit_sum;
    cur_left_val = uint256_get_bits(left, i);
    cur_right_val = uint256_get_bits(right, i);
    temp_sum = 0;
    uint32_t cur_left_bit;
    uint32_t cur_right_bit;
    for (int j = 0; j < 32; j++){
        if (j == 0) {
            cur_left_bit = cur_left_val & 1;
            cur_right_bit = cur_right_val & 1;
        } else {
            cur_left_bit = (cur_left_val >> 1) & 1;
            cur_right_bit = (cur_right_val >> 1) & 1;
        }
        bit_sum = cur_left_bit + cur_right_bit + carry;
        if (bit_sum > 1) {
            carry = bit_sum - 1;
            bit_sum %= 2;
        } else {
            carry = 0;
        }
        temp_sum |= bit_sum << j;
        //printf("j = %d, cur_left_bit = %d, cur_right_bit = %d, bit_sum = %d, carry = %d\n", j, cur_left_bit, cur_right_bit, bit_sum, carry);
    }
    sum.data[i] = temp_sum;
    //printf("i = %d, cur_left_val = %08X, cur_right_val = %08X, temp_sum = %08X, carry = %d, bit_sum = %d\n", i, cur_left_val, cur_right_val, temp_sum, carry, bit_sum);
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  UInt256 negated_right = uint256_negate(right);
  result = uint256_add(negated_right, left);
  return result;
}
// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  uint32_t cur_data;
  uint32_t cur_bit;
  uint32_t negated_data;
  for (int i = 0; i < 8; i++) {
    cur_data = uint256_get_bits(val, i);
    negated_data = 0;
    //printf("result data: %d\n", result.data[i]);
    for (int j = 0; j < 32; j++) {
        if (j == 0) {
            cur_bit = cur_data & 1;
        } else {
            cur_bit = (cur_data >> 1) & 1;
        }
        //printf("cur_bit: %d\n", cur_bit);
        if (cur_bit == 1) {
            cur_bit = 0;
        } else {
            cur_bit = 1;
        }
        //printf("cur_bit post flip: %d\n", cur_bit);
        negated_data |= cur_bit << j;
    }
    result.data[i] = negated_data;
    //printf("Negated data: %08x, i: %d\n", negated_data, i);
   }
   uint32_t one_partial_bit = 1;
   UInt256 one_full_bit = uint256_create_from_u32(one_partial_bit);
   result = uint256_add(result, one_full_bit);
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result = val;
  // if shifting more than 256, just circles back to shifting a small amount (257 = shifting 1 bit)
  nbits = nbits % 256;
  int ints_to_shift = nbits / 32;
  for (int i = 0; i < ints_to_shift; i++) {
    uint32_t left_block = result.data[7];
    for (int k = 7; k > 0; k--) {
      result.data[k] = result.data[k - 1];
    }
    result.data[0] = left_block;
  }

  uint32_t leftover_bits = nbits % 32;
  if (leftover_bits > 0) {
    uint32_t spill_over = 0;
    for (int x = 0; x < 8; x++) {
      uint32_t temp = result.data[x];
      result.data[x] = (result.data[x] << leftover_bits) | spill_over;
      spill_over = temp >> (32-leftover_bits);
    }
    result.data[0] = result.data[0] | spill_over;
  }
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result = val;
  // if shifting more than 256, just circles back to shifting a small amount (257 = shifting 1 bit)
  nbits = nbits % 256;
  int ints_to_shift = nbits / 32;
  for (int i = 0; i < ints_to_shift; i++) {
    uint32_t right_block = result.data[0];
    for (int k = 0; k < 7; k++) {
      result.data[k] = result.data[k + 1];
    }
    result.data[7] = right_block;
  }

  uint32_t leftover_bits = nbits % 32;
  if (leftover_bits > 0) {
    uint32_t temp = result.data[0];
    uint32_t spill_over = temp << (32-leftover_bits);
    result.data[0] = (result.data[0] >> leftover_bits);
    for (int j = 7; j > 0; j--) {
      uint32_t temp = result.data[j];
      result.data[j] = (result.data[j] >> (leftover_bits)) | spill_over;
      spill_over = temp << (32-leftover_bits);
    }
    result.data[7] = result.data[7] | spill_over;
  }
  return result;
}
