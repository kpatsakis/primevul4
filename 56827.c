static void test_count()
{
  assert_true_rule(
      "rule test { strings: $a = \"ssi\" condition: #a == 2 }",
      "mississippi");
}
