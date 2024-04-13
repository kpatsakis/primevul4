static int mailimf_day_name_parse(const char * message, size_t length,
				  size_t * indx, int * result)
{
  size_t cur_token;
  int day_of_week;
  int guessed_day;
  int r;

  cur_token = * indx;

  guessed_day = guess_day_name(message, length, cur_token);
  if (guessed_day == -1)
    return MAILIMF_ERROR_PARSE;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token,
					   day_names[guessed_day - 1].str);
  if (r != MAILIMF_NO_ERROR)
    return r;

  day_of_week = guessed_day;

  * result = day_of_week;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
