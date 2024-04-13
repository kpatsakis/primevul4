static int mailimf_day_parse(const char * message, size_t length,
			     size_t * indx, int * result)
{
  size_t cur_token;
  uint32_t day;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_number_parse(message, length, &cur_token, &day);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * result = day;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
