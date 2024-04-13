static void test_boolean_operators()
{
  assert_true_rule(
      "rule test { condition: true }", NULL);

  assert_true_rule(
      "rule test { condition: true or false }", NULL);

  assert_true_rule(
      "rule test { condition: true and true }", NULL);

  assert_true_rule(
      "rule test { condition: 0x1 and 0x2}", NULL);

  assert_false_rule(
      "rule test { condition: false }", NULL);

  assert_false_rule(
      "rule test { condition: true and false }", NULL);

  assert_false_rule(
      "rule test { condition: false or false }", NULL);
}
