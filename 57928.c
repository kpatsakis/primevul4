static int mailimf_day_of_week_parse(const char * message, size_t length,
				     size_t * indx, int * result)
{
  size_t cur_token;
  int day_of_week;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_day_name_parse(message, length, &cur_token, &day_of_week);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * indx = cur_token;
  * result = day_of_week;

  return MAILIMF_NO_ERROR;
}
