int mailimf_date_time_parse(const char * message, size_t length,
			    size_t * indx,
			    struct mailimf_date_time ** result)
{
  size_t cur_token;
  int day_of_week;
  struct mailimf_date_time * date_time;
  int day;
  int month;
  int year;
  int hour;
  int min;
  int sec;
  int zone;
  int r;

  cur_token = * indx;

  day_of_week = -1;
  r = mailimf_day_of_week_parse(message, length, &cur_token, &day_of_week);
  if (r == MAILIMF_NO_ERROR) {
    r = mailimf_comma_parse(message, length, &cur_token);
    if (r == MAILIMF_ERROR_PARSE) {
    }
    else if (r != MAILIMF_NO_ERROR) {
      return r;
    }
  }
  else if (r != MAILIMF_ERROR_PARSE)
    return r;

  day = 0;
  month = 0;
  year = 0;
  r = mailimf_date_parse(message, length, &cur_token, &day, &month, &year);
  if (r == MAILIMF_ERROR_PARSE) {
    r = mailimf_broken_date_parse(message, length, &cur_token, &day, &month, &year);
  }
  else if (r != MAILIMF_NO_ERROR) {
    return r;
  }

  r = mailimf_fws_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;
  
  hour = 0;
  min = 0;
  sec = 0;
  zone = 0;
  r = mailimf_time_parse(message, length, &cur_token,
			 &hour, &min, &sec, &zone);
  if (r != MAILIMF_NO_ERROR)
    return r;

  date_time = mailimf_date_time_new(day, month, year, hour, min, sec, zone);
  if (date_time == NULL)
    return MAILIMF_ERROR_MEMORY;

  * indx = cur_token;
  * result = date_time;

  return MAILIMF_NO_ERROR;
}
