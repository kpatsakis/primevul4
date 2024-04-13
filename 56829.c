static void test_global_rules()
{
  assert_true_rule(
      "global private rule global_rule { \
        condition: \
          true \
      } \
      rule test { \
        condition: true \
      }",
      NULL);

  assert_false_rule(
      "global private rule global_rule { \
        condition: \
          false \
      } \
      rule test { \
        condition: true \
      }",
      NULL);
}
