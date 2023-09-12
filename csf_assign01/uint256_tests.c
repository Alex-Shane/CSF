#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
  UInt256 rot; // value used to test rotations
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual) \
do { \
  ASSERT(expected.data[0] == actual.data[0]); \
  ASSERT(expected.data[1] == actual.data[1]); \
  ASSERT(expected.data[2] == actual.data[2]); \
  ASSERT(expected.data[3] == actual.data[3]); \
  ASSERT(expected.data[4] == actual.data[4]); \
  ASSERT(expected.data[5] == actual.data[5]); \
  ASSERT(expected.data[6] == actual.data[6]); \
  ASSERT(expected.data[7] == actual.data[7]); \
} while (0)

#define INIT_FROM_ARR(val, arr) \
do { \
  for (unsigned i = 0; i < 8; ++i) \
    val.data[i] = arr[i]; \
} while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_sub(TestObjs *objs);
void test_add(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_sub);
  TEST(test_add);
  TEST(test_negate);
  TEST(test_get_bits);
  TEST(test_create_from_u32);
  TEST(test_create);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_rotate_left);
  TEST(test_rotate_right);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval) {
  for (unsigned i = 0; i < 8; ++i) {
    val->data[i] = wordval;
  }
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"
  set_all(&objs->zero, 0);
  set_all(&objs->one, 0);
  objs->one.data[0] = 1U;
  set_all(&objs->max, 0xFFFFFFFFU);

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR(objs->msb_set, msb_set_data);

  // value with nonzero values in least significant and most significant words
  // Note that the hex representation of this value is
  //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
  uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
  INIT_FROM_ARR(objs->rot, rot_data);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_get_bits(TestObjs *objs) {
  ASSERT(0U == uint256_get_bits(objs->zero, 0));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 4));
  ASSERT(0U == uint256_get_bits(objs->zero, 5));
  ASSERT(0U == uint256_get_bits(objs->zero, 6));
  ASSERT(0U == uint256_get_bits(objs->zero, 7));

  ASSERT(1U == uint256_get_bits(objs->one, 0));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 4));
  ASSERT(0U == uint256_get_bits(objs->one, 5));
  ASSERT(0U == uint256_get_bits(objs->one, 6));
  ASSERT(0U == uint256_get_bits(objs->one, 7));

  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));

  // add test for git bits with rotation object which has scattered data
  ASSERT(0x000000ABU == uint256_get_bits(objs->rot, 0));
  ASSERT(0U == uint256_get_bits(objs->rot, 1));
  ASSERT(0U == uint256_get_bits(objs->rot, 2));
  ASSERT(0U == uint256_get_bits(objs->rot, 3));
  ASSERT(0U == uint256_get_bits(objs->rot, 4));
  ASSERT(0U == uint256_get_bits(objs->rot, 5));
  ASSERT(0U == uint256_get_bits(objs->rot, 6));
  ASSERT(0xCD000000U == uint256_get_bits(objs->rot, 7));
}

void test_create_from_u32(TestObjs *objs) {
  UInt256 zero = uint256_create_from_u32(0U);
  UInt256 one = uint256_create_from_u32(1U);

  ASSERT_SAME(objs->zero, zero);
  ASSERT_SAME(objs->one, one);

  // test max uint32
  UInt256 max_uint32 = uint256_create_from_u32(2147483647);
  uint32_t array[8] = {2147483647, 0,0,0,0,0,0,0};
  UInt256 expected = uint256_create(array);
  ASSERT_SAME(expected, max_uint32);

  //test random uint32
  UInt256 random = uint256_create_from_u32(576);
  uint32_t array2[8] = {576, 0,0,0,0,0,0,0};
  UInt256 expected2 = uint256_create(array2);
  ASSERT_SAME(expected2, random);
}

void test_create(TestObjs *objs) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create(data1);
  ASSERT(1U == val1.data[0]);
  ASSERT(2U == val1.data[1]);
  ASSERT(3U == val1.data[2]);
  ASSERT(4U == val1.data[3]);
  ASSERT(5U == val1.data[4]);
  ASSERT(6U == val1.data[5]);
  ASSERT(7U == val1.data[6]);
  ASSERT(8U == val1.data[7]);

  // test create with really big, random numbers
  uint32_t data2[8] = { 292847475, 2857372, 38574712, 585747376, 48577564, 58696, 790, 863 };
  UInt256 val2 = uint256_create(data2);
  ASSERT(292847475 == val2.data[0]);
  ASSERT(2857372 == val2.data[1]);
  ASSERT(38574712 == val2.data[2]);
  ASSERT(585747376 == val2.data[3]);
  ASSERT(48577564 == val2.data[4]);
  ASSERT(58696 == val2.data[5]);
  ASSERT(790 == val2.data[6]);
  ASSERT(863 == val2.data[7]);

  // test create with all zeros
  uint32_t data3[8] = {0,0,0,0,0,0,0,0};
  UInt256 zero_test = uint256_create(data3);
  ASSERT_SAME(objs->zero, zero_test);
}

void test_create_from_hex(TestObjs *objs) {
  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);

  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);

  // test that create_from_hex only reads rightmost chars with string longer than 64 characters
  UInt256 should_be_zero = uint256_create_from_hex("f0000000000000000000000000000000000000000000000000000000000000000");
  ASSERT_SAME(objs->zero, should_be_zero);

  // test create_from_hex works with random hex, size not multiple of 8
  UInt256 random = uint256_create_from_hex("123456abcdef");
  uint32_t array[8] = {1454099951, 4660, 0, 0, 0, 0, 0, 0 };
  UInt256 expected = uint256_create(array);
  ASSERT_SAME(expected, random);

  // test create_from_hex works with second random, longer, hex whose size is multiple of 8
  UInt256 random2 = uint256_create_from_hex("0123456789abcdef0123456789abcdef");
  uint32_t array2[8] = {2309737967, 19088743, 2309737967, 19088743, 0, 0, 0, 0 };
  UInt256 expected2 = uint256_create(array2);
  ASSERT_SAME(expected2, random2);
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  s = uint256_format_as_hex(objs->max);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);

  // test that creating hex from array of 32 bit ints works
  uint32_t array[8] = {4838373, 58473, 5832, 876, 458, 328, 987, 900};
  UInt256 val = uint256_create(array);
  s = uint256_format_as_hex(val);
  ASSERT(0 == strcmp("384000003db00000148000001ca0000036c000016c80000e4690049d3e5", s));
  free(s);

  // test that creating hex from least significant int works
  UInt256 val2 = uint256_create_from_u32(395948375);
  s = uint256_format_as_hex(val2);
  ASSERT(0 == strcmp("1799b157", s));
  free(s);

  // test that zeros are truncated from the end but not the front
  uint32_t array2[8] = {0,0,0,0,5,0,0,0};
  UInt256 val3 = uint256_create(array2);
  s = uint256_format_as_hex(val3);
  ASSERT(0 == strcmp("500000000000000000000000000000000",s));
  free(s);
}

void uint256_print(const UInt256 *val) {
  for (int i = 7; i >= 0; i--) {
    printf("%08x ", val->data[i]);
  }
  printf("\n");
}

void test_add(TestObjs *objs) {
  UInt256 result;

  result = uint256_add(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->zero, objs->one);
  ASSERT_SAME(objs->one, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(objs->one, objs->one);
  ASSERT_SAME(two, result);

  result = uint256_add(objs->max, objs->one);
  ASSERT_SAME(objs->zero, result);
}

void test_sub(TestObjs *objs) {
  UInt256 result;

  result = uint256_sub(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->one, objs->one);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->zero, objs->one);
  ASSERT_SAME(objs->max, result);
}

void test_negate(TestObjs *objs) {
  UInt256 result;

  result = uint256_negate(objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_negate(objs->one);
  ASSERT_SAME(objs->max, result);

  result = uint256_negate(objs->max);
  ASSERT_SAME(objs->one, result);
}

void test_rotate_left(TestObjs *objs) {
  UInt256 result;

  // rotating the value with just the most significant bit set
  // one position to the left should result in the value equal to 1
  // (i.e., the value with only the least significant bit set)
  result = uint256_rotate_left(objs->msb_set, 1);
  ASSERT_SAME(objs->one, result);

  // after rotating the "rot" value left by 4 bits, the resulting value should be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 4);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);

  // rotating left by more than 32 bits works as expected
  result = uint256_rotate_left(objs->rot, 36);
  ASSERT(0xD0000000U == result.data[0]);
  ASSERT(0x00000ABCU == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);

  // rotating left by more than 256 bits works the same as rotating by bits % 256
  result = uint256_rotate_left(objs->msb_set, 257);
  ASSERT_SAME(objs->one, result);

  // rotating left by 256 bits does nothing
  result = uint256_rotate_left(objs->rot, 256);
  ASSERT_SAME(objs->rot, result);
}

void test_rotate_right(TestObjs *objs) {
  UInt256 result;

  // rotating 1 right by 1 position should result in a value with just
  // the most-significant bit set
  result = uint256_rotate_right(objs->one, 1);
  ASSERT_SAME(objs->msb_set, result);

  // after rotating the "rot" value right by 4 bits, the resulting value should be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 4);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);

  //rotating right by more than 32 bits works as expected
  result = uint256_rotate_right(objs->rot, 68);
  ASSERT(0U == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0xBCD00000U == result.data[5]);
  ASSERT(0x0000000AU == result.data[6]);
  ASSERT(0U == result.data[7]);

  //rotating right by more than 256 bits works as expected
  result = uint256_rotate_right(objs->one, 257);
  ASSERT_SAME(objs->msb_set, result);

  //rotating right by 256 bits does nothing
  result = uint256_rotate_right(objs->rot, 256);
  ASSERT_SAME(objs->rot, result);
}
