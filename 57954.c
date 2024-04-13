static int mailimf_month_name_parse(const char * message, size_t length,
				    size_t * indx, int * result)
{
  size_t cur_token;
  int month;
  int guessed_month;
  int r;

  cur_token = * indx;

  guessed_month = guess_month(message, length, cur_token);
  if (guessed_month == -1)
    return MAILIMF_ERROR_PARSE;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token,
					   month_names[guessed_month - 1].str);
  if (r != MAILIMF_NO_ERROR)
    return r;

  month = guessed_month;

  * result = month;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
