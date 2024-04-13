static void test_at()
{
  assert_true_rule(
      "rule test { \
        strings: $a = \"ssi\" \
        condition: $a at 2 and $a at 5 }",
      "mississippi");

  assert_true_rule(
      "rule test { \
        strings: $a = \"mis\" \
        condition: $a at ~0xFF & 0xFF }",
      "mississippi");

  assert_true_rule_blob(
      "rule test { \
        strings: $a = { 00 00 00 00 ?? 74 65 78 74 } \
        condition: $a at 308}",
      PE32_FILE);
}
