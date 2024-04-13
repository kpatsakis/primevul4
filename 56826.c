static void test_comments()
{
  assert_true_rule(
      "rule test {\n\
         condition:\n\
             /*** this is a comment ***/\n\
             /* /* /*\n\
                 this is a comment\n\
             */\n\
             true\n\
      }",
      NULL);
}
