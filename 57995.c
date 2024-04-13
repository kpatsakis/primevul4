static int mailimf_year_parse(const char * message, size_t length,
			      size_t * indx, int * result)
{
  uint32_t number;
  size_t cur_token;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_number_parse(message, length, &cur_token, &number);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * indx = cur_token;
  * result = number;

  return MAILIMF_NO_ERROR;
}
