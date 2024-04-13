static void test_anonymous_strings()
{
  assert_true_rule(
      "rule test { strings: $ = \"a\" $ = \"b\" condition: all of them }",
      "ab");
}
