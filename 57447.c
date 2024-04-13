int _yr_re_is_word_char(
    uint8_t* input,
    uint8_t character_size)
{
  int result = ((isalnum(*input) || (*input) == '_'));

  if (character_size == 2)
    result = result && (*(input + 1) == 0);

  return result;
}
