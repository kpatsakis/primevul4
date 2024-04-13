int mailimf_fws_word_parse(const char * message, size_t length,
			   size_t * indx, char ** result, int * p_missing_closing_quote)
{
  size_t cur_token;
  char * word;
  int r;
  int missing_closing_quote;

  cur_token = * indx;
  missing_closing_quote = 0;

  r = mailimf_fws_atom_for_word_parse(message, length, &cur_token, &word, &missing_closing_quote);

  if (r == MAILIMF_ERROR_PARSE)
    r = mailimf_fws_quoted_string_parse(message, length, &cur_token, &word);

  if (r != MAILIMF_NO_ERROR)
    return r;

  * result = word;
  * indx = cur_token;
  * p_missing_closing_quote = missing_closing_quote;

  return MAILIMF_NO_ERROR;
}
