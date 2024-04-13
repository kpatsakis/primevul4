int mailimf_word_parse(const char * message, size_t length,
		       size_t * indx, char ** result)
{
  size_t cur_token;
  char * word;
  int r;

  cur_token = * indx;

  r = mailimf_atom_parse(message, length, &cur_token, &word);

  if (r == MAILIMF_ERROR_PARSE)
    r = mailimf_quoted_string_parse(message, length, &cur_token, &word);

  if (r != MAILIMF_NO_ERROR)
    return r;

  * result = word;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
