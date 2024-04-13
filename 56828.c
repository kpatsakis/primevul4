static void test_filesize()
{
  char rule[80];

  snprintf(
      rule,
      sizeof(rule),
      "rule test { condition: filesize == %zd }",
      sizeof(PE32_FILE));

  assert_true_rule_blob(
      rule,
      PE32_FILE);
}
