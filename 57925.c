static int mailimf_date_parse(const char * message, size_t length,
			      size_t * indx,
			      int * pday, int * pmonth, int * pyear)
{
  size_t cur_token;
  int day;
  int month;
  int year;
  int r;

  cur_token = * indx;

  day = 1;
  r = mailimf_day_parse(message, length, &cur_token, &day);
  if (r != MAILIMF_NO_ERROR)
    return r;

  month = 1;
  r = mailimf_month_parse(message, length, &cur_token, &month);
  if (r != MAILIMF_NO_ERROR)
    return r;

  year = 2001;
  r = mailimf_year_parse(message, length, &cur_token, &year);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * pday = day;
  * pmonth = month;
  * pyear = year;

  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
