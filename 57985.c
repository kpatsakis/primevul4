static int mailimf_time_of_day_parse(const char * message, size_t length,
				     size_t * indx,
				     int * phour, int * pmin,
				     int * psec)
{
  int hour;
  int min;
  int sec;
  size_t cur_token;
  int r;

  cur_token = * indx;

  r = mailimf_hour_parse(message, length, &cur_token, &hour);
  if (r != MAILIMF_NO_ERROR)
    return r;

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;

  r = mailimf_minute_parse(message, length, &cur_token, &min);
  if (r != MAILIMF_NO_ERROR)
    return r;

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r == MAILIMF_NO_ERROR) {
    r = mailimf_second_parse(message, length, &cur_token, &sec);
    if (r != MAILIMF_NO_ERROR)
      return r;
  }
  else if (r == MAILIMF_ERROR_PARSE)
    sec = 0;
  else
    return r;

  * phour = hour;
  * pmin = min;
  * psec = sec;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
