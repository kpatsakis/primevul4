static void test_bitwise_operators()
{
  assert_true_rule(
      "rule test { condition: 0x55 | 0xAA == 0xFF }",
      NULL);

  assert_true_rule(
      "rule test { condition: ~0xAA ^ 0x5A & 0xFF == (~0xAA) ^ (0x5A & 0xFF) }",
      NULL);

  assert_true_rule(
      "rule test { condition: ~0x55 & 0xFF == 0xAA }",
      NULL);

  assert_true_rule(
      "rule test { condition: 8 >> 2 == 2 }",
      NULL);

  assert_true_rule(
      "rule test { condition: 1 << 3 == 8 }",
      NULL);

  assert_true_rule(
      "rule test { condition: 1 | 3 ^ 3 == 1 | (3 ^ 3) }",
      NULL);

  assert_false_rule(
      "rule test { condition: ~0xAA ^ 0x5A & 0xFF == 0x0F }",
      NULL);

  assert_false_rule(
      "rule test { condition: 1 | 3 ^ 3 == (1 | 3) ^ 3}",
      NULL);

}
