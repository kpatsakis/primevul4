int main(int argc, char** argv)
{
  yr_initialize();

  test_boolean_operators();
  test_comparison_operators();
  test_arithmetic_operators();
  test_bitwise_operators();
  test_matches_operator();
  test_syntax();
  test_anonymous_strings();
  test_strings();
  test_wildcard_strings();
  test_hex_strings();
  test_count();
  test_at();
  test_in();
  test_offset();
  test_length();
  test_of();
  test_for();
  test_re();
  test_filesize();
  test_comments();
  test_modules();
  test_integer_functions();
  test_entrypoint();
  test_global_rules();

  #if defined(HASH_MODULE)
  test_hash_module();
  #endif

  test_file_descriptor();

  yr_finalize();

  return 0;
}
